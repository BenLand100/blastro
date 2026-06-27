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
#include "ImageReader.h"
#include "ImageWriter.h"
#include <QImage>

namespace blastro {

class QtImageIO : public ImageReader, public ImageWriter {
public:
    QtImageIO() = default;
    ~QtImageIO() override = default;

    // ImageReader interface
    bool supportsExtension(const std::string& ext) const override;
    ImageVariant readImage(const std::string& filepath) override;
    ImageBatchPtr readBatch(const std::vector<std::string>& filepaths) override;

    // ImageWriter interface
    bool writeImage(const std::string& filepath, const ImageVariant& image) override;
};

} // namespace blastro
