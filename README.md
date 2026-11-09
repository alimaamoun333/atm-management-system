# ATM Management System

A robust, console-based **ATM Management System** written in C. This application simulates a full-featured Automated Teller Machine (ATM) with multi-user support, authentication, account management, financial transactions (deposit, withdrawal, transfer), transaction history logging, and file-based data persistence.

---

## 🚀 Features

- 👤 **User Authentication & Management**
  - **Register Account**: Create a new account with a unique username, password, and initial balance.
  - **Secure Login**: Authenticate existing users against stored credentials.
  - **Account Deletion**: Safely unregister user accounts.
  - **Session Management**: Clean login and logout flow.

- 💰 **Banking Operations**
  - **Check Balance**: View current real-time account balance.
  - **Deposit Funds**: Deposit money into the account with positive value validation.
  - **Withdraw Funds**: Withdraw money with balance availability checks.
  - **Funds Transfer**: Transfer money to another registered user account securely (with atomic rollback protection if transaction fails).

- 📜 **Transaction History & Audit Logging**
  - Logs all deposits, withdrawals, outgoing transfers, and incoming transfers.
  - View individual transaction history linked to the user account.
  - Saved persistently to disk.

- 💾 **File Persistence & Data Integrity**
  - **User Store**: Saves user records to `users.txt`.
  - **Transaction Ledger**: Appends financial activities to `transactions.txt`.
  - Input sanitization and error handling for robust operation.

---

## 🛠️ Module Architecture

| Module | Source Files | Description |
| :--- | :--- | :--- |
| **Main Entry** | [main.c](file:///c:/Users/Ali_maamoun/Desktop/01%20NextEra/atm-management-system/main.c) | Initializes storage, launches the main application loop. |
| **Authentication** | [auth.h](file:///c:/Users/Ali_maamoun/Desktop/01%20NextEra/atm-management-system/auth.h), [auth.c](file:///c:/Users/Ali_maamoun/Desktop/01%20NextEra/atm-management-system/auth.c) | Handles user login, registration, credential reading, and validation. |
| **Account Operations** | [account.h](file:///c:/Users/Ali_maamoun/Desktop/01%20NextEra/atm-management-system/account.h), [account.c](file:///c:/Users/Ali_maamoun/Desktop/01%20NextEra/atm-management-system/account.c) | Implements balance checks, deposits, withdrawals, and inter-account transfers. |
| **User Management** | [user.h](file:///c:/Users/Ali_maamoun/Desktop/01%20NextEra/atm-management-system/user.h), [user.c](file:///c:/Users/Ali_maamoun/Desktop/01%20NextEra/atm-management-system/user.c) | Defines `User` and `Credentials` data structures. |
| **User Storage** | [user_store.h](file:///c:/Users/Ali_maamoun/Desktop/01%20NextEra/atm-management-system/user_store.h), [user_store.c](file:///c:/Users/Ali_maamoun/Desktop/01%20NextEra/atm-management-system/user_store.c) | Handles loading, saving, updating, and removing users in `users.txt`. |
| **Transaction Engine** | [transaction.h](file:///c:/Users/Ali_maamoun/Desktop/01%20NextEra/atm-management-system/transaction.h), [transaction.c](file:///c:/Users/Ali_maamoun/Desktop/01%20NextEra/atm-management-system/transaction.c) | Manages transaction records, history queries, and ledger persistence. |
| **UI & Menus** | [menu.h](file:///c:/Users/Ali_maamoun/Desktop/01%20NextEra/atm-management-system/menu.h), [menu.c](file:///c:/Users/Ali_maamoun/Desktop/01%20NextEra/atm-management-system/menu.c) | Renders main menu, ATM menu options, and routes user selections. |
| **Input Utility** | [input.h](file:///c:/Users/Ali_maamoun/Desktop/01%20NextEra/atm-management-system/input.h), [input.c](file:///c:/Users/Ali_maamoun/Desktop/01%20NextEra/atm-management-system/input.c) | Helper routines for clean, safe keyboard input parsing. |

---

## 📋 Prerequisites

- **C Compiler**: `gcc` (GCC / MinGW-w64 / Clang / MSVC)
- Standard C Libraries (`stdio.h`, `stdlib.h`, `string.h`)

---

## ⚙️ Compilation & Execution

### 1. Compiling with GCC (Windows / Linux / macOS)

Open your terminal or command prompt in the project root directory and run:

```bash
gcc main.c account.c auth.c input.c menu.c transaction.c user.c user_store.c -o atm.exe
```

### 2. Running the Application

**Windows:**
```powershell
.\atm.exe
```

**Linux / macOS:**
```bash
./atm.exe
```

---

## 📁 Data Files

- **`users.txt`**: Stores registered user accounts in formatted plain text (`username password balance`).
- **`transactions.txt`**: Records transaction ledger entries for deposits, withdrawals, and transfers.

---

## 📄 License

This project is created for educational and system administration demonstration purposes.
