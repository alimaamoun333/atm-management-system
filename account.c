#include <stdio.h>
#include <string.h>
#include <math.h>
#include "account.h"
#include "user.h"
#include "user_store.h"
#include "transaction.h"





static enum AccountResult RecordTransactionOrRollback(
    struct User *user,
    double oldBalance,
    double amount,
    enum TransactionType type
)
{
    struct Transaction transaction = {0};

    strncpy(
        transaction.username,
        user->username,
        sizeof(transaction.username) - 1
    );

    transaction.type = type;
    transaction.amount = amount;

    if (AddTransaction(&transaction))
    {
        return ACCOUNT_SUCCESS;
    }

    user->balance = oldBalance;

    if (!UpdateUser(user))
    {
        return ACCOUNT_ROLLBACK_ERROR;
    }

    return ACCOUNT_PERSISTENCE_ERROR;
}

double CheckBalance(const struct User *user)
{
    if (user == NULL)
    {
        return 0.0;
    }
    return user->balance;
}

enum AccountResult Deposit(struct User *user, double amount)
{
    if (user == NULL)
    {
        return ACCOUNT_INVALID_USER;
    }

    if (!isfinite(amount) || amount <= 0.0 || !isfinite(user->balance) ||
        !isfinite(user->balance + amount))
    {
        return ACCOUNT_INVALID_AMOUNT;
    }

    double oldBalance = user->balance;
    user->balance += amount;

    if (!UpdateUser(user))
    {
        user->balance = oldBalance;
        return ACCOUNT_PERSISTENCE_ERROR;
    }

    return RecordTransactionOrRollback(
        user,
        oldBalance,
        amount,
        TRANSACTION_DEPOSIT
    );
}

enum AccountResult Withdraw(struct User *user, double amount)
{
    if (user == NULL)
    {
        return ACCOUNT_INVALID_USER;
    }

    if (!isfinite(amount) || amount <= 0.0 || !isfinite(user->balance))
    {
        return ACCOUNT_INVALID_AMOUNT;
    }

    if (amount > user->balance)
    {
        return ACCOUNT_INSUFFICIENT_FUNDS;
    }

    double oldBalance = user->balance;
    user->balance -= amount;

    if (!UpdateUser(user))
    {
        user->balance = oldBalance;
        return ACCOUNT_PERSISTENCE_ERROR;
    }

    return RecordTransactionOrRollback(
        user,
        oldBalance,
        amount,
        TRANSACTION_WITHDRAW
    );
}

struct TransferSnapshot
{
    struct User sender;
    struct User receiver;
};

enum TransferCommitResult
{
    TRANSFER_COMMIT_SUCCESS,
    TRANSFER_COMMIT_FAILED_SAFE,
    TRANSFER_COMMIT_FAILED_UNSAFE
};

static enum AccountResult ValidateTransfer(
    const struct User *sender,
    const char receiverUsername[],
    double amount,
    struct User *receiver
)
{
    if (sender == NULL)
    {
        return ACCOUNT_INVALID_USER;
    }
    if (receiverUsername == NULL || receiverUsername[0] == '\0')
    {
        return ACCOUNT_INVALID_RECEIVER;
    }
    if (!isfinite(amount) || amount <= 0.0 || !isfinite(sender->balance))
    {
        return ACCOUNT_INVALID_AMOUNT;
    }
    if (amount > sender->balance)
    {
        return ACCOUNT_INSUFFICIENT_FUNDS;
    }
    if (!FindUserByUsername(receiverUsername, receiver))
    {
        return ACCOUNT_RECEIVER_NOT_FOUND;
    }
    if (!isfinite(receiver->balance + amount))
    {
        return ACCOUNT_INVALID_AMOUNT;
    }
    if (strcmp(sender->username, receiver->username) == 0)
    {
        return ACCOUNT_SELF_TRANSFER;
    }
    return ACCOUNT_SUCCESS;
}

static struct TransferSnapshot CreateTransferSnapshot(
    const struct User *sender,
    const struct User *receiver
)
{
    struct TransferSnapshot snapshot;
    snapshot.sender = *sender;
    snapshot.receiver = *receiver;
    return snapshot;
}

static void ApplyTransfer(
    struct User *sender,
    struct User *receiver,
    double amount
)
{
    sender->balance -= amount;
    receiver->balance += amount;
}

static void BuildTransferTransactions(
    const struct User *sender,
    const struct User *receiver,
    double amount,
    struct Transaction *outgoing,
    struct Transaction *incoming
)
{
    memset(outgoing, 0, sizeof(*outgoing));
    strcpy(outgoing->username, sender->username);
    strcpy(outgoing->relatedUsername, receiver->username);
    outgoing->type = TRANSACTION_TRANSFER_OUT;
    outgoing->amount = amount;

    memset(incoming, 0, sizeof(*incoming));
    strcpy(incoming->username, receiver->username);
    strcpy(incoming->relatedUsername, sender->username);
    incoming->type = TRANSACTION_TRANSFER_IN;
    incoming->amount = amount;
}

static enum TransferCommitResult CommitTransfer(
    const struct User *sender,
    const struct User *receiver,
    const struct Transaction *outgoing,
    const struct Transaction *incoming
)
{
    int transactionResult;

    if (!UpdateUser(sender) || !UpdateUser(receiver))
    {
        return TRANSFER_COMMIT_FAILED_SAFE;
    }

    transactionResult = AddTransferTransactions(outgoing, incoming);
    if (transactionResult > 0)
    {
        return TRANSFER_COMMIT_SUCCESS;
    }
    if (transactionResult < 0)
    {
        return TRANSFER_COMMIT_FAILED_UNSAFE;
    }
    return TRANSFER_COMMIT_FAILED_SAFE;
}

static int RollbackTransfer(
    struct User *sender,
    const struct TransferSnapshot *snapshot
)
{
    int senderRestored;
    int receiverRestored;

    *sender = snapshot->sender;
    senderRestored = UpdateUser(&snapshot->sender);
    receiverRestored = UpdateUser(&snapshot->receiver);
    return senderRestored && receiverRestored;
}

enum AccountResult Transfer(
    struct User *sender,
    const char receiverUsername[],
    double amount
)
{
    struct User receiver;
    struct TransferSnapshot snapshot;
    struct Transaction outgoing = {0};
    struct Transaction incoming = {0};
    enum AccountResult validationResult;
    enum TransferCommitResult commitResult;

    validationResult = ValidateTransfer(
        sender, receiverUsername, amount, &receiver
    );
    if (validationResult != ACCOUNT_SUCCESS)
    {
        return validationResult;
    }

    snapshot = CreateTransferSnapshot(sender, &receiver);
    ApplyTransfer(sender, &receiver, amount);
    BuildTransferTransactions(
        sender, &receiver, amount, &outgoing, &incoming
    );
    commitResult = CommitTransfer(
        sender, &receiver, &outgoing, &incoming
    );

    if (commitResult == TRANSFER_COMMIT_SUCCESS)
    {
        return ACCOUNT_SUCCESS;
    }
    if (!RollbackTransfer(sender, &snapshot) ||
        commitResult == TRANSFER_COMMIT_FAILED_UNSAFE)
    {
        return ACCOUNT_ROLLBACK_ERROR;
    }
    return ACCOUNT_PERSISTENCE_ERROR;
}

void Logout(const struct User *user)
{
    (void)user;
}
