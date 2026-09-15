#include <algorithm>
#include <iostream>
#include <vector>

bool enough(const std::vector<long long>& trees,
            long long required,
            long long cut_height) {
    long long collected = 0;
    for (const long long tree : trees) {
        if (tree > cut_height) {
            collected += tree - cut_height;
            if (collected >= required) {
                return true;
            }
        }
    }
    return false;
}

int main() {
    int n;
    long long required;
    if (!(std::cin >> n >> required) || n <= 0) {
        return 0;
    }

    std::vector<long long> trees(n);
    for (long long& tree : trees) {
        std::cin >> tree;
    }

    long long low = 0;
    long long high = *std::max_element(trees.begin(), trees.end());
    while (low < high) {
        const long long mid = low + (high - low + 1) / 2;
        if (enough(trees, required, mid)) {
            low = mid;
        } else {
            high = mid - 1;
        }
    }

    std::cout << low << '\n';
}
