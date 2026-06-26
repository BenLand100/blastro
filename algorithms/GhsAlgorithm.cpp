#include "GhsAlgorithm.h"
#include <cmath>
#include <algorithm>
#include <limits>

namespace blastro {

static double ghsFunction(double arg, double D, int form) {
    if (form > 0) {
        double denom = 1.0 + form * D * arg;
        if (denom <= 0.0) return 1.0;
        return 1.0 - std::pow(denom, -1.0 / form);
    } else if (form == 0) {
        return 1.0 - std::exp(-D * arg);
    } else if (form == -1) {
        double val = 1.0 + D * arg;
        if (val <= 0.0) return 0.0;
        return std::log(val);
    } else {
        // Form is negative and not -1
        double val = 1.0 - form * D * arg;
        if (val <= 0.0) {
            double denom = D * (form + 1);
            return (denom != 0.0) ? (1.0 / denom) : 0.0;
        }
        double expTerm = (form + 1.0) / form;
        double denom = D * (form + 1);
        return (denom != 0.0) ? ((1.0 - std::pow(val, expTerm)) / denom) : 0.0;
    }
}

static float evaluateGhsPixel(float val, double lowPoint, double highPoint, double symmetryPoint, double stretchFactor, int form) {
    if (std::isnan(val)) {
        return std::numeric_limits<float>::quiet_NaN();
    }

    double D = std::exp(stretchFactor) - 1.0;
    double spread = highPoint - lowPoint;
    if (spread < 1e-5) spread = 1e-5;

    double x = (static_cast<double>(val) - symmetryPoint) / spread;

    // Normalization bounds
    double bound = symmetryPoint / spread;
    double a = -ghsFunction(bound, D, form);
    double b = ghsFunction(1.0 - bound, D, form);
    double denom = b - a;
    if (std::abs(denom) < 1e-8) denom = 1e-8;

    double result = 0.0;
    if (x >= 0.0) {
        result = ghsFunction(x, D, form);
    } else {
        result = -ghsFunction(-x, D, form);
    }

    double normalized = (result - a) / denom;
    return static_cast<float>(std::clamp(normalized, 0.0, 1.0));
}

GrayscaleImagePtr GhsAlgorithm::stretchGrayscale(GrayscaleImagePtr src,
                                                double lowPoint,
                                                double highPoint,
                                                double symmetryPoint,
                                                double stretchFactor,
                                                int form) {
    if (!src) return nullptr;

    int w = src->width();
    int h = src->height();
    int numPixels = w * h;

    auto outBuffer = std::make_shared<ImageBuffer>(w, h);
    float* outData = outBuffer->data();
    const float* inData = src->buffer()->data();

    #pragma omp parallel for
    for (int i = 0; i < numPixels; ++i) {
        outData[i] = evaluateGhsPixel(inData[i], lowPoint, highPoint, symmetryPoint, stretchFactor, form);
    }

    return std::make_shared<GrayscaleImage>(outBuffer);
}

RGBImagePtr GhsAlgorithm::stretchRGB(RGBImagePtr src,
                                     double lowPoint,
                                     double highPoint,
                                     double symmetryPoint,
                                     double stretchFactor,
                                     int form,
                                     bool colorPreserving) {
    if (!src) return nullptr;

    int w = src->width();
    int h = src->height();
    int numPixels = w * h;

    if (colorPreserving) {
        // Color-Preserving Stretch (stretch luminance, scale RGB channels by luminance ratio)
        auto outR = std::make_shared<GrayscaleImage>(w, h);
        auto outG = std::make_shared<GrayscaleImage>(w, h);
        auto outB = std::make_shared<GrayscaleImage>(w, h);

        float* outRData = outR->buffer()->data();
        float* outGData = outG->buffer()->data();
        float* outBData = outB->buffer()->data();

        const float* rData = src->r()->buffer()->data();
        const float* gData = src->g()->buffer()->data();
        const float* bData = src->b()->buffer()->data();

        #pragma omp parallel for
        for (int i = 0; i < numPixels; ++i) {
            float r = rData[i];
            float g = gData[i];
            float b = bData[i];

            if (std::isnan(r) || std::isnan(g) || std::isnan(b)) {
                outRData[i] = std::numeric_limits<float>::quiet_NaN();
                outGData[i] = std::numeric_limits<float>::quiet_NaN();
                outBData[i] = std::numeric_limits<float>::quiet_NaN();
                continue;
            }

            // Average intensity serves as luminance
            float Y = (r + g + b) / 3.0f;
            if (Y < 1e-6f) {
                outRData[i] = 0.0f;
                outGData[i] = 0.0f;
                outBData[i] = 0.0f;
            } else {
                float Y_stretched = evaluateGhsPixel(Y, lowPoint, highPoint, symmetryPoint, stretchFactor, form);
                float ratio = Y_stretched / Y;

                outRData[i] = std::clamp(r * ratio, 0.0f, 1.0f);
                outGData[i] = std::clamp(g * ratio, 0.0f, 1.0f);
                outBData[i] = std::clamp(b * ratio, 0.0f, 1.0f);
            }
        }

        return std::make_shared<RGBImage>(outR, outG, outB);
    } else {
        // Independent Channel Stretch
        auto extR = stretchGrayscale(src->r(), lowPoint, highPoint, symmetryPoint, stretchFactor, form);
        auto extG = stretchGrayscale(src->g(), lowPoint, highPoint, symmetryPoint, stretchFactor, form);
        auto extB = stretchGrayscale(src->b(), lowPoint, highPoint, symmetryPoint, stretchFactor, form);
        return std::make_shared<RGBImage>(extR, extG, extB);
    }
}

} // namespace blastro
