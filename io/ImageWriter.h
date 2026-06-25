#pragma once
#include "core/GrayscaleImage.h"
#include "core/RGBImage.h"
#include "core/ImageBatch.h"
#include <string>
#include <memory>

namespace blastro {

class ImageWriter {
public:
    virtual ~ImageWriter() = default;
    
    virtual bool supportsExtension(const std::string& ext) const = 0;
    virtual bool writeImage(const std::string& filepath, const ImageVariant& image) = 0;
};

using ImageWriterPtr = std::shared_ptr<ImageWriter>;

} // namespace blastro
