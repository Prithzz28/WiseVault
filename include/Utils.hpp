#ifndef WISEVAULT_UTILS_HPP
#define WISEVAULT_UTILS_HPP

#include <iostream>
#include <string>
#include <limits>
#include <ctime>
#include <cstdint>
#include <iomanip>
#include <sstream>
#include <functional>

// ─────────────────────────────────────────────
//  ANSI Color Codes
// ─────────────────────────────────────────────
namespace Color {
    const std::string RESET   = "\033[0m";
    const std::string RED     = "\033[1;31m";
    const std::string GREEN   = "\033[1;32m";
    const std::string YELLOW  = "\033[1;33m";
    const std::string BLUE    = "\033[1;34m";
    const std::string MAGENTA = "\033[1;35m";
    const std::string CYAN    = "\033[1;36m";
    const std::string WHITE   = "\033[1;37m";
    const std::string DIM     = "\033[2m";
    const std::string BOLD    = "\033[1m";
}

// ─────────────────────────────────────────────
//  Box-drawing helpers
// ─────────────────────────────────────────────
namespace Box {
    inline void printHeader(const std::string& title, int width = 50) {
        std::cout << Color::CYAN;
        std::cout << "┌";
        for (int i = 0; i < width - 2; ++i) std::cout << "─";
        std::cout << "┐\n";

        int pad = width - 4 - static_cast<int>(title.size());
        int leftPad = pad / 2;
        int rightPad = pad - leftPad;
        std::cout << "│ ";
        for (int i = 0; i < leftPad; ++i) std::cout << " ";
        std::cout << Color::BOLD << Color::WHITE << title << Color::CYAN;
        for (int i = 0; i < rightPad; ++i) std::cout << " ";
        std::cout << " │\n";

        std::cout << "├";
        for (int i = 0; i < width - 2; ++i) std::cout << "─";
        std::cout << "┤\n";
        std::cout << Color::RESET;
    }

    inline void printRow(const std::string& label, const std::string& value, int width = 50) {
        int contentWidth = width - 4; // "│ " and " │"
        std::string content = label + value;
        int pad = contentWidth - static_cast<int>(content.size());
        if (pad < 0) pad = 0;
        std::cout << Color::CYAN << "│ " << Color::RESET
                  << Color::DIM << label << Color::RESET
                  << Color::WHITE << value << Color::RESET;
        for (int i = 0; i < pad; ++i) std::cout << " ";
        std::cout << Color::CYAN << " │" << Color::RESET << "\n";
    }

    inline void printFooter(int width = 50) {
        std::cout << Color::CYAN << "└";
        for (int i = 0; i < width - 2; ++i) std::cout << "─";
        std::cout << "┘" << Color::RESET << "\n";
    }

    inline void printSeparator(int width = 50) {
        std::cout << Color::CYAN << "├";
        for (int i = 0; i < width - 2; ++i) std::cout << "─";
        std::cout << "┤" << Color::RESET << "\n";
    }
}

// ─────────────────────────────────────────────
//  Safe Input Helpers (fixes cin desync & bad input)
// ─────────────────────────────────────────────
namespace Input {
    // Read an integer with validation; loops until valid
    inline int readInt(const std::string& prompt) {
        int value;
        while (true) {
            std::cout << Color::YELLOW << prompt << Color::RESET;
            std::cin >> value;
            if (std::cin.fail()) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << Color::RED << "  ✗ Invalid input. Please enter a number.\n" << Color::RESET;
            } else {
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                return value;
            }
        }
    }

    // Read a double with validation
    inline double readDouble(const std::string& prompt) {
        double value;
        while (true) {
            std::cout << Color::YELLOW << prompt << Color::RESET;
            std::cin >> value;
            if (std::cin.fail()) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << Color::RED << "  ✗ Invalid input. Please enter a number.\n" << Color::RESET;
            } else {
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                return value;
            }
        }
    }

    // Read a non-empty line
    inline std::string readLine(const std::string& prompt) {
        std::string value;
        while (true) {
            std::cout << Color::YELLOW << prompt << Color::RESET;
            std::getline(std::cin, value);
            if (!value.empty()) return value;
            std::cout << Color::RED << "  ✗ Input cannot be empty.\n" << Color::RESET;
        }
    }

    // Read a single word (no spaces)
    inline std::string readWord(const std::string& prompt) {
        std::string value;
        while (true) {
            std::cout << Color::YELLOW << prompt << Color::RESET;
            std::cin >> value;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            if (!value.empty()) return value;
            std::cout << Color::RED << "  ✗ Input cannot be empty.\n" << Color::RESET;
        }
    }
}

// ─────────────────────────────────────────────
//  Timestamp Formatting (replaces ctime)
// ─────────────────────────────────────────────
inline std::string formatTimestamp(time_t ts) {
    struct tm* tm_info = localtime(&ts);
    char buffer[32];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm_info);
    return std::string(buffer);
}

// ─────────────────────────────────────────────
//  Hidden Password Input (POSIX)
// ─────────────────────────────────────────────
#ifdef _WIN32
    #include <conio.h>
    inline std::string readPassword(const std::string& prompt) {
        std::cout << Color::YELLOW << prompt << Color::RESET;
        std::string pwd;
        char ch;
        while ((ch = _getch()) != '\r') {
            if (ch == '\b' && !pwd.empty()) {
                pwd.pop_back();
                std::cout << "\b \b";
            } else if (ch != '\b') {
                pwd += ch;
                std::cout << '*';
            }
        }
        std::cout << '\n';
        return pwd;
    }
