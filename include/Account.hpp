#ifndef WISEVAULT_ACCOUNT_HPP
#define WISEVAULT_ACCOUNT_HPP

#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <memory>
#include <sstream>
#include <cmath>
#include "TransactionRecord.hpp"
#include "Utils.hpp"

// ─────────────────────────────────────────────
//  Abstract Base Account
// ─────────────────────────────────────────────
class Account {
protected:
    int accountNumber;
    std::string holderName;
    double balance;
    std::string ownerUsername;
    std::vector<TransactionRecord> transactionLog;

public:
    Account() : accountNumber(0), balance(0.0) {}

    Account(int accNo, const std::string& name, double bal, const std::string& owner)
        : accountNumber(accNo), holderName(name), balance(bal), ownerUsername(owner) {}

    virtual ~Account() = default;

    // Pure virtual methods for polymorphism
    virtual std::string getAccountType() const = 0;
    virtual double getMinBalance() const = 0;
    virtual double calculateInterest() const = 0;
    virtual void display() const = 0;
    virtual std::unique_ptr<Account> clone() const = 0;

    void addTransactionRecord(const TransactionRecord& record) {
        transactionLog.push_back(record);
    }

    void setTransactionLog(const std::vector<TransactionRecord>& log) {
        transactionLog = log;
    }

    bool deposit(double amount) {
        if (amount <= 0) {
            printError("Deposit amount must be positive.");
            return false;
        }
        balance += amount;
        printSuccess("Deposit successful! Current balance: INR "
                     + std::to_string(balance));
        return true;
    }

    virtual bool withdraw(double amount) {
        if (amount <= 0) {
            printError("Withdrawal amount must be positive.");
            return false;
        }
        if (balance - amount < getMinBalance()) {
            printError("Insufficient balance. Minimum balance of INR "
                       + std::to_string(static_cast<int>(getMinBalance()))
                       + " must be maintained.");
            return false;
        }
        balance -= amount;
        printSuccess("Withdrawal successful! Current balance: INR "
                     + std::to_string(balance));
        return true;
    }

    void showTransactionHistory() const {
        if (transactionLog.empty()) {
            printInfo("No transactions found for this account.");
            return;
        }
        Box::printHeader("Transaction History — Acc #" + std::to_string(accountNumber));
        std::cout << Color::DIM << "  Date & Time           │ Type           │ Amount        │ Account\n" << Color::RESET;
        Box::printSeparator();
        for (const auto& record : transactionLog) {
            record.show();
        }
        Box::printFooter();
    }

    // Export statement to file
    std::string exportStatement() const {
        std::ostringstream oss;
        oss << "===============================================\n";
        oss << "       WiseVault — Account Statement\n";
        oss << "===============================================\n";
        oss << "Account Number : " << accountNumber << "\n";
        oss << "Account Holder : " << holderName << "\n";
        oss << "Account Type   : " << getAccountType() << "\n";
        oss << std::fixed << std::setprecision(2);
        oss << "Current Balance: INR " << balance << "\n";
        oss << "Date Generated : " << formatTimestamp(time(nullptr)) << "\n";
        oss << "-----------------------------------------------\n";
        oss << "Transactions:\n";
        if (transactionLog.empty()) {
            oss << "  No transactions recorded.\n";
        } else {
            for (const auto& tr : transactionLog) {
                oss << "  " << formatTimestamp(tr.getTimestamp())
                    << " | " << tr.getType()
                    << " | INR " << tr.getAmount()
                    << "\n";
            }
        }
        oss << "===============================================\n";
        return oss.str();
    }

    // Getters
    int getAccountNumber() const { return accountNumber; }
    double getBalance() const { return balance; }
    std::string getOwnerUsername() const { return ownerUsername; }
    std::string getHolderName() const { return holderName; }
    const std::vector<TransactionRecord>& getTransactionLog() const { return transactionLog; }

    void modifyAccount(const std::string& newName) {
        holderName = newName;
        printSuccess("Account holder name updated.");
    }

    // Serialize to file line: accNo|holderName|balance|accType|ownerUsername
    std::string serialize() const {
        std::ostringstream oss;
        oss << accountNumber << "|" << holderName << "|"
            << std::fixed << std::setprecision(2) << balance << "|"
            << getAccountType() << "|" << ownerUsername;
        return oss.str();
    }
};

// ─────────────────────────────────────────────
//  Savings Account
// ─────────────────────────────────────────────
class SavingsAccount : public Account {
private:
    double interestRate; // annual %
    static constexpr double MIN_BALANCE = 1000.0;

public:
    SavingsAccount() : interestRate(4.0) {}

