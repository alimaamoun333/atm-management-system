#include "transaction.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>

static const char *TransactionTypeToString(
    enum TransactionType type
)
{
    switch (type)
    {
    case TRANSACTION_DEPOSIT:
        return "DEPOSIT";

    case TRANSACTION_WITHDRAW:
        return "WITHDRAW";

    case TRANSACTION_TRANSFER_OUT:
        return "TRANSFER_OUT";

    case TRANSACTION_TRANSFER_IN:
        return "TRANSFER_IN";

    default:
        return NULL;
    }
}

static int IsValidTransaction(const struct Transaction *transaction)
{
    int isTransfer;

    if (transaction == NULL || transaction->username[0] == '\0' ||
        !isfinite(transaction->amount) || transaction->amount <= 0.0 ||
        TransactionTypeToString(transaction->type) == NULL)
    {
        return 0;
    }

    isTransfer = transaction->type == TRANSACTION_TRANSFER_OUT ||
                 transaction->type == TRANSACTION_TRANSFER_IN;
    return !isTransfer || transaction->relatedUsername[0] != '\0';
}

static int WriteTransaction(
    FILE *file,
    const struct Transaction *transaction
)
{
    return fprintf(
        file,
        "%s|%s|%.2f|%s\n",
        transaction->username,
        TransactionTypeToString(transaction->type),
        transaction->amount,
        transaction->relatedUsername
    ) >= 0;
}

static int StringToTransactionType(const char *str, enum TransactionType *type)
{
    if (str == NULL || type == NULL)
    {
        return 0;
    }

    if (strcmp(str, "DEPOSIT") == 0)
    {
        *type = TRANSACTION_DEPOSIT;
        return 1;
    }
    else if (strcmp(str, "WITHDRAW") == 0)
    {
        *type = TRANSACTION_WITHDRAW;
        return 1;
    }
    else if (strcmp(str, "TRANSFER_OUT") == 0)
    {
        *type = TRANSACTION_TRANSFER_OUT;
        return 1;
    }
    else if (strcmp(str, "TRANSFER_IN") == 0)
    {
        *type = TRANSACTION_TRANSFER_IN;
        return 1;
    }

    return 0;
}

static int ParseTransactionLine(const char line[], struct Transaction *result)
{
    if (line == NULL || result == NULL)
    {
        return 0;
    }

    char buffer[256];

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
    char *typeStr = strtok(NULL, "|");
    char *amountText = strtok(NULL, "|");
    char *relatedUsername = strtok(NULL, "|");
    char *extra = strtok(NULL, "|");

    if (
        username == NULL ||
        typeStr == NULL ||
        amountText == NULL ||
        extra != NULL
    )
    {
        return 0;
    }

    if (strlen(username) >= sizeof(result->username))
    {
        return 0;
    }

    enum TransactionType type;
    if (!StringToTransactionType(typeStr, &type))
    {
        return 0;
    }

    char *endPointer;
    double amount = strtod(amountText, &endPointer);

    if (endPointer == amountText || *endPointer != '\0')
    {
        return 0;
    }

    strcpy(result->username, username);
    result->type = type;
    result->amount = amount;
    if (relatedUsername != NULL && relatedUsername[0] != '\0')
    {
        if (strlen(relatedUsername) >= sizeof(result->relatedUsername))
        {
            return 0;
        }
        strcpy(result->relatedUsername, relatedUsername);
    }
    else
    {
        result->relatedUsername[0] = '\0';
    }

    return IsValidTransaction(result);
}

int GetTransactionsByUsername(
    const char username[],
    struct Transaction transactions[],
    int maxTransactions
)
{
    if (
        username == NULL ||
        username[0] == '\0' ||
        transactions == NULL ||
        maxTransactions <= 0
    )
    {
        return -1;
    }

    FILE *file = fopen("transactions.txt", "r");
    if (file == NULL)
    {
        return -1;
    }

    char line[256];
    int count = 0;

    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (strchr(line, '\n') == NULL && !feof(file))
        {
            fclose(file);
            return -1;
        }

        struct Transaction temp;

        if (!ParseTransactionLine(line, &temp))
        {
            fclose(file);
            return -1;
        }

        if (strcmp(temp.username, username) == 0)
        {
            if (count >= maxTransactions)
            {
                break;
            }

            transactions[count] = temp;
            count++;
        }
    }

    if (ferror(file))
    {
        fclose(file);
        return -1;
    }

    if (fclose(file) != 0)
    {
        return -1;
    }

    return count;
}

