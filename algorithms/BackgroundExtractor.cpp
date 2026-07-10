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

std::vector<std::pair<double, double>> BackgroundExtractor::generateSamplePoints(
    GrayscaleImagePtr img,
    double sigmaCut,
    double sampleFrac
) {
    std::vector<std::pair<double, double>> pts;
    if (!img) return pts;
    int w = img->width();
    int h = img->height();
    int numPixels = w * h;
    const float* data = img->buffer()->data();

    std::vector<float> sortedData;
    sortedData.reserve(numPixels);
    for (int i = 0; i < numPixels; ++i) {
        if (!std::isnan(data[i])) {
            sortedData.push_back(data[i]);
        }
    }
    if (sortedData.empty()) return pts;
    std::sort(sortedData.begin(), sortedData.end());
    double median = sortedData[sortedData.size() / 2];

    double sumSqDiff = 0.0;
    int validPixels = 0;
    for (int i = 0; i < numPixels; ++i) {
        float val = data[i];
        if (!std::isnan(val)) {
            double diff = val - median;
            sumSqDiff += diff * diff;
            validPixels++;
        }
    }
    double stddev = std::sqrt(sumSqDiff / (validPixels > 1 ? validPixels : 1));
    double lowBound = median - sigmaCut * stddev;
    double highBound = median + sigmaCut * stddev;

    std::minstd_rand rng(1337);
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            float val = data[y * w + x];
            if (val >= lowBound && val <= highBound) {
                if (dist(rng) < sampleFrac) {
                    pts.push_back({static_cast<double>(x), static_cast<double>(y)});
                }
            }
        }
    }
    return pts;
}

