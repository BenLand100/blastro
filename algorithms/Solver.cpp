/*
 * BLastro - Astronomical Image Processing Software
 * Copyright (C) 2026 Benjamin Land
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "Solver.h"
#include <cmath>
#include <algorithm>
#include <numeric>
#include <stdexcept>

namespace blastro {

struct SimplexVertex {
    std::vector<double> x;
    double val;

    bool operator<(const SimplexVertex& other) const {
        return val < other.val;
    }
};

Solver::Result Solver::nelderMead(CostFunction f,
                                  const std::vector<double>& x0,
                                  double tol,
                                  int maxIter) {
    int n = x0.size();
    if (n == 0) {
        throw std::invalid_argument("Dimension must be greater than zero");
    }

    // Coefficients
    const double alpha = 1.0;  // Reflection
    const double gamma = 2.0;  // Expansion
    const double rho = 0.5;    // Contraction
    const double sigma = 0.5;  // Shrink

    int nfev = 0;
    auto evaluate = [&](const std::vector<double>& x) -> double {
        nfev++;
        return f(x);
    };

    // 1. Initialize simplex of n + 1 vertices
    std::vector<SimplexVertex> simplex(n + 1);
    
    // First vertex is x0
    simplex[0].x = x0;
    simplex[0].val = evaluate(x0);

    // Other vertices are perturbed versions of x0
    for (int i = 1; i <= n; ++i) {
        std::vector<double> xi = x0;
        // Perturb the (i-1)-th coordinate
        double val = xi[i - 1];
        double step = (std::abs(val) > 1e-5) ? (0.05 * val) : 0.05;
        xi[i - 1] += step;
        
        simplex[i].x = xi;
        simplex[i].val = evaluate(xi);
    }

    // Initial sort
    std::sort(simplex.begin(), simplex.end());

    int nit = 0;
    bool success = false;

    while (nit < maxIter) {
        nit++;

        // Check convergence based on function value spread and coordinate spread
        double maxFuncDiff = 0.0;
        for (int i = 1; i <= n; ++i) {
            double diff = std::abs(simplex[i].val - simplex[0].val);
            if (diff > maxFuncDiff) {
                maxFuncDiff = diff;
            }
        }

        double maxCoordDiff = 0.0;
        for (int i = 1; i <= n; ++i) {
            for (int j = 0; j < n; ++j) {
                double diff = std::abs(simplex[i].x[j] - simplex[0].x[j]);
                if (diff > maxCoordDiff) {
                    maxCoordDiff = diff;
                }
            }
        }

        if (maxFuncDiff < tol && maxCoordDiff < tol) {
            success = true;
            break;
        }

        // 2. Centroid of the first n vertices (all except the worst, index n)
        std::vector<double> xbar(n, 0.0);
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                xbar[j] += simplex[i].x[j];
            }
        }
        for (int j = 0; j < n; ++j) {
            xbar[j] /= n;
        }

        // 3. Reflection
        std::vector<double> xr(n);
        for (int j = 0; j < n; ++j) {
            xr[j] = xbar[j] + alpha * (xbar[j] - simplex[n].x[j]);
        }
        double fxr = evaluate(xr);

        if (fxr >= simplex[0].val && fxr < simplex[n - 1].val) {
            // Reflected point is middle of the pack
            simplex[n].x = xr;
            simplex[n].val = fxr;
        } else if (fxr < simplex[0].val) {
            // Reflected point is the best so far, try Expansion
            std::vector<double> xe(n);
            for (int j = 0; j < n; ++j) {
                xe[j] = xbar[j] + gamma * (xr[j] - xbar[j]);
            }
            double fxe = evaluate(xe);

            if (fxe < fxr) {
                simplex[n].x = xe;
                simplex[n].val = fxe;
            } else {
                simplex[n].x = xr;
                simplex[n].val = fxr;
            }
        } else {
            // Reflected point is worse than second worst (fxr >= simplex[n-1].val)
            // Contraction
            bool contracted = false;
            if (fxr < simplex[n].val) {
                // Outside contraction
                std::vector<double> xc(n);
                for (int j = 0; j < n; ++j) {
                    xc[j] = xbar[j] + rho * (xr[j] - xbar[j]);
                }
                double fxc = evaluate(xc);
                if (fxc < fxr) {
                    simplex[n].x = xc;
                    simplex[n].val = fxc;
                    contracted = true;
                }
            } else {
                // Inside contraction
                std::vector<double> xc(n);
                for (int j = 0; j < n; ++j) {
                    xc[j] = xbar[j] + rho * (simplex[n].x[j] - xbar[j]);
                }
                double fxc = evaluate(xc);
                if (fxc < simplex[n].val) {
                    simplex[n].x = xc;
                    simplex[n].val = fxc;
                    contracted = true;
                }
            }

            if (!contracted) {
                // Shrink
                for (int i = 1; i <= n; ++i) {
                    for (int j = 0; j < n; ++j) {
                        simplex[i].x[j] = simplex[0].x[j] + sigma * (simplex[i].x[j] - simplex[0].x[j]);
                    }
                    simplex[i].val = evaluate(simplex[i].x);
                }
            }
        }

        // Re-sort simplex
        std::sort(simplex.begin(), simplex.end());
    }

    Result res;
    res.x = simplex[0].x;
    res.fun = simplex[0].val;
    res.success = success;
    res.nit = nit;
    res.nfev = nfev;
    return res;
}

} // namespace blastro
