#include "ImageBatch.h"
#include <stdexcept>

namespace blastro {

ImageBatch::ImageBatch(int count, LoaderFunc loader)
    : m_count(count), m_loader(loader), m_cache(count) {
    if (count <= 0) {
        throw std::invalid_argument("Batch must contain at least one image");
    }
    if (!loader) {
        throw std::invalid_argument("Batch loader function cannot be null");
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
        cached = m_loader(index);
    } else if (std::holds_alternative<RGBImagePtr>(cached) && std::get<RGBImagePtr>(cached) == nullptr) {
        cached = m_loader(index);
    } else if (cached.index() == 0 && !std::get<0>(cached)) {
        // Variant default-constructed (valueless or null)
        cached = m_loader(index);
    }
    
    return cached;
}

} // namespace blastro
