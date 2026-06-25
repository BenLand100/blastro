#include "ImageBuffer.h"
#include <stdexcept>

namespace blastro {

ImageBuffer::ImageBuffer(int width, int height)
    : m_width(width), m_height(height), m_data(width * height, 0.0f) {
    if (width <= 0 || height <= 0) {
        throw std::invalid_argument("Image dimensions must be positive");
    }
}

ImageBuffer::ImageBuffer(int width, int height, const std::vector<float>& data)
    : m_width(width), m_height(height), m_data(data) {
    if (width <= 0 || height <= 0) {
        throw std::invalid_argument("Image dimensions must be positive");
    }
    if (static_cast<size_t>(width * height) != data.size()) {
        throw std::invalid_argument("Data size does not match image dimensions");
    }
}

float ImageBuffer::pixel(int x, int y) const {
    if (x < 0 || x >= m_width || y < 0 || y >= m_height) {
        return 0.0f; // Safe out-of-bounds access
    }
    return m_data[y * m_width + x];
}

void ImageBuffer::setPixel(int x, int y, float value) {
    if (x >= 0 && x < m_width && y >= 0 && y < m_height) {
        m_data[y * m_width + x] = value;
    }
}

} // namespace blastro
