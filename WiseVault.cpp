#include <iostream>      // For cin, cout, endl
#include <string>        // For string class
#include <vector>        // For std::vector
#include <fstream>       // For file operations (if used)
#include <sstream>       // For stringstream (optional, for parsing)
#include <ctime>         // For timestamps (if used in transactions)
#include <iomanip>       // For formatting output (like currency, dates)
#include <cmath>         // For mathematical calculations (like pow function)
using namespace std;

// ========================
// TransactionRecord Class
// ========================
class TransactionRecord {
    int accountNumber;
    string type;
    double amount;
    time_t timestamp;

public:
    TransactionRecord(int accNo, const string &type, double amt)
        : accountNumber(accNo), type(type), amount(amt), timestamp(time(nullptr)) {}

    void show() const {
        cout << "\nAccount: " << accountNumber
             << ", Type: " << type
             << ", Amount: INR " << fixed << setprecision(2) << amount
             << ", Date: " << ctime(&timestamp);
    }
};

// ========================
// User Class
// ========================
class User {
public:
    string username;
    string password;
    string role; // "manager" or "user"

    User() {}
    User(string uname, string pwd, string r) : username(uname), password(pwd), role(r) {}
};

// ========================
// Account Class
// ========================
class Account {
private:
    int accountNumber;
    string name;
    double balance;
    string accountType;
    string ownerUsername;
    vector<TransactionRecord> transactionLog;

public:
    Account() {}

    Account(int accNo, const string &accName, double bal, const string &type, const string &owner)
        : accountNumber(accNo), name(accName), balance(bal), accountType(type), ownerUsername(owner) {}

    void addTransactionRecord(const TransactionRecord &record) {
        transactionLog.push_back(record);
    }

    void showAccount() const {
        cout << "\nAccount Number: " << accountNumber;
        cout << "\nAccount Holder: " << name;
        cout << "\nAccount Type  : " << accountType;
        cout << fixed << setprecision(2);
        cout << "\nBalance       : INR " << balance << endl;
    }

    void deposit(double amount) {
        balance += amount;
        cout << "Deposit successful! Current balance: INR " << balance << endl;
    }

    void withdraw(double amount) {
        if (amount > balance) {
            cout << "Insufficient balance.\n";
        } else {
            balance -= amount;
            cout << "Withdrawal successful! Current balance: INR " << balance << endl;
        }
    }

    void showTransactionHistory() const {
        if (transactionLog.empty()) {
            cout << "No transactions found for this account.\n";
            return;
        }

        cout << "\nTransaction History for Account " << accountNumber << ":\n";
        for (const auto &record : transactionLog)
            record.show();
    }

    int getAccountNumber() const { return accountNumber; }
    double getBalance() const { return balance; }
    string getOwnerUsername() const { return ownerUsername; }

    void modifyAccount(const string &newName, const string &newType) {
        name = newName;
        accountType = newType;
        cout << "Account modified successfully.\n";
    }
};

// =========================
// Forward declaration for Manager
// =========================
class Manager;

// =========================
// Transaction Class
// =========================
class Transaction {
public:
    void deposit(Account &acc, double amount);
    void withdraw(Account &acc, double amount);
    void deposit(Account &acc, double amount, Manager &manager);
    void withdraw(Account &acc, double amount, Manager &manager);
};

void Transaction::deposit(Account &acc, double amount) {
    acc.deposit(amount);
    acc.addTransactionRecord(TransactionRecord(acc.getAccountNumber(), "Deposit", amount));
}

void Transaction::withdraw(Account &acc, double amount) {
    if (acc.getBalance() >= amount) {
        acc.withdraw(amount);
        acc.addTransactionRecord(TransactionRecord(acc.getAccountNumber(), "Withdraw", amount));
    } else {
        cout << "Withdrawal failed: insufficient balance.\n";
    }
}

void Transaction::deposit(Account &acc, double amount, Manager &manager) {
    deposit(acc, amount); // No special logic for manager in current version
}

void Transaction::withdraw(Account &acc, double amount, Manager &manager) {
    withdraw(acc, amount); // No special logic for manager in current version
}

// =========================
// Loan Class
// =========================
class Loan {
private:
    int loanID;
    string borrowerName;
    string borrowerUsername;
    double principal;
    double rate;
    int tenure; // in months
    double emi;
    double balance;

public:
    Loan() {}

    Loan(int id, const string &name, const string &username, double p, int t, double r = 12.0)
        : loanID(id), borrowerName(name), borrowerUsername(username), principal(p), rate(r), tenure(t * 12) {

        double monthlyRate = (rate / 12) / 100;
        emi = (principal * monthlyRate * pow(1 + monthlyRate, tenure)) / (pow(1 + monthlyRate, tenure) - 1);
        balance = emi * tenure;
    }

