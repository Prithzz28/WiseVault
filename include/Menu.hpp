#ifndef WISEVAULT_MENU_HPP
#define WISEVAULT_MENU_HPP

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <algorithm>
#include "User.hpp"
#include "BankManager.hpp"
#include "Utils.hpp"

class Menu {
private:
    std::vector<User> users;
    BankManager bankManager;
    std::string loggedInUsername; // FIX: store username string, not raw pointer

    const std::string USERS_FILE = "data/users.txt";

    // ─── User Persistence ────────────────────────

    void loadUsers() {
        std::ifstream inFile(USERS_FILE);
        if (!inFile.is_open()) return;

        std::string line;
        while (std::getline(inFile, line)) {
            if (line.empty()) continue;
            try {
                users.push_back(User::deserialize(line));
            } catch (...) {
                // Skip malformed lines
            }
        }
    }

    void saveUsers() {
        std::ofstream outFile(USERS_FILE, std::ios::trunc);
        if (!outFile.is_open()) {
            printError("Error saving user data.");
            return;
        }
        for (const auto& user : users) {
            outFile << user.serialize() << "\n";
        }
    }

    void seedDefaultManager() {
        // Only seed if no manager exists
        bool hasManager = false;
        for (const auto& u : users) {
            if (u.isManager()) { hasManager = true; break; }
        }
        if (!hasManager) {
            users.push_back(User("Prithvi", "admin123", "manager"));
            saveUsers();
            printInfo("Default manager account created (Prithvi / admin123).");
        }
    }

    // ─── Lookup Helpers ──────────────────────────

    User* findUser(const std::string& username) {
        for (auto& u : users) {
            if (u.getUsername() == username) return &u;
        }
        return nullptr;
    }

    std::string getLoggedInRole() const {
        for (const auto& u : users) {
            if (u.getUsername() == loggedInUsername) return u.getRole();
        }
        return "";
    }

    bool isLoggedInManager() const {
        return getLoggedInRole() == "manager";
    }

    // ─── Auth Flows ──────────────────────────────

    bool login() {
        std::cout << "\n";
        Box::printHeader("Login");

        std::string uname = Input::readWord("  Username: ");
        std::string pwd = readPassword("  Password: ");

        for (auto& user : users) {
            if (user.getUsername() == uname && user.verifyPassword(pwd)) {
                loggedInUsername = uname;
                std::cout << "\n";
                printSuccess("Login successful! Welcome, " + uname
                             + " (" + user.getRole() + ")");
                return true;
            }
        }
        printError("Invalid credentials.");
        return false;
    }

    bool registerNewUser() {
        std::cout << "\n";
        Box::printHeader("New User Registration");

        std::string username = Input::readWord("  Username: ");

        // Check if username already exists
        if (findUser(username)) {
            printError("Username already exists. Try a different one.");
            return false;
        }

        std::string password = readPassword("  Password: ");
        if (password.empty()) {
            printError("Password cannot be empty.");
            return false;
        }

        users.push_back(User(username, password, "user"));
        saveUsers();
        loggedInUsername = username;
        printSuccess("Registration successful! You are now logged in as " + username + ".");
        return true;
    }

    // ─── User Menu (loop-based, no recursion) ────

