#include <cstdio>
#include <stdint.h>
#include <cstring>
#include <stdbool.h>
#include <memory>
#include <string>
#include <cstdarg>
#include <iostream>
#include <vector>
#include <signal.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/ptrace.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <cstdlib>

#include "process.h"

using namespace std;

struct process {
    /* Directory, where to run the process. */
    string directory;
    /* Command to be executed, without arguments. */
    string commandLine;
    /* Command arguments. */
    vector<string> args;
    
    /* Memory limit in bytes. */
    long long  memoryLimit;
    /* Time limit in milliseconds. */
    long long timeLimit;
    
    /* Redirection file for stdin, empty if not used. */
    string redirectStdinFile;
    /* Redirection file for stdout, empty if not used. */
    string redirectStdoutFile;
    /* Redirection file for stderr, empty if not used. */
    string redirectStderrFile;

    /* Is true iff process completed execution (completed, interrupted, signalled and so on). */
    bool completed;
    /* Process pid. */
    int pid;
    /* Process exit code or -1 if execution failed because of any reason. */
    int exitCode;
    /* state. */ 
    process_state state;
    /* Consumed time. */
    long long consumedTime;
    /* Consumed memory. */
    long long consumedMemory;

    /* Human-readable comment. */    
    string comment;
    
    /* Check idleness? */
    bool checkIdleness;
    
    /** Consecutive cycles with no CPU usage. */
    int idleCount;
};

static vector<string> tokenizeCommandLine(const string& commandLine) {
    vector<string> result;
    bool inQuotes = false;
    string buffer;
    for (size_t i = 0; i < commandLine.length(); i++) {
        if (commandLine[i] == '\"') {
            inQuotes = !inQuotes;
            continue;
        }
        if (!inQuotes && commandLine[i] <= ' ') {
            if (!buffer.empty() || (i > 0 && commandLine[i - 1] == '\"')) {
                result.push_back(buffer);
                buffer = "";
            }
            continue;
        }
        buffer.append(1, commandLine[i]);
    }
    if (!buffer.empty())
        result.push_back(buffer);
    return result;
}

static string formatByAp(const char* format, va_list ap) {
    // TODO: Possible buffer overflow!
    static int MAX_FORMAT_LENGTH = 1024 * 1024;
    
    char* msg = new char[MAX_FORMAT_LENGTH];
    vsprintf(msg, format, ap);
    string result = msg;
    delete[] msg;

    return result;
}

static string format(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    string result = formatByAp(fmt, ap);
    va_end(ap);
    return result;
}

static void fail(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    string result = format(fmt, ap);
    va_end(ap);
    fprintf(stderr, "FAIL: %s\n", result.c_str());
    exit(EXIT_FAILURE);
}

static string randomToken() {
    static const char* const t = "0123456789abcdef";
    string result;
    for (int i = 0; i < 32; i++) {
        result += t[rand() % 16];    
    }
    return result;
}

static void redirectFd(int fd, const char* const fileName, const char* const mode) {
    FILE* fp = fopen(fileName != NULL && strlen(fileName) ? fileName : "/dev/null", mode);
    
    if (!fp) 
        fail("Can't setup redirection from/to \"%s\"", fileName);
    
    if (dup2(fileno(fp), fd) == -1) 
        fail("Can't setup descriptor for redirection from/to \"%s\"", fileName);
}

void rlim(int resource, rlim_t limit)
{
    rlimit rl;
    rl.rlim_cur = limit;
    rl.rlim_max = limit;
    if (setrlimit(resource, &rl) == -1)
        fail("Can't setup rlimit (%d,%d)", resource, int(limit));
}

