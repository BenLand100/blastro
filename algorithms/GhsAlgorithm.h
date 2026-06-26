#pragma once
#include "core/GrayscaleImage.h"
#include "core/RGBImage.h"
#include "core/ImageBuffer.h"

namespace blastro {

class GhsAlgorithm {
public:
    // Performs the Generalized Hyperbolic Stretch (GHS) on a single channel.
    static GrayscaleImagePtr stretchGrayscale(GrayscaleImagePtr src,
                                             double lowPoint = 0.0,
                                             double highPoint = 1.0,
                                             double symmetryPoint = 0.5,
                                             double stretchFactor = 3.0,
                                             int form = 1);

    // Performs GHS on an RGB image.
    // colorPreserving: If true, stretches luminance and scales RGB, preserving hue/saturation.
    //                  If false, stretches R, G, B channels independently.
    static RGBImagePtr stretchRGB(RGBImagePtr src,
                                  double lowPoint = 0.0,
                                  double highPoint = 1.0,
                                  double symmetryPoint = 0.5,
                                  double stretchFactor = 3.0,
                                  int form = 1,
                                  bool colorPreserving = true);
};

} // namespace blastro
