#ifndef WISEVAULT_USER_HPP
#define WISEVAULT_USER_HPP

#include <string>
#include <sstream>
#include "Utils.hpp"

class User {
private:
    std::string username;
    std::string passwordHash; // stored as "salt$hash"
    std::string role;         // "manager" or "user"

public:
    User() : role("user") {}

    // Create new user (hashes the plaintext password)
    User(const std::string& uname, const std::string& plaintextPwd, const std::string& r, bool alreadyHashed = false)
        : username(uname), role(r)
    {
        if (alreadyHashed) {
            passwordHash = plaintextPwd; // already salt$hash format
        } else {
            std::string salt = Crypto::generateSalt(uname);
            passwordHash = Crypto::hashPassword(plaintextPwd, salt);
        }
    }

    bool verifyPassword(const std::string& plaintextPwd) const {
        return Crypto::verifyPassword(plaintextPwd, passwordHash);
    }

    std::string getUsername() const { return username; }
    std::string getRole() const { return role; }
    std::string getPasswordHash() const { return passwordHash; }

    bool isManager() const { return role == "manager"; }

    // Serialize: username|passwordHash|role
    std::string serialize() const {
        return username + "|" + passwordHash + "|" + role;
    }

    // Deserialize from pipe-separated line
    static User deserialize(const std::string& line) {
        std::istringstream iss(line);
        std::string uname, pwdHash, r;
        std::getline(iss, uname, '|');
        std::getline(iss, pwdHash, '|');
        std::getline(iss, r, '|');
        return User(uname, pwdHash, r, true); // already hashed
    }
};

#endif // WISEVAULT_USER_HPP
