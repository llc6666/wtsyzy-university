// Independent public learning example and exhaustive reference check.
#include <cstddef>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>

std::string smallest_subsequence(const std::string& s, std::size_t k) {
    if (k > s.size()) {
        throw std::invalid_argument("K exceeds string length");
    }
    for (char c : s) {
        if (c < 'a' || c > 'z') {
            throw std::invalid_argument("input must contain lowercase letters");
        }
    }
    std::size_t remaining = s.size() - k;
    std::string answer;
    answer.reserve(s.size());
    for (char c : s) {
        while (!answer.empty() && remaining > 0 && answer.back() > c) {
            answer.pop_back();
            --remaining;
        }
        answer.push_back(c);
    }
    answer.resize(k);
    return answer;
}

// Enumerate choices without the stack algorithm or its greedy decisions.
std::string brute_force(const std::string& s, std::size_t k) {
    std::string candidate;
    std::string best;
    bool found = false;
    std::function<void(std::size_t)> visit = [&](std::size_t position) {
        if (candidate.size() == k) {
            if (!found || candidate < best) {
                best = candidate;
                found = true;
            }
            return;
        }
        if (position == s.size() ||
            s.size() - position < k - candidate.size()) {
            return;
        }
        candidate.push_back(s[position]);
        visit(position + 1);
        candidate.pop_back();
        visit(position + 1);
    };
    visit(0);
    if (!found) {
        throw std::runtime_error("reference did not find a valid choice");
    }
    return best;
}

bool is_subsequence(const std::string& source, const std::string& candidate) {
    std::size_t matched = 0;
    for (char c : source) {
        if (matched < candidate.size() && c == candidate[matched]) {
            ++matched;
        }
    }
    return matched == candidate.size();
}

void self_test() {
    std::size_t checked = 0;
    std::size_t combinations = 1;
    for (std::size_t n = 0; n <= 8; ++n) {
        for (std::size_t encoded = 0; encoded < combinations; ++encoded) {
            std::string s(n, 'a');
            std::size_t digits = encoded;
            for (std::size_t i = 0; i < n; ++i) {
                s[i] = static_cast<char>('a' + digits % 3);
                digits /= 3;
            }
            for (std::size_t k = 0; k <= n; ++k) {
                const std::string answer = smallest_subsequence(s, k);
                if (answer.size() != k || !is_subsequence(s, answer) ||
                    answer != brute_force(s, k)) {
                    throw std::runtime_error("exhaustive mismatch: " + s +
                                             " K=" + std::to_string(k));
                }
                ++checked;
            }
        }
        combinations *= 3;
    }
    const std::string repeated(200'000, 'a');
    if (smallest_subsequence(repeated, 100'000) != std::string(100'000, 'a')) {
        throw std::runtime_error("large repeated case failed");
    }
    const std::string descending = std::string(100'000, 'z') +
                                   std::string(100'000, 'a');
    if (smallest_subsequence(descending, 100'000) != std::string(100'000, 'a')) {
        throw std::runtime_error("large descending case failed");
    }
    std::size_t invalid = 0;
    try {
        static_cast<void>(smallest_subsequence("abc", 4));
    } catch (const std::invalid_argument&) {
        ++invalid;
    }
    try {
        static_cast<void>(smallest_subsequence("aB", 1));
    } catch (const std::invalid_argument&) {
        ++invalid;
    }
    if (invalid != 2) {
        throw std::runtime_error("invalid inputs were not rejected");
    }
    std::cout << "PASS exhaustive_cases=" << checked
              << " large_cases=2 invalid_cases=" << invalid << '\n';
}

int main(int argc, char* argv[]) {
    try {
        if (argc == 2 && std::string(argv[1]) == "--self-test") {
            self_test();
            return 0;
        }
        if (argc != 1) {
            throw std::invalid_argument("usage: program [--self-test]");
        }
        std::string s;
        long long input_k = 0;
        if (!(std::cin >> s >> input_k) || input_k < 0 ||
            static_cast<unsigned long long>(input_k) > s.size()) {
            throw std::invalid_argument("expected lowercase_string K, 0<=K<=N");
        }
        std::cout << smallest_subsequence(s, static_cast<std::size_t>(input_k))
                  << '\n';
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "FAIL: " << error.what() << '\n';
        return 1;
    }
}
