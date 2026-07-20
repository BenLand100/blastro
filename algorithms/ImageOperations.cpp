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

#include "ImageOperations.h"
#include <cstring>
#include <algorithm>
#include <stdexcept>
#include <cmath>
#include <stack>

namespace blastro {
namespace ImageOperations {

ImageBufferPtr cloneBuffer(const ImageBufferPtr& src) {
    if (!src) return nullptr;
    auto dest = std::make_shared<ImageBuffer>(src->width(), src->height());
    std::copy(src->data(), src->data() + src->width() * src->height(), dest->data());
    dest->setBgeControlPoints(src->bgeControlPoints());
    return dest;
}

GrayscaleImagePtr cloneImage(const GrayscaleImagePtr& src) {
    if (!src) return nullptr;
    return std::make_shared<GrayscaleImage>(cloneBuffer(src->buffer()));
}

RGBImagePtr cloneImage(const RGBImagePtr& src) {
    if (!src) return nullptr;
    return std::make_shared<RGBImage>(
        cloneImage(src->r()),
        cloneImage(src->g()),
        cloneImage(src->b())
    );
}

ImageVariant cloneImageVariant(const ImageVariant& src) {
    if (std::holds_alternative<GrayscaleImagePtr>(src)) {
        return cloneImage(std::get<GrayscaleImagePtr>(src));
    } else if (std::holds_alternative<RGBImagePtr>(src)) {
        return cloneImage(std::get<RGBImagePtr>(src));
    }
    return ImageVariant();
}

void flipVertical(ImageBufferPtr buf) {
    if (!buf) return;
    int w = buf->width();
    int h = buf->height();
    float* data = buf->data();
    std::vector<float> tempRow(w);
    for (int y = 0; y < h / 2; ++y) {
        float* row1 = data + y * w;
        float* row2 = data + (h - 1 - y) * w;
        std::memcpy(tempRow.data(), row1, w * sizeof(float));
        std::memcpy(row1, row2, w * sizeof(float));
        std::memcpy(row2, tempRow.data(), w * sizeof(float));
    }
}

void mirrorHorizontal(ImageBufferPtr buf) {
    if (!buf) return;
    int w = buf->width();
    int h = buf->height();
    float* data = buf->data();
    for (int y = 0; y < h; ++y) {
        float* row = data + y * w;
        for (int x = 0; x < w / 2; ++x) {
            std::swap(row[x], row[w - 1 - x]);
        }
    }
}

void rotate180(ImageBufferPtr buf) {
    if (!buf) return;
    int numPixels = buf->width() * buf->height();
    float* data = buf->data();
    for (int i = 0; i < numPixels / 2; ++i) {
        std::swap(data[i], data[numPixels - 1 - i]);
    }
}

void flipVertical(ImageVariant& img) {
    if (std::holds_alternative<GrayscaleImagePtr>(img)) {
        flipVertical(std::get<GrayscaleImagePtr>(img)->buffer());
    } else if (std::holds_alternative<RGBImagePtr>(img)) {
        auto rgb = std::get<RGBImagePtr>(img);
        flipVertical(rgb->r()->buffer());
        flipVertical(rgb->g()->buffer());
        flipVertical(rgb->b()->buffer());
    }
}

void mirrorHorizontal(ImageVariant& img) {
    if (std::holds_alternative<GrayscaleImagePtr>(img)) {
        mirrorHorizontal(std::get<GrayscaleImagePtr>(img)->buffer());
    } else if (std::holds_alternative<RGBImagePtr>(img)) {
        auto rgb = std::get<RGBImagePtr>(img);
        mirrorHorizontal(rgb->r()->buffer());
        mirrorHorizontal(rgb->g()->buffer());
        mirrorHorizontal(rgb->b()->buffer());
    }
}

void rotate180(ImageVariant& img) {
    if (std::holds_alternative<GrayscaleImagePtr>(img)) {
        rotate180(std::get<GrayscaleImagePtr>(img)->buffer());
    } else if (std::holds_alternative<RGBImagePtr>(img)) {
        auto rgb = std::get<RGBImagePtr>(img);
        rotate180(rgb->r()->buffer());
        rotate180(rgb->g()->buffer());
        rotate180(rgb->b()->buffer());
    }
}

ImageBufferPtr rotate90CW(const ImageBufferPtr& buf) {
    if (!buf) return nullptr;
    int w = buf->width();
    int h = buf->height();
    auto newBuf = std::make_shared<ImageBuffer>(h, w);
    const float* src = buf->data();
    float* dst = newBuf->data();
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            int nx = h - 1 - y;
            int ny = x;
            dst[ny * h + nx] = src[y * w + x];
        }
    }
    return newBuf;
}

