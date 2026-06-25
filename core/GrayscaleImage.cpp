#include "GrayscaleImage.h"

namespace blastro {

GrayscaleImage::GrayscaleImage(int width, int height)
    : m_buffer(std::make_shared<ImageBuffer>(width, height)) {}

GrayscaleImage::GrayscaleImage(ImageBufferPtr buffer)
    : m_buffer(buffer) {}

} // namespace blastro
