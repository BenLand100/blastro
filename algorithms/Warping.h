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

namespace blastro {

class Warping {
public:
    // Warps a source image using backward-mapping bilinear interpolation based on translation (dx, dy) and rotation (theta).
    // drizzleScale controls the upsampling (1.0 = no upscale, 2.0 = 2x drizzle, etc.).
    // Out-of-bound pixels are set to NaN so that stacking algorithms can ignore them.
    static GrayscaleImagePtr warpGrayscale(GrayscaleImagePtr src,
                                          double dx, double dy, double theta,
                                          double drizzleScale = 1.0);

    static RGBImagePtr warpRGB(RGBImagePtr src,
                               double dx, double dy, double theta,
                               double drizzleScale = 1.0);
};

} // namespace blastro
