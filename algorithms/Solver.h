#pragma once
#include <vector>
#include <functional>

namespace blastro {

class Solver {
public:
    using CostFunction = std::function<double(const std::vector<double>&)>;

    struct Result {
        std::vector<double> x;
        double fun;
        bool success;
        int nit;
        int nfev;
    };

    // Nelder-Mead (downhill simplex) optimization
    static Result nelderMead(CostFunction f,
                             const std::vector<double>& x0,
                             double tol = 1e-5,
                             int maxIter = 1000);
};

} // namespace blastro
