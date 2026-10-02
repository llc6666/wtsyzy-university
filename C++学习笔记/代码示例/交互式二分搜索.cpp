// Offline verification model for exact binary search. Build with C++17.
#include <algorithm>
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <string>

int query_limit(int n) {
    if (n < 1) {
        throw std::invalid_argument("N must be positive");
    }
    int values = 0;
    int questions = 0;
    while (values < n) {
        values = values * 2 + 1;
        ++questions;
    }
    return questions;
}

class Oracle {
public:
    Oracle(int n, int hidden)
        : n_(n), hidden_(hidden), limit_(query_limit(n)) {}

    char ask(int value) {
        if (value < 1 || value > n_) {
            throw std::runtime_error("query outside candidate domain");
        }
        ++queries_;
        if (queries_ > limit_) {
            throw std::runtime_error("query limit exceeded");
        }
        if (value < hidden_) {
            return '<';
        }
        if (value > hidden_) {
            return '>';
        }
        return '=';
    }

    int queries() const { return queries_; }

private:
    int n_;
    int hidden_;
    int limit_;
    int queries_ = 0;
};

int find_hidden(int n, Oracle& oracle) {
    int left = 1;
    int right = n;
    while (left <= right) {
        const int middle = left + (right - left) / 2;
        const char reply = oracle.ask(middle);
        if (reply == '<') {
            left = middle + 1;
        } else if (reply == '>') {
            right = middle - 1;
        } else if (reply == '=') {
            return middle;
        } else {
            throw std::runtime_error("unknown oracle reply");
        }
    }
    throw std::runtime_error("a valid hidden value was not found");
}

void self_test() {
    std::size_t cases = 0;
    int maximum_questions = 0;
    for (int n = 1; n <= 1024; ++n) {
        for (int hidden = 1; hidden <= n; ++hidden) {
            Oracle oracle(n, hidden);
            if (find_hidden(n, oracle) != hidden ||
                oracle.queries() > query_limit(n)) {
                throw std::runtime_error("binary-search verification failed");
            }
            ++cases;
            maximum_questions = std::max(maximum_questions, oracle.queries());
        }
    }
    std::cout << "PASS cases=" << cases
              << " max_queries=" << maximum_questions << '\n';
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
        int n = 0;
        int hidden = 0;
        if (!(std::cin >> n >> hidden) || hidden < 1 || hidden > n) {
            throw std::invalid_argument("expected N hidden with 1<=hidden<=N");
        }
        Oracle oracle(n, hidden);
        std::cout << find_hidden(n, oracle) << '\n';
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "FAIL: " << error.what() << '\n';
        return 1;
    }
}
