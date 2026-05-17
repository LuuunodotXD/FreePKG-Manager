#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define INDEX "/var/lib/freepkg/amd64.index"
#define DB "/var/lib/freepkg/installed"

void trim_newline(char *s) {
    s[strcspn(s, "\n")] = 0;
}

void info_pkg(const char *pkg) {
    FILE *f = fopen(INDEX, "r");

    if (!f) {
        perror("index");
        exit(1);
    }

    char line[4096];

    while (fgets(line, sizeof(line), f)) {
        if (line[0] == '#')
            continue;

        char copy[4096];
        strcpy(copy, line);

        char *name = strtok(copy, "|");

        if (name && strcmp(name, pkg) == 0) {
            printf("%s", line);
            fclose(f);
            return;
        }
    }

    fclose(f);
    exit(1);
}

void installed(const char *pkg) {
    char path[512];

    snprintf(path, sizeof(path), "%s/%s", DB, pkg);

    if (access(path, F_OK) == 0)
        exit(0);

    exit(1);
}

void get_installed(const char *pkg) {
    char path[512];

    snprintf(path, sizeof(path), "%s/%s", DB, pkg);

    FILE *f = fopen(path, "r");

    if (!f)
        exit(1);

    char line[1024];

    fgets(line, sizeof(line), f);

    trim_newline(line);

    printf("%s\n", line);

    fclose(f);
}

void compare_dates(const char *a, const char *b) {
    long da = atol(a);
    long db = atol(b);

    if (da > db)
        printf("newer\n");
    else if (da == db)
        printf("same\n");
    else
        printf("older\n");
}

void list_installed() {
    char cmd[512];

    snprintf(cmd, sizeof(cmd),
             "find %s -type f ! -name '*.files' -printf '%%f\n'",
             DB);

    system(cmd);
}

int main(int argc, char **argv) {

    if (argc < 2)
        return 1;

    if (!strcmp(argv[1], "info")) {
        if (argc < 3) return 1;
        info_pkg(argv[2]);
    }

    else if (!strcmp(argv[1], "installed")) {
        if (argc < 3) return 1;
        installed(argv[2]);
    }

    else if (!strcmp(argv[1], "get-installed")) {
        if (argc < 3) return 1;
        get_installed(argv[2]);
    }

    else if (!strcmp(argv[1], "compare-date")) {
        if (argc < 4) return 1;
        compare_dates(argv[2], argv[3]);
    }

    else if (!strcmp(argv[1], "list-installed")) {
        list_installed();
    }

    return 0;
}