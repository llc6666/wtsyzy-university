// Independent public learning example. Build with C++17.
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

std::string fraction_digits(std::uint64_t numerator,
                            std::uint64_t denominator,
                            unsigned base,
                            unsigned limit) {
    if (denominator == 0 || denominator > 1'000'000'000ULL ||
        numerator >= denominator || base < 2 || base > 16 || limit == 0) {
        throw std::invalid_argument("invalid fraction, base or digit limit");
    }
    constexpr char digits[] = "0123456789ABCDEF";
    std::string result = "0.";
    if (numerator == 0) {
        return result + '0';
    }
    for (unsigned i = 0; i < limit && numerator != 0; ++i) {
        const std::uint64_t scaled = numerator * base;
        result.push_back(digits[scaled / denominator]);
        numerator = scaled % denominator;
    }
    if (numerator != 0) {
        result += "...";
    }
    return result;
}

int main() {
    try {
        struct Case {
            std::uint64_t numerator;
            std::uint64_t denominator;
            unsigned base;
            unsigned limit;
            std::string expected;
        };
        const std::vector<Case> cases{
            {5, 8, 2, 16, "0.101"},
            {5, 8, 8, 16, "0.5"},
            {5, 8, 16, 16, "0.A"},
            {1, 10, 2, 16, "0.0001100110011001..."},
            {0, 8, 2, 16, "0.0"},
            {1, 2, 2, 16, "0.1"},
            {1, 4, 16, 16, "0.4"},
        };
        std::size_t checks = 0;
        for (const auto& test : cases) {
            if (fraction_digits(test.numerator, test.denominator,
                                test.base, test.limit) != test.expected) {
                throw std::runtime_error("fraction conversion mismatch");
            }
            ++checks;
        }
        const double x = 0xA.Ap0;
        if (x != 10.625 || x != 0x1.54p3) {
            throw std::runtime_error("hex literal mismatch");
        }
        ++checks;
        std::cout << "5/8 base 2: " << fraction_digits(5, 8, 2, 16) << '\n'
                  << "5/8 base 8: " << fraction_digits(5, 8, 8, 16) << '\n'
                  << "5/8 base 16: " << fraction_digits(5, 8, 16, 16) << '\n'
                  << "1/10 base 2 (16 digits): "
                  << fraction_digits(1, 10, 2, 16) << '\n';
        double value = x;
        std::cout << "decimal: " << std::defaultfloat << value << '\n'
                  << "hexfloat: " << std::hexfloat << value << '\n'
                  << "decimal again: " << std::defaultfloat << value << '\n';
        if (value != x) {
            throw std::runtime_error("formatting changed the value");
        }
        ++checks;
        std::cout << "PASS checks=" << checks << '\n';
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "FAIL: " << error.what() << '\n';
        return 1;
    }
}
