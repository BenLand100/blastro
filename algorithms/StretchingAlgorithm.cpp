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

#include "StretchingAlgorithm.h"
#include <cmath>
#include <algorithm>
#include <limits>
#include "core/MathUtils.h"

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

struct GhsParams {
    double lowPoint;
    double D;
    double spread;
    double bound;
    double a;
    double denom;
    bool isLinear;
};

static GhsParams precomputeGhs(double lowPoint, double highPoint, double symmetryPoint, double stretchFactor, int form) {
    GhsParams p;
    p.lowPoint = lowPoint;
    p.D = std::exp(stretchFactor) - 1.0;
    p.spread = highPoint - lowPoint;
    if (p.spread < 1e-5) p.spread = 1e-5;
    p.isLinear = (stretchFactor < 1e-5);
    if (!p.isLinear) {
        p.bound = symmetryPoint / p.spread;
        p.a = -ghsFunction(p.bound, p.D, form);
        double b = ghsFunction(1.0 - p.bound, p.D, form);
        p.denom = b - p.a;
        if (std::abs(p.denom) < 1e-8) p.denom = 1e-8;
    }
    return p;
}

static float evaluateGhsPixel(float val, const GhsParams& p, double symmetryPoint, double shadowProtect, double highlightProtect, int form) {
    if (std::isnan(val)) {
        return std::numeric_limits<float>::quiet_NaN();
    }

    if (p.isLinear) {
        double stretched = std::clamp((static_cast<double>(val) - p.lowPoint) / p.spread, 0.0, 1.0);
        stretched = blastro::MathUtils::applyGhsProtection(val, symmetryPoint, shadowProtect, highlightProtect, stretched);
        return static_cast<float>(stretched);
    }

    double x = (static_cast<double>(val) - symmetryPoint) / p.spread;
    double result = 0.0;
    if (x >= 0.0) {
        result = ghsFunction(x, p.D, form);
    } else {
        result = -ghsFunction(-x, p.D, form);
    }

    double normalized = (result - p.a) / p.denom;
    double stretched = std::clamp(normalized, 0.0, 1.0);

    stretched = blastro::MathUtils::applyGhsProtection(val, symmetryPoint, shadowProtect, highlightProtect, stretched);

    return static_cast<float>(stretched);
}

struct HistogramParams {
    double blackpoint;
    double range;
    double midpoint;
    double m_minus_1;
    double two_m_minus_1;
};

static HistogramParams precomputeHistogram(double blackpoint, double whitepoint, double midpoint) {
    HistogramParams p;
    p.blackpoint = blackpoint;
    p.range = whitepoint - blackpoint;
    if (std::abs(p.range) < 1e-12) p.range = 1e-12;
    p.midpoint = midpoint;
    p.m_minus_1 = midpoint - 1.0;
    p.two_m_minus_1 = 2.0 * midpoint - 1.0;
    return p;
}

static float evaluateHistogramPixel(float val, const HistogramParams& p) {
    if (std::isnan(val)) {
        return std::numeric_limits<float>::quiet_NaN();
    }
    double x = (static_cast<double>(val) - p.blackpoint) / p.range;
    x = std::clamp(x, 0.0, 1.0);
    if (x <= 0.0) return 0.0f;
    if (x >= 1.0) return 1.0f;
    double m = p.midpoint;
    if (m <= 0.0) return 0.0f;
    if (m >= 1.0) return 1.0f;
    return static_cast<float>(p.m_minus_1 * x / (p.two_m_minus_1 * x - m));
}

GrayscaleImagePtr StretchingAlgorithm::stretchHistogramGrayscale(GrayscaleImagePtr src,
                                                                 double blackpoint,
                                                                 double whitepoint,
                                                                 double midpoint) {
    if (!src) return nullptr;
    int w = src->width();
    int h = src->height();
    int numPixels = w * h;

    auto outBuffer = std::make_shared<ImageBuffer>(w, h);
    float* outData = outBuffer->data();
    const float* inData = src->buffer()->data();

    HistogramParams p = precomputeHistogram(blackpoint, whitepoint, midpoint);

    #pragma omp parallel for
    for (int i = 0; i < numPixels; ++i) {
        outData[i] = evaluateHistogramPixel(inData[i], p);
    }

    return std::make_shared<GrayscaleImage>(outBuffer);
}

