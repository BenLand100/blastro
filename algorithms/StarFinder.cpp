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

#include "StarFinder.h"
#include "Solver.h"
#include <cmath>
#include <algorithm>
#include <numeric>
#include <stdexcept>
#include <vector>
#include <iostream>

namespace blastro {

// 1D separated Gaussian blur helper
static void gaussianBlur2D(const std::vector<float>& input, std::vector<float>& output, int w, int h, double sigma) {
    int radius = std::ceil(3.0 * sigma);
    std::vector<float> kernel(2 * radius + 1);
    float sum = 0.0f;
    for (int i = -radius; i <= radius; ++i) {
        kernel[i + radius] = std::exp(- (i * i) / (2.0f * sigma * sigma));
        sum += kernel[i + radius];
    }
    for (float& val : kernel) val /= sum;

    std::vector<float> temp(w * h, 0.0f);

    // Horizontal blur
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            float val = 0.0f;
            for (int k = -radius; k <= radius; ++k) {
                int kx = std::clamp(x + k, 0, w - 1);
                val += kernel[k + radius] * input[y * w + kx];
            }
            temp[y * w + x] = val;
        }
    }

    // Vertical blur
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            float val = 0.0f;
            for (int k = -radius; k <= radius; ++k) {
                int ky = std::clamp(y + k, 0, h - 1);
                val += kernel[k + radius] * temp[ky * w + x];
            }
            output[y * w + x] = val;
        }
    }
}

// 2D Gaussian profile helper
static double evaluateGaussian(double x, double y, double mean_x, double mean_y,
                                double sig_x, double sig_y, double theta, double A, double B) {
    double dx = x - mean_x;
    double dy = y - mean_y;
    double cost = std::cos(theta);
    double sint = std::sin(theta);
    double rot_x = dx * cost - dy * sint;
    double rot_y = dy * cost + dx * sint;

    if (sig_x < 1e-5) sig_x = 1e-5;
    if (sig_y < 1e-5) sig_y = 1e-5;

    return A * std::exp(- (rot_x * rot_x) / (2.0 * sig_x * sig_x) - (rot_y * rot_y) / (2.0 * sig_y * sig_y)) + B;
}

static double estimateBackgroundNoise(const float* data, int numPixels) {
    // Sample a subset of pixels to be fast (up to 20000 pixels)
    int sampleSize = std::min(numPixels, 20000);
    std::vector<float> sample(sampleSize);
    int step = numPixels / sampleSize;
    for (int i = 0; i < sampleSize; ++i) {
        sample[i] = data[i * step];
    }
    
    // Compute median
    std::sort(sample.begin(), sample.end());
    double median = sample[sampleSize / 2];
    
    // Compute MAD (Median Absolute Deviation)
    std::vector<double> absDiffs(sampleSize);
    for (int i = 0; i < sampleSize; ++i) {
        absDiffs[i] = std::abs(sample[i] - median);
    }
    std::sort(absDiffs.begin(), absDiffs.end());
    double mad = absDiffs[sampleSize / 2];
    
    // For normal distribution, sigma = 1.4826 * MAD
    double sigma = 1.4826 * mad;
    return sigma;
}

