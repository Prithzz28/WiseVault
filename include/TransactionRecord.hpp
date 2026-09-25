#ifndef WISEVAULT_TRANSACTION_RECORD_HPP
#define WISEVAULT_TRANSACTION_RECORD_HPP

#include <string>
#include <ctime>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include "Utils.hpp"

class TransactionRecord {
private:
    int accountNumber;
    std::string type;
    double amount;
    time_t timestamp;

public:
    TransactionRecord() : accountNumber(0), amount(0.0), timestamp(time(nullptr)) {}

    TransactionRecord(int accNo, const std::string& type, double amt)
        : accountNumber(accNo), type(type), amount(amt), timestamp(time(nullptr)) {}

    TransactionRecord(int accNo, const std::string& type, double amt, time_t ts)
        : accountNumber(accNo), type(type), amount(amt), timestamp(ts) {}

    void show() const {
        std::string color = (type == "Deposit" || type == "Transfer In")
                            ? Color::GREEN : Color::RED;
        std::cout << Color::DIM << "  " << formatTimestamp(timestamp) << Color::RESET
                  << " │ " << color << std::setw(14) << std::left << type << Color::RESET
                  << " │ INR " << Color::WHITE << std::fixed << std::setprecision(2)
                  << std::setw(12) << std::right << amount << Color::RESET
                  << " │ Acc# " << accountNumber << "\n";
    }

    int getAccountNumber() const { return accountNumber; }
    std::string getType() const { return type; }
    double getAmount() const { return amount; }
    time_t getTimestamp() const { return timestamp; }

    // Serialize to file line: accNo|type|amount|timestamp
    std::string serialize() const {
        std::ostringstream oss;
        oss << accountNumber << "|" << type << "|"
            << std::fixed << std::setprecision(2) << amount << "|" << timestamp;
        return oss.str();
    }

    // Deserialize from file line
    static TransactionRecord deserialize(const std::string& line) {
        std::istringstream iss(line);
        std::string token;
        int accNo; std::string type; double amt; time_t ts;

        std::getline(iss, token, '|'); accNo = std::stoi(token);
        std::getline(iss, type, '|');
        std::getline(iss, token, '|'); amt = std::stod(token);
        std::getline(iss, token, '|'); ts = static_cast<time_t>(std::stoll(token));

        return TransactionRecord(accNo, type, amt, ts);
    }
};

#endif // WISEVAULT_TRANSACTION_RECORD_HPP
