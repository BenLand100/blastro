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
