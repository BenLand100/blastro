#include "RGBImage.h"
#include <stdexcept>

namespace blastro {

RGBImage::RGBImage(int width, int height)
    : m_r(std::make_shared<GrayscaleImage>(width, height)),
      m_g(std::make_shared<GrayscaleImage>(width, height)),
      m_b(std::make_shared<GrayscaleImage>(width, height)) {}

RGBImage::RGBImage(GrayscaleImagePtr r, GrayscaleImagePtr g, GrayscaleImagePtr b)
    : m_r(r), m_g(g), m_b(b) {
    if (!r || !g || !b) {
        throw std::invalid_argument("RGB channels cannot be null");
    }
    if (r->width() != g->width() || r->width() != b->width() ||
        r->height() != g->height() || r->height() != b->height()) {
        throw std::invalid_argument("RGB channels must have matching dimensions");
    }
}

} // namespace blastro
