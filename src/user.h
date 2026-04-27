#ifndef USER_H
#define USER_H

typedef struct {
    char name[50];
} User;

void user_init(void);
int user_exists(const char* name);
void user_save(const char* name);
void user_load_current(void);
User* user_get_current(void);

#endif /* USER_H */