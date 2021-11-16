#include <algorithm>
#include <assert.h>
#include <vector>
#include "TestHelp.h"

void TestHelp::assert_same_elements(std::vector<int> &x, std::vector<int> &y) {
    assert(x.size() == y.size());

    std::sort(x.begin(), x.end());
    std::sort(y.begin(), y.end());

    int size = x.size();

    for(int i = 0; i < size; i++) {
        assert(x[i] == y[i]);
    }
}
