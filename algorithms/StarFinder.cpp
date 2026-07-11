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
                                       const std::string& method,
                                       int patchRadius,
                                       double minFwhm,
                                       double maxEccentricity,
                                       int maxRefinedStars) {
    if (!img) {
        throw std::runtime_error("StarFinder input image is null");
    }

    int w = img->width();
    int h = img->height();
    int numPixels = w * h;
    const float* data = img->buffer()->data();

    bool fastFit = (method == "centroid");
    bool isAdaptive = (method == "adaptive");

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

    // Adaptive Block-based Background & Noise estimation
    std::vector<double> blockMedians;
    std::vector<double> blockSigmas;
    int blockSize = 128;
    int blocksX = (w + blockSize - 1) / blockSize;
    int blocksY = (h + blockSize - 1) / blockSize;
    std::vector<int> candidateIndices;

    if (isAdaptive) {
        blockMedians.assign(blocksX * blocksY, 0.0);
        blockSigmas.assign(blocksX * blocksY, 0.0);

        #pragma omp parallel for collapse(2)
        for (int by = 0; by < blocksY; ++by) {
            for (int bx = 0; bx < blocksX; ++bx) {
                int x0 = bx * blockSize;
                int y0 = by * blockSize;
                int x1 = std::min(x0 + blockSize, w);
                int y1 = std::min(y0 + blockSize, h);

                // Sample every 4th pixel in x and y (1/16th of pixels) for fast median/MAD calculation
                std::vector<float> blockPixels;
                blockPixels.reserve((x1 - x0) * (y1 - y0) / 16 + 1);
                for (int y = y0; y < y1; y += 4) {
                    for (int x = x0; x < x1; x += 4) {
                        blockPixels.push_back(data[y * w + x]);
                    }
                }

                if (!blockPixels.empty()) {
                    auto midIt = blockPixels.begin() + blockPixels.size() / 2;
                    std::nth_element(blockPixels.begin(), midIt, blockPixels.end());
                    double median = *midIt;
                    
                    std::vector<float> absDiffs(blockPixels.size());
                    for (size_t i = 0; i < blockPixels.size(); ++i) {
                        absDiffs[i] = std::abs(blockPixels[i] - median);
                    }
                    auto madIt = absDiffs.begin() + absDiffs.size() / 2;
                    std::nth_element(absDiffs.begin(), madIt, absDiffs.end());
                    double mad = *madIt;
                    double sigma = 1.4826 * mad;

                    blockMedians[by * blocksX + bx] = median;
                    blockSigmas[by * blocksX + bx] = std::max(1e-5, sigma);
                }
            }
        }

        // Pre-calculate X and Y interpolation weights
        std::vector<int> bx0_arr(w), bx1_arr(w), by0_arr(h), by1_arr(h);
        std::vector<double> tx_arr(w), ty_arr(h);
        for (int x = 0; x < w; ++x) {
            double gx = (static_cast<double>(x) / blockSize) - 0.5;
            if (gx < 0.0) {
                bx0_arr[x] = 0; bx1_arr[x] = 0; tx_arr[x] = 0.0;
            } else {
                bx0_arr[x] = static_cast<int>(gx);
                bx1_arr[x] = std::min(bx0_arr[x] + 1, blocksX - 1);
                tx_arr[x] = gx - bx0_arr[x];
            }
        }
        for (int y = 0; y < h; ++y) {
            double gy = (static_cast<double>(y) / blockSize) - 0.5;
            if (gy < 0.0) {
                by0_arr[y] = 0; by1_arr[y] = 0; ty_arr[y] = 0.0;
            } else {
                by0_arr[y] = static_cast<int>(gy);
                by1_arr[y] = std::min(by0_arr[y] + 1, blocksY - 1);
                ty_arr[y] = gy - by0_arr[y];
            }
        }

        // Scan for local maxima in 5x5 window above local threshold
        #pragma omp parallel
        {
            std::vector<int> localCandidates;
            std::vector<double> bg_row(blocksX);
            std::vector<double> noise_row(blocksX);

            #pragma omp for nowait
            for (int y = 2; y < h - 2; ++y) {
                int by0 = by0_arr[y];
                int by1 = by1_arr[y];
                double ty = ty_arr[y];
                double inv_ty = 1.0 - ty;

                for (int bx = 0; bx < blocksX; ++bx) {
                    bg_row[bx] = inv_ty * blockMedians[by0 * blocksX + bx] + ty * blockMedians[by1 * blocksX + bx];
                    noise_row[bx] = inv_ty * blockSigmas[by0 * blocksX + bx] + ty * blockSigmas[by1 * blocksX + bx];
                }

                for (int x = 2; x < w - 2; ++x) {
                    int bx0 = bx0_arr[x];
                    int bx1 = bx1_arr[x];
                    double tx = tx_arr[x];

                    float localBg = (1.0 - tx) * bg_row[bx0] + tx * bg_row[bx1];
                    float localNoise = (1.0 - tx) * noise_row[bx0] + tx * noise_row[bx1];

                    int idx = y * w + x;
                    float val = data[idx];

                    if (val > localBg + snrMin * localNoise) {
                        // Adaptive Hot pixel check: direct neighbors must have at least 15% of peak amplitude
                        float neighborSum = 0.0f;
                        neighborSum += data[(y - 1) * w + x];
                        neighborSum += data[(y + 1) * w + x];
                        neighborSum += data[y * w + (x - 1)];
                        neighborSum += data[y * w + (x + 1)];
                        float avgNeighborAmp = (neighborSum / 4.0f) - localBg;
                        if (avgNeighborAmp < 0.15f * (val - localBg)) {
                            continue;
                        }

                        bool isMax = true;
                        for (int dy = -2; dy <= 2; ++dy) {
                            for (int dx = -2; dx <= 2; ++dx) {
                                if (dx == 0 && dy == 0) continue;
                                float neighborVal = data[(y + dy) * w + (x + dx)];
                                if (neighborVal > val) {
                                    isMax = false;
                                    break;
                                }
                                if (neighborVal == val && (dy < 0 || (dy == 0 && dx < 0))) {
                                    isMax = false;
                                    break;
                                }
                            }
                            if (!isMax) break;
                        }
                        if (isMax) {
                            localCandidates.push_back(idx);
                        }
                    }
                }
            }
            #pragma omp critical
            {
                candidateIndices.insert(candidateIndices.end(), localCandidates.begin(), localCandidates.end());
            }
        }
        
        // Sort candidate indices descending by local peak amplitude to get brightest first
        struct PeakCandidate {
            int idx;
            float amplitude;
        };
        std::vector<PeakCandidate> candidates;
        candidates.reserve(candidateIndices.size());
        for (int idx : candidateIndices) {
            int x = idx % w;
            int y = idx / w;
            int bx = std::min(x / blockSize, blocksX - 1);
            int by = std::min(y / blockSize, blocksY - 1);
            float bg = blockMedians[by * blocksX + bx];
            candidates.push_back({idx, data[idx] - bg});
        }
        std::sort(candidates.begin(), candidates.end(), [](const PeakCandidate& a, const PeakCandidate& b) {
            return a.amplitude > b.amplitude;
        });
        
        candidateIndices.resize(candidates.size());
        for (size_t i = 0; i < candidates.size(); ++i) {
            candidateIndices[i] = candidates[i].idx;
        }
    }

    std::vector<int> indices;
    std::vector<float> reg_search;
    if (!isAdaptive) {
        // 2. Identify candidate regions above mean + 3*stddev
        reg_search.assign(numPixels, 0.0f);
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
        indices.resize(numPixels);
        std::iota(indices.begin(), indices.end(), 0);
        std::sort(indices.begin(), indices.end(), [&](int a, int b) {
            return reg_search[a] > reg_search[b];
        });
    } else {
        indices = candidateIndices;
    }

    std::vector<Star> detectedStars;
    int attempts = 0;
    int maxAttempts = isAdaptive ? 50000 : 2000;
    int limit = maxStars;

    // Occupancy grid to prevent overlapping fits in adaptive mode (O(1) checks)
    std::vector<uint8_t> occupied(numPixels, 0);

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
        if (detectedStars.size() >= static_cast<size_t>(limit) || attempts >= maxAttempts) {
            break;
        }

        // Skip if this region was already zeroed out (assigned to another star)
        if (!isAdaptive && reg_search[idx] == 0.0f) {
            continue;
        }

        int x_max = idx % w;
        int y_max = idx / w;

        // Check distance against already detected stars to prevent overlapping fits in SOTA mode
        if (isAdaptive && occupied[idx]) {
            continue;
        }

        // Check if patch is entirely within image bounds
        if (x_max - patchRadius >= 0 && x_max + patchRadius < w &&
            y_max - patchRadius >= 0 && y_max + patchRadius < h) {
            
            // Zero out patch region in search mask to avoid duplicate detection (legacy mode only)
            if (!isAdaptive) {
                for (int py = -patchRadius; py <= patchRadius; ++py) {
                    for (int px = -patchRadius; px <= patchRadius; ++px) {
                        reg_search[(y_max + py) * w + (x_max + px)] = 0.0f;
                    }
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

            // SNR Check
            double localNoise = globalNoiseSigma;
            if (isAdaptive) {
                double gx = (static_cast<double>(x_max) / blockSize) - 0.5;
                double gy = (static_cast<double>(y_max) / blockSize) - 0.5;

                int bx0 = std::clamp(static_cast<int>(std::floor(gx)), 0, blocksX - 1);
                int by0 = std::clamp(static_cast<int>(std::floor(gy)), 0, blocksY - 1);
                int bx1 = std::clamp(bx0 + 1, 0, blocksX - 1);
                int by1 = std::clamp(by0 + 1, 0, blocksY - 1);

                double tx = gx - std::floor(gx);
                double ty = gy - std::floor(gy);
                if (bx0 == bx1) tx = 0.0;
                if (by0 == by1) ty = 0.0;

                double v00 = blockSigmas[by0 * blocksX + bx0];
                double v10 = blockSigmas[by0 * blocksX + bx1];
                double v01 = blockSigmas[by1 * blocksX + bx0];
                double v11 = blockSigmas[by1 * blocksX + bx1];
                
                localNoise = (1.0 - tx) * (1.0 - ty) * v00 +
                             tx * (1.0 - ty) * v10 +
                             (1.0 - tx) * ty * v01 +
                             tx * ty * v11;
            }
            double amplitude = patchMax - patchMin;
            if (amplitude <= 0.0 || (amplitude / localNoise) < snrMin) {
                continue;
            }

            double finalX = x_max;
            double finalY = y_max;
            double fwhm = 0.0;
            double star_lvl = 0.0;
            double bkg_lvl = 0.0;
            double eccentricity = 0.0;
            bool fitSuccess = false;

            // In adaptive mode, we always run centroiding first inside the candidate loop,
            // and then run Nelder-Mead refinement only on the top maxStars brightest stars later.
            bool runNelderMeadNow = !isAdaptive && !fastFit;

            if (!runNelderMeadNow) {
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
                    fwhm = std::sqrt(8.0 * std::log(2.0) * (variance / 2.0));
                    eccentricity = 0.0;
                    
                    double dist = std::sqrt(sumX * sumX + sumY * sumY) / sumWeights;
                    if (dist < 3.0 && fwhm > minFwhm && star_lvl > 0.0 && ((star_lvl / localNoise) > snrMin)) {
                        fitSuccess = true;
                    }
                }
            } else {
                // 2D Gaussian Profile Fitting using Nelder-Mead solver (legacy mode only)
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
                    double mean_x = p[0];
                    double mean_y = p[1];
                    double sig_x = std::max(1e-5, std::abs(p[2]));
                    double sig_y = std::max(1e-5, std::abs(p[3]));
                    double theta = p[4];
                    double A = p[5];
                    double B = p[6];

                    double cost = std::cos(theta);
                    double sint = std::sin(theta);
                    double inv_sig_x2 = 1.0 / (2.0 * sig_x * sig_x);
                    double inv_sig_y2 = 1.0 / (2.0 * sig_y * sig_y);

                    const double* px = pxGrid.data();
                    const double* py = pyGrid.data();
                    const double* pval = patchValues.data();
                    int numPixels = patchDim * patchDim;

                    double residualSumSq = 0.0;
                    for (int i = 0; i < numPixels; ++i) {
                        double dx = px[i] - mean_x;
                        double dy = py[i] - mean_y;
                        double rot_x = dx * cost - dy * sint;
                        double rot_y = dy * cost + dx * sint;
                        double exponent = -(rot_x * rot_x) * inv_sig_x2 - (rot_y * rot_y) * inv_sig_y2;
                        
                        double fitVal = B;
                        if (exponent > -10.0) {
                            fitVal += A * std::exp(exponent);
                        }
                        
                        double diff = pval[i] - fitVal;
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

                    if (dist < 2.0 && fwhm > minFwhm && eccentricity < maxEccentricity && star_lvl > 0.0 && ((star_lvl / localNoise) > snrMin)) {
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
                star.snr = star_lvl / std::max(1e-5, localNoise);
                detectedStars.push_back(star);

                // Mark the region around this star as occupied in adaptive mode
                if (isAdaptive) {
                    int cx = static_cast<int>(std::round(finalX));
                    int cy = static_cast<int>(std::round(finalY));
                    int rad = patchRadius;
                    int minX = std::max(0, cx - rad);
                    int maxX = std::min(w - 1, cx + rad);
                    int minY = std::max(0, cy - rad);
                    int maxY = std::min(h - 1, cy + rad);
                    for (int oy = minY; oy <= maxY; ++oy) {
                        for (int ox = minX; ox <= maxX; ++ox) {
                            if ((ox - cx) * (ox - cx) + (oy - cy) * (oy - cy) < rad * rad) {
                                occupied[oy * w + ox] = 1;
                            }
                        }
                    }
                }
            }
        }
    }

    // Adaptive Refinement Phase: Run high-precision Nelder-Mead on the top maxStars brightest stars
    if (isAdaptive && !detectedStars.empty()) {
        // Sort all detected centroid stars descending by peak brightness
        std::sort(detectedStars.begin(), detectedStars.end(), [](const Star& a, const Star& b) {
            return a.peak > b.peak;
        });

        int numRefine = std::min(static_cast<int>(detectedStars.size()), maxRefinedStars);

        #pragma omp parallel
        {
            std::vector<double> patchValues(patchDim * patchDim);
            std::vector<double> x0(7);
            
            #pragma omp for schedule(dynamic)
            for (int i = 0; i < numRefine; ++i) {
                Star& star = detectedStars[i];
                int cx = static_cast<int>(std::round(star.x));
                int cy = static_cast<int>(std::round(star.y));
                
                // Ensure patch is within bounds
                if (cx - patchRadius < 0 || cx + patchRadius >= w ||
                    cy - patchRadius < 0 || cy + patchRadius >= h) {
                    continue;
                }

                // Extract patch pixel values
                double patchMax = -1e30;
                double patchMin = 1e30;
                for (int py = -patchRadius; py <= patchRadius; ++py) {
                    for (int px = -patchRadius; px <= patchRadius; ++px) {
                        double val = data[(cy + py) * w + (cx + px)];
                        int pIdx = (py + patchRadius) * patchDim + (px + patchRadius);
                        patchValues[pIdx] = val;
                        if (val > patchMax) patchMax = val;
                        if (val < patchMin) patchMin = val;
                    }
                }

                x0[0] = 0.0;                              // mean_x relative to patch center
                x0[1] = 0.0;                              // mean_y relative to patch center
                x0[2] = 2.0;                              // sig_x
                x0[3] = 2.0;                              // sig_y
                x0[4] = 0.0;                              // theta
                x0[5] = patchMax - patchMin;              // A (amplitude)
                x0[6] = patchMin;                         // B (background)

                auto residualFn = [&](const std::vector<double>& p) -> double {
                    double mean_x = p[0];
                    double mean_y = p[1];
                    double sig_x = std::max(1e-5, std::abs(p[2]));
                    double sig_y = std::max(1e-5, std::abs(p[3]));
                    double theta = p[4];
                    double A = p[5];
                    double B = p[6];

                    double cost = std::cos(theta);
                    double sint = std::sin(theta);
                    double inv_sig_x2 = 1.0 / (2.0 * sig_x * sig_x);
                    double inv_sig_y2 = 1.0 / (2.0 * sig_y * sig_y);

                    const double* px = pxGrid.data();
                    const double* py = pyGrid.data();
                    const double* pval = patchValues.data();
                    int numPixels = patchDim * patchDim;

                    double residualSumSq = 0.0;
                    for (int j = 0; j < numPixels; ++j) {
                        double dx = px[j] - mean_x;
                        double dy = py[j] - mean_y;
                        double rot_x = dx * cost - dy * sint;
                        double rot_y = dy * cost + dx * sint;
                        double exponent = -(rot_x * rot_x) * inv_sig_x2 - (rot_y * rot_y) * inv_sig_y2;
                        
                        double fitVal = B;
                        if (exponent > -10.0) {
                            fitVal += A * std::exp(exponent);
                        }
                        
                        double diff = pval[j] - fitVal;
                        residualSumSq += diff * diff;
                    }
                    return residualSumSq;
                };

                // Run solver
                bool refinedSuccess = false;
                auto fitRes = Solver::nelderMead(residualFn, x0, 1e-4, 500);
                if (fitRes.success) {
                    double mean_x = fitRes.x[0];
                    double mean_y = fitRes.x[1];
                    double sig_x = std::abs(fitRes.x[2]);
                    double sig_y = std::abs(fitRes.x[3]);
                    double theta = fitRes.x[4];
                    double star_lvl = fitRes.x[5];
                    double bkg_lvl = fitRes.x[6];

                    double dist = std::sqrt(mean_x * mean_x + mean_y * mean_y);
                    double fwhm = std::sqrt(2.0 * std::log(2.0)) * (sig_x + sig_y);

                    double eccentricity = 0.0;
                    double max_sig = std::max(sig_x, sig_y);
                    double min_sig = std::min(sig_x, sig_y);
                    if (max_sig > 1e-5) {
                        eccentricity = std::sqrt(1.0 - (min_sig * min_sig) / (max_sig * max_sig));
                    }

                    // Calculate local noise at refined position
                    double localNoise = globalNoiseSigma;
                    if (isAdaptive) {
                        double gx = (static_cast<double>(cx) / blockSize) - 0.5;
                        double gy = (static_cast<double>(cy) / blockSize) - 0.5;

                        int bx0 = std::clamp(static_cast<int>(std::floor(gx)), 0, blocksX - 1);
                        int by0 = std::clamp(static_cast<int>(std::floor(gy)), 0, blocksY - 1);
                        int bx1 = std::clamp(bx0 + 1, 0, blocksX - 1);
                        int by1 = std::clamp(by0 + 1, 0, blocksY - 1);

                        double tx = gx - std::floor(gx);
                        double ty = gy - std::floor(gy);
                        if (bx0 == bx1) tx = 0.0;
                        if (by0 == by1) ty = 0.0;

                        double v00 = blockSigmas[by0 * blocksX + bx0];
                        double v10 = blockSigmas[by0 * blocksX + bx1];
                        double v01 = blockSigmas[by1 * blocksX + bx0];
                        double v11 = blockSigmas[by1 * blocksX + bx1];
                        
                        localNoise = (1.0 - tx) * (1.0 - ty) * v00 +
                                     tx * (1.0 - ty) * v10 +
                                     (1.0 - tx) * ty * v01 +
                                     tx * ty * v11;
                    }

                    // If refinement is valid and within patch boundaries, apply it
                    if (dist < 2.0 && fwhm >= minFwhm && fwhm <= 20.0 && eccentricity < maxEccentricity && star_lvl > 0.0) {
                        star.x = cx + mean_x;
                        star.y = cy + mean_y;
                        star.fwhm = fwhm;
                        star.peak = star_lvl;
                        star.background = bkg_lvl;
                        star.eccentricity = eccentricity;
                        star.snr = star_lvl / std::max(1e-5, localNoise);
                        refinedSuccess = true;
                    }
                }
                if (!refinedSuccess) {
                    star.peak = -1.0;
                }
            }
        }

        // Remove stars that failed refinement
        detectedStars.erase(
            std::remove_if(detectedStars.begin(), detectedStars.end(), [](const Star& s) {
                return s.peak < 0.0;
            }),
            detectedStars.end()
        );
    }

    return detectedStars;
}

} // namespace blastro
