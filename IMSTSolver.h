#ifndef IMST_SOLVER_H
#define IMST_SOLVER_H

#include <vector>
#include <tuple>

class IMSTSolver {
public:
    virtual void solve() = 0;
    virtual int getMSTWeight() const = 0;
    virtual const std::vector<std::tuple<int, int, int>>& getMSTEdges() const = 0;

    // New methods for additional operations
    virtual int getDiameter() const = 0;
    virtual double getAverageDistance() const = 0;
    virtual int getShortestDistance(int xi, int xj) const = 0;

    virtual ~IMSTSolver() = default;
};

#endif // IMST_SOLVER_H