std::vector<Star> StarFinder::findStars(GrayscaleImagePtr img,
                                       int maxStars,
                                       double snrMin,
                                       bool fastFit,
                                       int patchRadius,
                                       double minFwhm,
                                       double maxEccentricity) {
    if (!img) {
        throw std::runtime_error("StarFinder input image is null");
    }

    int w = img->width();
    int h = img->height();
    int numPixels = w * h;
    const float* data = img->buffer()->data();

    // 1. Compute mean and standard deviation of the image
    double sum = 0.0;
    double sumSq = 0.0;
    float maxVal = -1e30f;
    for (int i = 0; i < numPixels; ++i) {
        float val = data[i];
        sum += val;
        sumSq += val * val;
        if (val > maxVal) maxVal = val;
    }
    double mean = sum / numPixels;
    double stddev = std::sqrt(std::max(0.0, (sumSq / numPixels) - (mean * mean)));
    double threshold = mean + 3.0 * stddev;

    // Robust estimate of background noise standard deviation for SNR calculations
    double globalNoiseSigma = std::max(estimateBackgroundNoise(data, numPixels), 1e-5);

    // 2. Identify candidate regions above mean + 3*stddev
    std::vector<float> reg_search(numPixels, 0.0f);
    for (int i = 0; i < numPixels; ++i) {
        if (data[i] > threshold) {
            reg_search[i] = 1.0f;
        }
    }

    // Apply double Gaussian blur (sigma = 3.0) to smooth peaks
    std::vector<float> blurred1(numPixels, 0.0f);
    std::vector<float> blurred2(numPixels, 0.0f);
    gaussianBlur2D(reg_search, blurred1, w, h, 3.0);
    gaussianBlur2D(blurred1, blurred2, w, h, 3.0);
    reg_search = blurred2;

    // Sort indices by blurred intensity descending
    std::vector<int> indices(numPixels);
    std::iota(indices.begin(), indices.end(), 0);
    std::sort(indices.begin(), indices.end(), [&](int a, int b) {
        return reg_search[a] > reg_search[b];
    });

    std::vector<Star> detectedStars;
    int attempts = 0;
    int maxAttempts = 2000;

    // Pre-calculate relative coordinate grid for patch fitting
    int patchDim = 2 * patchRadius + 1;
    std::vector<double> pxGrid(patchDim * patchDim);
    std::vector<double> pyGrid(patchDim * patchDim);
    for (int py = -patchRadius; py <= patchRadius; ++py) {
        for (int px = -patchRadius; px <= patchRadius; ++px) {
            int idx = (py + patchRadius) * patchDim + (px + patchRadius);
            pxGrid[idx] = px;
            pyGrid[idx] = py;
        }
    }

    for (int idx : indices) {
        if (detectedStars.size() >= static_cast<size_t>(maxStars) || attempts >= maxAttempts) {
            break;
        }

        // Skip if this region was already zeroed out (assigned to another star)
        if (reg_search[idx] == 0.0f) {
            continue;
        }

        int x_max = idx % w;
        int y_max = idx / w;

        // Check if patch is entirely within image bounds
        if (x_max - patchRadius >= 0 && x_max + patchRadius < w &&
            y_max - patchRadius >= 0 && y_max + patchRadius < h) {
            
            // Zero out patch region in search mask to avoid duplicate detection
            for (int py = -patchRadius; py <= patchRadius; ++py) {
                for (int px = -patchRadius; px <= patchRadius; ++px) {
                    reg_search[(y_max + py) * w + (x_max + px)] = 0.0f;
                }
            }

            attempts++;

            // Extract patch pixel values
            std::vector<double> patchValues(patchDim * patchDim);
            double patchMax = -1e30;
            double patchMin = 1e30;
            double patchSum = 0.0;
            for (int py = -patchRadius; py <= patchRadius; ++py) {
                for (int px = -patchRadius; px <= patchRadius; ++px) {
                    double val = data[(y_max + py) * w + (x_max + px)];
                    int pIdx = (py + patchRadius) * patchDim + (px + patchRadius);
                    patchValues[pIdx] = val;
                    if (val > patchMax) patchMax = val;
                    if (val < patchMin) patchMin = val;
                    patchSum += val;
                }
            }

            // SNR Check (noise-based)
            double amplitude = patchMax - patchMin;
            if (amplitude <= 0.0 || (amplitude / globalNoiseSigma) < snrMin) {
                continue;
            }

            double finalX = x_max;
            double finalY = y_max;
            double fwhm = 0.0;
            double star_lvl = 0.0;
            double bkg_lvl = 0.0;
            double eccentricity = 0.0;
            bool fitSuccess = false;

            if (fastFit) {
                // Centroiding (Fast) Mode
                bkg_lvl = patchMin;
                double sumWeights = 0.0;
                double sumX = 0.0;
                double sumY = 0.0;
                for (int i = 0; i < patchDim * patchDim; ++i) {
                    double wVal = std::max(0.0, patchValues[i] - bkg_lvl);
                    sumWeights += wVal;
                    sumX += pxGrid[i] * wVal;
                    sumY += pyGrid[i] * wVal;
                }
                
                if (sumWeights > 1e-5) {
                    finalX += sumX / sumWeights;
                    finalY += sumY / sumWeights;
                    star_lvl = patchMax - bkg_lvl;

                    // Rough FWHM estimation based on variance
                    double sumVar = 0.0;
                    double meanDx = sumX / sumWeights;
                    double meanDy = sumY / sumWeights;
                    for (int i = 0; i < patchDim * patchDim; ++i) {
                        double wVal = std::max(0.0, patchValues[i] - bkg_lvl);
                        double dx = pxGrid[i] - meanDx;
                        double dy = pyGrid[i] - meanDy;
                        sumVar += (dx * dx + dy * dy) * wVal;
                    }
                    double variance = sumVar / sumWeights;
                    fwhm = std::sqrt(8.0 * std::log(2.0) * (variance / 2.0)); // average over 2D standard deviation
                    eccentricity = 0.0; // centroiding doesn't compute orientation/eccentricity
                    fitSuccess = true;
                }
            } else {
                // 2D Gaussian Profile Fitting using Nelder-Mead solver
                // Parameters: mean_x, mean_y, sig_x, sig_y, theta, A, B
                std::vector<double> x0 = {
                    0.0,                              // mean_x
                    0.0,                              // mean_y
                    2.0,                              // sig_x
                    2.0,                              // sig_y
                    0.0,                              // theta
                    patchMax - patchMin,              // A (amplitude)
                    patchMin                          // B (background)
                };

                auto residualFn = [&](const std::vector<double>& p) -> double {
                    double residualSumSq = 0.0;
                    for (int i = 0; i < patchDim * patchDim; ++i) {
                        double fitVal = evaluateGaussian(pxGrid[i], pyGrid[i], p[0], p[1], p[2], p[3], p[4], p[5], p[6]);
                        double diff = patchValues[i] - fitVal;
                        residualSumSq += diff * diff;
                    }
                    return residualSumSq;
                };

                auto fitRes = Solver::nelderMead(residualFn, x0, 1e-4, 500);
                if (fitRes.success) {
                    double mean_x = fitRes.x[0];
                    double mean_y = fitRes.x[1];
                    double sig_x = std::abs(fitRes.x[2]);
                    double sig_y = std::abs(fitRes.x[3]);
                    double theta = fitRes.x[4];
                    star_lvl = fitRes.x[5];
                    bkg_lvl = fitRes.x[6];

                    double dist = std::sqrt(mean_x * mean_x + mean_y * mean_y);
                    fwhm = std::sqrt(2.0 * std::log(2.0)) * (sig_x + sig_y);
                    
                    double max_sig = std::max(sig_x, sig_y);
                    double min_sig = std::min(sig_x, sig_y);
                    if (max_sig > 1e-5) {
                        eccentricity = std::sqrt(1.0 - (min_sig * min_sig) / (max_sig * max_sig));
                    } else {
                        eccentricity = 0.0;
                    }

                    finalX += mean_x;
                    finalY += mean_y;

                    // Validate fit outputs
                    if (dist < 2.0 && fwhm > minFwhm && eccentricity < maxEccentricity && star_lvl > 0.0 && ((star_lvl / globalNoiseSigma) > snrMin)) {
                        fitSuccess = true;
                    }
                }
            }

            if (fitSuccess && fwhm > minFwhm && eccentricity < maxEccentricity) {
                Star star;
                star.x = finalX;
                star.y = finalY;
                star.fwhm = fwhm;
                star.peak = star_lvl;
                star.background = bkg_lvl;
                star.eccentricity = eccentricity;
                detectedStars.push_back(star);
            }
        }
    }

    return detectedStars;
}

} // namespace blastro
