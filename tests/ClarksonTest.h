#ifndef ClarksonTest_h
#define ClarksonTest_h

#include "../lib/AffineFunction.h"

class ClarksonTest {
public:
    static void run();

private:
    static void simple_polygon_test();
    static void triangle_many_redundant_test();
    static AffineFunction of_list(double input[]);
};

#endif /* ClarksonTest_h */