    void showLoanDetails() const {
        cout << "\nLoan ID          : " << loanID;
        cout << "\nBorrower Name    : " << borrowerName;
        cout << "\nPrincipal Amount : INR " << principal;
        cout << "\nInterest Rate    : " << rate << "%";
        cout << "\nTenure           : " << tenure / 12 << " years (" << tenure << " months)";
        cout << "\nMonthly EMI      : INR " << emi;
        cout << "\nTotal Payable    : INR " << balance << endl;
    }

    int getLoanID() const { return loanID; }
    string getBorrowerUsername() const { return borrowerUsername; }

    void makePayment(double amount) {
        if (amount >= balance) {
            cout << "Loan paid off successfully!\n";
            balance = 0;
        } else {
            balance -= amount;
            cout << "Payment successful. Remaining Balance: INR " << balance << endl;
        }
    }
};

// ============================
// Manager Class
// ============================
class Manager
{
private:
    vector<Account> accounts;
    vector<Loan> loans;
    int nextAccNo = 1001;
    int nextLoanID = 1;
    vector<TransactionRecord> transactions;
    void recordTransaction(int accNo, const string &type, double amount)
    {
        transactions.push_back(TransactionRecord(accNo, type, amount));
    }

public:
    void createAccount(const string &name, double balance, const string &type, const string &ownerUsername)
    {
        accounts.push_back(Account(nextAccNo, name, balance, type, ownerUsername));
        cout << "Account created successfully! Account Number: " << nextAccNo++ << endl;
    }

    vector<Account> getUserAccounts(string username)
    {
        vector<Account> result;
        for (auto &acc : accounts)
        {
            if (acc.getOwnerUsername() == username)
                result.push_back(acc);
        }
        return result;
    }

    Account *findAccount(int accNo, string username = "", bool isManager = false)
    {
        for (auto &acc : accounts)
        {
            if (acc.getAccountNumber() == accNo)
            {
                if (isManager || acc.getOwnerUsername() == username)
                    return &acc;
            }
        }
        return nullptr;
    }

    void closeAccount(int accNo, string username = "", bool isManager = false)
    {
        for (auto it = accounts.begin(); it != accounts.end(); ++it)
        {
            if (it->getAccountNumber() == accNo && (isManager || it->getOwnerUsername() == username))
            {
                accounts.erase(it);
                cout << "Account closed successfully.\n";
                return;
            }
        }
        cout << "Account not found or permission denied.\n";
    }

    void showAllAccounts()
    {
        if (accounts.empty())
        {
            cout << "No accounts available.\n";
            return;
        }
        for (auto &acc : accounts)
        {
            acc.showAccount();
            cout << "----------------------------\n";
        }
    }

    void applyLoan(const string &name, const string &username, double principal, int tenure)
    {
        loans.push_back(Loan(nextLoanID, name, username, principal, tenure));
        cout << "Loan application successful! Loan ID: " << nextLoanID++ << endl;
    }

    vector<Loan> getUserLoans(string username)
    {
        vector<Loan> result;
        for (auto &loan : loans)
        {
            if (loan.getBorrowerUsername() == username)
                result.push_back(loan);
        }
        return result;
    }

    Loan *findLoan(int loanID, string username = "", bool isManager = false)
    {
        for (auto &loan : loans)
        {
            if (loan.getLoanID() == loanID)
            {
                if (isManager || loan.getBorrowerUsername() == username)
                    return &loan;
            }
        }
        return nullptr;
    }

    void showAllLoans()
    {
        if (loans.empty())
        {
            cout << "No loans available.\n";
            return;
        }
        for (auto &loan : loans)
        {
            loan.showLoanDetails();
            cout << "----------------------------\n";
        }
    }

    void recordGlobalTransaction(const TransactionRecord &record)
    {
        transactions.push_back(record);
    }

    // Optional: show all global transactions
    void showAllTransactions()
    {
        if (transactions.empty())
        {
            cout << "No transactions have been recorded.\n";
            return;
        }
        for (const auto &tr : transactions)
            tr.show();
    }

};

// ==============================
// UserInteraction Class
// ==============================
class UserInteraction
{
private:
    vector<User> users;
    Manager manager;
    Transaction transaction;
    User *loggedInUser = nullptr;

public:
    UserInteraction()
    {
        // load users from file
        ifstream inFile("users.txt");
        string uname, pwd, role;
        while (inFile >> uname >> pwd >> role)
        {
            users.push_back(User(uname, pwd, role));
        }
        ofstream outFile("users.txt", ios::app);
        if (users.empty()) 
        {
            users.push_back(User("Prithvi", "admin123", "manager"));
            outFile << "Prithvi admin123 manager\n";
        }
        outFile.close();

        // Add default users for testing
        users.push_back(User("Prithvi", "admin123", "manager"));
        users.push_back(User("Atharv", "user123", "user"));
    }


