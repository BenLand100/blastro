#pragma once
#include "GrayscaleImage.h"

namespace blastro {

class RGBImage {
public:
    RGBImage(int width, int height);
    RGBImage(GrayscaleImagePtr r, GrayscaleImagePtr g, GrayscaleImagePtr b);
    ~RGBImage() = default;
    
    int width() const { return m_r->width(); }
    int height() const { return m_r->height(); }
    
    GrayscaleImagePtr r() { return m_r; }
    GrayscaleImagePtr g() { return m_g; }
    GrayscaleImagePtr b() { return m_b; }
    
    const GrayscaleImagePtr r() const { return m_r; }
    const GrayscaleImagePtr g() const { return m_g; }
    const GrayscaleImagePtr b() const { return m_b; }

private:
    GrayscaleImagePtr m_r;
    GrayscaleImagePtr m_g;
    GrayscaleImagePtr m_b;
};

using RGBImagePtr = std::shared_ptr<RGBImage>;

} // namespace blastro
