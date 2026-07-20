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

#pragma once
#include "core/ImageBuffer.h"
#include "core/ImageBatch.h"
#include <QRect>

namespace blastro {
namespace ImageOperations {

// Clones
ImageBufferPtr cloneBuffer(const ImageBufferPtr& src);
GrayscaleImagePtr cloneImage(const GrayscaleImagePtr& src);
RGBImagePtr cloneImage(const RGBImagePtr& src);
ImageVariant cloneImageVariant(const ImageVariant& src);

// Simple mutations (in-place)
void flipVertical(ImageBufferPtr buf);
void mirrorHorizontal(ImageBufferPtr buf);
void rotate180(ImageBufferPtr buf);

void flipVertical(ImageVariant& img);
void mirrorHorizontal(ImageVariant& img);
void rotate180(ImageVariant& img);

// Simple mutations (creating new buffers/images due to dimension changes)
ImageBufferPtr rotate90CW(const ImageBufferPtr& buf);
ImageBufferPtr rotate90CCW(const ImageBufferPtr& buf);
ImageBufferPtr crop(const ImageBufferPtr& buf, const QRect& rect);

ImageVariant rotate90CW(const ImageVariant& img);
ImageVariant rotate90CCW(const ImageVariant& img);
ImageVariant crop(const ImageVariant& img, const QRect& rect);

struct FrameTransformInfo {
    std::array<double, 6> transform = {1.0, 0.0, 0.0, 0.0, 1.0, 0.0};
    int width = 0;
    int height = 0;
};

// Calculates the largest axis-aligned rectangle contained within the transformed bounds
// of every provided frame info across target dimensions targetW x targetH.
QRect findLargestBoundingRectangle(const std::vector<FrameTransformInfo>& frames,
                                   int targetW, int targetH,
                                   double drizzleScale = 1.0);

} // namespace ImageOperations
} // namespace blastro

