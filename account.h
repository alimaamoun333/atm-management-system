#ifndef ACCOUNT_H
#define ACCOUNT_H

#include "user.h"

enum AccountResult
{
    ACCOUNT_SUCCESS,
    ACCOUNT_INVALID_USER,
    ACCOUNT_INVALID_AMOUNT,
    ACCOUNT_INSUFFICIENT_FUNDS,
    ACCOUNT_INVALID_RECEIVER,
    ACCOUNT_RECEIVER_NOT_FOUND,
    ACCOUNT_SELF_TRANSFER,
    ACCOUNT_PERSISTENCE_ERROR,
    ACCOUNT_ROLLBACK_ERROR
};

double CheckBalance(const struct User *user);

enum AccountResult Deposit(
    struct User *user,
    double amount
);

enum AccountResult Withdraw(
    struct User *user,
    double amount
);

enum AccountResult Transfer(
    struct User *sender,
    const char receiverUsername[],
    double amount
);

void Logout(const struct User *user);

#endif
