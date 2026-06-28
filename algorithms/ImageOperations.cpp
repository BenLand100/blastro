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
        return std::make_shared<GrayscaleImage>(crop(src->buffer(), rect));
    } else if (std::holds_alternative<RGBImagePtr>(img)) {
        auto src = std::get<RGBImagePtr>(img);
        return std::make_shared<RGBImage>(
            std::make_shared<GrayscaleImage>(crop(src->r()->buffer(), rect)),
            std::make_shared<GrayscaleImage>(crop(src->g()->buffer(), rect)),
            std::make_shared<GrayscaleImage>(crop(src->b()->buffer(), rect))
        );
    }
    return ImageVariant();
}

} // namespace ImageOperations
} // namespace blastro
