#pragma once
#include <vector>
#include <memory>

namespace blastro {

class ImageBuffer {
public:
    ImageBuffer(int width, int height);
    ImageBuffer(int width, int height, const std::vector<float>& data);
    ~ImageBuffer() = default;

    int width() const { return m_width; }
    int height() const { return m_height; }
    
    float pixel(int x, int y) const;
    void setPixel(int x, int y, float value);
    
    const float* data() const { return m_data.data(); }
    float* data() { return m_data.data(); }
    const std::vector<float>& rawData() const { return m_data; }

private:
    int m_width;
    int m_height;
    std::vector<float> m_data;
};

using ImageBufferPtr = std::shared_ptr<ImageBuffer>;

} // namespace blastro