ImageBufferPtr rotate90CCW(const ImageBufferPtr& buf) {
    if (!buf) return nullptr;
    int w = buf->width();
    int h = buf->height();
    auto newBuf = std::make_shared<ImageBuffer>(h, w);
    const float* src = buf->data();
    float* dst = newBuf->data();
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            int nx = y;
            int ny = w - 1 - x;
            dst[ny * h + nx] = src[y * w + x];
        }
    }
    return newBuf;
}

ImageBufferPtr crop(const ImageBufferPtr& buf, const QRect& rect) {
    if (!buf) return nullptr;
    int cw = rect.width();
    int ch = rect.height();
    int rx = rect.x();
    int ry = rect.y();
    if (cw <= 0 || ch <= 0 || rx < 0 || ry < 0 || rx + cw > buf->width() || ry + ch > buf->height()) {
        throw std::runtime_error("Invalid crop dimensions");
    }
    auto newBuf = std::make_shared<ImageBuffer>(cw, ch);
    const float* src = buf->data();
    float* dst = newBuf->data();
    int w = buf->width();
    for (int y = 0; y < ch; ++y) {
        const float* srcRow = src + (ry + y) * w + rx;
        float* dstRow = dst + y * cw;
        std::memcpy(dstRow, srcRow, cw * sizeof(float));
    }
    return newBuf;
}

ImageVariant rotate90CW(const ImageVariant& img) {
    if (std::holds_alternative<GrayscaleImagePtr>(img)) {
        auto src = std::get<GrayscaleImagePtr>(img);
        return std::make_shared<GrayscaleImage>(rotate90CW(src->buffer()));
    } else if (std::holds_alternative<RGBImagePtr>(img)) {
        auto src = std::get<RGBImagePtr>(img);
        return std::make_shared<RGBImage>(
            std::make_shared<GrayscaleImage>(rotate90CW(src->r()->buffer())),
            std::make_shared<GrayscaleImage>(rotate90CW(src->g()->buffer())),
            std::make_shared<GrayscaleImage>(rotate90CW(src->b()->buffer()))
        );
    }
    return ImageVariant();
}

ImageVariant rotate90CCW(const ImageVariant& img) {
    if (std::holds_alternative<GrayscaleImagePtr>(img)) {
        auto src = std::get<GrayscaleImagePtr>(img);
        return std::make_shared<GrayscaleImage>(rotate90CCW(src->buffer()));
    } else if (std::holds_alternative<RGBImagePtr>(img)) {
        auto src = std::get<RGBImagePtr>(img);
        return std::make_shared<RGBImage>(
            std::make_shared<GrayscaleImage>(rotate90CCW(src->r()->buffer())),
            std::make_shared<GrayscaleImage>(rotate90CCW(src->g()->buffer())),
            std::make_shared<GrayscaleImage>(rotate90CCW(src->b()->buffer()))
        );
    }
    return ImageVariant();
}

ImageVariant crop(const ImageVariant& img, const QRect& rect) {
    if (std::holds_alternative<GrayscaleImagePtr>(img)) {
        auto src = std::get<GrayscaleImagePtr>(img);
        if (!src) return ImageVariant();
        auto croppedBuf = crop(src->buffer(), rect);
        if (!croppedBuf) return ImageVariant();
        auto newImg = std::make_shared<GrayscaleImage>(croppedBuf);
        ImageMetadata meta = src->metadata();
        if (meta.fitsKeywords.count("CRPIX1")) {
            try { meta.fitsKeywords["CRPIX1"] = std::to_string(std::stod(meta.fitsKeywords["CRPIX1"]) - rect.x()); } catch (...) {}
        }
        if (meta.fitsKeywords.count("CRPIX2")) {
            try { meta.fitsKeywords["CRPIX2"] = std::to_string(std::stod(meta.fitsKeywords["CRPIX2"]) - rect.y()); } catch (...) {}
        }
        newImg->setMetadata(meta);
        return newImg;
    } else if (std::holds_alternative<RGBImagePtr>(img)) {
        auto src = std::get<RGBImagePtr>(img);
        if (!src) return ImageVariant();
        auto rBuf = crop(src->r()->buffer(), rect);
        auto gBuf = crop(src->g()->buffer(), rect);
        auto bBuf = crop(src->b()->buffer(), rect);
        if (!rBuf || !gBuf || !bBuf) return ImageVariant();
        
        ImageMetadata meta = src->metadata();
        if (meta.fitsKeywords.count("CRPIX1")) {
            try { meta.fitsKeywords["CRPIX1"] = std::to_string(std::stod(meta.fitsKeywords["CRPIX1"]) - rect.x()); } catch (...) {}
        }
        if (meta.fitsKeywords.count("CRPIX2")) {
            try { meta.fitsKeywords["CRPIX2"] = std::to_string(std::stod(meta.fitsKeywords["CRPIX2"]) - rect.y()); } catch (...) {}
        }

        auto newR = std::make_shared<GrayscaleImage>(rBuf);
        auto newG = std::make_shared<GrayscaleImage>(gBuf);
        auto newB = std::make_shared<GrayscaleImage>(bBuf);
        newR->setMetadata(meta);
        newG->setMetadata(meta);
        newB->setMetadata(meta);

        auto newRgb = std::make_shared<RGBImage>(newR, newG, newB);
        newRgb->setMetadata(meta);
        return newRgb;
    }
    return ImageVariant();
}

