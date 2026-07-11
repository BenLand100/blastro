/*
 * BLastro - Astronomical Image Processing Software
 * Copyright (C) 2026 Benjamin Land
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "BackgroundExtractor.h"
#include "core/MathUtils.h"
#include <cmath>
#include <random>
#include <algorithm>
#include <limits>
#include <numeric>
#include <stdexcept>
#include <iostream>
#include "core/Logger.h"

namespace blastro {

struct FitPoint {
    double x_norm;
    double y_norm;
    double z;
};

static std::vector<double> getPolynomialTerms(double x, double y, int order) {
    std::vector<double> terms;
    for (int i = 0; i <= order; ++i) {
        for (int j = 0; j <= order; ++j) {
            if (i + j <= order) {
                terms.push_back(std::pow(x, i) * std::pow(y, j));
            }
        }
    }
    return terms;
}

static bool getGlobalStats(GrayscaleImagePtr img, double& median, double& sn) {
    if (!img) return false;
    int w = img->width();
    int h = img->height();
    int numPixels = w * h;
    const float* data = img->buffer()->data();
    MathUtils::estimateImageStats(data, numPixels, median, sn, 2000);
    return true;
}

static bool getLocalPatchStats(GrayscaleImagePtr img, int cx, int cy, int patchRadius, double& median, double& sn) {
    int w = img->width();
    int h = img->height();
    const float* data = img->buffer()->data();
    std::vector<float> patchValues;
    patchValues.reserve((2 * patchRadius + 1) * (2 * patchRadius + 1));
    for (int dy = -patchRadius; dy <= patchRadius; ++dy) {
        for (int dx = -patchRadius; dx <= patchRadius; ++dx) {
            int px = cx + dx;
            int py = cy + dy;
            if (px >= 0 && px < w && py >= 0 && py < h) {
                float val = data[py * w + px];
                if (!std::isnan(val)) {
                    patchValues.push_back(val);
                }
            }
        }
    }
    if (patchValues.empty()) return false;
    median = MathUtils::computeMedian(patchValues);
    sn = MathUtils::computeRousseeuwSn(patchValues);
    return true;
}

std::vector<std::pair<double, double>> BackgroundExtractor::generateGridPoints(
    GrayscaleImagePtr img,
    int cols,
    int rows,
    double maxDeviation,
    double maxStructure
) {
    std::vector<std::pair<double, double>> pts;
    if (!img) return pts;
    int w = img->width();
    int h = img->height();

    double globalMedian = 0.0, globalSn = 0.0;
    if (!getGlobalStats(img, globalMedian, globalSn)) {
        return pts;
    }

    double marginX = w * 0.08;
    double marginY = h * 0.08;
    double stepX = (w - 2 * marginX) / std::max(1, cols - 1);
    double stepY = (h - 2 * marginY) / std::max(1, rows - 1);

    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            double x = marginX + c * stepX;
            double y = marginY + r * stepY;

            double localMedian = 0.0, localSn = 0.0;
            if (getLocalPatchStats(img, static_cast<int>(std::round(x)), static_cast<int>(std::round(y)), 15, localMedian, localSn)) {
                double thresholdVal = globalMedian + maxDeviation * globalSn;
                if (localMedian > thresholdVal) {
                    continue; // Exclude
                }
                if (localSn > maxStructure * globalSn) {
                    continue; // Exclude
                }
            } else {
                continue; // Exclude if patch has no valid data
            }

            pts.push_back({x, y});
        }
    }
    return pts;
}

GrayscaleImagePtr BackgroundExtractor::extractGrayscale(GrayscaleImagePtr src,
                                                       int order,
                                                       int gridSize,
                                                       double huberDelta,
                                                       bool normalize,
                                                       ProgressCallback progress,
                                                       int progressStart,
                                                       int progressEnd,
                                                       const std::string& method,
                                                       double rbfSmoothing,
                                                       const std::vector<std::pair<double, double>>& customControlPoints,
                                                       double maxDeviation,
                                                       double maxStructure,
                                                       const std::string& imageName) {
    if (!src) return nullptr;

    std::string prefix = imageName.empty() ? "" : "[" + imageName + "] ";

    int w = src->width();
    int h = src->height();
    int numPixels = w * h;
    const float* data = src->buffer()->data();

    std::vector<FitPoint> samples;
    const auto& ctrlPts = !customControlPoints.empty() ? customControlPoints : src->buffer()->bgeControlPoints();
    if (!ctrlPts.empty()) {
        if (progress) progress(progressStart + (progressEnd - progressStart) * 10 / 100);
        Logger::info("BackgroundExtraction", QString("%1Using %2 control points for background fitting...").arg(QString::fromStdString(prefix)).arg(ctrlPts.size()));
        for (const auto& pt : ctrlPts) {
            int cx = static_cast<int>(std::round(pt.first));
            int cy = static_cast<int>(std::round(pt.second));
            double sumVal = 0.0;
            int count = 0;
            for (int dy = -2; dy <= 2; ++dy) {
                for (int dx = -2; dx <= 2; ++dx) {
                    int px = cx + dx;
                    int py = cy + dy;
                    if (px >= 0 && px < w && py >= 0 && py < h) {
                        float val = src->buffer()->pixel(px, py);
                        if (!std::isnan(val)) {
                            sumVal += val;
                            count++;
                        }
                    }
                }
            }
            double zVal = std::numeric_limits<double>::quiet_NaN();
            if (count > 0) {
                zVal = sumVal / count;
            } else {
                float centerVal = src->buffer()->pixel(cx, cy);
                if (!std::isnan(centerVal)) {
                    zVal = centerVal;
                }
            }

            if (!std::isnan(zVal)) {
                FitPoint fpt;
                fpt.x_norm = (2.0 * pt.first - w) / w;
                fpt.y_norm = (2.0 * pt.second - h) / h;
                fpt.z = zVal;
                samples.push_back(fpt);
            }
        }
    } else {
        if (progress) progress(progressStart + (progressEnd - progressStart) * 5 / 100);
        Logger::info("BackgroundExtraction", QString("%1Auto-generating sample grid of size %2x%2...").arg(QString::fromStdString(prefix)).arg(gridSize));

        std::vector<std::pair<double, double>> pts = generateGridPoints(src, gridSize, gridSize, maxDeviation, maxStructure);

        for (const auto& pt : pts) {
            int cx = static_cast<int>(std::round(pt.first));
            int cy = static_cast<int>(std::round(pt.second));
            double sumVal = 0.0;
            int count = 0;
            for (int dy = -2; dy <= 2; ++dy) {
                for (int dx = -2; dx <= 2; ++dx) {
                    int px = cx + dx;
                    int py = cy + dy;
                    if (px >= 0 && px < w && py >= 0 && py < h) {
                        float val = data[py * w + px];
                        if (!std::isnan(val)) {
                            sumVal += val;
                            count++;
                        }
                    }
                }
            }
            double zVal = std::numeric_limits<double>::quiet_NaN();
            if (count > 0) {
                zVal = sumVal / count;
            } else {
                float centerVal = data[cy * w + cx];
                if (!std::isnan(centerVal)) {
                    zVal = centerVal;
                }
            }

            if (!std::isnan(zVal)) {
                FitPoint fpt;
                fpt.x_norm = (2.0 * pt.first - w) / w;
                fpt.y_norm = (2.0 * pt.second - h) / h;
                fpt.z = zVal;
                samples.push_back(fpt);
            }
        }
    }

    if (samples.empty()) {
        throw std::runtime_error("No valid background sample points found");
    }
    Logger::info("BackgroundExtraction", QString("%1Selected %2 sample points for robust fitting (grid_size = %3).").arg(QString::fromStdString(prefix)).arg(samples.size()).arg(gridSize));

    auto outBuffer = std::make_shared<ImageBuffer>(w, h);
    float* outData = outBuffer->data();

    if (method == "RBF") {
        Logger::info("BackgroundExtraction", QString("%1Solving robust Thin Plate Spline RBF fit (%2 centers, smoothing = %3)...")
                     .arg(QString::fromStdString(prefix)).arg(samples.size()).arg(rbfSmoothing));

        int n = samples.size();
        int m = n + 3;
        std::vector<double> weights(n, 1.0);
        std::vector<double> rbfCoeffs(m, 0.0);

        const int maxIRLSIterations = 8;
        for (int iter = 0; iter < maxIRLSIterations; ++iter) {
            if (progress) {
                int stepProgress = progressStart + (progressEnd - progressStart) * (20 + 5 * iter) / 100;
                progress(stepProgress);
            }

            std::vector<double> A(m * m, 0.0);
            std::vector<double> B(m, 0.0);

            for (int i = 0; i < n; ++i) {
                for (int j = 0; j < n; ++j) {
                    double dx = samples[i].x_norm - samples[j].x_norm;
                    double dy = samples[i].y_norm - samples[j].y_norm;
                    double r2 = dx*dx + dy*dy;
                    double val = 0.0;
                    if (r2 > 0.0) {
                        val = 0.5 * r2 * std::log(r2);
                    }
                    A[i * m + j] = val;
                }

                double reg = rbfSmoothing;
                if (weights[i] > 1e-6) {
                    reg /= weights[i];
                } else {
                    reg = 1e6;
                }
                A[i * m + i] += reg;
            }

            for (int i = 0; i < n; ++i) {
                double xi = samples[i].x_norm;
                double yi = samples[i].y_norm;

                A[i * m + n] = 1.0;
                A[i * m + n + 1] = xi;
                A[i * m + n + 2] = yi;

                A[n * m + i] = 1.0;
                A[(n + 1) * m + i] = xi;
                A[(n + 2) * m + i] = yi;
            }

            for (int i = 0; i < n; ++i) {
                B[i] = samples[i].z;
            }

            auto nextCoeffs = MathUtils::solveGaussianElimination(A, B, m);
            if (nextCoeffs.empty()) {
                Logger::warning("BackgroundExtraction", QString("%1RBF IRLS Iteration %2: Linear system solve failed, stopping early.").arg(QString::fromStdString(prefix)).arg(iter + 1));
                break;
            }
            rbfCoeffs = nextCoeffs;

            for (int i = 0; i < n; ++i) {
                double xi = samples[i].x_norm;
                double yi = samples[i].y_norm;

                double fitVal = rbfCoeffs[n] + rbfCoeffs[n + 1] * xi + rbfCoeffs[n + 2] * yi;
                for (int j = 0; j < n; ++j) {
                    double dx = xi - samples[j].x_norm;
                    double dy = yi - samples[j].y_norm;
                    double r2 = dx*dx + dy*dy;
                    if (r2 > 0.0) {
                        fitVal += rbfCoeffs[j] * 0.5 * r2 * std::log(r2);
                    }
                }

                double residual = samples[i].z - fitVal;
                double absRes = std::abs(residual);

                if (absRes <= huberDelta) {
                    weights[i] = 1.0;
                } else {
                    weights[i] = huberDelta / absRes;
                }
            }
        }

        if (progress) progress(progressStart + (progressEnd - progressStart) * 65 / 100);
        Logger::info("BackgroundExtraction", QString("%1Evaluating RBF background model and performing subtraction...").arg(QString::fromStdString(prefix)));

        std::vector<double> x_norms(w);
        for (int x = 0; x < w; ++x) x_norms[x] = (2.0 * x - w) / w;

        double c0 = rbfCoeffs[n];
        double c1 = rbfCoeffs[n + 1];
        double c2 = rbfCoeffs[n + 2];

        std::vector<double> sx(n), sy(n), half_coeffs(n);
        for (int i = 0; i < n; ++i) {
            sx[i] = samples[i].x_norm;
            sy[i] = samples[i].y_norm;
            half_coeffs[i] = rbfCoeffs[i] * 0.5;
        }

        constexpr int LUT_SIZE = 1048576;
        constexpr double MAX_R2 = 8.0;
        constexpr double R2_LUT_MULT = (LUT_SIZE - 1) / MAX_R2;
        std::vector<double> r2_log_lut(LUT_SIZE);
        for (int i = 0; i < LUT_SIZE; ++i) {
            double r2 = i * MAX_R2 / (LUT_SIZE - 1);
            r2_log_lut[i] = (r2 > 0.0) ? r2 * std::log(r2) : 0.0;
        }

        #pragma omp parallel
        {
            std::vector<double> dy2(n);
            #pragma omp for
            for (int y = 0; y < h; ++y) {
                double y_norm = (2.0 * y - h) / h;
                for (int i = 0; i < n; ++i) {
                    double dy = y_norm - sy[i];
                    dy2[i] = dy * dy;
                }
                for (int x = 0; x < w; ++x) {
                    double x_norm = x_norms[x];
                    double bkgVal = c0 + c1 * x_norm + c2 * y_norm;
                    for (int i = 0; i < n; ++i) {
                        double dx = x_norm - sx[i];
                        double r2 = dx * dx + dy2[i];
                        int idx = static_cast<int>(r2 * R2_LUT_MULT);
                        if (idx >= 0 && idx < LUT_SIZE) {
                            bkgVal += half_coeffs[i] * r2_log_lut[idx];
                        } else if (r2 > 0.0) {
                            bkgVal += half_coeffs[i] * r2 * std::log(r2);
                        }
                    }
                    outData[y * w + x] = data[y * w + x] - static_cast<float>(bkgVal);
                }
            }
        }
    } else {
        int numTerms = 0;
        for (int i = 0; i <= order; ++i) {
            for (int j = 0; j <= order; ++j) {
                if (i + j <= order) numTerms++;
            }
        }

        Logger::info("BackgroundExtraction", QString("%1Solving robust 2D polynomial surface fit (order = %2, %3 terms)...").arg(QString::fromStdString(prefix)).arg(order).arg(numTerms));
        std::vector<double> coeffs(numTerms, 0.0);
        std::vector<double> weights(samples.size(), 1.0);

        const int maxIRLSIterations = 8;
        for (int iter = 0; iter < maxIRLSIterations; ++iter) {
            if (progress) {
                int stepProgress = progressStart + (progressEnd - progressStart) * (20 + 5 * iter) / 100;
                progress(stepProgress);
            }

            std::vector<double> A(numTerms * numTerms, 0.0);
            std::vector<double> B(numTerms, 0.0);

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

            auto nextCoeffs = MathUtils::solveCholesky(A, B, numTerms);
            if (nextCoeffs.empty()) {
                Logger::warning("BackgroundExtraction", QString("%1IRLS Iteration %2: Cholesky decomposition failed or became unstable, stopping early.").arg(QString::fromStdString(prefix)).arg(iter + 1));
                break; 
            }
            coeffs = nextCoeffs;

            for (size_t p = 0; p < samples.size(); ++p) {
                const auto& pt = samples[p];
                std::vector<double> terms = getPolynomialTerms(pt.x_norm, pt.y_norm, order);
                double fitVal = 0.0;
                for (int i = 0; i < numTerms; ++i) {
                    fitVal += terms[i] * coeffs[i];
                }

                double residual = pt.z - fitVal;
                double absRes = std::abs(residual);

                if (absRes <= huberDelta) {
                    weights[p] = 1.0;
                } else {
                    weights[p] = huberDelta / absRes;
                }
            }
        }

        if (progress) progress(progressStart + (progressEnd - progressStart) * 65 / 100);
        Logger::info("BackgroundExtraction", QString("%1Evaluating background model and performing subtraction...").arg(QString::fromStdString(prefix)));

        std::vector<std::vector<double>> x_pows(w, std::vector<double>(order + 1, 1.0));
        for (int x = 0; x < w; ++x) {
            double x_norm = (2.0 * x - w) / w;
            for (int i = 1; i <= order; ++i) {
                x_pows[x][i] = x_pows[x][i - 1] * x_norm;
            }
        }

        #pragma omp parallel
        {
            std::vector<double> y_pow(order + 1, 1.0);
            #pragma omp for
            for (int y = 0; y < h; ++y) {
                double y_norm = (2.0 * y - h) / h;
                for (int j = 1; j <= order; ++j) {
                    y_pow[j] = y_pow[j - 1] * y_norm;
                }

                for (int x = 0; x < w; ++x) {
                    double bkgVal = 0.0;
                    int k = 0;
                    for (int i = 0; i <= order; ++i) {
                        double xp = x_pows[x][i];
                        for (int j = 0; j <= order - i; ++j) {
                            bkgVal += coeffs[k++] * xp * y_pow[j];
                        }
                    }
                    outData[y * w + x] = data[y * w + x] - static_cast<float>(bkgVal);
                }
            }
        }
    }

    if (normalize) {
        if (progress) progress(progressStart + (progressEnd - progressStart) * 85 / 100);
        
        std::vector<float> inSample;
        std::vector<float> outSample;
        inSample.reserve(100000);
        outSample.reserve(100000);
        int stride = std::max(1, numPixels / 100000);
        for (int i = 0; i < numPixels; i += stride) {
            if (!std::isnan(data[i])) {
                inSample.push_back(data[i]);
            }
            if (!std::isnan(outData[i])) {
                outSample.push_back(outData[i]);
            }
        }
        if (!inSample.empty() && !outSample.empty()) {
            double medianIn = MathUtils::computeMedian(inSample);
            double medianOut = MathUtils::computeMedian(outSample);
            float restoreOffset = static_cast<float>(medianIn - medianOut);
            
            Logger::info("BackgroundExtraction", QString("%1Normalized: median_in=%2, median_out=%3, offset=%4")
                         .arg(QString::fromStdString(prefix)).arg(medianIn).arg(medianOut).arg(restoreOffset));

            #pragma omp parallel for
            for (int i = 0; i < numPixels; ++i) {
                outData[i] += restoreOffset;
            }
        }
    }

    if (progress) progress(progressEnd);
    return std::make_shared<GrayscaleImage>(outBuffer);
}

RGBImagePtr BackgroundExtractor::extractRGB(RGBImagePtr src,
                                            int order,
                                            int gridSize,
                                            double huberDelta,
                                            bool normalize,
                                            ProgressCallback progress,
                                            const std::string& method,
                                            double rbfSmoothing,
                                            const std::vector<std::pair<double, double>>& customControlPoints,
                                            double maxDeviation,
                                            double maxStructure,
                                            const std::string& imageName) {
    if (!src) return nullptr;

    GrayscaleImagePtr extR, extG, extB;

    #pragma omp parallel sections
    {
        #pragma omp section
        {
            extR = extractGrayscale(src->r(), order, gridSize, huberDelta, normalize, nullptr, 0, 100, method, rbfSmoothing, customControlPoints, maxDeviation, maxStructure, imageName.empty() ? "Red" : imageName + " [Red]");
        }
        #pragma omp section
        {
            extG = extractGrayscale(src->g(), order, gridSize, huberDelta, normalize, nullptr, 0, 100, method, rbfSmoothing, customControlPoints, maxDeviation, maxStructure, imageName.empty() ? "Green" : imageName + " [Green]");
        }
        #pragma omp section
        {
            extB = extractGrayscale(src->b(), order, gridSize, huberDelta, normalize, nullptr, 0, 100, method, rbfSmoothing, customControlPoints, maxDeviation, maxStructure, imageName.empty() ? "Blue" : imageName + " [Blue]");
        }
    }

    if (progress) progress(100);
    return std::make_shared<RGBImage>(extR, extG, extB);
}

} // namespace blastro
