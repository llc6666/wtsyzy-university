#include <algorithm>
#include <cassert>
#include <cstddef>
#include <deque>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

std::vector<int> sliding_max(const std::vector<int>& values, std::size_t width) {
    if (values.empty() || width == 0 || width > values.size()) {
        throw std::invalid_argument("width must be in [1, values.size()]");
    }

    std::deque<std::size_t> candidates;
    std::vector<int> result;
    result.reserve(values.size() - width + 1);

    for (std::size_t i = 0; i < values.size(); ++i) {
        while (!candidates.empty() && values[candidates.back()] <= values[i]) {
            candidates.pop_back();
        }
        candidates.push_back(i);

        const std::size_t left = i + 1 >= width ? i + 1 - width : 0;
        while (!candidates.empty() && candidates.front() < left) {
            candidates.pop_front();
        }

        if (i + 1 >= width) {
            result.push_back(values[candidates.front()]);
        }
    }
    return result;
}

std::vector<int> brute_force(const std::vector<int>& values, std::size_t width) {
    std::vector<int> result;
    for (std::size_t left = 0; left + width <= values.size(); ++left) {
        result.push_back(*std::max_element(values.begin() + left,
                                           values.begin() + left + width));
    }
    return result;
}

void run_self_test() {
    const std::vector<std::pair<std::vector<int>, std::size_t>> cases = {
        {{3, 1, 4, 1, 5, 9, 2}, 3},
        {{7}, 1},
        {{1, 2, 3, 4}, 1},
        {{4, 3, 2, 1}, 4},
        {{5, 5, 5, 5}, 2},
        {{2, 1, 2, 1, 2}, 3},
    };
    for (const auto& [values, width] : cases) {
        assert(sliding_max(values, width) == brute_force(values, width));
    }

    std::mt19937 generator(20261010U);
    std::uniform_int_distribution<int> size_distribution(1, 40);
    std::uniform_int_distribution<int> value_distribution(-100, 100);
    for (int trial = 0; trial < 5000; ++trial) {
        const int size = size_distribution(generator);
        std::vector<int> values(static_cast<std::size_t>(size));
        for (int& value : values) {
            value = value_distribution(generator);
        }
        std::uniform_int_distribution<int> width_distribution(1, size);
        const std::size_t width = static_cast<std::size_t>(width_distribution(generator));
        assert(sliding_max(values, width) == brute_force(values, width));
    }
    std::cout << "self-test passed\n";
}

int main(int argc, char** argv) {
    if (argc == 2 && std::string(argv[1]) == "--self-test") {
        run_self_test();
        return 0;
    }

    std::size_t size = 0;
    std::size_t width = 0;
    if (!(std::cin >> size >> width) || size == 0 || width == 0 || width > size) {
        std::cerr << "input: n k followed by n integers, with 1 <= k <= n\n";
        return 2;
    }

    std::vector<int> values(size);
    for (int& value : values) {
        if (!(std::cin >> value)) {
            std::cerr << "not enough values\n";
            return 2;
        }
    }

    const auto result = sliding_max(values, width);
    for (std::size_t i = 0; i < result.size(); ++i) {
        if (i != 0) {
            std::cout << ' ';
        }
        std::cout << result[i];
    }
    std::cout << '\n';
    return 0;
}
