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
