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

#include "BackgroundExtractor.h"
#include <cmath>
#include <random>
#include <algorithm>
#include <numeric>
#include <stdexcept>
#include <iostream>
#include "core/Logger.h"

namespace blastro {

// Helper: Solve A * x = B using Cholesky decomposition A = L * L^T
static std::vector<double> solveCholesky(const std::vector<double>& A, const std::vector<double>& B, int m) {
    std::vector<double> L(m * m, 0.0);
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j <= i; ++j) {
            double sum = 0.0;
            for (int k = 0; k < j; ++k) {
                sum += L[i * m + k] * L[j * m + k];
            }
            if (i == j) {
                double val = A[i * m + i] - sum;
                if (val <= 0.0) return {}; // Not positive definite
                L[i * m + j] = std::sqrt(val);
            } else {
                L[i * m + j] = (A[i * m + j] - sum) / L[j * m + j];
            }
        }
    }

    // Solve L * y = B (Forward substitution)
    std::vector<double> y(m);
    for (int i = 0; i < m; ++i) {
        double sum = 0.0;
        for (int k = 0; k < i; ++k) {
            sum += L[i * m + k] * y[k];
        }
        y[i] = (B[i] - sum) / L[i * m + i];
    }

    // Solve L^T * x = y (Backward substitution)
    std::vector<double> x(m);
    for (int i = m - 1; i >= 0; --i) {
        double sum = 0.0;
        for (int k = i + 1; k < m; ++k) {
            sum += L[k * m + i] * x[k];
        }
        x[i] = (y[i] - sum) / L[i * m + i];
    }

    // Fallback: If Cholesky fails or becomes unstable, we can return empty or try a simple ridge regularizer.
    return x;
}

struct FitPoint {
    double x_norm;
    double y_norm;
    double z;
};

// Computes the polynomial terms for a given coordinate
static std::vector<double> getPolynomialTerms(double x, double y, int order) {
    std::vector<double> terms;
    // Terms are x^i * y^j where i + j <= order
    for (int i = 0; i <= order; ++i) {
        for (int j = 0; j <= order; ++j) {
            if (i + j <= order) {
                terms.push_back(std::pow(x, i) * std::pow(y, j));
            }
        }
    }
    return terms;
}