QRect findLargestBoundingRectangle(const std::vector<FrameTransformInfo>& frames,
                                   int targetW, int targetH,
                                   double drizzleScale) {
    if (frames.empty() || targetW <= 0 || targetH <= 0) {
        return QRect(0, 0, 0, 0);
    }
    if (drizzleScale <= 0.0) drizzleScale = 1.0;

    struct InvertedFrame {
        double inv_a, inv_b, inv_tx;
        double inv_c, inv_d, inv_ty;
        double minX_src, maxX_src;
        double minY_src, maxY_src;
    };

    std::vector<InvertedFrame> invFrames;
    invFrames.reserve(frames.size());

    for (const auto& f : frames) {
        if (f.width <= 0 || f.height <= 0) continue;
        double a = f.transform[0];
        double b = f.transform[1];
        double tx = f.transform[2];
        double c = f.transform[3];
        double d = f.transform[4];
        double ty = f.transform[5];

        double det = a * d - b * c;
        if (std::abs(det) < 1e-9) continue;

        double inv_det = 1.0 / det;
        InvertedFrame inv;
        inv.inv_a = d * inv_det;
        inv.inv_b = -b * inv_det;
        inv.inv_tx = (-d * tx + b * ty) * inv_det;
        inv.inv_c = -c * inv_det;
        inv.inv_d = a * inv_det;
        inv.inv_ty = (-a * ty + c * tx) * inv_det;

        inv.minX_src = 1.0;
        inv.maxX_src = f.width - 2.0;
        inv.minY_src = 1.0;
        inv.maxY_src = f.height - 2.0;

        invFrames.push_back(inv);
    }

    if (invFrames.empty()) {
        return QRect(0, 0, targetW, targetH);
    }

    std::vector<int> heights(targetW, 0);
    int maxArea = 0;
    QRect bestRect(0, 0, 0, 0);

    for (int y = 0; y < targetH; ++y) {
        double yt = y / drizzleScale;

        int rowStart = 0;
        int rowEnd = targetW - 1;

        for (const auto& f : invFrames) {
            double Mx = f.inv_a / drizzleScale;
            double Cx = f.inv_b * yt + f.inv_tx;

            double My = f.inv_c / drizzleScale;
            double Cy = f.inv_d * yt + f.inv_ty;

            // Solve L <= M * X + C <= U
            auto updateRange = [&](double M, double C, double L, double U) {
                if (std::abs(M) < 1e-12) {
                    if (C < L || C > U) {
                        rowStart = targetW; // Invalid
                    }
                } else if (M > 0.0) {
                    int minX = static_cast<int>(std::ceil((L - C) / M));
                    int maxX = static_cast<int>(std::floor((U - C) / M));
                    rowStart = std::max(rowStart, minX);
                    rowEnd = std::min(rowEnd, maxX);
                } else {
                    int minX = static_cast<int>(std::ceil((U - C) / M));
                    int maxX = static_cast<int>(std::floor((L - C) / M));
                    rowStart = std::max(rowStart, minX);
                    rowEnd = std::min(rowEnd, maxX);
                }
            };

            updateRange(Mx, Cx, f.minX_src, f.maxX_src);
            updateRange(My, Cy, f.minY_src, f.maxY_src);

            if (rowStart > rowEnd) break;
        }

        // Update heights for monotonic stack
        for (int x = 0; x < targetW; ++x) {
            if (x >= rowStart && x <= rowEnd && rowStart <= rowEnd) {
                heights[x] += 1;
            } else {
                heights[x] = 0;
            }
        }

        // Monotonic stack to find max rectangle ending at row y
        std::stack<int> st;
        for (int x = 0; x <= targetW; ++x) {
            int h = (x < targetW) ? heights[x] : 0;
            while (!st.empty() && h < heights[st.top()]) {
                int topH = heights[st.top()];
                st.pop();
                int leftX = st.empty() ? 0 : (st.top() + 1);
                int currentW = x - leftX;
                int area = topH * currentW;
                if (area > maxArea) {
                    maxArea = area;
                    bestRect = QRect(leftX, y - topH + 1, currentW, topH);
                }
            }
            st.push(x);
        }
    }

    return bestRect;
}

} // namespace ImageOperations
} // namespace blastro

