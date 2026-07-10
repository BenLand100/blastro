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
                                             double sigmaCut = 3.0,
                                             double sampleFrac = 0.01,
                                             double huberDelta = 5.0,
                                             bool equalize = true,
                                             ProgressCallback progress = nullptr,
                                             int progressStart = 0,
                                             int progressEnd = 100,
                                             const std::string& method = "Polynomial",
                                             double rbfSmoothing = 0.0,
                                             bool restoreMedian = false,
                                             const std::vector<std::pair<double, double>>& customControlPoints = {});

    static RGBImagePtr extractRGB(RGBImagePtr src,
                                  int order = 3,
                                  double sigmaCut = 3.0,
                                  double sampleFrac = 0.01,
                                  double huberDelta = 5.0,
                                  bool equalize = true,
                                  ProgressCallback progress = nullptr,
                                  const std::string& method = "Polynomial",
                                  double rbfSmoothing = 0.0,
                                  bool restoreMedian = false,
                                  const std::vector<std::pair<double, double>>& customControlPoints = {});

    // Automatically generates sample points based on statistics
    static std::vector<std::pair<double, double>> generateSamplePoints(GrayscaleImagePtr img,
                                                                       double sigmaCut = 3.0,
                                                                       double sampleFrac = 0.01);
};

} // namespace blastro
