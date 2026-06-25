#include "ImageBatch.h"
#include <stdexcept>

namespace blastro {

ImageBatch::ImageBatch(int count, LoaderFunc loader, const std::vector<std::string>& names, const std::vector<std::string>& filepaths)
    : m_count(count), m_names(names), m_filepaths(filepaths), m_selected(count, true), m_cache(count) {
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

ImageVariant ImageBatch::getImage(int index) {
    if (index < 0 || index >= m_count) {
        throw std::out_of_range("Batch index out of range");
    }
    
    // Check if cached
    auto& cached = m_cache[index];
    if (std::holds_alternative<GrayscaleImagePtr>(cached) && std::get<GrayscaleImagePtr>(cached) == nullptr) {
        // Not loaded yet, call the loader
        cached = m_loaders[index]();
    } else if (std::holds_alternative<RGBImagePtr>(cached) && std::get<RGBImagePtr>(cached) == nullptr) {
        cached = m_loaders[index]();
    } else if (cached.index() == 0 && !std::get<0>(cached)) {
        // Variant default-constructed (valueless or null)
        cached = m_loaders[index]();
    }
    
    return cached;
}

std::string ImageBatch::frameName(int index) const {
    if (index < 0 || index >= m_count) {
        throw std::out_of_range("Batch index out of range");
    }
    return m_names[index];
}

bool ImageBatch::isFrameSelected(int index) const {
    if (index < 0 || index >= m_count) {
        throw std::out_of_range("Batch index out of range");
    }
    return m_selected[index];
}

void ImageBatch::setFrameSelected(int index, bool selected) {
    if (index < 0 || index >= m_count) {
        throw std::out_of_range("Batch index out of range");
    }
    m_selected[index] = selected;
}

std::string ImageBatch::frameFilepath(int index) const {
    if (index < 0 || index >= m_count) {
        throw std::out_of_range("Batch index out of range");
    }
    return m_filepaths[index];
}

void ImageBatch::setFrameFilepath(int index, const std::string& path) {
    if (index < 0 || index >= m_count) {
        throw std::out_of_range("Batch index out of range");
    }
    m_filepaths[index] = path;
}

void ImageBatch::addFrame(const std::string& name, const std::string& filepath, FrameLoader loader) {
    m_count++;
    m_names.push_back(name);
    m_filepaths.push_back(filepath);
    m_selected.push_back(true);
    m_cache.push_back(ImageVariant());
    m_loaders.push_back(loader);
}

} // namespace blastro
