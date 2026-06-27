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
