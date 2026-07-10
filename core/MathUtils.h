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
#include <vector>
#include <array>
#include <utility>

namespace blastro {
namespace MathUtils {

    // Solves A * x = B using Gaussian Elimination with partial pivoting
    std::vector<double> solveGaussianElimination(const std::vector<double>& A, const std::vector<double>& B, int m);

    // Solves A * x = B using Cholesky decomposition A = L * L^T
    std::vector<double> solveCholesky(const std::vector<double>& A, const std::vector<double>& B, int m);

    // Affine transformation matrices
    std::array<double, 6> multiplyAffine(const std::array<double, 6>& A, const std::array<double, 6>& B);
    std::array<double, 6> invertAffine(const std::array<double, 6>& T);
    std::pair<double, double> transformPoint(const std::array<double, 6>& T, double x, double y);

    // Statistics
    double computeMedian(std::vector<double>& values);
    float computeMedian(std::vector<float>& values);
    
    // Rousseeuw Sn estimator (robust scale estimator). O(N^2) complexity, parallelized with OpenMP.
    double computeRousseeuwSn(const std::vector<double>& values);
    float computeRousseeuwSn(const std::vector<float>& values);

    // Estimate median and Sn of a float buffer (skipping NaNs), using sampling if requested.
    void estimateImageStats(const float* data, int numPixels, double& median, double& sn, int maxSamples = 2000);

} // namespace MathUtils
} // namespace blastro
