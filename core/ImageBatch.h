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
    using FrameLoader = std::function<ImageVariant()>;

    ImageBatch(int count, LoaderFunc loader, const std::vector<std::string>& names = {}, const std::vector<std::string>& filepaths = {});
    ~ImageBatch() = default;
    
    int count() const { return m_count; }
    ImageVariant getImage(int index);

    std::string frameName(int index) const;
    bool isFrameSelected(int index) const;
    void setFrameSelected(int index, bool selected);

    std::string frameFilepath(int index) const;
    void setFrameFilepath(int index, const std::string& path);
    void addFrame(const std::string& name, const std::string& filepath, FrameLoader loader);

private:
    int m_count;
    std::vector<FrameLoader> m_loaders;
    std::vector<ImageVariant> m_cache;
    std::vector<std::string> m_names;
    std::vector<std::string> m_filepaths;
    std::vector<bool> m_selected;
};

using ImageBatchPtr = std::shared_ptr<ImageBatch>;

} // namespace blastro
