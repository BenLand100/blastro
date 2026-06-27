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
