#pragma once
#include "core/GrayscaleImage.h"
#include "core/RGBImage.h"
#include "core/ImageBatch.h"
#include <string>
#include <vector>
#include <memory>

namespace blastro {

class ImageReader {
public:
    virtual ~ImageReader() = default;
    
    virtual bool supportsExtension(const std::string& ext) const = 0;
    virtual ImageVariant readImage(const std::string& filepath) = 0;
    virtual ImageBatchPtr readBatch(const std::vector<std::string>& filepaths) = 0;
};

using ImageReaderPtr = std::shared_ptr<ImageReader>;

} // namespace blastro
