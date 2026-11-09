#ifndef TRANSACTION_H
#define TRANSACTION_H



enum TransactionType
{
    TRANSACTION_DEPOSIT,
    TRANSACTION_WITHDRAW,
    TRANSACTION_TRANSFER_OUT,
    TRANSACTION_TRANSFER_IN
};

struct Transaction
{
    char username[50];
    enum TransactionType type;
    double amount;
    char relatedUsername[50];    
};

int AddTransaction(const struct Transaction *transaction);

/* Returns 1 on success, 0 if no records were committed, and -1 if rollback failed. */
int AddTransferTransactions(
    const struct Transaction *outgoing,
    const struct Transaction *incoming
);

int GetTransactionsByUsername(
    const char username[],
    struct Transaction transactions[],
    int maxTransactions
);

#endif