GrayscaleImagePtr BackgroundExtractor::extractGrayscale(GrayscaleImagePtr src,
                                                       int order,
                                                       double sigmaCut,
                                                       double sampleFrac,
                                                       double huberDelta,
                                                       bool equalize,
                                                       ProgressCallback progress,
                                                       int progressStart,
                                                       int progressEnd) {
    if (!src) return nullptr;

    int w = src->width();
    int h = src->height();
    int numPixels = w * h;
    const float* data = src->buffer()->data();

    // 1. Compute median and standard deviation of the channel
    if (progress) progress(progressStart + (progressEnd - progressStart) * 5 / 100);
    Logger::info("BackgroundExtraction", QString("Analyzing image data statistics (%1x%2 pixels)...").arg(w).arg(h));
    
    std::vector<float> sortedData(data, data + numPixels);
    std::sort(sortedData.begin(), sortedData.end());
    double median = sortedData[numPixels / 2];

    double sumSqDiff = 0.0;
    for (int i = 0; i < numPixels; ++i) {
        double diff = data[i] - median;
        sumSqDiff += diff * diff;
    }
    double stddev = std::sqrt(sumSqDiff / numPixels);
    Logger::info("BackgroundExtraction", QString("  Channel Stats: Median = %1, StdDev = %2").arg(median).arg(stddev));

    // 2. Select background sample points within sigmaCut of median
    if (progress) progress(progressStart + (progressEnd - progressStart) * 15 / 100);
    double lowBound = median - sigmaCut * stddev;
    double highBound = median + sigmaCut * stddev;
    Logger::info("BackgroundExtraction", QString("  Selecting background sample points in range [%1, %2] (sigma_cut = %3)...")
                 .arg(lowBound).arg(highBound).arg(sigmaCut));

    std::vector<FitPoint> samples;
    std::minstd_rand rng(1337); // Deterministic seed for reproducibility
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            float val = data[y * w + x];
            if (val >= lowBound && val <= highBound) {
                // Probabilistic sampling based on sampleFrac
                if (dist(rng) < sampleFrac) {
                    FitPoint pt;
                    // Normalize coordinates to [-1, 1] to prevent numerical overflow at high orders
                    pt.x_norm = (2.0 * x - w) / w;
                    pt.y_norm = (2.0 * y - h) / h;
                    pt.z = val;
                    samples.push_back(pt);
                }
            }
        }
    }

    if (samples.empty()) {
        throw std::runtime_error("No valid background sample points found");
    }
    Logger::info("BackgroundExtraction", QString("  Selected %1 sample points for robust fitting (sample_frac = %2).").arg(samples.size()).arg(sampleFrac));

    // Number of polynomial terms
    int numTerms = 0;
    for (int i = 0; i <= order; ++i) {
        for (int j = 0; j <= order; ++j) {
            if (i + j <= order) numTerms++;
        }
    }

    // 3. Solve robust polynomial fit using Iteratively Reweighted Least Squares (IRLS)
    Logger::info("BackgroundExtraction", QString("  Solving robust 2D polynomial surface fit (order = %1, %2 terms, max IRLS iterations = 8)...").arg(order).arg(numTerms));
    std::vector<double> coeffs(numTerms, 0.0);
    std::vector<double> weights(samples.size(), 1.0);

    const int maxIRLSIterations = 8;
    for (int iter = 0; iter < maxIRLSIterations; ++iter) {
        if (progress) {
            int stepProgress = progressStart + (progressEnd - progressStart) * (20 + 5 * iter) / 100;
            progress(stepProgress);
        }

        // Build Normal Equations: A * c = B
        std::vector<double> A(numTerms * numTerms, 0.0);
        std::vector<double> B(numTerms, 0.0);

        // Add small ridge regularization to diagonal to guarantee positive definiteness
        for (int i = 0; i < numTerms; ++i) {
            A[i * numTerms + i] = 1e-6; 
        }

        for (size_t p = 0; p < samples.size(); ++p) {
            const auto& pt = samples[p];
            double wVal = weights[p];
            std::vector<double> terms = getPolynomialTerms(pt.x_norm, pt.y_norm, order);

            for (int i = 0; i < numTerms; ++i) {
                for (int j = 0; j < numTerms; ++j) {
                    A[i * numTerms + j] += terms[i] * terms[j] * wVal;
                }
                B[i] += terms[i] * pt.z * wVal;
            }
        }

        // Solve linear system via Cholesky
        auto nextCoeffs = solveCholesky(A, B, numTerms);
        if (nextCoeffs.empty()) {
            Logger::warning("BackgroundExtraction", QString("    IRLS Iteration %1: Cholesky decomposition failed or became unstable, stopping early.").arg(iter + 1));
            break; 
        }
        coeffs = nextCoeffs;

        // Update residuals and weights using robust Huber loss influence
        double sumAbsRes = 0.0;
        for (size_t p = 0; p < samples.size(); ++p) {
            const auto& pt = samples[p];
            std::vector<double> terms = getPolynomialTerms(pt.x_norm, pt.y_norm, order);
            
            double fitVal = 0.0;
            for (int i = 0; i < numTerms; ++i) {
                fitVal += terms[i] * coeffs[i];
            }

            double residual = pt.z - fitVal;
            double absRes = std::abs(residual);
            sumAbsRes += absRes;

            // Huber Weight Function
            if (absRes <= huberDelta) {
                weights[p] = 1.0;
            } else {
                weights[p] = huberDelta / absRes;
            }
        }
        
        if (iter == 0 || iter == maxIRLSIterations - 1) {
            Logger::info("BackgroundExtraction", QString("    IRLS Iteration %1/8: Mean Absolute Residual = %2")
                         .arg(iter + 1).arg(sumAbsRes / samples.size()));
        }
    }

    // 4. Evaluate fitted background polynomial and subtract from source
    if (progress) progress(progressStart + (progressEnd - progressStart) * 65 / 100);
    Logger::info("BackgroundExtraction", QString("  Evaluating background model and performing subtraction..."));
    
    auto outBuffer = std::make_shared<ImageBuffer>(w, h);
    float* outData = outBuffer->data();

    // Cache terms mapping for rows/cols in full image
    std::vector<double> x_norms(w);
    for (int x = 0; x < w; ++x) x_norms[x] = (2.0 * x - w) / w;

    #pragma omp parallel for
    for (int y = 0; y < h; ++y) {
        double y_norm = (2.0 * y - h) / h;
        for (int x = 0; x < w; ++x) {
            double x_norm = x_norms[x];
            std::vector<double> terms = getPolynomialTerms(x_norm, y_norm, order);
            
            double bkgVal = 0.0;
            for (int i = 0; i < numTerms; ++i) {
                bkgVal += terms[i] * coeffs[i];
            }

            outData[y * w + x] = data[y * w + x] - static_cast<float>(bkgVal);
        }
    }

    // 5. Shift background-subtracted channel to a safe floor (neutral background peak)
    if (equalize) {
        if (progress) progress(progressStart + (progressEnd - progressStart) * 85 / 100);
        Logger::info("BackgroundExtraction", QString("  Performing background channel equalization..."));
        
        std::vector<float> outSorted(outData, outData + numPixels);
        std::sort(outSorted.begin(), outSorted.end());
        double outMedian = outSorted[numPixels / 2];

        double outSumSq = 0.0;
        for (int i = 0; i < numPixels; ++i) {
            double diff = outData[i] - outMedian;
            outSumSq += diff * diff;
        }
        double outStd = std::sqrt(outSumSq / numPixels);

        // Safe floor offset
        float offset = -outMedian + sigmaCut * outStd;
        Logger::info("BackgroundExtraction", QString("    Equalization complete: Shifted by offset = %1 to pedestal %2")
                     .arg(offset).arg(sigmaCut * outStd));
        
        #pragma omp parallel for
        for (int i = 0; i < numPixels; ++i) {
            outData[i] += offset;
        }
    }

    if (progress) progress(progressEnd);
    return std::make_shared<GrayscaleImage>(outBuffer);
}

