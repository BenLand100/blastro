#pragma once
#include "ImageBuffer.h"

namespace blastro {

class GrayscaleImage {
public:
    GrayscaleImage(int width, int height);
    GrayscaleImage(ImageBufferPtr buffer);
    ~GrayscaleImage() = default;
    
    int width() const { return m_buffer->width(); }
    int height() const { return m_buffer->height(); }
    
    ImageBufferPtr buffer() { return m_buffer; }
    const ImageBufferPtr buffer() const { return m_buffer; }

private:
    ImageBufferPtr m_buffer;
};

using GrayscaleImagePtr = std::shared_ptr<GrayscaleImage>;

} // namespace blastro