    void viewTransactions(bool isManager)
    {
        int accNo;
        cout << "Enter account number: ";
        cin >> accNo;
        Account *acc = manager.findAccount(accNo, loggedInUser->username, isManager);
        if (acc)
        {
            acc->showTransactionHistory();
        }
        else
        {
            cout << "Account not found or permission denied.\n";
        }
    }

    void login()
    {
        string uname, pwd;
        cout << "Username: ";
        cin >> uname;
        cout << "Password: ";
        cin >> pwd;

        for (auto &user : users)
        {
            if (user.username == uname && user.password == pwd)
            {
                loggedInUser = &user;
                cout << "Login successful! Welcome, " << user.username << " (" << user.role << ")\n";
                return;
            }
        }
        cout << "Invalid credentials. Try again.\n";
        login(); // Retry login
    }

    void start() {
        int choice;
        cout << "\n==== Welcome to WiseVault ====\n";
        cout << "1. Login\n";
        cout << "2. Register\n";
        cout << "Enter your choice: ";
        cin >> choice;
    
        switch (choice) {
            case 1:
                login();
                break;
            case 2:
                registerNewUser();
                start(); // Restart the menu after registration
                return;
            default:
                cout << "Invalid choice. Try again.\n";
                start();
                return;
        }
    
        if (loggedInUser->role == "manager")
            managerMenu();
        else
            userMenu();
    }
    

    // ========== USER MENU ==========
    void userMenu()
    {
        int choice;
        do
        {
            cout << "\n==== User Menu (" << loggedInUser->username << ") ====\n";
            cout << "1. View My Accounts\n";
            cout << "2. Modify My Account\n";
            cout << "3. Deposit\n";
            cout << "4. Withdraw\n";
            cout << "5. Apply for Loan\n";
            cout << "6. View My Loans\n";
            cout << "7. Make Loan Payment\n";
            cout << "8. View Transaction History\n";
            cout << "9. Logout\n";

            cout << "Enter choice: ";
            cin >> choice;

            switch (choice)
            {
            case 1:
                for (auto &acc : manager.getUserAccounts(loggedInUser->username))
                    acc.showAccount();
                break;
            case 2:
                modifyAccount(false);
                break;
            case 3:
                depositAmount(false);
                break;
            case 4:
                withdrawAmount(false);
                break;
            case 5:
                applyLoan();
                break;
            case 6:
                for (auto &loan : manager.getUserLoans(loggedInUser->username))
                    loan.showLoanDetails();
                break;
            case 7:
                makeLoanPayment(false);
                break;
            case 8:
                viewTransactions(false);
                break;
            case 9:
                cout << "Logged out.\nTeam Polymorphs wishes you a great day ahead!\n";
                start();
                return;

            default:
                cout << "Invalid choice.\n";
            }

        } while (true);
    }

    // ========== MANAGER MENU ==========
    void managerMenu()
    {
        int choice;
        do
        {
            cout << "\n==== Manager Menu ====\n";
            cout << "1. Create Account\n";
            cout << "2. Show All Accounts\n";
            cout << "3. Close Account\n";
            cout << "4. Apply Loan for User\n";
            cout << "5. Show All Loans\n";
            cout << "6. Make Loan Payment\n";
            cout << "7. View Account Transaction History\n";
            cout << "8. Logout\n";

            cout << "Enter choice: ";
            cin >> choice;

            switch (choice)
            {
            case 1:
                createAccount();
                break;
            case 2:
                manager.showAllAccounts();
                break;
            case 3:
                closeAccount(true);
                break;
            case 4:
                applyLoan();
                break;
            case 5:
                manager.showAllLoans();
                break;
            case 6:
                makeLoanPayment(true);
                break;
            case 7:
                viewTransactions(true);
                break;
            case 8:
                cout << "Logged out.\n";
                start();
                return;

            default:
                cout << "Invalid choice.\n";
            }

        } while (true);
    }

