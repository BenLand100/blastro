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
#include "StarFinder.h"
#include <vector>
#include <array>

namespace blastro {

struct Triangle {
    std::array<double, 3> dists; // Sorted side lengths: d1 >= d2 >= d3
    std::array<Star, 3> abc;     // Canonically ordered vertices (opposite to dists)
};

struct AlignmentResult {
    bool success = false;
    double dx = 0.0;
    double dy = 0.0;
    double theta = 0.0;
    double rmsError = 0.0;
    int matchedStars = 0;
};

class ConstellationMatcher {
public:
    // Computes the rotation and translation that maps the target stars onto the reference stars.
    static AlignmentResult match(const std::vector<Star>& refStars,
                                 const std::vector<Star>& targetStars,
                                 int kNearest = 7,
                                 double matchTolerance = 1.5);

private:
    static std::vector<Triangle> buildTriangles(const std::vector<Star>& stars, int kNearest);
};

} // namespace blastro