RGBImagePtr StretchingAlgorithm::stretchHistogramRGB(RGBImagePtr src,
                                                     const std::array<double, 3>& blackpoint,
                                                     const std::array<double, 3>& whitepoint,
                                                     const std::array<double, 3>& midpoint,
                                                     bool colorPreserving) {
    if (!src) return nullptr;
    int w = src->width();
    int h = src->height();
    int numPixels = w * h;

    if (colorPreserving) {
        auto outR = std::make_shared<GrayscaleImage>(w, h);
        auto outG = std::make_shared<GrayscaleImage>(w, h);
        auto outB = std::make_shared<GrayscaleImage>(w, h);

        float* outRData = outR->buffer()->data();
        float* outGData = outG->buffer()->data();
        float* outBData = outB->buffer()->data();

        const float* rData = src->r()->buffer()->data();
        const float* gData = src->g()->buffer()->data();
        const float* bData = src->b()->buffer()->data();

        HistogramParams p = precomputeHistogram(blackpoint[0], whitepoint[0], midpoint[0]);

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

            float Y = (r + g + b) / 3.0f;
            if (Y < 1e-6f) {
                outRData[i] = 0.0f;
                outGData[i] = 0.0f;
                outBData[i] = 0.0f;
            } else {
                float Y_stretched = evaluateHistogramPixel(Y, p);
                float ratio = Y_stretched / Y;

                outRData[i] = std::clamp(r * ratio, 0.0f, 1.0f);
                outGData[i] = std::clamp(g * ratio, 0.0f, 1.0f);
                outBData[i] = std::clamp(b * ratio, 0.0f, 1.0f);
            }
        }

        return std::make_shared<RGBImage>(outR, outG, outB);
    } else {
        auto extR = stretchHistogramGrayscale(src->r(), blackpoint[0], whitepoint[0], midpoint[0]);
        auto extG = stretchHistogramGrayscale(src->g(), blackpoint[1], whitepoint[1], midpoint[1]);
        auto extB = stretchHistogramGrayscale(src->b(), blackpoint[2], whitepoint[2], midpoint[2]);
        return std::make_shared<RGBImage>(extR, extG, extB);
    }
}

GrayscaleImagePtr StretchingAlgorithm::stretchGhsGrayscale(GrayscaleImagePtr src,
                                                           double lowPoint,
                                                           double highPoint,
                                                           double symmetryPoint,
                                                           double stretchFactor,
                                                           double shadowProtect,
                                                           double highlightProtect,
                                                           int form) {
    if (!src) return nullptr;
    int w = src->width();
    int h = src->height();
    int numPixels = w * h;

    auto outBuffer = std::make_shared<ImageBuffer>(w, h);
    float* outData = outBuffer->data();
    const float* inData = src->buffer()->data();

    GhsParams p = precomputeGhs(lowPoint, highPoint, symmetryPoint, stretchFactor, form);

    #pragma omp parallel for
    for (int i = 0; i < numPixels; ++i) {
        outData[i] = evaluateGhsPixel(inData[i], p, symmetryPoint, shadowProtect, highlightProtect, form);
    }

    return std::make_shared<GrayscaleImage>(outBuffer);
}

