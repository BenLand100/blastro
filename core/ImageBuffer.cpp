/*
 * BLastro - Astronomical Image Processing Software
 * Copyright (C) 2026 Benjamin Land
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

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
    : m_width(width), m_height(height), m_data(data.begin(), data.end()) {
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
