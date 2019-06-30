#ifndef LinearFunction_h
#define LinearFunction_h

/*!
 * Represents a mathematical linear function (a*x+b) that is defined by slope a and intercept b
 */
class LinearFunction {
public:
    double a;
    double b;

    LinearFunction() {};

    LinearFunction(double a, double b) : a(a), b(b) {};

    double calculate_interaction_with(LinearFunction lf) {
        return (lf.b - b) / (a - lf.a);
    }
};

#endif /* LinearFunction_h */
