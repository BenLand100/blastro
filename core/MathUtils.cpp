/*
 * BLastro - Astronomical Image Processing Software
 * Copyright (C) 2026 Benjamin Land
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "MathUtils.h"
#include <cmath>
#include <algorithm>
#include <stdexcept>
#include <omp.h>

namespace blastro {
namespace MathUtils {

std::vector<double> solveGaussianElimination(const std::vector<double>& A, const std::vector<double>& B, int m) {
    std::vector<double> x(m, 0.0);
    std::vector<std::vector<double>> aug(m, std::vector<double>(m + 1, 0.0));
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < m; ++j) {
            aug[i][j] = A[i * m + j];
        }
        aug[i][m] = B[i];
    }

    for (int i = 0; i < m; ++i) {
        int pivotRow = i;
        double maxVal = std::abs(aug[i][i]);
        for (int r = i + 1; r < m; ++r) {
            if (std::abs(aug[r][i]) > maxVal) {
                maxVal = std::abs(aug[r][i]);
                pivotRow = r;
            }
        }

        if (maxVal < 1e-12) {
            return {};
        }

        if (pivotRow != i) {
            std::swap(aug[i], aug[pivotRow]);
        }

        for (int r = i + 1; r < m; ++r) {
            double factor = aug[r][i] / aug[i][i];
            for (int c = i; c <= m; ++c) {
                aug[r][c] -= factor * aug[i][c];
            }
        }
    }

    for (int i = m - 1; i >= 0; --i) {
        double sum = 0.0;
        for (int j = i + 1; j < m; ++j) {
            sum += aug[i][j] * x[j];
        }
        x[i] = (aug[i][m] - sum) / aug[i][i];
    }

    return x;
}

std::vector<double> solveCholesky(const std::vector<double>& A, const std::vector<double>& B, int m) {
    std::vector<double> L(m * m, 0.0);
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j <= i; ++j) {
            double sum = 0.0;
            for (int k = 0; k < j; ++k) {
                sum += L[i * m + k] * L[j * m + k];
            }
            if (i == j) {
                double val = A[i * m + i] - sum;
                if (val <= 0.0) return {};
                L[i * m + j] = std::sqrt(val);
            } else {
                L[i * m + j] = (A[i * m + j] - sum) / L[j * m + j];
            }
        }
    }

    // Solve L * y = B
    std::vector<double> y(m, 0.0);
    for (int i = 0; i < m; ++i) {
        double sum = 0.0;
        for (int k = 0; k < i; ++k) {
            sum += L[i * m + k] * y[k];
        }
        y[i] = (B[i] - sum) / L[i * m + i];
    }

    // Solve L^T * x = y
    std::vector<double> x(m, 0.0);
    for (int i = m - 1; i >= 0; --i) {
        double sum = 0.0;
        for (int k = i + 1; k < m; ++k) {
            sum += L[k * m + i] * x[k];
        }
        x[i] = (y[i] - sum) / L[i * m + i];
    }

    return x;
}

std::array<double, 6> multiplyAffine(const std::array<double, 6>& A, const std::array<double, 6>& B) {
    double a  = A[0]*B[0] + A[1]*B[3];
    double b  = A[0]*B[1] + A[1]*B[4];
    double tx = A[0]*B[2] + A[1]*B[5] + A[2];
    
    double c  = A[3]*B[0] + A[4]*B[3];
    double d  = A[3]*B[1] + A[4]*B[4];
    double ty = A[3]*B[2] + A[4]*B[5] + A[5];
    
    return {a, b, tx, c, d, ty};
}

std::array<double, 6> invertAffine(const std::array<double, 6>& T) {
    double a = T[0];
    double b = T[1];
    double tx = T[2];
    double c = T[3];
    double d = T[4];
    double ty = T[5];
    double det = a * d - b * c;
    if (std::abs(det) < 1e-9) {
        return {1.0, 0.0, 0.0, 0.0, 1.0, 0.0};
    }
    double inv_det = 1.0 / det;
    double inv_a = d * inv_det;
    double inv_b = -b * inv_det;
    double inv_tx = (b * ty - d * tx) * inv_det;
    double inv_c = -c * inv_det;
    double inv_d = a * inv_det;
    double inv_ty = (c * tx - a * ty) * inv_det;
    return {inv_a, inv_b, inv_tx, inv_c, inv_d, inv_ty};
}

std::pair<double, double> transformPoint(const std::array<double, 6>& T, double x, double y) {
    double nx = T[0] * x + T[1] * y + T[2];
    double ny = T[3] * x + T[4] * y + T[5];
    return {nx, ny};
}

double computeMedian(std::vector<double>& values) {
    if (values.empty()) return 0.0;
    size_t n = values.size();
    auto midIt = values.begin() + n / 2;
    std::nth_element(values.begin(), midIt, values.end());
    if (n % 2 == 1) {
        return *midIt;
    } else {
        auto maxIt = std::max_element(values.begin(), midIt);
        return (*maxIt + *midIt) * 0.5;
    }
}

float computeMedian(std::vector<float>& values) {
    if (values.empty()) return 0.0f;
    size_t n = values.size();
    auto midIt = values.begin() + n / 2;
    std::nth_element(values.begin(), midIt, values.end());
    if (n % 2 == 1) {
        return *midIt;
    } else {
        auto maxIt = std::max_element(values.begin(), midIt);
        return (*maxIt + *midIt) * 0.5f;
    }
}

double computeRousseeuwSn(const std::vector<double>& values) {
    if (values.size() < 2) return 0.0;
    size_t n = values.size();
    std::vector<double> outerMedians(n);
    #pragma omp parallel for schedule(dynamic)
    for (size_t i = 0; i < n; ++i) {
        std::vector<double> diffs(n);
        for (size_t j = 0; j < n; ++j) {
            diffs[j] = std::abs(values[i] - values[j]);
        }
        outerMedians[i] = computeMedian(diffs);
    }
    return 1.1926 * computeMedian(outerMedians);
}

float computeRousseeuwSn(const std::vector<float>& values) {
    if (values.size() < 2) return 0.0f;
    size_t n = values.size();
    std::vector<float> outerMedians(n);
    #pragma omp parallel for schedule(dynamic)
    for (size_t i = 0; i < n; ++i) {
        std::vector<float> diffs(n);
        for (size_t j = 0; j < n; ++j) {
            diffs[j] = std::abs(values[i] - values[j]);
        }
        outerMedians[i] = computeMedian(diffs);
    }
    return 1.1926f * computeMedian(outerMedians);
}

void estimateImageStats(const float* data, int numPixels, double& median, double& sn, int maxSamples) {
    std::vector<double> sample;
    sample.reserve(maxSamples > 0 ? maxSamples : numPixels);
    if (maxSamples > 0 && numPixels > maxSamples) {
        int step = numPixels / maxSamples;
        for (int i = 0; i < numPixels; i += step) {
            if (!std::isnan(data[i])) {
                sample.push_back(data[i]);
            }
            if (sample.size() >= static_cast<size_t>(maxSamples)) break;
        }
    } else {
        for (int i = 0; i < numPixels; ++i) {
            if (!std::isnan(data[i])) {
                sample.push_back(data[i]);
            }
        }
    }
    if (sample.empty()) {
        median = 0.0;
        sn = 0.0;
        return;
    }
    median = computeMedian(sample);
    sn = computeRousseeuwSn(sample);
}

} // namespace MathUtils
} // namespace blastro
