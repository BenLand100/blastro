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
#include "core/RGBImage.h"
#include <vector>
#include <functional>
#include <string>

namespace blastro {

class BackgroundExtractor {
public:
    using ProgressCallback = std::function<void(int)>;

    // Fits a 2D polynomial surface to the background of a single channel and subtracts it.
    // order: Polynomial order (1 to 5).
    // sigmaCut: Standard deviation multiplier to reject outliers (stars, noise).
    // sampleFrac: Fraction of image pixels to sample for fitting (e.g. 0.01 = 1%).
    // huberDelta: Delta parameter for robust Huber loss (default: 5.0).
    // equalize: If true, offsets the channels such that the overall background level is centered at a safe floor.
    static GrayscaleImagePtr extractGrayscale(GrayscaleImagePtr src,
                                             int order = 3,
                                             double sigmaCut = 3.0,
                                             double sampleFrac = 0.01,
                                             double huberDelta = 5.0,
                                             bool equalize = true,
                                             ProgressCallback progress = nullptr,
                                             int progressStart = 0,
                                             int progressEnd = 100,
                                             const std::string& method = "Polynomial",
                                             double rbfSmoothing = 0.0);

    static RGBImagePtr extractRGB(RGBImagePtr src,
                                  int order = 3,
                                  double sigmaCut = 3.0,
                                  double sampleFrac = 0.01,
                                  double huberDelta = 5.0,
                                  bool equalize = true,
                                  ProgressCallback progress = nullptr,
                                  const std::string& method = "Polynomial",
                                  double rbfSmoothing = 0.0);
};

} // namespace blastro
