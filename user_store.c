#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include "user_store.h"
#define MAX_USERS 100
#define MAX_LINE_LENGTH 256

static struct User users[MAX_USERS];
static int userCount = 0;

int InitializeUserStore(void) {
    // Implementation for initializing user store
    
    return LoadUsers(); // Return 1 if successful, 0 otherwise
}

int LoadUsers(void)
{
    FILE *file = fopen("users.txt", "r");

    if (file == NULL)
    {
        userCount = 0;
        return 1;
    }

    struct User tempUsers[MAX_USERS];
    int tempCount = 0;
    char line[MAX_LINE_LENGTH];

    while (fgets(line, sizeof(line), file) != NULL)
    {
        /*
         * If no newline was read and EOF was not reached,
         * the line was too large for the buffer.
         */
        if (strchr(line, '\n') == NULL && !feof(file))
        {
            fclose(file);
            return 0;
        }

        /*
         * Check capacity before storing another user.
         */
        if (tempCount >= MAX_USERS)
        {
            fclose(file);
            return 0;
        }

        struct User tempUser;

        /*
         * Validate and parse the current line.
         */
        if (!ParseUserLine(line, &tempUser))
        {
            fclose(file);
            return 0;
        }

        for (int i = 0; i < tempCount; i++)
        {
            if (strcmp(tempUsers[i].username, tempUser.username) == 0)
            {
                fclose(file);
                return 0;
            }
        }

        /*
         * Store in temporary storage.
         * Do not modify the real user store yet.
         */
        tempUsers[tempCount] = tempUser;
        tempCount++;
    }

    /*
     * fgets() returns NULL for both EOF and read errors.
     * Reject the load if reading failed.
     */
    if (ferror(file))
    {
        fclose(file);
        return 0;
    }

    fclose(file);

    /*
     * Commit only after the entire file has been
     * successfully read and validated.
     */
    for (int i = 0; i < tempCount; i++)
    {
        users[i] = tempUsers[i];
    }

    userCount = tempCount;

    return 1;
}

int ParseUserLine(const char line[], struct User *result)
{
    if (line == NULL || result == NULL)
    {
        return 0;
    }

    char buffer[MAX_LINE_LENGTH];

    if (strlen(line) >= sizeof(buffer))
    {
        return 0;
    }

    strcpy(buffer, line);

    buffer[strcspn(buffer, "\r\n")] = '\0';

    if (buffer[0] == '\0')
    {
     return 0;
    }

    char *username = strtok(buffer, "|");
    char *password = strtok(NULL, "|");
    char *balanceText = strtok(NULL, "|");
    char *extra = strtok(NULL, "|");

    if (
        username == NULL ||
        password == NULL ||
        balanceText == NULL ||
        extra != NULL
    )
    {
        return 0;
    }

    char *endPointer;
    double balance = strtod(balanceText, &endPointer);

    if (endPointer == balanceText || *endPointer != '\0' ||
        !isfinite(balance) || balance < 0.0)
    {
        return 0;
    }

    if (
    strlen(username) >= sizeof(result->username) ||
    strlen(password) >= sizeof(result->password)
    )
    {
    return 0;
    }

    strcpy(result->username, username);
    strcpy(result->password, password);
    result->balance = balance;
    
    return 1;
}

int FindUserByUsername(const char username[], struct User *result) {
    if (username == NULL || username[0] == '\0' || result == NULL) {
        return 0;
    }

    if (strlen(username) >= sizeof(users[0].username)) {
        return 0;
    }

    // Implementation for finding user by username
    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].username, username) == 0) {
            *result = users[i];
            return 1; // Return 1 if found, 0 otherwise
        }
    }
    return 0;
}

int AddUser(const struct User *user)
{
    if (user == NULL || user->username[0] == '\0' ||
        user->password[0] == '\0' || !isfinite(user->balance) ||
        user->balance < 0.0)
    {
        return 0;
    }

    if (userCount >= MAX_USERS)
    {
        return 0;
    }

    struct User existingUser;

    if (FindUserByUsername(user->username, &existingUser))
    {
        return 0;
    }

    users[userCount] = *user;
    userCount++;

    if (!SaveUsers())
    {
        userCount--;
        return 0;
    }

    return 1;
}

int UpdateUser(const struct User *user)
{
    if (user == NULL || user->username[0] == '\0' ||
        user->password[0] == '\0' || !isfinite(user->balance) ||
        user->balance < 0.0)
    {
        return 0;
    }

    for (int i = 0; i < userCount; i++)
    {
        if (strcmp(users[i].username, user->username) == 0)
        {
            struct User oldUser = users[i];

            users[i] = *user;

            if (!SaveUsers())
            {
                users[i] = oldUser;
                return 0;
            }

            return 1;
        }
    }

    return 0;
}

int RemoveUser(const char username[])
{
    if (username == NULL || username[0] == '\0')
    {
        return 0;
    }

    int index = -1;
    for (int i = 0; i < userCount; i++)
    {
        if (strcmp(users[i].username, username) == 0)
        {
            index = i;
            break;
        }
    }

    if (index < 0)
    {
        return 0;
    }

    struct User removedUser = users[index];
    for (int i = index; i < userCount - 1; i++)
    {
        users[i] = users[i + 1];
    }
    userCount--;

    if (SaveUsers())
    {
        return 1;
    }

    for (int i = userCount; i > index; i--)
    {
        users[i] = users[i - 1];
    }
    users[index] = removedUser;
    userCount++;
    return 0;
}

int SaveUsers(void)
{
    FILE *file = fopen("users.tmp", "w");

    if (file == NULL)
    {
        return 0;
    }

    for (int i = 0; i < userCount; i++)
    {
        int result = fprintf(
            file,
            "%s|%s|%.2f\n",
            users[i].username,
            users[i].password,
            users[i].balance
        );

        if (result < 0)
        {
            fclose(file);
            remove("users.tmp");
            return 0;
        }
    }

    if (fflush(file) != 0)
    {
        fclose(file);
        remove("users.tmp");
        return 0;
    }

    if (fclose(file) != 0)
    {
        remove("users.tmp");
        return 0;
    }

    if (rename("users.tmp", "users.txt") == 0)
    {
        return 1;
    }

    errno = 0;
    FILE *backup = fopen("users.bak", "rb");
    if (backup != NULL)
    {
        fclose(backup);
        remove("users.tmp");
        return 0;
    }
    if (errno != ENOENT)
    {
        remove("users.tmp");
        return 0;
    }

    errno = 0;
    int backedUp = (rename("users.txt", "users.bak") == 0);
    if (!backedUp && errno != ENOENT)
    {
        remove("users.tmp");
        return 0;
    }

    if (rename("users.tmp", "users.txt") == 0)
    {
        remove("users.bak");
        return 1;
    }

    if (backedUp)
    {
        rename("users.bak", "users.txt");
    }

    remove("users.tmp");
    return 0;
}
