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
