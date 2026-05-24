#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

enum {
    DEFAULT_BELL_THRESHOLD_SECONDS = 30,
    MAX_LABEL_BYTES = 1024,
};

struct timer_state {
    time_t started_at;
    int bell_threshold_seconds;
    char label[MAX_LABEL_BYTES];
};

struct start_arguments {
    int threshold_seconds;
    char label[MAX_LABEL_BYTES];
};

static void print_help(FILE *stream) {
    fprintf(stream,
        "Usage:\n"
        "  task-timer start [--threshold seconds] [label]\n"
        "  task-timer stop\n"
        "  task-timer finish\n"
        "  task-timer status\n"
        "  task-timer clear\n");
}

static int parse_int(const char *value, int min, int max, int *out) {
    char *end = NULL;
    long parsed;

    errno = 0;
    parsed = strtol(value, &end, 10);
    if (errno != 0 || end == value || *end != '\0' || parsed < min || parsed > max) {
        return 0;
    }
    *out = (int)parsed;
    return 1;
}

static int exe_dir(char *buffer, size_t buffer_size) {
    ssize_t length = readlink("/proc/self/exe", buffer, buffer_size - 1);
    char *slash;

    if (length < 0 || (size_t)length >= buffer_size - 1) {
        return 0;
    }
    buffer[length] = '\0';
    slash = strrchr(buffer, '/');
    if (slash == NULL) {
        return 0;
    }
    *slash = '\0';
    return 1;
}

static int make_path(char *buffer, size_t buffer_size, const char *leaf) {
    char dir[PATH_MAX];
    if (!exe_dir(dir, sizeof(dir))) {
        return 0;
    }
    if (snprintf(buffer, buffer_size, "%s/%s", dir, leaf) >= (int)buffer_size) {
        return 0;
    }
    return 1;
}

static int state_path(char *buffer, size_t buffer_size) {
    return make_path(buffer, buffer_size, ".task-timer-state");
}

static void trim_newline(char *value) {
    size_t length = strlen(value);
    while (length > 0 && (value[length - 1] == '\n' || value[length - 1] == '\r')) {
        value[--length] = '\0';
    }
}

static int read_state(struct timer_state *state) {
    char path[PATH_MAX];
    char started_line[64];
    char threshold_line[64];
    FILE *stream;
    long long started;
    int threshold;

    if (!state_path(path, sizeof(path))) {
        return 0;
    }
    stream = fopen(path, "r");
    if (stream == NULL) {
        return 0;
    }

    if (fgets(started_line, sizeof(started_line), stream) == NULL ||
        fgets(threshold_line, sizeof(threshold_line), stream) == NULL ||
        fgets(state->label, sizeof(state->label), stream) == NULL) {
        fclose(stream);
        return 0;
    }
    fclose(stream);

    trim_newline(started_line);
    trim_newline(threshold_line);
    trim_newline(state->label);

    if (sscanf(started_line, "%lld", &started) != 1 || !parse_int(threshold_line, 0, 86400, &threshold)) {
        return 0;
    }

    state->started_at = (time_t)started;
    state->bell_threshold_seconds = threshold;
    if (state->label[0] == '\0') {
        snprintf(state->label, sizeof(state->label), "task");
    }
    return 1;
}

static int write_state(const struct timer_state *state) {
    char path[PATH_MAX];
    FILE *stream;

    if (!state_path(path, sizeof(path))) {
        return 0;
    }
    stream = fopen(path, "w");
    if (stream == NULL) {
        return 0;
    }

    fprintf(stream, "%lld\n%d\n%s\n", (long long)state->started_at, state->bell_threshold_seconds, state->label);
    return fclose(stream) == 0;
}

static int delete_state(void) {
    char path[PATH_MAX];
    if (!state_path(path, sizeof(path))) {
        return 0;
    }
    if (unlink(path) == 0 || errno == ENOENT) {
        return 1;
    }
    return 0;
}

static void format_elapsed(time_t seconds, char *buffer, size_t buffer_size) {
    long value = seconds < 0 ? 0 : (long)seconds;
    long hours = value / 3600;
    long minutes = (value % 3600) / 60;
    long secs = value % 60;

    if (hours > 0) {
        snprintf(buffer, buffer_size, "%ld:%02ld:%02ld", hours, minutes, secs);
    } else {
        snprintf(buffer, buffer_size, "%ld:%02ld", minutes, secs);
    }
}

static int join_tail(int argc, char **argv, int start_index, char *buffer, size_t buffer_size) {
    int i;
    size_t used = 0;

    buffer[0] = '\0';
    for (i = start_index; i < argc; i++) {
        size_t length = strlen(argv[i]);
        if (used + (used > 0 ? 1 : 0) + length >= buffer_size) {
            return 0;
        }
        if (used > 0) {
            buffer[used++] = ' ';
            buffer[used] = '\0';
        }
        memcpy(buffer + used, argv[i], length + 1);
        used += length;
    }
    if (buffer[0] == '\0') {
        snprintf(buffer, buffer_size, "task");
    }
    return 1;
}

