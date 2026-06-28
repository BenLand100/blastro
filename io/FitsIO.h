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

namespace blastro {

struct FitsHeaderInfo {
    double exposureTime = 0.0;
    std::string filter = "None";
    std::string imageType = "Light";
    int binningX = 1;
    int binningY = 1;
    int width = 0;
    int height = 0;
    double gain = 0.0;
    std::string objectName = "";
};

class FitsIO : public ImageReader, public ImageWriter {
public:
    FitsIO() = default;
    ~FitsIO() override = default;

    static bool readHeaderInfo(const std::string& filepath, FitsHeaderInfo& info);

    // ImageReader interface
    bool supportsExtension(const std::string& ext) const override;
    ImageVariant readImage(const std::string& filepath) override;
    ImageBatchPtr readBatch(const std::vector<std::string>& filepaths) override;

    // 2D Patch support
    ImageVariant readImagePatch(const std::string& filepath, int xStart, int yStart, int patchW, int patchH, int planeIndex = 0);
    bool writeImagePatch(const std::string& filepath, const ImageVariant& patch, int xStart, int yStart);

    // ImageWriter interface
    bool writeImage(const std::string& filepath, const ImageVariant& image) override;
    bool writeBatch(const std::string& filepath, ImageBatchPtr batch);
};

} // namespace blastro
