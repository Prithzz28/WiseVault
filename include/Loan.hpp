#ifndef WISEVAULT_LOAN_HPP
#define WISEVAULT_LOAN_HPP

#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cmath>
#include "Utils.hpp"

class Loan {
private:
    int loanID;
    std::string borrowerName;
    std::string borrowerUsername;
    double principal;
    double rate;         // annual interest rate %
    int tenure;          // in months
    double emi;
    double balance;      // remaining balance

public:
    Loan() : loanID(0), principal(0), rate(0), tenure(0), emi(0), balance(0) {}

    Loan(int id, const std::string& name, const std::string& username,
         double p, int tenureYears, double r = 12.0)
        : loanID(id), borrowerName(name), borrowerUsername(username),
          principal(p), rate(r), tenure(tenureYears * 12)
    {
        double monthlyRate = (rate / 12.0) / 100.0;
        if (monthlyRate > 0) {
            emi = (principal * monthlyRate * pow(1 + monthlyRate, tenure))
                  / (pow(1 + monthlyRate, tenure) - 1);
        } else {
            emi = principal / tenure;
        }
        balance = emi * tenure;
    }

    // Constructor for deserialization (directly set all fields)
    Loan(int id, const std::string& name, const std::string& username,
         double p, double r, int tenureMonths, double emiVal, double bal)
        : loanID(id), borrowerName(name), borrowerUsername(username),
          principal(p), rate(r), tenure(tenureMonths), emi(emiVal), balance(bal) {}

    void showLoanDetails() const {
        Box::printHeader("Loan Details");
        Box::printRow("Loan ID        : ", std::to_string(loanID));
        Box::printRow("Borrower       : ", borrowerName);
        Box::printRow("Username       : ", borrowerUsername);

        std::ostringstream pOss;
        pOss << std::fixed << std::setprecision(2) << principal;
        Box::printRow("Principal      : ", "INR " + pOss.str());

        std::ostringstream rOss;
        rOss << std::fixed << std::setprecision(1) << rate << "%";
        Box::printRow("Interest Rate  : ", rOss.str());

        Box::printRow("Tenure         : ", std::to_string(tenure / 12) + " years ("
                      + std::to_string(tenure) + " months)");

        std::ostringstream eOss;
        eOss << std::fixed << std::setprecision(2) << emi;
        Box::printRow("Monthly EMI    : ", "INR " + eOss.str());

        std::ostringstream bOss;
        bOss << std::fixed << std::setprecision(2) << balance;
        Box::printRow("Balance Due    : ", "INR " + bOss.str());
        Box::printFooter();
    }

    int getLoanID() const { return loanID; }
    std::string getBorrowerUsername() const { return borrowerUsername; }
    std::string getBorrowerName() const { return borrowerName; }
    double getPrincipal() const { return principal; }
    double getRate() const { return rate; }
    int getTenure() const { return tenure; }
    double getEMI() const { return emi; }
    double getBalance() const { return balance; }

    bool makePayment(double amount) {
        if (amount <= 0) {
            printError("Payment amount must be positive.");
            return false;
        }
        if (balance <= 0) {
            printInfo("This loan has already been paid off.");
            return false;
        }
        if (amount >= balance) {
            printSuccess("Loan paid off successfully!");
            balance = 0;
        } else {
            balance -= amount;
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(2) << balance;
            printSuccess("Payment successful. Remaining balance: INR " + oss.str());
        }
        return true;
    }

    // Serialize: loanID|borrowerName|username|principal|rate|tenure|emi|balance
    std::string serialize() const {
        std::ostringstream oss;
        oss << loanID << "|" << borrowerName << "|" << borrowerUsername << "|"
            << std::fixed << std::setprecision(2) << principal << "|"
            << rate << "|" << tenure << "|" << emi << "|" << balance;
        return oss.str();
    }

    // Deserialize from pipe-separated line
    static Loan deserialize(const std::string& line) {
        std::istringstream iss(line);
        std::string token;
        int id; std::string name, uname; double p, r, emiVal, bal; int t;

        std::getline(iss, token, '|'); id = std::stoi(token);
        std::getline(iss, name, '|');
        std::getline(iss, uname, '|');
        std::getline(iss, token, '|'); p = std::stod(token);
        std::getline(iss, token, '|'); r = std::stod(token);
        std::getline(iss, token, '|'); t = std::stoi(token);
        std::getline(iss, token, '|'); emiVal = std::stod(token);
        std::getline(iss, token, '|'); bal = std::stod(token);

        return Loan(id, name, uname, p, r, t, emiVal, bal);
    }
};

#endif // WISEVAULT_LOAN_HPP
