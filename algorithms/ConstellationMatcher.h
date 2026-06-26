#pragma once
#include "StarFinder.h"
#include <vector>
#include <array>

namespace blastro {

struct Triangle {
    std::array<double, 3> dists; // Sorted side lengths: d1 >= d2 >= d3
    std::array<Star, 3> abc;     // Canonically ordered vertices (opposite to dists)
};

struct AlignmentResult {
    bool success = false;
    double dx = 0.0;
    double dy = 0.0;
    double theta = 0.0;
    double rmsError = 0.0;
    int matchedStars = 0;
};

class ConstellationMatcher {
public:
    // Computes the rotation and translation that maps the target stars onto the reference stars.
    static AlignmentResult match(const std::vector<Star>& refStars,
                                 const std::vector<Star>& targetStars,
                                 int kNearest = 7,
                                 double matchTolerance = 1.5);

private:
    static std::vector<Triangle> buildTriangles(const std::vector<Star>& stars, int kNearest);
};

} // namespace blastro
