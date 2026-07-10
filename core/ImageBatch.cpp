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

#include "ImageBatch.h"
#include <stdexcept>

namespace blastro {

ImageBatch::ImageBatch(int count, LoaderFunc loader, const std::vector<std::string>& names, const std::vector<std::string>& filepaths)
    : m_count(count), m_names(names), m_filepaths(filepaths), m_selected(count, true), m_cache(count), m_metadata(count) {
    if (count <= 0) {
        throw std::invalid_argument("Batch must contain at least one image");
    }
    if (!loader) {
        throw std::invalid_argument("Batch loader function cannot be null");
    }

    m_loaders.resize(count);
    for (int i = 0; i < count; ++i) {
        m_loaders[i] = [loader, i]() { return loader(i); };
    }
    
    // Ensure m_names is populated with fallback values if empty or partially empty
    if (m_names.size() < static_cast<size_t>(count)) {
        m_names.resize(count);
    }
    for (int i = 0; i < count; ++i) {
        if (m_names[i].empty()) {
            m_names[i] = "Frame " + std::to_string(i + 1);
        }
    }

    // Ensure m_filepaths is populated
    if (m_filepaths.size() < static_cast<size_t>(count)) {
        m_filepaths.resize(count);
    }
}

int ImageBatch::count() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_count;
}

ImageVariant ImageBatch::getImage(int index) {
    if (index < 0 || index >= m_count) {
        throw std::out_of_range("Batch index out of range");
    }
    
    bool needsLoad = false;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        const auto& cached = m_cache[index];
        if (std::holds_alternative<GrayscaleImagePtr>(cached) && std::get<GrayscaleImagePtr>(cached) == nullptr) {
            needsLoad = true;
        } else if (std::holds_alternative<RGBImagePtr>(cached) && std::get<RGBImagePtr>(cached) == nullptr) {
            needsLoad = true;
        } else if (cached.index() == 0 && !std::get<0>(cached)) {
            needsLoad = true;
        } else {
            return cached;
        }
    }
    
    // Load without holding the lock so multiple threads can load different images simultaneously
    ImageVariant loaded = m_loaders[index]();
    
    std::lock_guard<std::mutex> lock(m_mutex);
    m_cache[index] = loaded;
    return loaded;
}

std::string ImageBatch::frameName(int index) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (index < 0 || index >= m_count) {
        throw std::out_of_range("Batch index out of range");
    }
    return m_names[index];
}

bool ImageBatch::isFrameSelected(int index) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (index < 0 || index >= m_count) {
        throw std::out_of_range("Batch index out of range");
    }
    return m_selected[index];
}

void ImageBatch::setFrameSelected(int index, bool selected) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (index < 0 || index >= m_count) {
        throw std::out_of_range("Batch index out of range");
    }
    m_selected[index] = selected;
}

std::string ImageBatch::frameFilepath(int index) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (index < 0 || index >= m_count) {
        throw std::out_of_range("Batch index out of range");
    }
    return m_filepaths[index];
}

void ImageBatch::setFrameFilepath(int index, const std::string& path) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (index < 0 || index >= m_count) {
        throw std::out_of_range("Batch index out of range");
    }
    m_filepaths[index] = path;
}

void ImageBatch::setFrameLoader(int index, FrameLoader loader) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (index < 0 || index >= m_count) {
        throw std::out_of_range("Batch index out of range");
    }
    m_loaders[index] = loader;
}

void ImageBatch::addFrame(const std::string& name, const std::string& filepath, FrameLoader loader) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_count++;
    m_names.push_back(name);
    m_filepaths.push_back(filepath);
    m_selected.push_back(true);
    m_cache.push_back(ImageVariant());
    m_loaders.push_back(loader);
    m_metadata.push_back(FrameMetadata());
}

void ImageBatch::clearCache(int index) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (index < 0 || index >= m_count) {
        throw std::out_of_range("Batch index out of range");
    }
    m_cache[index] = ImageVariant();
}

void ImageBatch::clearCache() {
    std::lock_guard<std::mutex> lock(m_mutex);
    for (int i = 0; i < m_count; ++i) {
        m_cache[i] = ImageVariant();
    }
}

FrameMetadata ImageBatch::frameMetadata(int index) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (index < 0 || index >= m_count) {
        throw std::out_of_range("Batch index out of range");
    }
    return m_metadata[index];
}

void ImageBatch::setFrameMetadata(int index, const FrameMetadata& metadata) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (index < 0 || index >= m_count) {
        throw std::out_of_range("Batch index out of range");
    }
    m_metadata[index] = metadata;
}

} // namespace blastro
