#ifndef WISEVAULT_BANK_MANAGER_HPP
#define WISEVAULT_BANK_MANAGER_HPP

#include <vector>
#include <string>
#include <memory>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include "Account.hpp"
#include "Loan.hpp"
#include "TransactionRecord.hpp"
#include "Utils.hpp"

class BankManager {
private:
    std::vector<std::unique_ptr<Account>> accounts;
    std::vector<Loan> loans;
    std::vector<TransactionRecord> globalTransactions;
    int nextAccNo;
    int nextLoanID;

    const std::string ACCOUNTS_FILE = "data/accounts.txt";
    const std::string LOANS_FILE = "data/loans.txt";
    const std::string TRANSACTIONS_FILE = "data/transactions.txt";

public:
    BankManager() : nextAccNo(1001), nextLoanID(1) {
        loadAccounts();
        loadLoans();
        loadTransactions();
    }

    ~BankManager() {
        saveAll();
    }

    // ─── Persistence: Load ───────────────────────

    void loadAccounts() {
        std::ifstream inFile(ACCOUNTS_FILE);
        if (!inFile.is_open()) return;

        std::string line;
        while (std::getline(inFile, line)) {
            if (line.empty()) continue;
            try {
                std::istringstream iss(line);
                std::string token;
                int accNo; std::string name; double bal; std::string type; std::string owner;

                std::getline(iss, token, '|'); accNo = std::stoi(token);
                std::getline(iss, name, '|');
                std::getline(iss, token, '|'); bal = std::stod(token);
                std::getline(iss, type, '|');
                std::getline(iss, owner, '|');

                accounts.push_back(createAccountFromData(accNo, name, bal, type, owner));
                if (accNo >= nextAccNo) nextAccNo = accNo + 1;
            } catch (...) {
                // Skip malformed lines
            }
        }
    }

    void loadLoans() {
        std::ifstream inFile(LOANS_FILE);
        if (!inFile.is_open()) return;

        std::string line;
        while (std::getline(inFile, line)) {
            if (line.empty()) continue;
            try {
                Loan loan = Loan::deserialize(line);
                loans.push_back(loan);
                if (loan.getLoanID() >= nextLoanID) nextLoanID = loan.getLoanID() + 1;
            } catch (...) {
                // Skip malformed lines
            }
        }
    }

    void loadTransactions() {
        std::ifstream inFile(TRANSACTIONS_FILE);
        if (!inFile.is_open()) return;

        std::string line;
        while (std::getline(inFile, line)) {
            if (line.empty()) continue;
            try {
                TransactionRecord tr = TransactionRecord::deserialize(line);
                globalTransactions.push_back(tr);

                // Also attach to the corresponding account's local log
                for (auto& acc : accounts) {
                    if (acc->getAccountNumber() == tr.getAccountNumber()) {
                        acc->addTransactionRecord(tr);
                        break;
                    }
                }
            } catch (...) {
                // Skip malformed lines
            }
        }
    }

    // ─── Persistence: Save ───────────────────────

    void saveAll() {
        saveAccounts();
        saveLoans();
        saveTransactions();
    }

    void saveAccounts() {
        std::ofstream outFile(ACCOUNTS_FILE, std::ios::trunc);
        if (!outFile.is_open()) return;
        for (const auto& acc : accounts) {
            outFile << acc->serialize() << "\n";
        }
    }

    void saveLoans() {
        std::ofstream outFile(LOANS_FILE, std::ios::trunc);
        if (!outFile.is_open()) return;
        for (const auto& loan : loans) {
            outFile << loan.serialize() << "\n";
        }
    }

    void saveTransactions() {
        std::ofstream outFile(TRANSACTIONS_FILE, std::ios::trunc);
        if (!outFile.is_open()) return;
        for (const auto& tr : globalTransactions) {
            outFile << tr.serialize() << "\n";
        }
    }

    // ─── Account Operations ──────────────────────

    int createAccount(const std::string& name, double balance,
                       const std::string& type, const std::string& ownerUsername)
    {
        int accNo = nextAccNo++;
        accounts.push_back(createAccountFromData(accNo, name, balance, type, ownerUsername));
        printSuccess("Account created successfully! Account Number: " + std::to_string(accNo));
        saveAccounts();
        return accNo;
    }

    Account* findAccount(int accNo, const std::string& username = "", bool isManager = false) {
        for (auto& acc : accounts) {
            if (acc->getAccountNumber() == accNo) {
                if (isManager || acc->getOwnerUsername() == username)
                    return acc.get();
            }
        }
        return nullptr;
    }

    std::vector<Account*> getUserAccounts(const std::string& username) {
        std::vector<Account*> result;
        for (auto& acc : accounts) {
            if (acc->getOwnerUsername() == username)
                result.push_back(acc.get());
        }
        return result;
    }

    void closeAccount(int accNo, const std::string& username = "", bool isManager = false) {
        for (auto it = accounts.begin(); it != accounts.end(); ++it) {
            if ((*it)->getAccountNumber() == accNo &&
                (isManager || (*it)->getOwnerUsername() == username))
            {
                accounts.erase(it);
                printSuccess("Account #" + std::to_string(accNo) + " closed successfully.");
                saveAccounts();
                return;
            }
        }
        printError("Account not found or permission denied.");
    }

    void showAllAccounts() {
        if (accounts.empty()) {
            printInfo("No accounts available.");
            return;
        }
        for (const auto& acc : accounts) {
            acc->display();
        }
    }

    // ─── Transaction Operations ──────────────────

