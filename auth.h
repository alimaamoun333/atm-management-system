#ifndef AUTH_H
#define AUTH_H

#include "user.h"

struct Credentials ReadCredentials(void);

int ValidateCredentials(
    const struct User *user,
    struct Credentials credentials
);

int Login(struct User *user);
int RegisterUser(void);
int RemoveRegisteredUser(void);

struct User ReadRegistrationData(void);

#endif
