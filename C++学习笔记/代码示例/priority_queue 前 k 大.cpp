#include <cstddef>
#include <functional>
#include <iostream>
#include <queue>
#include <vector>

int main() {
    int n;
    int k;
    std::cin >> n >> k;

    if (n <= 0 || k <= 0 || k > n) {
        return 0;
    }

    std::priority_queue<
        int,
        std::vector<int>,
        std::greater<int>
    > top_k;

    for (int i = 0; i < n; ++i) {
        int value;
        std::cin >> value;
        top_k.push(value);

        if (top_k.size() > static_cast<std::size_t>(k)) {
            top_k.pop();
        }

        if (i >= k - 1) {
            if (i > k - 1) {
                std::cout << ' ';
            }
            std::cout << top_k.top();
        }
    }

    std::cout << '\n';
}