RGBImagePtr BackgroundExtractor::extractRGB(RGBImagePtr src,
                                            int order,
                                            double sigmaCut,
                                            double sampleFrac,
                                            double huberDelta,
                                            bool equalize,
                                            ProgressCallback progress) {
    if (!src) return nullptr;

    // 1. Process each channel independently WITHOUT individual equalization
    Logger::info("BackgroundExtraction", "Processing Red channel background extraction...");
    auto extR = extractGrayscale(src->r(), order, sigmaCut, sampleFrac, huberDelta, false, progress, 5, 33);
    
    Logger::info("BackgroundExtraction", "Processing Green channel background extraction...");
    auto extG = extractGrayscale(src->g(), order, sigmaCut, sampleFrac, huberDelta, false, progress, 33, 66);
    
    Logger::info("BackgroundExtraction", "Processing Blue channel background extraction...");
    auto extB = extractGrayscale(src->b(), order, sigmaCut, sampleFrac, huberDelta, false, progress, 66, 90);

    // 2. If equalize is true, apply coordinated equalization to neutralize the background
    if (equalize) {
        if (progress) progress(93);
        Logger::info("BackgroundExtraction", "Performing coordinated RGB background equalization...");
        
        int w = src->width();
        int h = src->height();
        int numPixels = w * h;

        auto getStats = [numPixels](GrayscaleImagePtr img, double& median, double& stddev) {
            const float* data = img->buffer()->data();
            std::vector<float> sorted(data, data + numPixels);
            std::sort(sorted.begin(), sorted.end());
            median = sorted[numPixels / 2];

            double sumSq = 0.0;
            for (int i = 0; i < numPixels; ++i) {
                double diff = data[i] - median;
                sumSq += diff * diff;
            }
            stddev = std::sqrt(sumSq / numPixels);
        };

        double medR, stdR;
        double medG, stdG;
        double medB, stdB;

        getStats(extR, medR, stdR);
        getStats(extG, medG, stdG);
        getStats(extB, medB, stdB);

        // Common pedestal based on the maximum noise standard deviation of the channels
        double maxStd = std::max({stdR, stdG, stdB});
        float commonPedestal = static_cast<float>(sigmaCut * maxStd);

        // Coordinated offsets to shift all channel medians to the exact same pedestal
        float offsetR = static_cast<float>(-medR + commonPedestal);
        float offsetG = static_cast<float>(-medG + commonPedestal);
        float offsetB = static_cast<float>(-medB + commonPedestal);

        Logger::info("BackgroundExtraction", QString("  Coordinated Equalization stats: R_med=%1, G_med=%2, B_med=%3, common_pedestal=%4")
                     .arg(medR).arg(medG).arg(medB).arg(commonPedestal));
        Logger::info("BackgroundExtraction", QString("  Applying offsets: R_offset=%1, G_offset=%2, B_offset=%3")
                     .arg(offsetR).arg(offsetG).arg(offsetB));

        float* dataR = extR->buffer()->data();
        float* dataG = extG->buffer()->data();
        float* dataB = extB->buffer()->data();

        #pragma omp parallel for
        for (int i = 0; i < numPixels; ++i) {
            dataR[i] += offsetR;
            dataG[i] += offsetG;
            dataB[i] += offsetB;
        }
    }

    if (progress) progress(100);
    return std::make_shared<RGBImage>(extR, extG, extB);
}

} // namespace blastro