    void userMenu() {
        bool running = true;
        while (running) {
            std::cout << "\n";
            Box::printHeader("User Menu (" + loggedInUsername + ")");
            std::cout << Color::CYAN << "│" << Color::RESET
                      << "  1. View My Accounts       6. View My Loans      " << Color::CYAN << "│\n" << Color::RESET;
            std::cout << Color::CYAN << "│" << Color::RESET
                      << "  2. Modify My Account      7. Make Loan Payment   " << Color::CYAN << "│\n" << Color::RESET;
            std::cout << Color::CYAN << "│" << Color::RESET
                      << "  3. Deposit                8. View Transactions   " << Color::CYAN << "│\n" << Color::RESET;
            std::cout << Color::CYAN << "│" << Color::RESET
                      << "  4. Withdraw               9. Export Statement    " << Color::CYAN << "│\n" << Color::RESET;
            std::cout << Color::CYAN << "│" << Color::RESET
                      << "  5. Apply for Loan        10. Fund Transfer       " << Color::CYAN << "│\n" << Color::RESET;
            std::cout << Color::CYAN << "│" << Color::RESET
                      << "  0. Logout                                        " << Color::CYAN << "│\n" << Color::RESET;
            Box::printFooter();

            int choice = Input::readInt("  Enter choice: ");

            switch (choice) {
            case 1: viewMyAccounts(); break;
            case 2: modifyAccount(false); break;
            case 3: depositAmount(false); break;
            case 4: withdrawAmount(false); break;
            case 5: applyLoan(false); break;
            case 6: viewMyLoans(); break;
            case 7: makeLoanPayment(false); break;
            case 8: viewTransactions(false); break;
            case 9: exportStatement(); break;
            case 10: fundTransfer(false); break;
            case 0:
                printInfo("Logged out. Team Polymorphs wishes you a great day ahead!");
                loggedInUsername.clear();
                running = false;
                break;
            default:
                printError("Invalid choice.");
            }
        }
    }

    // ─── Manager Menu (loop-based, no recursion) ─

    void managerMenu() {
        // Role-based authorization enforcement
        if (!isLoggedInManager()) {
            printError("Access denied. Manager privileges required.");
            return;
        }

        bool running = true;
        while (running) {
            std::cout << "\n";
            Box::printHeader("Manager Menu (" + loggedInUsername + ")");
            std::cout << Color::CYAN << "│" << Color::RESET
                      << "  1. Create Account          7.  View All Transactions " << Color::CYAN << "│\n" << Color::RESET;
            std::cout << Color::CYAN << "│" << Color::RESET
                      << "  2. Show All Accounts       8.  Apply Loan for User   " << Color::CYAN << "│\n" << Color::RESET;
            std::cout << Color::CYAN << "│" << Color::RESET
                      << "  3. Close Account           9.  Show All Loans        " << Color::CYAN << "│\n" << Color::RESET;
            std::cout << Color::CYAN << "│" << Color::RESET
                      << "  4. Deposit (any account)   10. Make Loan Payment     " << Color::CYAN << "│\n" << Color::RESET;
            std::cout << Color::CYAN << "│" << Color::RESET
                      << "  5. Withdraw (any account)  11. Apply Interest        " << Color::CYAN << "│\n" << Color::RESET;
            std::cout << Color::CYAN << "│" << Color::RESET
                      << "  6. Fund Transfer           12. Add User Account      " << Color::CYAN << "│\n" << Color::RESET;
            std::cout << Color::CYAN << "│" << Color::RESET
                      << "  0. Logout                                            " << Color::CYAN << "│\n" << Color::RESET;
            Box::printFooter();

            int choice = Input::readInt("  Enter choice: ");

            switch (choice) {
            case 1:  createAccount(); break;
            case 2:  bankManager.showAllAccounts(); break;
            case 3:  closeAccount(true); break;
            case 4:  depositAmount(true); break;
            case 5:  withdrawAmount(true); break;
            case 6:  fundTransfer(true); break;
            case 7:  bankManager.showAllTransactions(); break;
            case 8:  applyLoanForUser(); break;   // FIX: asks for target username
            case 9:  bankManager.showAllLoans(); break;
            case 10: makeLoanPayment(true); break;
            case 11: bankManager.applyInterestToSavings(); break;
            case 12: addUserAccount(); break;
            case 0:
                printInfo("Logged out.");
                loggedInUsername.clear();
                running = false;
                break;
            default:
                printError("Invalid choice.");
            }
        }
    }

    // ─── Shared Operations ───────────────────────

    void viewMyAccounts() {
        auto accs = bankManager.getUserAccounts(loggedInUsername);
        if (accs.empty()) {
            printInfo("You have no accounts.");
            return;
        }
        for (auto* acc : accs) {
            acc->display();
        }
    }

    void viewMyLoans() {
        auto loans = bankManager.getUserLoans(loggedInUsername);
        if (loans.empty()) {
            printInfo("You have no loans.");
            return;
        }
        for (auto* loan : loans) {
            loan->showLoanDetails();
        }
    }

