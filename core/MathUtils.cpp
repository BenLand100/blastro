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

double computeMAD(std::vector<double>& values) {
    if (values.empty()) return 0.0;
    double median = computeMedian(values);
    std::vector<double> absDiffs(values.size());
    for (size_t i = 0; i < values.size(); ++i) {
        absDiffs[i] = std::abs(values[i] - median);
    }
    return computeMedian(absDiffs);
}

float computeMAD(std::vector<float>& values) {
    if (values.empty()) return 0.0f;
    float median = computeMedian(values);
    std::vector<float> absDiffs(values.size());
    for (size_t i = 0; i < values.size(); ++i) {
        absDiffs[i] = std::abs(values[i] - median);
    }
    return computeMedian(absDiffs);
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

double applyGhsProtection(double val, double symmetryPoint, double shadowProtect, double highlightProtect, double stretched) {
    // Apply shadow protection if active (shadowProtect > 0.0)
    if (shadowProtect > 0.0 && val < symmetryPoint) {
        double w_s = shadowProtect * std::exp(-val / (symmetryPoint - val + 1e-9));
        return (1.0 - w_s) * stretched + w_s * val;
    }

    // Apply highlight protection if active (highlightProtect < 1.0)
    double effectiveHighlightProtect = 1.0 - highlightProtect;
    if (effectiveHighlightProtect > 0.0 && val > symmetryPoint) {
        double w_h = effectiveHighlightProtect * std::exp(-(1.0 - val) / (val - symmetryPoint + 1e-9));
        return (1.0 - w_h) * stretched + w_h * val;
    }

    return stretched;
}

struct SimplexVertex {
    std::vector<double> x;
    double val;

    bool operator<(const SimplexVertex& other) const {
        return val < other.val;
    }
};

NelderMeadResult nelderMead(std::function<double(const std::vector<double>&)> f,
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
        double val = xi[i - 1];
        double step = (std::abs(val) > 1e-5) ? (0.05 * val) : 0.05;
        xi[i - 1] += step;
        
        simplex[i].x = xi;
        simplex[i].val = evaluate(xi);
    }

    std::sort(simplex.begin(), simplex.end());

    int nit = 0;
    bool success = false;

    while (nit < maxIter) {
        nit++;

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
            simplex[n].x = xr;
            simplex[n].val = fxr;
        } else if (fxr < simplex[0].val) {
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
            bool contracted = false;
            if (fxr < simplex[n].val) {
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
                for (int i = 1; i <= n; ++i) {
                    for (int j = 0; j < n; ++j) {
                        simplex[i].x[j] = simplex[0].x[j] + sigma * (simplex[i].x[j] - simplex[0].x[j]);
                    }
                    simplex[i].val = evaluate(simplex[i].x);
                }
            }
        }

        std::sort(simplex.begin(), simplex.end());
    }

    NelderMeadResult res;
    res.x = simplex[0].x;
    res.fun = simplex[0].val;
    res.success = success;
    res.nit = nit;
    res.nfev = nfev;
    return res;
}

MonotoneCubicSpline::MonotoneCubicSpline(const std::vector<double>& x_in, const std::vector<double>& y_in) {
    if (x_in.size() != y_in.size() || x_in.empty()) {
        return;
    }
    x = x_in;
    y = y_in;
    int n = x.size();
    m.resize(n, 0.0);

    if (n == 1) {
        return;
    }
    if (n == 2) {
        m[0] = m[1] = (y[1] - y[0]) / (x[1] - x[0]);
        return;
    }

    std::vector<double> secant(n - 1);
    for (int i = 0; i < n - 1; ++i) {
        secant[i] = (y[i + 1] - y[i]) / (x[i + 1] - x[i]);
    }

    m[0] = secant[0];
    m[n - 1] = secant[n - 2];
    for (int i = 1; i < n - 1; ++i) {
        if (secant[i - 1] * secant[i] <= 0.0) {
            m[i] = 0.0;
        } else {
            // Harmonic mean of secants
            double w1 = 2 * (x[i + 1] - x[i]) + (x[i] - x[i - 1]);
            double w2 = (x[i + 1] - x[i]) + 2 * (x[i] - x[i - 1]);
            if (w1 + w2 == 0.0) m[i] = 0.0;
            else m[i] = (w1 + w2) / (w1 / secant[i - 1] + w2 / secant[i]);
        }
    }
}

double MonotoneCubicSpline::evaluate(double x_val) const {
    if (x.empty()) return 0.0;
    if (x.size() == 1) return y[0];
    if (x_val <= x.front()) return y.front();
    if (x_val >= x.back()) return y.back();

    auto it = std::lower_bound(x.begin(), x.end(), x_val);
    int i = std::distance(x.begin(), it) - 1;
    if (i < 0) i = 0;

    double h = x[i + 1] - x[i];
    if (h == 0.0) return y[i];

    double t = (x_val - x[i]) / h;
    double t2 = t * t;
    double t3 = t2 * t;

    double h00 = 2 * t3 - 3 * t2 + 1;
    double h10 = t3 - 2 * t2 + t;
    double h01 = -2 * t3 + 3 * t2;
    double h11 = t3 - t2;

    return h00 * y[i] + h10 * h * m[i] + h01 * y[i + 1] + h11 * h * m[i + 1];
}

std::vector<float> computeCurvesLUT(const std::vector<double>& x_in, const std::vector<double>& y_in) {
    MonotoneCubicSpline spline(x_in, y_in);
    int lutSize = 65536;
    std::vector<float> lut(lutSize);
    if (x_in.empty()) {
        for (int i = 0; i < lutSize; ++i) lut[i] = static_cast<float>(i) / (lutSize - 1);
        return lut;
    }
    for (int i = 0; i < lutSize; ++i) {
        double x_val = static_cast<double>(i) / (lutSize - 1);
        lut[i] = std::clamp(static_cast<float>(spline.evaluate(x_val)), 0.0f, 1.0f);
    }
    return lut;
}

} // namespace MathUtils
} // namespace blastro
