#include "Warping.h"
#include <cmath>
#include <limits>
#include <algorithm>

namespace blastro {

GrayscaleImagePtr Warping::warpGrayscale(GrayscaleImagePtr src,
                                        double dx, double dy, double theta,
                                        double drizzleScale) {
    if (!src) return nullptr;

    int w_in = src->width();
    int h_in = src->height();

    int w_out = std::round(w_in * drizzleScale);
    int h_out = std::round(h_in * drizzleScale);

    auto outBuffer = std::make_shared<ImageBuffer>(w_out, h_out);
    float* outData = outBuffer->data();
    const float* inData = src->buffer()->data();

    double cosT = std::cos(theta);
    double sinT = std::sin(theta);

    // Parallelize pixel warping with OpenMP
    #pragma omp parallel for
    for (int y_out = 0; y_out < h_out; ++y_out) {
        for (int x_out = 0; x_out < w_out; ++x_out) {
            double x_ref = x_out / drizzleScale;
            double y_ref = y_out / drizzleScale;

            // Backward mapping (resolving where the reference space lands in the input space)
            double x_img = (x_ref - dx) * cosT + (y_ref - dy) * sinT;
            double y_img = -(x_ref - dx) * sinT + (y_ref - dy) * cosT;

            long idx_out = y_out * w_out + x_out;

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

                // If any of the input pixels is NaN, propagate NaN to the output
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

    return std::make_shared<GrayscaleImage>(outBuffer);
}

RGBImagePtr Warping::warpRGB(RGBImagePtr src,
                             double dx, double dy, double theta,
                             double drizzleScale) {
    if (!src) return nullptr;

    // Warp each channel independently
    auto warpedR = warpGrayscale(src->r(), dx, dy, theta, drizzleScale);
    auto warpedG = warpGrayscale(src->g(), dx, dy, theta, drizzleScale);
    auto warpedB = warpGrayscale(src->b(), dx, dy, theta, drizzleScale);

    return std::make_shared<RGBImage>(warpedR, warpedG, warpedB);
}

} // namespace blastro
