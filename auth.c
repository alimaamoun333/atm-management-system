#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "user.h"
#include "auth.h"
#include "user_store.h"
#include "input.h"

int Login(struct User *user)
{
    if (user == NULL)
    {
        return 0;
    }

    struct Credentials creds = ReadCredentials();
    struct User foundUser;

    if (FindUserByUsername(creds.username, &foundUser) && ValidateCredentials(&foundUser, creds))
    {
        *user = foundUser;
        return 1;
    }
    else
    {
        return 0;
    }
}

int RegisterUser(void)
{
    struct User newUser = ReadRegistrationData();
    if (AddUser(&newUser))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int RemoveRegisteredUser(void)
{
    struct Credentials credentials = ReadCredentials();
    struct User user;

    if (!FindUserByUsername(credentials.username, &user) ||
        !ValidateCredentials(&user, credentials))
    {
        return 0;
    }

    return RemoveUser(credentials.username);
}

struct User ReadRegistrationData(void)
{
    struct User newUser;
    while (!ReadString("Enter username: ", newUser.username, sizeof(newUser.username)))
    {
        printf("Invalid username. Please try again.\n");
    }
    while (!ReadString("Enter password: ", newUser.password, sizeof(newUser.password)))
    {
        printf("Invalid password. Please try again.\n");
    }
    newUser.balance = 0.0; // Initialize balance to 0 for new users
    return newUser;
}

struct Credentials ReadCredentials(void)
{
    struct Credentials creds;
    while (!ReadString("Enter username: ", creds.username, sizeof(creds.username)))
    {
        printf("Invalid username. Please try again.\n");
    }
    while (!ReadString("Enter password: ", creds.password, sizeof(creds.password)))
    {
        printf("Invalid password. Please try again.\n");
    }
    return creds;
}

int ValidateCredentials(const struct User *user, struct Credentials credentials)
{
    if (user == NULL || user->username[0] == '\0' ||
        user->password[0] == '\0' || credentials.username[0] == '\0' ||
        credentials.password[0] == '\0')
    {
        return 0;
    }

    if (
        strcmp(credentials.username, user->username) == 0 &&
        strcmp(credentials.password, user->password) == 0
    )
    {
        return 1;
    }

    return 0;
}
