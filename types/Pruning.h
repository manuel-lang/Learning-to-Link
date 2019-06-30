#ifndef Pruning_h
#define Pruning_h

/*!
 * Defines one way of pruning a tree of different clusters into k clusters
 */
class Pruning {
public:
    double cost;

    Pruning(double cost) : cost(cost) {}

    Pruning() : cost(0) {}
};

#endif /* Pruning_h */
