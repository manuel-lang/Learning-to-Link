#ifndef AlphaRange_h
#define AlphaRange_h

/*!
 * Defines an interval with boundaries min and max and a cost that is constant within this interval.
 */
class AlphaRange {
public:
    double min;
    double max;
    double cost;

    AlphaRange(double min, double max, double cost) : min(min), max(max), cost(cost) {}
};

#endif /* AlphaRange_h */