    SavingsAccount(int accNo, const std::string& name, double bal,
                   const std::string& owner, double rate = 4.0)
        : Account(accNo, name, bal, owner), interestRate(rate) {}

    std::string getAccountType() const override { return "Savings"; }
    double getMinBalance() const override { return MIN_BALANCE; }

    double calculateInterest() const override {
        return balance * (interestRate / 100.0) / 12.0; // monthly interest
    }

    void applyInterest() {
        double interest = calculateInterest();
        if (interest > 0) {
            balance += interest;
            addTransactionRecord(TransactionRecord(accountNumber, "Interest", interest));
        }
    }

    void display() const override {
        Box::printHeader("Savings Account");
        Box::printRow("Account No   : ", std::to_string(accountNumber));
        Box::printRow("Holder       : ", holderName);
        Box::printRow("Type         : ", "Savings");

        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << balance;
        Box::printRow("Balance      : ", "INR " + oss.str());

        std::ostringstream rateOss;
        rateOss << std::fixed << std::setprecision(1) << interestRate << "% p.a.";
        Box::printRow("Interest Rate: ", rateOss.str());

        Box::printRow("Min Balance  : ", "INR 1,000.00");
        Box::printRow("Owner        : ", ownerUsername);
        Box::printFooter();
    }

    std::unique_ptr<Account> clone() const override {
        return std::make_unique<SavingsAccount>(*this);
    }

    double getInterestRate() const { return interestRate; }
};

// ─────────────────────────────────────────────
//  Current Account
// ─────────────────────────────────────────────
class CurrentAccount : public Account {
private:
    double overdraftLimit;
    static constexpr double MIN_BALANCE = 5000.0;

public:
    CurrentAccount() : overdraftLimit(10000.0) {}

    CurrentAccount(int accNo, const std::string& name, double bal,
                   const std::string& owner, double odLimit = 10000.0)
        : Account(accNo, name, bal, owner), overdraftLimit(odLimit) {}

    std::string getAccountType() const override { return "Current"; }
    double getMinBalance() const override { return MIN_BALANCE; }
    double calculateInterest() const override { return 0.0; } // No interest on current

    bool withdraw(double amount) override {
        if (amount <= 0) {
            printError("Withdrawal amount must be positive.");
            return false;
        }
        double effectiveBalance = balance + overdraftLimit;
        if (amount > effectiveBalance) {
            printError("Exceeds available balance + overdraft limit (INR "
                       + std::to_string(static_cast<int>(overdraftLimit)) + ").");
            return false;
        }
        if (balance - amount < -overdraftLimit) {
            printError("Overdraft limit exceeded.");
            return false;
        }
        balance -= amount;
        if (balance < 0) {
            printWarning("Account is in overdraft. Balance: INR " + std::to_string(balance));
        } else {
            printSuccess("Withdrawal successful! Current balance: INR "
                         + std::to_string(balance));
        }
        return true;
    }

    void display() const override {
        Box::printHeader("Current Account");
        Box::printRow("Account No     : ", std::to_string(accountNumber));
        Box::printRow("Holder         : ", holderName);
        Box::printRow("Type           : ", "Current");

        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << balance;
        Box::printRow("Balance        : ", "INR " + oss.str());

        std::ostringstream odOss;
        odOss << std::fixed << std::setprecision(2) << overdraftLimit;
        Box::printRow("Overdraft Limit: ", "INR " + odOss.str());

        Box::printRow("Min Balance    : ", "INR 5,000.00");
        Box::printRow("Owner          : ", ownerUsername);
        Box::printFooter();
    }

    std::unique_ptr<Account> clone() const override {
        return std::make_unique<CurrentAccount>(*this);
    }

    double getOverdraftLimit() const { return overdraftLimit; }
};

// Factory function to create Account from serialized data
inline std::unique_ptr<Account> createAccountFromData(
    int accNo, const std::string& name, double bal,
    const std::string& type, const std::string& owner)
{
    if (type == "Savings" || type == "Saving") {
        return std::make_unique<SavingsAccount>(accNo, name, bal, owner);
    } else if (type == "Current") {
        return std::make_unique<CurrentAccount>(accNo, name, bal, owner);
    }
    // Default to Savings
    return std::make_unique<SavingsAccount>(accNo, name, bal, owner);
}

#endif // WISEVAULT_ACCOUNT_HPP
