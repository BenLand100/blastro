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
#include "core/GrayscaleImage.h"
#include "core/Star.h"
#include <vector>
#include <string>

namespace blastro {

class StarFinder {
public:
    // Finds stars in a grayscale image.
    // Methods: "centroid" (fast centroid), "gaussian" (global threshold + Gaussian fit), "adaptive" (local block-based max search + hybrid fit)
    static std::vector<Star> findStars(GrayscaleImagePtr img,
                                       int maxStars = 10000,
                                       double snrMin = 5.0,
                                       const std::string& method = "adaptive",
                                       int patchRadius = 10,
                                       double minFwhm = 2.0,
                                       double maxEccentricity = 0.85,
                                       int maxRefinedStars = 250);
};

} // namespace blastro
