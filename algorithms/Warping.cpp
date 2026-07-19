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

#include "Warping.h"
#include <cmath>
#include <limits>
#include <algorithm>

namespace blastro {

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

inline double lanczosKernel(double x, double a) {
    if (x == 0.0) return 1.0;
    if (std::abs(x) >= a) return 0.0;
    double pix = M_PI * x;
    return a * std::sin(pix) * std::sin(pix / a) / (pix * pix);
}

inline double bicubicKernel(double x) {
    double absX = std::abs(x);
    if (absX < 1.0) {
        return 1.5 * absX * absX * absX - 2.5 * absX * absX + 1.0;
    } else if (absX < 2.0) {
        return -0.5 * absX * absX * absX + 2.5 * absX * absX - 4.0 * absX + 2.0;
    }
    return 0.0;
}

GrayscaleImagePtr Warping::warpGrayscale(GrayscaleImagePtr src,
                                        const std::array<double, 6>& transform,
                                        double drizzleScale,
                                        const std::string& interpolation) {
    if (!src) return nullptr;

    int w_in = src->width();
    int h_in = src->height();

    int w_out = std::round(w_in * drizzleScale);
    int h_out = std::round(h_in * drizzleScale);

    auto outBuffer = std::make_shared<ImageBuffer>(w_out, h_out);
    float* outData = outBuffer->data();
    const float* inData = src->buffer()->data();

    double a = transform[0];
    double b = transform[1];
    double tx = transform[2];
    double c = transform[3];
    double d = transform[4];
    double ty = transform[5];

    double det = a * d - b * c;
    double inv_a = 1.0, inv_b = 0.0, inv_tx = 0.0;
    double inv_c = 0.0, inv_d = 1.0, inv_ty = 0.0;

    if (std::abs(det) > 1e-9) {
        double inv_det = 1.0 / det;
        inv_a = d * inv_det;
        inv_b = -b * inv_det;
        inv_tx = (-d * tx + b * ty) * inv_det;
        inv_c = -c * inv_det;
        inv_d = a * inv_det;
        inv_ty = (c * tx - a * ty) * inv_det;
    }

    // Parallelize pixel warping with OpenMP
    #pragma omp parallel for
    for (int y_out = 0; y_out < h_out; ++y_out) {
        for (int x_out = 0; x_out < w_out; ++x_out) {
            double x_ref = x_out / drizzleScale;
            double y_ref = y_out / drizzleScale;

            // Backward mapping (resolving where the reference space lands in the input space)
            double x_img = inv_a * x_ref + inv_b * y_ref + inv_tx;
            double y_img = inv_c * x_ref + inv_d * y_ref + inv_ty;

            long idx_out = y_out * w_out + x_out;

            if (interpolation == "lanczos3") {
                int x0 = std::floor(x_img);
                int y0 = std::floor(y_img);
                if (x0 - 2 < 0 || x0 + 3 >= w_in || y0 - 2 < 0 || y0 + 3 >= h_in) {
                    outData[idx_out] = std::numeric_limits<float>::quiet_NaN();
                } else {
                    double w_x[6], w_y[6];
                    for (int i = 0; i < 6; ++i) w_x[i] = lanczosKernel(x_img - (x0 - 2 + i), 3.0);
                    for (int j = 0; j < 6; ++j) w_y[j] = lanczosKernel(y_img - (y0 - 2 + j), 3.0);

                    double sum_val = 0.0;
                    double sum_weight = 0.0;
                    bool has_nan = false;

                    for (int j = 0; j < 6; ++j) {
                        int py = y0 - 2 + j;
                        for (int i = 0; i < 6; ++i) {
                            int px = x0 - 2 + i;
                            float val = inData[py * w_in + px];
                            if (std::isnan(val)) {
                                has_nan = true;
                                break;
                            }
                            double w = w_x[i] * w_y[j];
                            sum_val += val * w;
                            sum_weight += w;
                        }
                        if (has_nan) break;
                    }

                    if (has_nan || sum_weight <= 0.0) {
                        outData[idx_out] = std::numeric_limits<float>::quiet_NaN();
                    } else {
                        outData[idx_out] = static_cast<float>(sum_val / sum_weight);
                    }
                }
            } else if (interpolation == "lanczos4") {
                int x0 = std::floor(x_img);
                int y0 = std::floor(y_img);
                if (x0 - 3 < 0 || x0 + 4 >= w_in || y0 - 3 < 0 || y0 + 4 >= h_in) {
                    outData[idx_out] = std::numeric_limits<float>::quiet_NaN();
                } else {
                    double w_x[8], w_y[8];
                    for (int i = 0; i < 8; ++i) w_x[i] = lanczosKernel(x_img - (x0 - 3 + i), 4.0);
                    for (int j = 0; j < 8; ++j) w_y[j] = lanczosKernel(y_img - (y0 - 3 + j), 4.0);

                    double sum_val = 0.0;
                    double sum_weight = 0.0;
                    bool has_nan = false;

                    for (int j = 0; j < 8; ++j) {
                        int py = y0 - 3 + j;
                        for (int i = 0; i < 8; ++i) {
                            int px = x0 - 3 + i;
                            float val = inData[py * w_in + px];
                            if (std::isnan(val)) {
                                has_nan = true;
                                break;
                            }
                            double w = w_x[i] * w_y[j];
                            sum_val += val * w;
                            sum_weight += w;
                        }
                        if (has_nan) break;
                    }

                    if (has_nan || sum_weight <= 0.0) {
                        outData[idx_out] = std::numeric_limits<float>::quiet_NaN();
                    } else {
                        outData[idx_out] = static_cast<float>(sum_val / sum_weight);
                    }
                }
            } else if (interpolation == "bicubic") {
                int x0 = std::floor(x_img);
                int y0 = std::floor(y_img);
                if (x0 - 1 < 0 || x0 + 2 >= w_in || y0 - 1 < 0 || y0 + 2 >= h_in) {
                    outData[idx_out] = std::numeric_limits<float>::quiet_NaN();
                } else {
                    double w_x[4], w_y[4];
                    for (int i = 0; i < 4; ++i) w_x[i] = bicubicKernel(x_img - (x0 - 1 + i));
                    for (int j = 0; j < 4; ++j) w_y[j] = bicubicKernel(y_img - (y0 - 1 + j));

                    double sum_val = 0.0;
                    double sum_weight = 0.0;
                    bool has_nan = false;

                    for (int j = 0; j < 4; ++j) {
                        int py = y0 - 1 + j;
                        for (int i = 0; i < 4; ++i) {
                            int px = x0 - 1 + i;
                            float val = inData[py * w_in + px];
                            if (std::isnan(val)) {
                                has_nan = true;
                                break;
                            }
                            double w = w_x[i] * w_y[j];
                            sum_val += val * w;
                            sum_weight += w;
                        }
                        if (has_nan) break;
                    }

                    if (has_nan || sum_weight <= 0.0) {
                        outData[idx_out] = std::numeric_limits<float>::quiet_NaN();
                    } else {
                        outData[idx_out] = static_cast<float>(sum_val / sum_weight);
                    }
                }
            } else {
                // Default: Bilinear
                if (x_img < 0.0 || x_img >= w_in - 1 || y_img < 0.0 || y_img >= h_in - 1) {
                    outData[idx_out] = std::numeric_limits<float>::quiet_NaN();
                } else {
                    int x0 = std::floor(x_img);
                    int y0 = std::floor(y_img);
                    int x1 = x0 + 1;
                    int y1 = y0 + 1;

                    double u = x_img - x0;
                    double v = y_img - y0;

                    float p00 = inData[y0 * w_in + x0];
                    float p10 = inData[y0 * w_in + x1];
                    float p01 = inData[y1 * w_in + x0];
                    float p11 = inData[y1 * w_in + x1];

                    if (std::isnan(p00) || std::isnan(p10) || std::isnan(p01) || std::isnan(p11)) {
                        outData[idx_out] = std::numeric_limits<float>::quiet_NaN();
                    } else {
                        double val = (1.0 - u) * (1.0 - v) * p00 +
                                     u * (1.0 - v) * p10 +
                                     (1.0 - u) * v * p01 +
                                     u * v * p11;
                        outData[idx_out] = static_cast<float>(val);
                    }
                }
            }
        }
    }

    return std::make_shared<GrayscaleImage>(outBuffer);
}

std::pair<GrayscaleImagePtr, GrayscaleImagePtr> Warping::warpDrizzleGrayscale(
    GrayscaleImagePtr src,
    const std::array<double, 6>& transform,
    double drizzleScale,
    double dropShrink) {
    if (!src) return {nullptr, nullptr};

    int w_in = src->width();
    int h_in = src->height();

    int w_out = std::round(w_in * drizzleScale);
    int h_out = std::round(h_in * drizzleScale);

    auto dataBuffer = std::make_shared<ImageBuffer>(w_out, h_out);
    auto weightBuffer = std::make_shared<ImageBuffer>(w_out, h_out);
    float* outData = dataBuffer->data();
    float* outWeight = weightBuffer->data();
    const float* inData = src->buffer()->data();

    // Initialize with 0
    std::fill(outData, outData + w_out * h_out, 0.0f);
    std::fill(outWeight, outWeight + w_out * h_out, 0.0f);

    double a = transform[0];
    double b = transform[1];
    double tx = transform[2];
    double c = transform[3];
    double d = transform[4];
    double ty = transform[5];

    // Forward map each input pixel to the output grid
    #pragma omp parallel for
    for (int y_in = 0; y_in < h_in; ++y_in) {
        for (int x_in = 0; x_in < w_in; ++x_in) {
            float val = inData[y_in * w_in + x_in];
            if (std::isnan(val)) continue;

            // Map the center of the input pixel
            double x_mapped = a * x_in + b * y_in + tx;
            double y_mapped = c * x_in + d * y_in + ty;

            // Scale to output grid
            x_mapped *= drizzleScale;
            y_mapped *= drizzleScale;

            // Compute the size of the drop in the output space
            // Assuming affine matrix has roughly uniform scaling, but technically
            // an input pixel maps to a parallelogram. For simplicity and standard drizzle,
            // we approximate it as a square drop on the output grid.
            // A 1x1 input pixel has size `drizzleScale` in the output grid (ignoring scale from alignment transform itself).
            // Actually, the alignment transform could scale the image.
            double dropSizeX = drizzleScale * dropShrink;
            double dropSizeY = drizzleScale * dropShrink;
            
            // For a more robust mapping considering the transform scaling:
            double scaleX = std::sqrt(a*a + c*c) * drizzleScale;
            double scaleY = std::sqrt(b*b + d*d) * drizzleScale;
            dropSizeX = scaleX * dropShrink;
            dropSizeY = scaleY * dropShrink;

            double halfDropX = dropSizeX * 0.5;
            double halfDropY = dropSizeY * 0.5;

            double x_min = x_mapped - halfDropX;
            double x_max = x_mapped + halfDropX;
            double y_min = y_mapped - halfDropY;
            double y_max = y_mapped + halfDropY;

            int px_min = std::max(0, static_cast<int>(std::floor(x_min)));
            int px_max = std::min(w_out - 1, static_cast<int>(std::floor(x_max)));
            int py_min = std::max(0, static_cast<int>(std::floor(y_min)));
            int py_max = std::min(h_out - 1, static_cast<int>(std::floor(y_max)));

            // Compute overlaps for each output pixel the drop touches
            for (int py = py_min; py <= py_max; ++py) {
                double out_y_min = py;
                double out_y_max = py + 1.0;
                double overlapY = std::max(0.0, std::min(y_max, out_y_max) - std::max(y_min, out_y_min));
                if (overlapY <= 0.0) continue;

                for (int px = px_min; px <= px_max; ++px) {
                    double out_x_min = px;
                    double out_x_max = px + 1.0;
                    double overlapX = std::max(0.0, std::min(x_max, out_x_max) - std::max(x_min, out_x_min));
                    
                    double overlapArea = overlapX * overlapY;
                    if (overlapArea > 0.0) {
                        long idx_out = py * w_out + px;
                        #pragma omp atomic
                        outData[idx_out] += static_cast<float>(val * overlapArea);
                        #pragma omp atomic
                        outWeight[idx_out] += static_cast<float>(overlapArea);
                    }
                }
            }
        }
    }

    return {std::make_shared<GrayscaleImage>(dataBuffer), std::make_shared<GrayscaleImage>(weightBuffer)};
}

RGBImagePtr Warping::warpRGB(RGBImagePtr src,
                             const std::array<double, 6>& transform,
                             double drizzleScale,
                             const std::string& interpolation) {
    if (!src) return nullptr;

    auto warpedR = warpGrayscale(src->r(), transform, drizzleScale, interpolation);
    auto warpedG = warpGrayscale(src->g(), transform, drizzleScale, interpolation);
    auto warpedB = warpGrayscale(src->b(), transform, drizzleScale, interpolation);

    return std::make_shared<RGBImage>(warpedR, warpedG, warpedB);
}

} // namespace blastro