    bool doDeposit(Account* acc, double amount) {
        if (acc->deposit(amount)) {
            TransactionRecord tr(acc->getAccountNumber(), "Deposit", amount);
            acc->addTransactionRecord(tr);
            globalTransactions.push_back(tr);
            saveAccounts();
            saveTransactions();
            return true;
        }
        return false;
    }

    bool doWithdraw(Account* acc, double amount) {
        if (acc->withdraw(amount)) {
            TransactionRecord tr(acc->getAccountNumber(), "Withdraw", amount);
            acc->addTransactionRecord(tr);
            globalTransactions.push_back(tr);
            saveAccounts();
            saveTransactions();
            return true;
        }
        return false;
    }

    // Inter-account fund transfer
    bool transfer(int fromAccNo, int toAccNo, double amount,
                  const std::string& username, bool isManager = false)
    {
        Account* fromAcc = findAccount(fromAccNo, username, isManager);
        Account* toAcc = findAccount(toAccNo, "", true); // any account can receive

        if (!fromAcc) {
            printError("Source account not found or permission denied.");
            return false;
        }
        if (!toAcc) {
            printError("Destination account #" + std::to_string(toAccNo) + " not found.");
            return false;
        }
        if (fromAccNo == toAccNo) {
            printError("Cannot transfer to the same account.");
            return false;
        }
        if (amount <= 0) {
            printError("Transfer amount must be positive.");
            return false;
        }

        // Check sufficient balance (considering min balance)
        if (fromAcc->getBalance() - amount < fromAcc->getMinBalance()) {
            printError("Insufficient balance. Minimum balance of INR "
                       + std::to_string(static_cast<int>(fromAcc->getMinBalance()))
                       + " must be maintained.");
            return false;
        }

        // Atomic-like: debit first, then credit
        // Use direct balance manipulation to avoid double messages
        if (!fromAcc->withdraw(amount)) return false;
        fromAcc->addTransactionRecord(TransactionRecord(fromAccNo, "Transfer Out", amount));
        globalTransactions.push_back(TransactionRecord(fromAccNo, "Transfer Out", amount));

        toAcc->deposit(amount);
        toAcc->addTransactionRecord(TransactionRecord(toAccNo, "Transfer In", amount));
        globalTransactions.push_back(TransactionRecord(toAccNo, "Transfer In", amount));

        printSuccess("Transferred INR " + std::to_string(amount) + " from Acc#"
                     + std::to_string(fromAccNo) + " to Acc#" + std::to_string(toAccNo));

        saveAccounts();
        saveTransactions();
        return true;
    }

    // ─── Loan Operations ─────────────────────────

    int applyLoan(const std::string& name, const std::string& username,
                   double principal, int tenureYears)
    {
        int id = nextLoanID++;
        loans.push_back(Loan(id, name, username, principal, tenureYears));
        printSuccess("Loan application successful! Loan ID: " + std::to_string(id));
        saveLoans();
        return id;
    }

    Loan* findLoan(int loanID, const std::string& username = "", bool isManager = false) {
        for (auto& loan : loans) {
            if (loan.getLoanID() == loanID) {
                if (isManager || loan.getBorrowerUsername() == username)
                    return &loan;
            }
        }
        return nullptr;
    }

    std::vector<Loan*> getUserLoans(const std::string& username) {
        std::vector<Loan*> result;
        for (auto& loan : loans) {
            if (loan.getBorrowerUsername() == username)
                result.push_back(&loan);
        }
        return result;
    }

    void showAllLoans() {
        if (loans.empty()) {
            printInfo("No loans available.");
            return;
        }
        for (const auto& loan : loans) {
            loan.showLoanDetails();
        }
    }

    bool makeLoanPayment(int loanID, double amount, const std::string& username, bool isManager) {
        Loan* loan = findLoan(loanID, username, isManager);
        if (!loan) {
            printError("Loan not found or permission denied.");
            return false;
        }
        if (loan->makePayment(amount)) {
            // Log as transaction on first account of the loan holder
            auto userAccs = getUserAccounts(loan->getBorrowerUsername());
            if (!userAccs.empty()) {
                TransactionRecord tr(userAccs[0]->getAccountNumber(), "Loan Payment", amount);
                userAccs[0]->addTransactionRecord(tr);
                globalTransactions.push_back(tr);
                printInfo("Loan payment recorded in transaction history of Account #"
                         + std::to_string(userAccs[0]->getAccountNumber()));
            }
            saveLoans();
            saveTransactions();
            return true;
        }
        return false;
    }

    // ─── Interest Accrual ────────────────────────

    void applyInterestToSavings() {
        int count = 0;
        for (auto& acc : accounts) {
            SavingsAccount* sa = dynamic_cast<SavingsAccount*>(acc.get());
            if (sa) {
                sa->applyInterest();
                count++;
            }
        }
        if (count > 0) {
            printSuccess("Interest applied to " + std::to_string(count) + " savings account(s).");
            saveAccounts();
        } else {
            printInfo("No savings accounts found.");
        }
    }

    // ─── Global Transaction View ─────────────────

    void showAllTransactions() {
        if (globalTransactions.empty()) {
            printInfo("No transactions have been recorded.");
            return;
        }
        Box::printHeader("All Transactions");
        std::cout << Color::DIM << "  Date & Time           │ Type           │ Amount        │ Account\n" << Color::RESET;
        Box::printSeparator();
        for (const auto& tr : globalTransactions) {
            tr.show();
        }
        Box::printFooter();
    }

    void recordGlobalTransaction(const TransactionRecord& record) {
        globalTransactions.push_back(record);
    }
};

#endif // WISEVAULT_BANK_MANAGER_HPP