static void execute(process& p) {
    p.state = BEFORE;
    p.comment = "";
    
    // Setup directory.
    if (!p.directory.empty() && chdir(p.directory.c_str()))
        exit(EXIT_FAILURE);

    // Setup redirection.
    redirectFd(0, p.redirectStdinFile.c_str(), "r");
    redirectFd(1, p.redirectStdoutFile.c_str(), "w");
    redirectFd(2, p.redirectStderrFile.c_str(), "w");
    
    // Setup limits.
    /*
    if (p.timeLimit > 0)
        rlim(RLIMIT_CPU, p.timeLimit / 1000 + 5); // Given + 5 seconds.
    if (p.memoryLimit > 0)
        rlim(RLIMIT_AS, p.memoryLimit + 1024 * 1024 * 32); // Given + 32 megabytes.
    */
        
    char* commandLine = strdup(p.commandLine.c_str());
    char** argv = new char*[p.args.size() + 2];
    argv[0] = strdup(p.commandLine.c_str());
    for (size_t i = 0; i < p.args.size(); i++)
        argv[i + 1] = strdup(p.args[i].c_str());
    argv[p.args.size() + 1] = NULL;

    char** envp = new char*[2];
    envp[0] = strdup((string("PATH=") + getenv("PATH")).c_str());
    envp[1] = NULL;

    p.state = RUNNING;    
    execvp(commandLine, argv);
    
    p.completed = true;
    p.state = FAILED;
    p.exitCode = -1;
    
    if (errno == ENOENT)
        p.comment = "No such file";

    if (errno == E2BIG)
        p.comment = "Too large arguments";
}

long long ptime(pid_t pid) {
    int tickspersec = sysconf(_SC_CLK_TCK);
    char name[128], buf[512];
    sprintf(name, "/proc/%ld/stat", (long)pid);
    FILE* f = fopen(name, "r");
    int result = 0, i;
    for (i = 0; i < 15; i++) {
        fscanf(f, "%s", buf);
        if (i >= 13)
            result += atoi(buf);
    }
    fclose(f);
    return (int)(result * 1000.0 / tickspersec + 0.5);
}

long long pmemory(pid_t pid) {
    char name[128], buf[512];
    sprintf(name, "/proc/%ld/status", (long)pid);
    FILE* f = fopen(name, "r");
    while (f && NULL != fgets(buf, sizeof(buf), f)) {
        char key[512], value[512], units[512];
        sscanf(buf, "%s %s %s", key, value, units);
        if (strcmp(key, "VmPeak:") == 0) {
            int num = atoi(value);
            fclose(f);
            if (strcmp(units, "kB") == 0) {
                return num * 1024LL;
            } else {
                fail("Unexpected units %s", units);
            }
        }
    }
    fclose(f);
    return 0;
}

static void update(process& p) {
    long long time = ptime(p.pid);
    
    if (p.consumedTime == time)
	p.idleCount++;
    else
	p.idleCount = 0;
	
    p.consumedTime = max(p.consumedTime, time);
    p.consumedMemory = max(p.consumedMemory, pmemory(p.pid));
}

static void updateByRusage(process& p, struct rusage& usage) {
    p.consumedTime = max(p.consumedTime, (long long)(usage.ru_utime.tv_sec * 1000LL 
        + usage.ru_utime.tv_usec / 1000LL 
        + usage.ru_stime.tv_sec * 1000LL 
        + usage.ru_stime.tv_usec / 1000LL));
    p.consumedMemory = max(p.consumedMemory, usage.ru_maxrss * 1024LL);
}

static bool isIdle(const process& p, long long passedTimeMillis) {
    return p.checkIdleness && (passedTimeMillis > 1000 && p.idleCount > 100
	|| passedTimeMillis > 5000 && passedTimeMillis > 10 * p.timeLimit && p.timeLimit > 0);
}

