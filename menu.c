#include <stdio.h>
#include "menu.h"
#include "auth.h"
#include "user.h"
#include "account.h"
#include "user_store.h"
#include "input.h"
#include "transaction.h"

void ShowMainMenu(void)
{
    printf("Welcome to the ATM Management System\n");
    printf("1. Login\n");
    printf("2. Register\n");
    printf("3. Remove Account\n");
    printf("4. Exit\n");
}
int ReadMainChoice(void)
{
    int choice;
    if (!ReadInt("Enter your choice: ", &choice))
    {
        return -1;
    }
    return choice;
}

void HandleMainChoice(int choice, struct User *user)
{
    switch (choice)
    {
    case 1:
        if (Login(user))
        {
            printf("Login successful! Welcome, %s.\n", user->username);
            RunATMMenu(user);
        }
        else
        {
            printf("Invalid username or password. Please try again.\n");
        }
        break;
    case 2:
        if (RegisterUser())
        {
            printf("Registration successful!\n");
        }
        else
        {
            printf("Registration failed.\n");
        }
        break;
    case 3:
        if (RemoveRegisteredUser())
        {
            printf("Account removed successfully.\n");
        }
        else
        {
            printf("Account removal failed. Invalid credentials or storage error.\n");
        }
        break;
    case 4:
        printf("Thank you for using the ATM Management System.\n");
        break;
    default:
        printf("Invalid choice. Please try again.\n");
        break;
    }
      
}

void ShowATMMenu(void)
{
    printf("\nATM Menu\n");
    printf("1. Check Balance\n");
    printf("2. Deposit\n");
    printf("3. Withdraw\n");
    printf("4. Transfer\n");
    printf("5. View Transaction History\n");
    printf("6. Logout\n");
}

int ReadATMChoice(void)
{
    int choice;
    if (!ReadInt("Enter your choice: ", &choice))
    {
        return -1;
    }
    return choice;
}

static void HandleDeposit(struct User *user)
{
    double amount;
    enum AccountResult result;

    if (!ReadDouble("Enter deposit amount: $", &amount))
    {
        printf("Invalid input.\n");
        return;
    }
    result = Deposit(user, amount);
    if (result == ACCOUNT_SUCCESS)
        printf("Deposit successful. New balance: $%.2f\n", user->balance);
    else if (result == ACCOUNT_INVALID_AMOUNT)
        printf("Invalid deposit amount. Please enter a positive value.\n");
    else if (result == ACCOUNT_PERSISTENCE_ERROR)
        printf("Deposit failed. Balance was not changed.\n");
    else
        printf("Deposit failed.\n");
}

static void HandleWithdrawal(struct User *user)
{
    double amount;
    enum AccountResult result;

    if (!ReadDouble("Enter withdrawal amount: $", &amount))
    {
        printf("Invalid input.\n");
        return;
    }
    result = Withdraw(user, amount);
    if (result == ACCOUNT_SUCCESS)
        printf("Withdrawal successful. New balance: $%.2f\n", user->balance);
    else if (result == ACCOUNT_INVALID_AMOUNT)
        printf("Invalid withdrawal amount. Please enter a positive value.\n");
    else if (result == ACCOUNT_INSUFFICIENT_FUNDS)
        printf("Insufficient funds. Current balance: $%.2f\n", user->balance);
    else if (result == ACCOUNT_PERSISTENCE_ERROR)
        printf("Withdrawal failed. Balance was not changed.\n");
    else
        printf("Withdrawal failed.\n");
}

static void HandleTransfer(struct User *user)
{
    char receiverUsername[50];
    double amount;
    enum AccountResult result;

    if (!ReadString("Enter receiver username: ", receiverUsername,
                    sizeof(receiverUsername)))
    {
        printf("Invalid receiver username.\n");
        return;
    }
    if (!ReadDouble("Enter transfer amount: $", &amount))
    {
        printf("Invalid input.\n");
        return;
    }
    result = Transfer(user, receiverUsername, amount);
    if (result == ACCOUNT_SUCCESS)
        printf("Transfer successful. New balance: $%.2f\n", user->balance);
    else if (result == ACCOUNT_INVALID_USER)
        printf("Transfer failed: invalid sender.\n");
    else if (result == ACCOUNT_INVALID_RECEIVER)
        printf("Transfer failed: invalid receiver username.\n");
    else if (result == ACCOUNT_INVALID_AMOUNT)
        printf("Invalid transfer amount. Please enter a positive value.\n");
    else if (result == ACCOUNT_INSUFFICIENT_FUNDS)
        printf("Insufficient funds. Current balance: $%.2f\n", user->balance);
    else if (result == ACCOUNT_RECEIVER_NOT_FOUND)
        printf("Transfer failed: receiver does not exist.\n");
    else if (result == ACCOUNT_SELF_TRANSFER)
        printf("Transfer failed: sender and receiver must be different.\n");
    else if (result == ACCOUNT_PERSISTENCE_ERROR)
        printf("Transfer failed. Both balances were restored.\n");
    else if (result == ACCOUNT_ROLLBACK_ERROR)
        printf("Transfer failed and could not be completely rolled back.\n");
    else
        printf("Transfer failed.\n");
}

static void HandleTransactionHistory(const struct User *user)
{
    struct Transaction transactions[100];
    int count;

    if (user == NULL)
    {
        printf("Invalid user.\n");
        return;
    }
    count = GetTransactionsByUsername(user->username, transactions, 100);
    if (count < 0)
    {
        printf("Failed to retrieve transaction history.\n");
        return;
    }
    if (count == 0)
    {
        printf("No transactions found for user %s.\n", user->username);
        return;
    }

    printf("\n--- Transaction History for %s (%d record%s) ---\n",
           user->username, count, count == 1 ? "" : "s");
    for (int i = 0; i < count; i++)
    {
        if (transactions[i].type == TRANSACTION_TRANSFER_OUT)
            printf("%d. Transfer Out to %s | Amount: $%.2f\n", i + 1,
                   transactions[i].relatedUsername, transactions[i].amount);
        else if (transactions[i].type == TRANSACTION_TRANSFER_IN)
            printf("%d. Transfer In from %s | Amount: $%.2f\n", i + 1,
                   transactions[i].relatedUsername, transactions[i].amount);
        else
            printf("%d. %-8s | Amount: $%.2f\n", i + 1,
                   transactions[i].type == TRANSACTION_DEPOSIT
                       ? "Deposit" : "Withdraw",
                   transactions[i].amount);
    }
}

int HandleATMChoice(int choice, struct User *user)
{
    switch (choice)
    {
    case 1:
        printf("Current balance: $%.2f\n", CheckBalance(user));
        break;
    case 2:
        HandleDeposit(user);
        break;
    case 3:
        HandleWithdrawal(user);
        break;
    case 4:
        HandleTransfer(user);
        break;
    case 5:
        HandleTransactionHistory(user);
        break;
    case 6:
        if (user != NULL)
        {
            printf("User %s logged out successfully.\n", user->username);
        }
        Logout(user);
        return 0; // Set running to 0 to exit the loop
    default:
        printf("Invalid choice. Please try again.\n");
        break;
    }
    return 1;
}

void RunATMMenu(struct User *user)
{
    int running = 1;
    while (running)
    {
        ShowATMMenu();
        int choice = ReadATMChoice();
        running = HandleATMChoice(choice, user);
    }
}
