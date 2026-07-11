/*
 * BLastro - Astronomical Image Processing Software
 * Copyright (C) 2026 Benjamin Land
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#pragma once
#include "core/GrayscaleImage.h"
#include "core/RGBImage.h"
#include <vector>
#include <utility>
#include <functional>
#include <string>

namespace blastro {

class BackgroundExtractor {
public:
    using ProgressCallback = std::function<void(int)>;

    // Fits a 2D polynomial surface to the background of a single channel and subtracts it.
    static GrayscaleImagePtr extractGrayscale(GrayscaleImagePtr src,
                                             int order = 3,
                                             int gridSize = 10,
                                             double huberDelta = 5.0,
                                             bool normalize = true,
                                             ProgressCallback progress = nullptr,
                                             int progressStart = 0,
                                             int progressEnd = 100,
                                             const std::string& method = "Polynomial",
                                             double rbfSmoothing = 0.0,
                                             const std::vector<std::pair<double, double>>& customControlPoints = {},
                                             double maxDeviation = 3.0,
                                             double maxStructure = 1.5,
                                             const std::string& imageName = "");

    static RGBImagePtr extractRGB(RGBImagePtr src,
                                  int order = 3,
                                  int gridSize = 10,
                                  double huberDelta = 5.0,
                                  bool normalize = true,
                                  ProgressCallback progress = nullptr,
                                  const std::string& method = "Polynomial",
                                  double rbfSmoothing = 0.0,
                                  const std::vector<std::pair<double, double>>& customControlPoints = {},
                                  double maxDeviation = 3.0,
                                  double maxStructure = 1.5,
                                  const std::string& imageName = "");

    // Automatically generates sample points based on statistics (grid-based)
    static std::vector<std::pair<double, double>> generateGridPoints(GrayscaleImagePtr img,
                                                                     int cols = 10,
                                                                     int rows = 10,
                                                                     double maxDeviation = 3.0,
                                                                     double maxStructure = 1.5);
};

} // namespace blastro
