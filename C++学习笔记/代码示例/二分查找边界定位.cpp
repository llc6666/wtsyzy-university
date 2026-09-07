#include <algorithm>
#include <iostream>
#include <vector>

int find_value(const std::vector<int>& values, int target) {
    int left = 0;
    int right = static_cast<int>(values.size()) - 1;

    while (left <= right) {
        const int middle = left + (right - left) / 2;
        if (values[middle] < target) {
            left = middle + 1;
        } else if (values[middle] > target) {
            right = middle - 1;
        } else {
            return middle;
        }
    }

    return -1;
}

int first_ge(const std::vector<int>& values, int target) {
    int left = 0;
    int right = static_cast<int>(values.size());

    while (left < right) {
        const int middle = left + (right - left) / 2;
        if (values[middle] >= target) {
            right = middle;
        } else {
            left = middle + 1;
        }
    }

    return left;
}

int main() {
    const std::vector<int> values{0, 1, 1, 1, 8, 9, 9};

    std::cout << "find(8) = " << find_value(values, 8) << '\n';
    std::cout << "find(2) = " << find_value(values, 2) << '\n';
    std::cout << "first_ge(1) = " << first_ge(values, 1) << '\n';
    std::cout << "first_ge(7) = " << first_ge(values, 7) << '\n';
    std::cout << "first_ge(10) = " << first_ge(values, 10) << '\n';

    const auto position = std::lower_bound(values.begin(), values.end(), 8);
    std::cout << "lower_bound(8) = " << (position - values.begin()) << '\n';
}