static int IsValidTransferPair(
    const struct Transaction *outgoing,
    const struct Transaction *incoming
)
{
    return
        IsValidTransaction(outgoing) &&
        IsValidTransaction(incoming) &&
        outgoing->type == TRANSACTION_TRANSFER_OUT &&
        incoming->type == TRANSACTION_TRANSFER_IN &&
        outgoing->username[0] != '\0' &&
        incoming->username[0] != '\0' &&
        outgoing->amount == incoming->amount &&
        strcmp(outgoing->username, incoming->relatedUsername) == 0 &&
        strcmp(incoming->username, outgoing->relatedUsername) == 0;
}

static int AppendTransactions(
    const struct Transaction *first,
    const struct Transaction *second
)
{
    const char *transactionPath = "transactions.txt";
    const char *temporaryPath = "transactions.tmp";
    const char *backupPath = "transactions.bak";
    int originalExists = 1;

    errno = 0;
    FILE *srcFile = fopen(transactionPath, "rb");
    if (srcFile == NULL)
    {
        if (errno != ENOENT)
        {
            return 0;
        }
        originalExists = 0;
    }

    FILE *tmpFile = fopen(temporaryPath, "wb");
    if (tmpFile == NULL)
    {
        if (srcFile != NULL)
        {
            fclose(srcFile);
        }
        return 0;
    }

    if (srcFile != NULL)
    {
        char buffer[4096];
        size_t bytesRead;
        int copyFailed = 0;

        while ((bytesRead = fread(buffer, 1, sizeof(buffer), srcFile)) > 0)
        {
            if (fwrite(buffer, 1, bytesRead, tmpFile) != bytesRead)
            {
                copyFailed = 1;
                break;
            }
        }

        if (ferror(srcFile))
        {
            copyFailed = 1;
        }

        if (fclose(srcFile) != 0)
        {
            copyFailed = 1;
        }

        if (copyFailed)
        {
            fclose(tmpFile);
            remove(temporaryPath);
            return 0;
        }
    }

    int writeFailed = !WriteTransaction(tmpFile, first) ||
        (second != NULL && !WriteTransaction(tmpFile, second)) ||
        fflush(tmpFile) != 0;

    if (writeFailed)
    {
        fclose(tmpFile);
        remove(temporaryPath);
        return 0;
    }

    if (fclose(tmpFile) != 0)
    {
        remove(temporaryPath);
        return 0;
    }

    if (!originalExists)
    {
        if (rename(temporaryPath, transactionPath) == 0)
        {
            return 1;
        }
        remove(temporaryPath);
        return 0;
    }

    /* POSIX replaces the destination atomically; implementations that do not
       support replacement fall through to the standard-C backup sequence. */
    if (rename(temporaryPath, transactionPath) == 0)
    {
        return 1;
    }

    {
        FILE *backupFile = fopen(backupPath, "rb");
        if (backupFile != NULL)
        {
            fclose(backupFile);
            remove(temporaryPath);
            return 0;
        }
        if (errno != ENOENT)
        {
            remove(temporaryPath);
            return 0;
        }
    }

    if (rename(transactionPath, backupPath) != 0)
    {
        remove(temporaryPath);
        return 0;
    }

    if (rename(temporaryPath, transactionPath) == 0)
    {
        /* The committed transaction file is authoritative. A leftover backup
           is harmless and must not turn a successful commit into a failure. */
        remove(backupPath);
        return 1;
    }

    {
        int transactionRestored = rename(backupPath, transactionPath) == 0;
        remove(temporaryPath);
        return transactionRestored ? 0 : -1;
    }
}

int AddTransaction(const struct Transaction *transaction)
{
    if (!IsValidTransaction(transaction))
    {
        return 0;
    }
    return AppendTransactions(transaction, NULL) > 0;
}

int AddTransferTransactions(
    const struct Transaction *outgoing,
    const struct Transaction *incoming
)
{
    if (!IsValidTransferPair(outgoing, incoming))
    {
        return 0;
    }
    return AppendTransactions(outgoing, incoming);
}
