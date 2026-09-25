# WiseVault – C++ Banking Management System

WiseVault is a **console-based Banking Management System** developed in **C++17** using **Object-Oriented Programming (OOP)**, **inheritance & polymorphism**, **file handling**, and **security-first** design principles.

The project simulates core banking operations including account management, deposits, withdrawals, inter-account transfers, loan management, interest accrual, and transaction history tracking — all with persistent file storage.

---

## 🚀 Features

### 🔐 User Account Management
- Secure user registration and login
- **Password hashing** with salted SHA-256 (no plaintext passwords)
- **Hidden password input** (masked with `*` in terminal)
- Role-based access control (User / Manager)

### 💰 Banking Operations
- Deposit and withdraw money
- **Inter-account fund transfers** with balance validation
- **Minimum balance enforcement** (₹1,000 for Savings, ₹5,000 for Current)
- **Overdraft facility** for Current accounts

### 🏦 Loan Management
- Apply for loans with EMI calculation
- Make loan installment payments
- Track loan details and remaining balance
- Manager can apply loans on behalf of users

### 📜 Transaction History
- Comprehensive transaction logging with timestamps
- Color-coded transaction display (green for credits, red for debits)
- **Export account statements** to formatted text files

### 💾 Full Data Persistence
- All data persists across program restarts:
  - `data/users.txt` — User credentials (hashed)
  - `data/accounts.txt` — Account details and balances
  - `data/loans.txt` — Loan records
  - `data/transactions.txt` — Complete transaction history

### 🎨 Terminal UI
- ANSI color output (Green/Red/Cyan/Yellow)
- Box-drawing characters for clean receipts and displays
- ASCII art branding

---

## 🛠️ Technologies Used

- **Language:** C++17
- **Paradigm:** Object-Oriented Programming with Inheritance & Polymorphism
- **Core Concepts:**
  - Abstract base classes and virtual methods
  - Smart pointers (`std::unique_ptr`)
  - Salted SHA-256 password hashing
  - POSIX `termios` for hidden input
  - Clean input validation (no `cin` desync)
  - Loop-based menus (no stack overflow from recursion)

---

## 📂 Project Structure

```
WiseVault/
├── include/
│   ├── Account.hpp          # Abstract Account + SavingsAccount + CurrentAccount
│   ├── BankManager.hpp      # Central banking operations & file persistence
│   ├── Loan.hpp             # Loan management with EMI calculation
│   ├── Menu.hpp             # UI menus (loop-based, no recursion)
│   ├── TransactionRecord.hpp # Transaction logging with timestamps
│   ├── User.hpp             # User auth with password hashing
│   └── Utils.hpp            # Colors, input helpers, SHA-256, box-drawing
├── src/
│   └── main.cpp             # Entry point
├── data/
│   ├── users.txt            # User credentials (auto-created)
│   ├── accounts.txt         # Account data (auto-created)
│   ├── loans.txt            # Loan records (auto-created)
│   └── transactions.txt     # Transaction log (auto-created)
├── CMakeLists.txt           # CMake build configuration
├── Makefile                 # GNU Make build configuration
└── README.md                # This file
```

---

## ▶️ How to Run

1. Clone the repository:
   ```bash
   git clone https://github.com/your-username/WiseVault.git
   ```

2. Navigate to the project directory:
   ```bash
   cd WiseVault
   ```

3. Build and run:
   ```bash
   # Using Make
   make run

   # Or manually
   g++ -std=c++17 -Wall -Wextra -Iinclude src/main.cpp -o WiseVault -lm
   ./WiseVault
   ```

### Default Manager Login
- **Username:** `Prithvi`
- **Password:** `admin123`

---

## 🏛️ Architecture Highlights

### Account Hierarchy (Polymorphism)
```
Account (abstract)
├── SavingsAccount
│   ├── Interest rate: 4% p.a.
│   ├── Min balance: ₹1,000
│   └── Interest accrual feature
└── CurrentAccount
    ├── Overdraft limit: ₹10,000
    ├── Min balance: ₹5,000
    └── No interest
```

### Security
- Passwords are salted and hashed with SHA-256 before storage
- Password input is masked in the terminal
- Role-based authorization prevents users from accessing manager functions

---

## 🎯 Use Case

This project is suitable for:
- Learning **Object-Oriented Programming in C++17**
- Understanding **inheritance, polymorphism, and design patterns**
- Demonstrating **file handling and data persistence**
- Academic projects requiring **security-aware design**
- Demonstrating backend logic and system design skills

---

## 👤 Author

**Prithvi Mujumdar**
Computer Science & Engineering Student

---

## 📜 License

This project is created for educational purposes.