RGBImagePtr StretchingAlgorithm::stretchGhsRGB(RGBImagePtr src,
                                               const std::array<double, 3>& lowPoint,
                                               const std::array<double, 3>& highPoint,
                                               const std::array<double, 3>& symmetryPoint,
                                               const std::array<double, 3>& stretchFactor,
                                               const std::array<double, 3>& shadowProtect,
                                               const std::array<double, 3>& highlightProtect,
                                               int form,
                                               bool colorPreserving) {
    if (!src) return nullptr;
    int w = src->width();
    int h = src->height();
    int numPixels = w * h;

    if (colorPreserving) {
        auto outR = std::make_shared<GrayscaleImage>(w, h);
        auto outG = std::make_shared<GrayscaleImage>(w, h);
        auto outB = std::make_shared<GrayscaleImage>(w, h);

        float* outRData = outR->buffer()->data();
        float* outGData = outG->buffer()->data();
        float* outBData = outB->buffer()->data();

        const float* rData = src->r()->buffer()->data();
        const float* gData = src->g()->buffer()->data();
        const float* bData = src->b()->buffer()->data();

        GhsParams p = precomputeGhs(lowPoint[0], highPoint[0], symmetryPoint[0], stretchFactor[0], form);

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

            float Y = (r + g + b) / 3.0f;
            if (Y < 1e-6f) {
                outRData[i] = 0.0f;
                outGData[i] = 0.0f;
                outBData[i] = 0.0f;
            } else {
                float Y_stretched = evaluateGhsPixel(Y, p, symmetryPoint[0], shadowProtect[0], highlightProtect[0], form);
                float ratio = Y_stretched / Y;

                outRData[i] = std::clamp(r * ratio, 0.0f, 1.0f);
                outGData[i] = std::clamp(g * ratio, 0.0f, 1.0f);
                outBData[i] = std::clamp(b * ratio, 0.0f, 1.0f);
            }
        }

        return std::make_shared<RGBImage>(outR, outG, outB);
    } else {
        auto extR = stretchGhsGrayscale(src->r(), lowPoint[0], highPoint[0], symmetryPoint[0], stretchFactor[0], shadowProtect[0], highlightProtect[0], form);
        auto extG = stretchGhsGrayscale(src->g(), lowPoint[1], highPoint[1], symmetryPoint[1], stretchFactor[1], shadowProtect[1], highlightProtect[1], form);
        auto extB = stretchGhsGrayscale(src->b(), lowPoint[2], highPoint[2], symmetryPoint[2], stretchFactor[2], shadowProtect[2], highlightProtect[2], form);
        return std::make_shared<RGBImage>(extR, extG, extB);
    }
}

void StretchingAlgorithm::rgbToHsl(float r, float g, float b, float& h, float& s, float& l) {
    float mx = std::max({r, g, b});
    float mn = std::min({r, g, b});
    l = (mx + mn) / 2.0f;
    if (mx == mn) {
        h = s = 0.0f;
    } else {
        float d = mx - mn;
        s = l > 0.5f ? d / (2.0f - mx - mn) : d / (mx + mn);
        if (mx == r) h = (g - b) / d + (g < b ? 6.0f : 0.0f);
        else if (mx == g) h = (b - r) / d + 2.0f;
        else h = (r - g) / d + 4.0f;
        h /= 6.0f;
    }
}

static float hue2rgb(float p, float q, float t) {
    if (t < 0.0f) t += 1.0f;
    if (t > 1.0f) t -= 1.0f;
    if (t < 1.0f/6.0f) return p + (q - p) * 6.0f * t;
    if (t < 1.0f/2.0f) return q;
    if (t < 2.0f/3.0f) return p + (q - p) * (2.0f/3.0f - t) * 6.0f;
    return p;
}

void StretchingAlgorithm::hslToRgb(float h, float s, float l, float& r, float& g, float& b) {
    if (s == 0.0f) {
        r = g = b = l;
    } else {
        float q = l < 0.5f ? l * (1.0f + s) : l + s - l * s;
        float p = 2.0f * l - q;
        r = hue2rgb(p, q, h + 1.0f/3.0f);
        g = hue2rgb(p, q, h);
        b = hue2rgb(p, q, h - 1.0f/3.0f);
    }
}

