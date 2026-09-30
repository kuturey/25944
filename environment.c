#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ulimit.h>
#include <sys/resource.h>
#include <errno.h>
#include <string.h>

#define MAX_OPTIONS 100

struct option_item {
    int option;
    char *value;
};

extern char **environ;

int parse_number(const char *str, long *result)
{
    char *end;
    long value;

    if (str == NULL || *str == '\0') {
        return -1;
    }

    errno = 0;
    end = NULL;

    value = strtol(str, &end, 10);

    if (errno == ERANGE ||
        end == str ||
        *end != '\0' ||
        value < 0) {
        return -1;
    }

    *result = value;
    return 0;
}

int main(int argc, char *argv[])
{
    int opt;
    int count = 0;
    int i;
    char cwd[1024];

    struct option_item options[MAX_OPTIONS];

    while ((opt = getopt(argc, argv, ":ispucC:dvV:U:")) != -1) {

        if (count >= MAX_OPTIONS) {
            fprintf(stderr, "Too many options\n");
            return 1;
        }

        if (opt == '?') {
            fprintf(stderr, "Invalid option: -%c\n", optopt);
            return 1;
        }

        if (opt == ':') {
            fprintf(stderr, "Option -%c requires an argument\n", optopt);
            return 1;
        }

        options[count].option = opt;
        options[count].value = optarg;

        count++;
    }

    for (i = count - 1; i >= 0; i--) {

        switch (options[i].option) {

        case 'i':
            printf("real uid: %d\n", (int)getuid());
            printf("effective uid: %d\n", (int)geteuid());
            printf("real gid: %d\n", (int)getgid());
            printf("effective gid: %d\n", (int)getegid());
            break;

        case 's':
            if (setpgid(0, 0) == -1) {
                perror("setpgid");
                return 1;
            }

            printf("process became process group leader\n");
            break;

        case 'p':
            printf("pid: %d\n", (int)getpid());
            printf("ppid: %d\n", (int)getppid());
            printf("pgid: %d\n", (int)getpgrp());
            break;

        case 'u': {
            long limit;

            errno = 0;
            limit = ulimit(UL_GETFSIZE);

            if (limit == -1 && errno != 0) {
                perror("ulimit");
                return 1;
            }

            printf("ulimit: %ld\n", limit);
            break;
        }

        case 'U': {
            long value;

            if (parse_number(options[i].value, &value) != 0) {
                fprintf(stderr,
                        "Invalid value for -U: %s\n",
                        options[i].value);
                return 1;
            }

            if (ulimit(UL_SETFSIZE, value) == -1) {
                perror("ulimit");
                return 1;
            }

            printf("ulimit changed to: %ld\n", value);
            break;
        }

        case 'c': {
            struct rlimit limit;

            if (getrlimit(RLIMIT_CORE, &limit) == -1) {
                perror("getrlimit");
                return 1;
            }

            if (limit.rlim_cur == RLIM_INFINITY) {
                printf("core file size: unlimited\n");
            }
            else {
                printf("core file size: %lu bytes\n",
                       (unsigned long)limit.rlim_cur);
            }

            break;
        }

        case 'C': {
            long value;
            struct rlimit limit;

            if (parse_number(options[i].value, &value) != 0) {
                fprintf(stderr,
                        "Invalid value for -C: %s\n",
                        options[i].value);
                return 1;
            }

            if (getrlimit(RLIMIT_CORE, &limit) == -1) {
                perror("getrlimit");
                return 1;
            }

            limit.rlim_cur = value;

            if (setrlimit(RLIMIT_CORE, &limit) == -1) {
                perror("setrlimit");
                return 1;
            }

            printf("core file size changed to: %ld bytes\n", value);
            break;
        }

        case 'd':
            if (getcwd(cwd, sizeof(cwd)) == NULL) {
                perror("getcwd");
                return 1;
            }

            printf("current directory: %s\n", cwd);
            break;

        case 'v': {
            char **env;

            for (env = environ; *env != NULL; env++) {
                printf("%s\n", *env);
            }

            break;
        }

        case 'V': {
            char *equal;
            char *name;
            char *value;

            if (options[i].value == NULL) {
                fprintf(stderr, "Invalid argument for -V\n");
                return 1;
            }

            equal = strchr(options[i].value, '=');

            if (equal == NULL || equal == options[i].value) {
                fprintf(stderr,
                        "Invalid environment variable: %s\n",
                        options[i].value);
                return 1;
            }

            *equal = '\0';

            name = options[i].value;
            value = equal + 1;

            if (setenv(name, value, 1) == -1) {
                perror("setenv");
                return 1;
            }

            printf("environment variable changed: %s=%s\n",
                   name, value);

            *equal = '=';

            break;
        }

        default:
            fprintf(stderr, "Unknown option\n");
            return 1;
        }
    }

    return 0;
}