    void createAccount() {
        std::cout << "\n";
        Box::printHeader("Create New Account");
        std::string name = Input::readLine("  Account holder name: ");
        double bal = Input::readDouble("  Initial deposit (INR): ");
        std::string type = Input::readLine("  Account type (Savings/Current): ");
        std::string ownerUsername = Input::readLine("  Owner username: ");

        // Validate account type
        if (type != "Savings" && type != "Saving" && type != "Current") {
            printError("Invalid account type. Use 'Savings' or 'Current'.");
            return;
        }

        // Validate minimum initial deposit
        double minBal = (type == "Current") ? 5000.0 : 1000.0;
        if (bal < minBal) {
            printError("Initial deposit must be at least INR " + std::to_string(static_cast<int>(minBal))
                       + " for " + type + " accounts.");
            return;
        }

        bankManager.createAccount(name, bal, type, ownerUsername);
    }

    void addUserAccount() {
        // Manager creates a user + account together
        std::cout << "\n";
        Box::printHeader("Add User & Account");
        std::string uname = Input::readWord("  New username: ");

        if (findUser(uname)) {
            printInfo("User already exists. Creating account for existing user.");
        } else {
            std::string pwd = readPassword("  Password for new user: ");
            users.push_back(User(uname, pwd, "user"));
            saveUsers();
            printSuccess("User '" + uname + "' created.");
        }

        std::string holderName = Input::readLine("  Account holder name: ");
        double bal = Input::readDouble("  Initial deposit (INR): ");
        std::string type = Input::readLine("  Account type (Savings/Current): ");

        if (type != "Savings" && type != "Saving" && type != "Current") {
            printError("Invalid account type.");
            return;
        }

        double minBal = (type == "Current") ? 5000.0 : 1000.0;
        if (bal < minBal) {
            printError("Initial deposit must be at least INR " + std::to_string(static_cast<int>(minBal)));
            return;
        }

        bankManager.createAccount(holderName, bal, type, uname);
    }

    void modifyAccount(bool isManager) {
        int accNo = Input::readInt("  Enter account number: ");
        Account* acc = bankManager.findAccount(accNo, loggedInUsername, isManager);
        if (acc) {
            std::string newName = Input::readLine("  Enter new holder name: ");
            acc->modifyAccount(newName);
            bankManager.saveAccounts();
        } else {
            printError("Account not found or permission denied.");
        }
    }

    void depositAmount(bool isManager) {
        int accNo = Input::readInt("  Enter account number: ");
        double amount = Input::readDouble("  Enter deposit amount (INR): ");
        Account* acc = bankManager.findAccount(accNo, loggedInUsername, isManager);
        if (acc) {
            bankManager.doDeposit(acc, amount);
        } else {
            printError("Account not found or permission denied.");
        }
    }

    void withdrawAmount(bool isManager) {
        int accNo = Input::readInt("  Enter account number: ");
        double amount = Input::readDouble("  Enter withdrawal amount (INR): ");
        Account* acc = bankManager.findAccount(accNo, loggedInUsername, isManager);
        if (acc) {
            bankManager.doWithdraw(acc, amount);
        } else {
            printError("Account not found or permission denied.");
        }
    }

    void fundTransfer(bool isManager) {
        std::cout << "\n";
        Box::printHeader("Fund Transfer");
        int fromAcc = Input::readInt("  From account number: ");
        int toAcc = Input::readInt("  To account number: ");
        double amount = Input::readDouble("  Transfer amount (INR): ");
        bankManager.transfer(fromAcc, toAcc, amount, loggedInUsername, isManager);
    }

    void applyLoan(bool /*isManager*/) {
        std::cout << "\n";
        Box::printHeader("Loan Application");
        std::string name = Input::readLine("  Borrower name: ");
        double principal = Input::readDouble("  Loan principal (INR): ");
        int tenure = Input::readInt("  Tenure (years): ");
        printInfo("Default interest rate: 12% p.a.");
        bankManager.applyLoan(name, loggedInUsername, principal, tenure);
    }

