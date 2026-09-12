#include <cassert>
#include <iostream>
#include <vector>

int first_not_less(const std::vector<int>& values, int target) {
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
    const std::vector<int> values{2, 5, 5, 12, 19};
    assert(first_not_less(values, 1) == 0);
    assert(first_not_less(values, 5) == 1);
    assert(first_not_less(values, 12) == 3);
    assert(first_not_less(values, 20) == 5);
    std::cout << "boundary checks passed\n";
}