    // ========== SHARED FUNCTIONS ==========
    void createAccount()
    {
        string name, type, uname, pwd;
        double bal;

        cout << "Enter account holder name: ";
        cin.ignore();
        getline(cin, name);

        cout << "Enter initial deposit: INR ";
        cin >> bal;

        cout << "Enter account type (Saving/Current): ";
        cin.ignore();
        getline(cin, type);

        cout << "Enter username of account owner: ";
        getline(cin, uname);

        cout << "Enter password for new user: ";
        getline(cin, pwd);

        users.push_back(User(uname, pwd, "user"));
        // Append new user to file
        ofstream outFile("users.txt",ios::app);
        if(outFile.is_open())
        {
            outFile<<uname<<" "<<pwd<<" user\n";
            outFile.close();
        }
        else
        {
            cout<<"Error opening file\n";
        }

        manager.createAccount(name, bal, type, uname);
    }

    void modifyAccount(bool isManager)
    {
        int accNo;
        string name, type;
        cout << "Enter account number: ";
        cin >> accNo;
        Account *acc = manager.findAccount(accNo, loggedInUser->username, isManager);
        if (acc)
        {
            cout << "Enter new name: ";
            cin.ignore();
            getline(cin, name);
            cout << "Enter new account type: ";
            getline(cin, type);
            acc->modifyAccount(name, type);
        }
        else
        {
            cout << "Account not found or permission denied.\n";
        }
    }

    void depositAmount(bool isManager)
    {
        int accNo;
        double amount;
        cout << "Enter account number: ";
        cin >> accNo;
        cout << "Enter deposit amount: INR ";
        cin >> amount;
        Account *acc = manager.findAccount(accNo, loggedInUser->username, isManager);
        if (acc)
        {
            transaction.deposit(*acc, amount, manager);
        }
        else
        {
            cout << "Account not found or permission denied.\n";
        }
    }

    void withdrawAmount(bool isManager)
    {
        int accNo;
        double amount;
        cout << "Enter account number: ";
        cin >> accNo;
        cout << "Enter withdrawal amount: ";
        cin >> amount;
        Account *acc = manager.findAccount(accNo, loggedInUser->username, isManager);
        if (acc)
        {
            transaction.withdraw(*acc, amount, manager);
        }
        else
        {
            cout << "Account not found or permission denied.\n";
        }
    }

    void applyLoan()
    {
        string name;
        double principal;
        int tenure;
        cout << "Enter borrower name: ";
        cin.ignore();
        getline(cin, name);
        cout << "Enter principal: ";
        cin >> principal;
        cout << "Enter tenure (years): ";
        cin >> tenure;
        cout << "Default Interest Rate is 12%\n";
        manager.applyLoan(name, loggedInUser->username, principal, tenure);
    }

    void makeLoanPayment(bool isManager)
{
    int loanID;
    double amount;
    cout << "Enter Loan ID: ";
    cin >> loanID;
    cout << "Enter amount: ";
    cin >> amount;
    Loan *loan = manager.findLoan(loanID, loggedInUser->username, isManager);
    if (loan)
    {
        loan->makePayment(amount);

        // Find a related account for logging the transaction
        auto accounts = manager.getUserAccounts(loggedInUser->username);
        if (!accounts.empty())
        {
            // Log to the first account (you can change this logic if needed)
            Account* acc = manager.findAccount(accounts[0].getAccountNumber(), loggedInUser->username, isManager);
            if (acc)
            {
                acc->addTransactionRecord(TransactionRecord(acc->getAccountNumber(), "Loan Payment", amount));
                cout << "Loan payment recorded in transaction history of Account " << acc->getAccountNumber() << ".\n";
            }
        }
        else
        {
            cout << "No account found to record the transaction.\n";
        }
    }
    else
    {
        cout << "Loan not found or permission denied.\n";
    }
}


    void closeAccount(bool isManager)
    {
        int accNo;
        cout << "Enter account number to close: ";
        cin >> accNo;
        manager.closeAccount(accNo, loggedInUser->username, isManager);
    }

    void registerNewUser() {
        string username, password;
    
        cout << "\n--- New User Registration ---\n";
        cout << "Enter username: ";
        cin >> username;
        cout << "Enter password: ";
        cin >> password;
    
        // Check if username already exists
        for (auto &user : users) {
            if (user.username == username) {
                cout << "Username already exists. Try a different one.\n";
                return;
            }
        }
    
        User newUser(username, password, "user");
        users.push_back(newUser);
    
        // Save to file
        ofstream outFile("users.txt", ios::app);
        if (outFile.is_open()) {
            outFile << username << " " << password << " user\n";
            outFile.close();
            cout << "Registration successful! You are now logged in as " << username << ".\n";
        } else {
            cout << "Error saving user data to file.\n";
            return;
        }
    
        // ✅ Automatically log the new user in
        loggedInUser = &users.back();
    }
    
};

// ==============================
// Main Function
// ==============================
int main()
{
    UserInteraction ui;
    ui.start();
    return 0;
}
