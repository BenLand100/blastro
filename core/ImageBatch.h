#pragma once
#include "GrayscaleImage.h"
#include "RGBImage.h"
#include <vector>
#include <variant>
#include <functional>
#include <memory>

namespace blastro {

using ImageVariant = std::variant<GrayscaleImagePtr, RGBImagePtr>;

class ImageBatch {
public:
    using LoaderFunc = std::function<ImageVariant(int)>;

    ImageBatch(int count, LoaderFunc loader);
    ~ImageBatch() = default;
    
    int count() const { return m_count; }
    ImageVariant getImage(int index);

private:
    int m_count;
    LoaderFunc m_loader;
    std::vector<ImageVariant> m_cache;
};

using ImageBatchPtr = std::shared_ptr<ImageBatch>;

} // namespace blastro