static int parse_threshold_option(const char *arg, const char **value) {
    const char *names[] = { "--threshold", "/threshold", "-t" };
    size_t i;

    for (i = 0; i < sizeof(names) / sizeof(names[0]); i++) {
        size_t length = strlen(names[i]);
        if (strcmp(arg, names[i]) == 0) {
            *value = NULL;
            return 1;
        }
        if (strncmp(arg, names[i], length) == 0 && (arg[length] == '=' || arg[length] == ':')) {
            *value = arg + length + 1;
            return 1;
        }
    }
    return 0;
}

static int parse_start_arguments(int argc, char **argv, struct start_arguments *arguments) {
    int i = 2;

    arguments->threshold_seconds = DEFAULT_BELL_THRESHOLD_SECONDS;
    arguments->label[0] = '\0';

    while (i < argc) {
        const char *value = NULL;
        if (!parse_threshold_option(argv[i], &value)) {
            break;
        }
        if (value == NULL) {
            if (++i >= argc) {
                return 0;
            }
            value = argv[i];
        }
        if (!parse_int(value, 0, 86400, &arguments->threshold_seconds)) {
            return 0;
        }
        i++;
    }

    return join_tail(argc, argv, i, arguments->label, sizeof(arguments->label));
}

static int run_ring(void) {
    const char *leaves[] = { "ring", "ring-bell", "ring-bell.exe", "ring-bell.cmd" };
    size_t i;

    for (i = 0; i < sizeof(leaves) / sizeof(leaves[0]); i++) {
        char path[PATH_MAX];
        pid_t pid;
        int status = 0;

        if (!make_path(path, sizeof(path), leaves[i]) || access(path, X_OK) != 0) {
            continue;
        }

        pid = fork();
        if (pid < 0) {
            continue;
        }
        if (pid == 0) {
            execl(path, path, (char *)NULL);
            _exit(127);
        }
        if (waitpid(pid, &status, 0) > 0 && WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            return 1;
        }
        return 0;
    }

    fputc('\a', stdout);
    fflush(stdout);
    return 1;
}

static int command_start(int argc, char **argv) {
    struct start_arguments arguments;
    struct timer_state state;

    if (!parse_start_arguments(argc, argv, &arguments)) {
        fprintf(stderr, "Usage: task-timer start [--threshold seconds] [label]\n");
        return 64;
    }

    state.started_at = time(NULL);
    state.bell_threshold_seconds = arguments.threshold_seconds;
    snprintf(state.label, sizeof(state.label), "%s", arguments.label);

    if (!write_state(&state)) {
        fprintf(stderr, "Timer error: could not write state.\n");
        return 1;
    }

    printf("Started timer for %s. Bell threshold: %d.\n", state.label, state.bell_threshold_seconds);
    return 0;
}

static int command_finish(void) {
    struct timer_state state;
    time_t elapsed_seconds;
    char elapsed[32];
    int should_ring;
    int rang = 0;

    if (!read_state(&state)) {
        printf("No running timer.\n");
        return 1;
    }

    elapsed_seconds = time(NULL) - state.started_at;
    format_elapsed(elapsed_seconds, elapsed, sizeof(elapsed));
    should_ring = elapsed_seconds >= state.bell_threshold_seconds;
    if (should_ring) {
        rang = run_ring();
    }
    delete_state();

    if (should_ring && rang) {
        printf("Elapsed: %s. Bell: rung at threshold %d.\n", elapsed, state.bell_threshold_seconds);
    } else {
        printf("Elapsed: %s. Bell: skipped at threshold %d.\n", elapsed, state.bell_threshold_seconds);
    }

    return should_ring && !rang ? 1 : 0;
}

static int command_status(void) {
    struct timer_state state;
    char elapsed[32];

    if (!read_state(&state)) {
        printf("No running timer.\n");
        return 1;
    }

    format_elapsed(time(NULL) - state.started_at, elapsed, sizeof(elapsed));
    printf("Running: %s. Elapsed: %s. Bell threshold: %d.\n", state.label, elapsed, state.bell_threshold_seconds);
    return 0;
}

static int command_clear(void) {
    if (!delete_state()) {
        fprintf(stderr, "Timer error: could not clear state.\n");
        return 1;
    }
    printf("Cleared timer.\n");
    return 0;
}

int main(int argc, char **argv) {
    const char *command = argc > 1 ? argv[1] : "status";

    if (strcmp(command, "start") == 0) {
        return command_start(argc, argv);
    }
    if (strcmp(command, "stop") == 0 || strcmp(command, "finish") == 0) {
        return command_finish();
    }
    if (strcmp(command, "status") == 0) {
        return command_status();
    }
    if (strcmp(command, "clear") == 0) {
        return command_clear();
    }
    if (strcmp(command, "help") == 0 || strcmp(command, "--help") == 0 || strcmp(command, "-h") == 0 || strcmp(command, "/?") == 0) {
        print_help(stdout);
        return 0;
    }

    fprintf(stderr, "Unknown command. Use: start, stop, finish, status, clear.\n");
    return 64;
}
