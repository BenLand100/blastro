#pragma once
#include "core/GrayscaleImage.h"
#include "core/RGBImage.h"
#include <vector>
#include <functional>

namespace blastro {

class BackgroundExtractor {
public:
    using ProgressCallback = std::function<void(int)>;

    // Fits a 2D polynomial surface to the background of a single channel and subtracts it.
    // order: Polynomial order (1 to 5).
    // sigmaCut: Standard deviation multiplier to reject outliers (stars, noise).
    // sampleFrac: Fraction of image pixels to sample for fitting (e.g. 0.01 = 1%).
    // huberDelta: Delta parameter for robust Huber loss (default: 5.0).
    // equalize: If true, offsets the channels such that the overall background level is centered at a safe floor.
    static GrayscaleImagePtr extractGrayscale(GrayscaleImagePtr src,
                                             int order = 3,
                                             double sigmaCut = 3.0,
                                             double sampleFrac = 0.01,
                                             double huberDelta = 5.0,
                                             bool equalize = true,
                                             ProgressCallback progress = nullptr,
                                             int progressStart = 0,
                                             int progressEnd = 100);

    static RGBImagePtr extractRGB(RGBImagePtr src,
                                  int order = 3,
                                  double sigmaCut = 3.0,
                                  double sampleFrac = 0.01,
                                  double huberDelta = 5.0,
                                  bool equalize = true,
                                  ProgressCallback progress = nullptr);
};

} // namespace blastro