RGBImagePtr StretchingAlgorithm::stretchHistogramHSL(RGBImagePtr src,
                                                     double blackpoint,
                                                     double whitepoint,
                                                     double midpoint,
                                                     bool stretchSaturation) {
    if (!src) return nullptr;
    int w = src->width();
    int h = src->height();
    int numPixels = w * h;

    auto outR = std::make_shared<GrayscaleImage>(w, h);
    auto outG = std::make_shared<GrayscaleImage>(w, h);
    auto outB = std::make_shared<GrayscaleImage>(w, h);

    float* outRData = outR->buffer()->data();
    float* outGData = outG->buffer()->data();
    float* outBData = outB->buffer()->data();

    const float* rData = src->r()->buffer()->data();
    const float* gData = src->g()->buffer()->data();
    const float* bData = src->b()->buffer()->data();

    HistogramParams p = precomputeHistogram(blackpoint, whitepoint, midpoint);

    #pragma omp parallel for
    for (int i = 0; i < numPixels; ++i) {
        float r = rData[i];
        float g = gData[i];
        float b = bData[i];

        if (std::isnan(r) || std::isnan(g) || std::isnan(b)) {
            outRData[i] = outGData[i] = outBData[i] = std::numeric_limits<float>::quiet_NaN();
            continue;
        }

        float H, S, L;
        rgbToHsl(r, g, b, H, S, L);

        if (stretchSaturation) {
            S = evaluateHistogramPixel(S, p);
        } else {
            L = evaluateHistogramPixel(L, p);
        }

        hslToRgb(H, S, L, r, g, b);

        outRData[i] = std::clamp(r, 0.0f, 1.0f);
        outGData[i] = std::clamp(g, 0.0f, 1.0f);
        outBData[i] = std::clamp(b, 0.0f, 1.0f);
    }

    return std::make_shared<RGBImage>(outR, outG, outB);
}

RGBImagePtr StretchingAlgorithm::stretchGhsHSL(RGBImagePtr src,
                                               double lowPoint,
                                               double highPoint,
                                               double symmetryPoint,
                                               double stretchFactor,
                                               double shadowProtect,
                                               double highlightProtect,
                                               int form,
                                               bool stretchSaturation) {
    if (!src) return nullptr;
    int w = src->width();
    int h = src->height();
    int numPixels = w * h;

    auto outR = std::make_shared<GrayscaleImage>(w, h);
    auto outG = std::make_shared<GrayscaleImage>(w, h);
    auto outB = std::make_shared<GrayscaleImage>(w, h);

    float* outRData = outR->buffer()->data();
    float* outGData = outG->buffer()->data();
    float* outBData = outB->buffer()->data();

    const float* rData = src->r()->buffer()->data();
    const float* gData = src->g()->buffer()->data();
    const float* bData = src->b()->buffer()->data();

    GhsParams p = precomputeGhs(lowPoint, highPoint, symmetryPoint, stretchFactor, form);

    #pragma omp parallel for
    for (int i = 0; i < numPixels; ++i) {
        float r = rData[i];
        float g = gData[i];
        float b = bData[i];

        if (std::isnan(r) || std::isnan(g) || std::isnan(b)) {
            outRData[i] = outGData[i] = outBData[i] = std::numeric_limits<float>::quiet_NaN();
            continue;
        }

        float H, S, L;
        rgbToHsl(r, g, b, H, S, L);

        if (stretchSaturation) {
            S = evaluateGhsPixel(S, p, symmetryPoint, shadowProtect, highlightProtect, form);
        } else {
            L = evaluateGhsPixel(L, p, symmetryPoint, shadowProtect, highlightProtect, form);
        }

        hslToRgb(H, S, L, r, g, b);

        outRData[i] = std::clamp(r, 0.0f, 1.0f);
        outGData[i] = std::clamp(g, 0.0f, 1.0f);
        outBData[i] = std::clamp(b, 0.0f, 1.0f);
    }

    return std::make_shared<RGBImage>(outR, outG, outB);
}

} // namespace blastro
