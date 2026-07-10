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
#include "RGBImage.h"
#include "algorithms/StarFinder.h"
#include "ImageMetadata.h"
#include <vector>
#include <variant>
#include <functional>
#include <memory>
#include <mutex>
#include <array>

namespace blastro {

using ImageVariant = std::variant<GrayscaleImagePtr, RGBImagePtr>;

struct FrameMetadata {
    bool registered = false;
    double dx = 0.0;
    double dy = 0.0;
    double theta = 0.0;
    std::array<double, 6> transform = {1.0, 0.0, 0.0, 0.0, 1.0, 0.0}; // [a, b, tx, c, d, ty]
    int starCount = 0;
    double fwhm = 0.0;
    double snr = 0.0;
    std::vector<Star> stars; // Stored registered stars (in-memory only)
    ImageMetadata baseMetadata;
};

class ImageBatch {
public:
    using LoaderFunc = std::function<ImageVariant(int)>;
    using FrameLoader = std::function<ImageVariant()>;

    ImageBatch(int count, LoaderFunc loader, const std::vector<std::string>& names = {}, const std::vector<std::string>& filepaths = {});
    ~ImageBatch() = default;
    
    int count() const;
    ImageVariant getImage(int index);

    std::string frameName(int index) const;
    bool isFrameSelected(int index) const;
    void setFrameSelected(int index, bool selected);

    std::string frameFilepath(int index) const;
    void setFrameFilepath(int index, const std::string& path);
    void setFrameLoader(int index, FrameLoader loader);
    void addFrame(const std::string& name, const std::string& filepath, FrameLoader loader);

    // Memory management: clear cached frames from RAM
    void clearCache(int index);
    void clearCache();

    FrameMetadata frameMetadata(int index) const;
    void setFrameMetadata(int index, const FrameMetadata& metadata);

private:
    int m_count;
    std::vector<FrameLoader> m_loaders;
    std::vector<ImageVariant> m_cache;
    std::vector<std::string> m_names;
    std::vector<std::string> m_filepaths;
    std::vector<bool> m_selected;
    std::vector<FrameMetadata> m_metadata;
    mutable std::mutex m_mutex;
};

using ImageBatchPtr = std::shared_ptr<ImageBatch>;

} // namespace blastro

