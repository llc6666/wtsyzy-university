#include <algorithm>
#include <iostream>
#include <limits>
#include <vector>

struct Activity {
    int start;
    int end;
};

int main() {
    int n = 0;
    std::cin >> n;

    std::vector<Activity> activities(n);
    for (Activity& activity : activities) {
        std::cin >> activity.start >> activity.end;
    }

    std::sort(activities.begin(), activities.end(),
              [](const Activity& left, const Activity& right) {
                  return left.end < right.end;
              });

    int selected = 0;
    int lastEnd = std::numeric_limits<int>::lowest();
    for (const Activity& activity : activities) {
        if (activity.start >= lastEnd) {
            ++selected;
            lastEnd = activity.end;
        }
    }

    std::cout << selected << '\n';
}
