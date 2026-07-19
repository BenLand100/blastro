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
#include "core/GrayscaleImage.h"
#include "core/RGBImage.h"
#include "core/ImageBuffer.h"
#include <array>
#include <string>

namespace blastro {

class Warping {
public:
    // Warps a source image using backward-mapping interpolation based on a 6-element affine matrix.
    // drizzleScale controls the upsampling (1.0 = no upscale, 2.0 = 2x drizzle, etc.).
    // Out-of-bound pixels are set to NaN so that stacking algorithms can ignore them.
    static GrayscaleImagePtr warpGrayscale(GrayscaleImagePtr src,
                                          const std::array<double, 6>& transform,
                                          double drizzleScale = 1.0,
                                          const std::string& interpolation = "bilinear");

    static RGBImagePtr warpRGB(RGBImagePtr src,
                               const std::array<double, 6>& transform,
                               double drizzleScale = 1.0,
                               const std::string& interpolation = "bilinear");

    // Drizzle forward-mapping
    // Returns a pair: {Data Image, Weight Image}
    static std::pair<GrayscaleImagePtr, GrayscaleImagePtr> warpDrizzleGrayscale(
        GrayscaleImagePtr src,
        const std::array<double, 6>& transform,
        double drizzleScale = 1.0,
        double dropShrink = 1.0);
};

} // namespace blastro
