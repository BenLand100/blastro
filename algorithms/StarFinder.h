#pragma once
#include "core/GrayscaleImage.h"
#include <vector>

namespace blastro {

struct Star {
    double x = 0.0;
    double y = 0.0;
    double fwhm = 0.0;
    double peak = 0.0;
    double background = 0.0;
    double eccentricity = 0.0;
};

class StarFinder {
public:
    // Finds stars in a grayscale image.
    // If fastFit is true, it uses centroiding. If false, it fits a 2D Gaussian using Nelder-Mead.
    static std::vector<Star> findStars(GrayscaleImagePtr img,
                                       int maxStars = 250,
                                       double snrMin = 5.0,
                                       bool fastFit = false,
                                       int patchRadius = 10,
                                       double minFwhm = 2.0,
                                       double maxEccentricity = 0.85);
};

} // namespace blastro