    // FIX: Manager applying loan for a user — asks for the target username
    void applyLoanForUser() {
        std::cout << "\n";
        Box::printHeader("Apply Loan for User");
        std::string targetUsername = Input::readWord("  Enter customer username: ");

        // Verify user exists
        if (!findUser(targetUsername)) {
            printError("User '" + targetUsername + "' not found.");
            return;
        }

        std::string name = Input::readLine("  Borrower name: ");
        double principal = Input::readDouble("  Loan principal (INR): ");
        int tenure = Input::readInt("  Tenure (years): ");
        printInfo("Default interest rate: 12% p.a.");
        bankManager.applyLoan(name, targetUsername, principal, tenure);
    }

    void makeLoanPayment(bool isManager) {
        int loanID = Input::readInt("  Enter Loan ID: ");
        double amount = Input::readDouble("  Enter payment amount (INR): ");
        bankManager.makeLoanPayment(loanID, amount, loggedInUsername, isManager);
    }

    void viewTransactions(bool isManager) {
        int accNo = Input::readInt("  Enter account number: ");
        Account* acc = bankManager.findAccount(accNo, loggedInUsername, isManager);
        if (acc) {
            acc->showTransactionHistory();
        } else {
            printError("Account not found or permission denied.");
        }
    }

    void exportStatement() {
        int accNo = Input::readInt("  Enter account number: ");
        Account* acc = bankManager.findAccount(accNo, loggedInUsername, false);
        if (!acc) {
            printError("Account not found or permission denied.");
            return;
        }

        std::string filename = "data/statement_" + std::to_string(accNo) + ".txt";
        std::ofstream outFile(filename);
        if (outFile.is_open()) {
            outFile << acc->exportStatement();
            outFile.close();
            printSuccess("Statement exported to " + filename);
        } else {
            printError("Failed to create statement file.");
        }
    }

    void closeAccount(bool isManager) {
        int accNo = Input::readInt("  Enter account number to close: ");
        bankManager.closeAccount(accNo, loggedInUsername, isManager);
    }

public:
    Menu() {
        loadUsers();
        seedDefaultManager(); // FIX: Only seeds if no manager exists
    }

    // ─── Main Loop (no recursion) ────────────────

    void run() {
        bool running = true;
        while (running) {
            std::cout << "\n";
            std::cout << Color::BOLD << Color::CYAN;
            std::cout << "  ╦ ╦┬┌─┐┌─┐╦  ╦┌─┐┬ ┬┬ ┌┬┐\n";
            std::cout << "  ║║║│└─┐├┤ ╚╗╔╝├─┤│ ││  │ \n";
            std::cout << "  ╚╩╝┴└─┘└─┘ ╚╝ ┴ ┴└─┘┴─┘┴ \n";
            std::cout << Color::RESET;
            std::cout << Color::DIM << "  Your Trusted Banking Partner\n" << Color::RESET;

            Box::printHeader("Main Menu");
            std::cout << Color::CYAN << "│" << Color::RESET
                      << "  1. Login                                         " << Color::CYAN << "│\n" << Color::RESET;
            std::cout << Color::CYAN << "│" << Color::RESET
                      << "  2. Register                                      " << Color::CYAN << "│\n" << Color::RESET;
            std::cout << Color::CYAN << "│" << Color::RESET
                      << "  0. Exit                                          " << Color::CYAN << "│\n" << Color::RESET;
            Box::printFooter();

            int choice = Input::readInt("  Enter your choice: ");

            switch (choice) {
            case 1:
                if (login()) {
                    if (isLoggedInManager())
                        managerMenu();
                    else
                        userMenu();
                }
                break;
            case 2:
                if (registerNewUser()) {
                    userMenu(); // Auto-login after registration
                }
                break;
            case 0:
                std::cout << "\n";
                printInfo("Thank you for using WiseVault. Goodbye!");
                std::cout << Color::DIM << "  All data has been saved.\n" << Color::RESET;
                running = false;
                break;
            default:
                printError("Invalid choice.");
            }
        }
    }
};

#endif // WISEVAULT_MENU_HPP
