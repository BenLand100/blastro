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