static void waitFor(process& p) {
    p.idleCount = 0;
    long long startTimeMillis = time(NULL);
    int iter = 0;

    // Main loop.
    while (!p.completed) {
	iter++;
        int status;
        pid_t wres;
        struct rusage usage;
        wres = wait4(p.pid, &status,
            WUNTRACED | WCONTINUED | WNOHANG, &usage);
            
        // Is running?
        if (wres == 0) {
            update(p);
            if (p.consumedTime > p.timeLimit && p.timeLimit > 0) {
                p.completed = true;
                p.exitCode = -1;
                p.state = TIME_EXCEEDED;
                p.comment = format("Process has been time limited [timeLimit=%lld ms]", p.timeLimit);
                kill(p.pid, SIGKILL);
                return;
            }
            if (p.consumedMemory > p.memoryLimit && p.memoryLimit > 0) {
                p.completed = true;
                p.exitCode = -1;
                p.state = MEMORY_EXCEEDED;
                p.comment = format("Process has been memory limited [memoryLimit=%lld kb]", p.timeLimit / 1024);
                kill(p.pid, SIGKILL);
                return;
            }
            long long passed = (time(NULL) - startTimeMillis) * 1000LL;
            if (isIdle(p, passed)) {
        	p.completed = true;
        	p.exitCode = -1;
        	p.state = IDLENESS_EXCEEDED;
        	p.comment = "Process hangs or something like it";
        	kill(p.pid, SIGKILL);
        	return;
            }
            usleep(min(1000 * iter, 50000));
        } else {
            updateByRusage(p, usage);
            if (p.consumedTime > p.timeLimit && p.timeLimit > 0) {
                p.completed = true;
                p.exitCode = -1;
                p.state = TIME_EXCEEDED;
                p.comment = format("Process has been time limited [timeLimit=%lld ms]", p.timeLimit);
                return;
            }
            if (p.consumedMemory > p.memoryLimit && p.memoryLimit > 0) {
                p.completed = true;
                p.exitCode = -1;
                p.state = MEMORY_EXCEEDED;
                p.comment = format("Process has been memory limited [memoryLimit=%lld kb]", p.timeLimit / 1024);
                return;
            }
            if (wres == -1 && errno == EINTR) {
                p.completed = true;
                p.exitCode = -1;
                p.state = FAILED;
                p.comment = "Process has been killed [errno == EINTR]";
                return;
            } else if (wres == p.pid && WIFSTOPPED(status)) {
                p.completed = true;
                p.exitCode = -1;
                p.state = FAILED;
                p.comment = "Process has been killed [WIFSTOPPED]";
                return;
            } else if (wres == p.pid && WIFEXITED(status)) {
                p.completed = true;
                p.exitCode = WEXITSTATUS(status);
                p.comment = "";
                p.state = EXITED;
                return;
            } else if (wres == p.pid && WIFSIGNALED(status)) {
                p.completed = true;
                p.exitCode = -1;
                p.comment = "Process has been killed [WIFSIGNALED]";
                p.state = FAILED;
                return;
            } else {
                p.completed = true;
                p.exitCode = -1;
                p.comment = format("Unexpected process state: wres=%d, status=%d.", int(wres), status);
                p.state = FAILED;
                return;
            }
        }
    }
}

static long long time_ms() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000LL + tv.tv_usec / 1000LL;
}

static process_outcome run(process& p) {
    if (p.redirectStdoutFile.empty()) {
	char name[] = "/tmp/process_output_XXXXXX";
	close(mkstemp(name));
        p.redirectStdoutFile = name;
    }
    
    if (p.redirectStderrFile.empty()) {
	char name[] = "/tmp/process_error_XXXXXX";
	close(mkstemp(name));
        p.redirectStderrFile = name;
    }

    process_outcome result;
    p.consumedMemory = p.consumedTime = 0LL;
    p.state = FAILED;
    p.exitCode = -1;
    p.completed = false;
    p.comment = "Can't fork().";
    long long start = time_ms();
    
    if (!(p.pid = fork())) {
        execute(p);
        exit(EXIT_FAILURE);
    } else {
        if (p.pid > 0)
            waitFor(p);
        kill(p.pid, SIGKILL);
        result.state = p.state;
        result.exit_code = p.exitCode;
        result.time = p.consumedTime;
        result.memory = p.consumedMemory;
        result.output_file = p.redirectStdoutFile;
        result.error_file = p.redirectStderrFile;
        result.comment = p.comment;
        result.passed_time = time_ms() - start;
    }
    
    return result;
}

process_outcome run(const std::string& command_line, const process_params& params)
{
    process p;
    p.directory = params.directory;
    p.timeLimit = params.time_limit;
    p.memoryLimit = params.memory_limit;
    p.redirectStdinFile = params.input_file;
    p.redirectStdoutFile = params.output_file;
    p.redirectStderrFile = params.error_file;
    p.checkIdleness = params.check_idleness;
    
    vector<string> args = tokenizeCommandLine(command_line);
    p.commandLine = args[0];
    for (size_t i = 1; i < args.size(); i++)
	p.args.push_back(args[i]);
    return run(p);
}
