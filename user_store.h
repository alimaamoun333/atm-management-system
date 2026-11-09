#ifndef USER_STORE_H
#define USER_STORE_H

#include "user.h"

int InitializeUserStore(void);

int FindUserByUsername(
    const char username[],
    struct User *result
);

int AddUser(
    const struct User *user
);

int UpdateUser(
    const struct User *user
);

int RemoveUser(const char username[]);

int LoadUsers(void);
int ParseUserLine(const char line[], struct User *result);
int SaveUsers(void);
#endif