#else
    #include <termios.h>
    #include <unistd.h>
    inline std::string readPassword(const std::string& prompt) {
        std::cout << Color::YELLOW << prompt << Color::RESET;
        struct termios oldt, newt;
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);

        std::string pwd;
        char ch;
        while (read(STDIN_FILENO, &ch, 1) == 1 && ch != '\n') {
            if (ch == 127 || ch == '\b') { // backspace
                if (!pwd.empty()) {
                    pwd.pop_back();
                    std::cout << "\b \b";
                }
            } else {
                pwd += ch;
                std::cout << '*';
            }
        }
        std::cout << '\n';
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        return pwd;
    }
#endif

// ─────────────────────────────────────────────
//  Simple SHA-256 Hash (self-contained, no deps)
//  Used for password hashing with salt
// ─────────────────────────────────────────────
namespace Crypto {

    // Minimal self-contained SHA-256 implementation
    namespace detail {
        inline uint32_t rotr(uint32_t x, uint32_t n) { return (x >> n) | (x << (32 - n)); }
        inline uint32_t ch(uint32_t x, uint32_t y, uint32_t z) { return (x & y) ^ (~x & z); }
        inline uint32_t maj(uint32_t x, uint32_t y, uint32_t z) { return (x & y) ^ (x & z) ^ (y & z); }
        inline uint32_t sig0(uint32_t x) { return rotr(x, 2) ^ rotr(x, 13) ^ rotr(x, 22); }
        inline uint32_t sig1(uint32_t x) { return rotr(x, 6) ^ rotr(x, 11) ^ rotr(x, 25); }
        inline uint32_t gam0(uint32_t x) { return rotr(x, 7) ^ rotr(x, 18) ^ (x >> 3); }
        inline uint32_t gam1(uint32_t x) { return rotr(x, 17) ^ rotr(x, 19) ^ (x >> 10); }

        static const uint32_t K[64] = {
            0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
            0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
            0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
            0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
            0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
            0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
            0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
            0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
        };
    }

    inline std::string sha256(const std::string& input) {
        // State hash values
        uint32_t hv[8] = {
            0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
            0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
        };

        // Pre-processing: padding
        std::string msg = input;
        uint64_t bitLen = msg.size() * 8;
        msg += static_cast<char>(0x80);
        while (msg.size() % 64 != 56) msg += static_cast<char>(0x00);
        for (int i = 7; i >= 0; --i) msg += static_cast<char>((bitLen >> (i * 8)) & 0xFF);

        // Process each 512-bit block
        for (size_t offset = 0; offset < msg.size(); offset += 64) {
            uint32_t w[64];
            for (int i = 0; i < 16; ++i) {
                w[i] = (static_cast<uint8_t>(msg[offset + i * 4]) << 24) |
                        (static_cast<uint8_t>(msg[offset + i * 4 + 1]) << 16) |
                        (static_cast<uint8_t>(msg[offset + i * 4 + 2]) << 8) |
                        (static_cast<uint8_t>(msg[offset + i * 4 + 3]));
            }
            for (int i = 16; i < 64; ++i) {
                w[i] = detail::gam1(w[i-2]) + w[i-7] + detail::gam0(w[i-15]) + w[i-16];
            }

            // Working variables (use array to avoid naming conflicts)
            uint32_t sv[8];
            for (int i = 0; i < 8; ++i) sv[i] = hv[i];

            for (int i = 0; i < 64; ++i) {
                uint32_t t1 = sv[7] + detail::sig1(sv[4])
                            + detail::ch(sv[4], sv[5], sv[6])
                            + detail::K[i] + w[i];
                uint32_t t2 = detail::sig0(sv[0])
                            + detail::maj(sv[0], sv[1], sv[2]);
                sv[7] = sv[6]; sv[6] = sv[5]; sv[5] = sv[4];
                sv[4] = sv[3] + t1;
                sv[3] = sv[2]; sv[2] = sv[1]; sv[1] = sv[0];
                sv[0] = t1 + t2;
            }
            for (int i = 0; i < 8; ++i) hv[i] += sv[i];
        }

        // Produce hex string
        std::ostringstream oss;
        oss << std::hex << std::setfill('0');
        for (int i = 0; i < 8; ++i) oss << std::setw(8) << hv[i];
        return oss.str();
    }

    // Salted hash: salt$hash
    inline std::string hashPassword(const std::string& password, const std::string& salt) {
        return salt + "$" + sha256(salt + password);
    }

    // Generate a simple salt from username + timestamp
    inline std::string generateSalt(const std::string& username) {
        std::ostringstream oss;
        oss << username.substr(0, 4) << time(nullptr) % 100000;
        return sha256(oss.str()).substr(0, 16); // 16-char salt
    }

    // Verify a password against stored "salt$hash"
    inline bool verifyPassword(const std::string& password, const std::string& stored) {
        auto pos = stored.find('$');
        if (pos == std::string::npos) return false;
        std::string salt = stored.substr(0, pos);
        std::string expected = salt + "$" + sha256(salt + password);
        return expected == stored;
    }
}

// ─────────────────────────────────────────────
//  Status message helpers
// ─────────────────────────────────────────────
inline void printSuccess(const std::string& msg) {
    std::cout << Color::GREEN << "  ✓ " << msg << Color::RESET << "\n";
}

inline void printError(const std::string& msg) {
    std::cout << Color::RED << "  ✗ " << msg << Color::RESET << "\n";
}

inline void printInfo(const std::string& msg) {
    std::cout << Color::CYAN << "  ℹ " << msg << Color::RESET << "\n";
}

inline void printWarning(const std::string& msg) {
    std::cout << Color::YELLOW << "  ⚠ " << msg << Color::RESET << "\n";
}

#endif // WISEVAULT_UTILS_HPP
