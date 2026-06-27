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

class StretchingAlgorithm {
public:
    // Performs traditional Histogram Transformation (HT) on a single channel.
    // blackpoint: [0, 1]
    // whitepoint: [0, 1]
    // midpoint: [0, 1] (relative to [blackpoint, whitepoint])
    static GrayscaleImagePtr stretchHistogramGrayscale(GrayscaleImagePtr src,
                                                       double blackpoint = 0.0,
                                                       double whitepoint = 1.0,
                                                       double midpoint = 0.5);

    // Performs HT on an RGB image.
    static RGBImagePtr stretchHistogramRGB(RGBImagePtr src,
                                           double blackpoint = 0.0,
                                           double whitepoint = 1.0,
                                           double midpoint = 0.5,
                                           bool colorPreserving = true);

    // Performs Generalized Hyperbolic Stretch (GHS) on a single channel.
    static GrayscaleImagePtr stretchGhsGrayscale(GrayscaleImagePtr src,
                                                 double lowPoint = 0.0,
                                                 double highPoint = 1.0,
                                                 double symmetryPoint = 0.5,
                                                 double stretchFactor = 3.0,
                                                 double shadowProtect = 0.0,
                                                 double highlightProtect = 0.0,
                                                 int form = 1);

    // Performs GHS on an RGB image.
    static RGBImagePtr stretchGhsRGB(RGBImagePtr src,
                                     double lowPoint = 0.0,
                                     double highPoint = 1.0,
                                     double symmetryPoint = 0.5,
                                     double stretchFactor = 3.0,
                                     double shadowProtect = 0.0,
                                     double highlightProtect = 0.0,
                                     int form = 1,
                                     bool colorPreserving = true);
};

} // namespace blastro
