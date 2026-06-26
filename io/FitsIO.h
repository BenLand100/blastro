#pragma once
#include "ImageReader.h"
#include "ImageWriter.h"

namespace blastro {

class FitsIO : public ImageReader, public ImageWriter {
public:
    FitsIO() = default;
    ~FitsIO() override = default;

    // ImageReader interface
    bool supportsExtension(const std::string& ext) const override;
    ImageVariant readImage(const std::string& filepath) override;
    ImageBatchPtr readBatch(const std::vector<std::string>& filepaths) override;

    // 2D Patch support
    ImageVariant readImagePatch(const std::string& filepath, int xStart, int yStart, int patchW, int patchH);
    bool writeImagePatch(const std::string& filepath, const ImageVariant& patch, int xStart, int yStart);

    // ImageWriter interface
    bool writeImage(const std::string& filepath, const ImageVariant& image) override;
    bool writeBatch(const std::string& filepath, ImageBatchPtr batch);
};

} // namespace blastro