GrayscaleImagePtr BackgroundExtractor::extractGrayscale(GrayscaleImagePtr src,
                                                       int order,
                                                       double sigmaCut,
                                                       double sampleFrac,
                                                       double huberDelta,
                                                       bool equalize,
                                                       ProgressCallback progress,
                                                       int progressStart,
                                                       int progressEnd,
                                                       const std::string& method,
                                                       double rbfSmoothing,
                                                       bool restoreMedian,
                                                       const std::vector<std::pair<double, double>>& customControlPoints) {
    if (!src) return nullptr;

    int w = src->width();
    int h = src->height();
    int numPixels = w * h;
    const float* data = src->buffer()->data();

    std::vector<FitPoint> samples;
    const auto& ctrlPts = !customControlPoints.empty() ? customControlPoints : src->buffer()->bgeControlPoints();
    if (!ctrlPts.empty()) {
        if (progress) progress(progressStart + (progressEnd - progressStart) * 10 / 100);
        Logger::info("BackgroundExtraction", QString("  Using %1 control points for background fitting...").arg(ctrlPts.size()));
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
        Logger::info("BackgroundExtraction", QString("Analyzing image data statistics (%1x%2 pixels)...").arg(w).arg(h));
        
        std::vector<float> sortedData;
        sortedData.reserve(numPixels);
        for (int i = 0; i < numPixels; ++i) {
            if (!std::isnan(data[i])) {
                sortedData.push_back(data[i]);
            }
        }
        if (sortedData.empty()) {
            throw std::runtime_error("No valid pixels in the image channel for background extraction");
        }
        std::sort(sortedData.begin(), sortedData.end());
        int validCount = sortedData.size();
        double median = sortedData[validCount / 2];

        double sumSqDiff = 0.0;
        int validPixels = 0;
        for (int i = 0; i < numPixels; ++i) {
            float val = data[i];
            if (!std::isnan(val)) {
                double diff = val - median;
                sumSqDiff += diff * diff;
                validPixels++;
            }
        }
        double stddev = std::sqrt(sumSqDiff / (validPixels > 1 ? validPixels : 1));
        Logger::info("BackgroundExtraction", QString("  Channel Stats: Median = %1, StdDev = %2").arg(median).arg(stddev));

        if (progress) progress(progressStart + (progressEnd - progressStart) * 15 / 100);
        double lowBound = median - sigmaCut * stddev;
        double highBound = median + sigmaCut * stddev;
        Logger::info("BackgroundExtraction", QString("  Selecting background sample points in range [%1, %2] (sigma_cut = %3)...")
                     .arg(lowBound).arg(highBound).arg(sigmaCut));

        std::minstd_rand rng(1337);
        std::uniform_real_distribution<double> dist(0.0, 1.0);

        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                float val = data[y * w + x];
                if (val >= lowBound && val <= highBound) {
                    if (dist(rng) < sampleFrac) {
                        FitPoint pt;
                        pt.x_norm = (2.0 * x - w) / w;
                        pt.y_norm = (2.0 * y - h) / h;
                        pt.z = val;
                        samples.push_back(pt);
                    }
                }
            }
        }
    }

    if (samples.empty()) {
        throw std::runtime_error("No valid background sample points found");
    }
    Logger::info("BackgroundExtraction", QString("  Selected %1 sample points for robust fitting (sample_frac = %2).").arg(samples.size()).arg(sampleFrac));

    auto outBuffer = std::make_shared<ImageBuffer>(w, h);
    float* outData = outBuffer->data();

    if (method == "RBF") {
        Logger::info("BackgroundExtraction", QString("  Solving robust Thin Plate Spline RBF fit (%1 centers, smoothing = %2, max IRLS iterations = 8)...")
                     .arg(samples.size()).arg(rbfSmoothing));

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
                Logger::warning("BackgroundExtraction", QString("    RBF IRLS Iteration %1: Linear system solve failed, stopping early.").arg(iter + 1));
                break;
            }
            rbfCoeffs = nextCoeffs;

            double sumAbsRes = 0.0;
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
                sumAbsRes += absRes;

                if (absRes <= huberDelta) {
                    weights[i] = 1.0;
                } else {
                    weights[i] = huberDelta / absRes;
                }
            }

            if (iter == 0 || iter == maxIRLSIterations - 1) {
                Logger::info("BackgroundExtraction", QString("    RBF IRLS Iteration %1/8: Mean Absolute Residual = %2")
                             .arg(iter + 1).arg(sumAbsRes / n));
            }
        }

        if (progress) progress(progressStart + (progressEnd - progressStart) * 65 / 100);
        Logger::info("BackgroundExtraction", QString("  Evaluating RBF background model and performing subtraction..."));

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

        #pragma omp parallel for
        for (int y = 0; y < h; ++y) {
            double y_norm = (2.0 * y - h) / h;
            for (int x = 0; x < w; ++x) {
                double x_norm = x_norms[x];

                double bkgVal = c0 + c1 * x_norm + c2 * y_norm;
                for (int i = 0; i < n; ++i) {
                    double dx = x_norm - sx[i];
                    double dy = y_norm - sy[i];
                    double r2 = dx*dx + dy*dy;
                    if (r2 > 0.0) {
                        bkgVal += half_coeffs[i] * r2 * std::log(r2);
                    }
                }

                outData[y * w + x] = data[y * w + x] - static_cast<float>(bkgVal);
            }
        }
    } else {
        int numTerms = 0;
        for (int i = 0; i <= order; ++i) {
            for (int j = 0; j <= order; ++j) {
                if (i + j <= order) numTerms++;
            }
        }

        Logger::info("BackgroundExtraction", QString("  Solving robust 2D polynomial surface fit (order = %1, %2 terms, max IRLS iterations = 8)...").arg(order).arg(numTerms));
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
                Logger::warning("BackgroundExtraction", QString("    IRLS Iteration %1: Cholesky decomposition failed or became unstable, stopping early.").arg(iter + 1));
                break; 
            }
            coeffs = nextCoeffs;

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

        if (progress) progress(progressStart + (progressEnd - progressStart) * 65 / 100);
        Logger::info("BackgroundExtraction", QString("  Evaluating background model and performing subtraction..."));

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
    }

    if (restoreMedian) {
        if (progress) progress(progressStart + (progressEnd - progressStart) * 85 / 100);
        
        double sumIn = 0.0;
        long countIn = 0;
        for (int i = 0; i < numPixels; ++i) {
            if (!std::isnan(data[i])) {
                sumIn += data[i];
                countIn++;
            }
        }
        double meanIn = countIn > 0 ? (sumIn / countIn) : 0.0;

        double sumOut = 0.0;
        long countOut = 0;
        for (int i = 0; i < numPixels; ++i) {
            if (!std::isnan(outData[i])) {
                sumOut += outData[i];
                countOut++;
            }
        }
        double meanOut = countOut > 0 ? (sumOut / countOut) : 0.0;

        float restoreOffset = static_cast<float>(meanIn - meanOut);
        Logger::info("BackgroundExtraction", QString("  Restored Mean: mean_in=%1, mean_out=%2, offset=%3")
                     .arg(meanIn).arg(meanOut).arg(restoreOffset));

        #pragma omp parallel for
        for (int i = 0; i < numPixels; ++i) {
            outData[i] += restoreOffset;
        }
    } else if (equalize) {
        if (progress) progress(progressStart + (progressEnd - progressStart) * 85 / 100);
        Logger::info("BackgroundExtraction", QString("  Performing background channel equalization..."));

        std::vector<float> outSorted;
        outSorted.reserve(numPixels);
        for (int i = 0; i < numPixels; ++i) {
            if (!std::isnan(outData[i])) {
                outSorted.push_back(outData[i]);
            }
        }
        if (outSorted.empty()) {
            throw std::runtime_error("No valid pixels in the background-subtracted channel for equalization");
        }
        std::sort(outSorted.begin(), outSorted.end());
        int validCount = outSorted.size();
        double outMedian = outSorted[validCount / 2];

        double outSumSq = 0.0;
        int validPixels = 0;
        for (int i = 0; i < numPixels; ++i) {
            float val = outData[i];
            if (!std::isnan(val)) {
                double diff = val - outMedian;
                outSumSq += diff * diff;
                validPixels++;
            }
        }
        double outStd = std::sqrt(outSumSq / (validPixels > 1 ? validPixels : 1));

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
                                            ProgressCallback progress,
                                            const std::string& method,
                                            double rbfSmoothing,
                                            bool restoreMedian,
                                            const std::vector<std::pair<double, double>>& customControlPoints) {
    if (!src) return nullptr;

    Logger::info("BackgroundExtraction", "Processing Red channel background extraction...");
    auto extR = extractGrayscale(src->r(), order, sigmaCut, sampleFrac, huberDelta, equalize && !restoreMedian, progress, 5, 33, method, rbfSmoothing, restoreMedian, customControlPoints);
    
    Logger::info("BackgroundExtraction", "Processing Green channel background extraction...");
    auto extG = extractGrayscale(src->g(), order, sigmaCut, sampleFrac, huberDelta, equalize && !restoreMedian, progress, 33, 66, method, rbfSmoothing, restoreMedian, customControlPoints);
    
    Logger::info("BackgroundExtraction", "Processing Blue channel background extraction...");
    auto extB = extractGrayscale(src->b(), order, sigmaCut, sampleFrac, huberDelta, equalize && !restoreMedian, progress, 66, 90, method, rbfSmoothing, restoreMedian, customControlPoints);

    if (equalize && !restoreMedian) {
        if (progress) progress(93);
        Logger::info("BackgroundExtraction", "Performing coordinated RGB background equalization...");
        
        int w = src->width();
        int h = src->height();
        int numPixels = w * h;

        auto getStats = [numPixels](GrayscaleImagePtr img, double& median, double& stddev) {
            const float* data = img->buffer()->data();
            std::vector<float> sorted;
            sorted.reserve(numPixels);
            for (int i = 0; i < numPixels; ++i) {
                if (!std::isnan(data[i])) {
                    sorted.push_back(data[i]);
                }
            }
            if (sorted.empty()) {
                median = 0.0;
                stddev = 0.0;
                return;
            }
            std::sort(sorted.begin(), sorted.end());
            int validCount = sorted.size();
            median = sorted[validCount / 2];

            double sumSq = 0.0;
            int validPixels = 0;
            for (int i = 0; i < numPixels; ++i) {
                float val = data[i];
                if (!std::isnan(val)) {
                    double diff = val - median;
                    sumSq += diff * diff;
                    validPixels++;
                }
            }
            stddev = std::sqrt(sumSq / (validPixels > 1 ? validPixels : 1));
        };

        double medR, stdR;
        double medG, stdG;
        double medB, stdB;

        getStats(extR, medR, stdR);
        getStats(extG, medG, stdG);
        getStats(extB, medB, stdB);

        double maxStd = std::max({stdR, stdG, stdB});
        float commonPedestal = static_cast<float>(sigmaCut * maxStd);

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
