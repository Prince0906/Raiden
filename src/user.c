#include "user.h"
#include "string.h"
#include <stdio.h>

static User current_user;
static const char* USER_FILE = "users.txt";

void user_init(void) {
    // memset equivalent
    for (size_t i = 0; i < sizeof(current_user.name); i++) {
        current_user.name[i] = 0;
    }
}

int user_exists(const char* name) {
    FILE* f = fopen(USER_FILE, "r");
    if (!f) return 0;
    char line[51];
    while (fgets(line, sizeof(line), f)) {
        // remove newline
        char* p = line;
        while (*p && *p != '\n') p++;
        *p = 0;
        if (my_strcmp(line, name) == 0) {
            fclose(f);
            return 1;
        }
    }
    fclose(f);
    return 0;
}

void user_save(const char* name) {
    if (user_exists(name)) return; // already exists
    FILE* f = fopen(USER_FILE, "a");
    if (f) {
        fprintf(f, "%s\n", name);
        fclose(f);
    }
    my_strcpy(current_user.name, name);
    // Save current
    f = fopen("current_user.txt", "w");
    if (f) {
        fprintf(f, "%s\n", name);
        fclose(f);
    }
}

void user_load_current(void) {
    FILE* f = fopen("current_user.txt", "r");
    if (f) {
        fgets(current_user.name, sizeof(current_user.name), f);
        char* p = current_user.name;
        while (*p && *p != '\n') p++;
        *p = 0;
        fclose(f);
    }
}

User* user_get_current(void) {
    return &current_user;
}