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

#pragma once
#include <vector>
#include <utility>
#include <memory>
#include <cstdlib>
#include <new>

namespace blastro {

template <typename T, size_t Alignment>
struct AlignedAllocator {
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    template <typename U>
    struct rebind {
        using other = AlignedAllocator<U, Alignment>;
    };

    AlignedAllocator() noexcept = default;
    template <typename U>
    AlignedAllocator(const AlignedAllocator<U, Alignment>&) noexcept {}

    T* allocate(size_t n) {
        if (n == 0) return nullptr;
        void* ptr = nullptr;
        if (posix_memalign(&ptr, Alignment, n * sizeof(T)) != 0) {
            throw std::bad_alloc();
        }
        return reinterpret_cast<T*>(ptr);
    }

    void deallocate(T* p, size_t) noexcept {
        free(p);
    }

    bool operator==(const AlignedAllocator&) const noexcept { return true; }
    bool operator!=(const AlignedAllocator&) const noexcept { return false; }
};

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
    const std::vector<float, AlignedAllocator<float, 32>>& rawData() const { return m_data; }

    const std::vector<std::pair<double, double>>& bgeControlPoints() const { return m_bgeControlPoints; }
    void setBgeControlPoints(const std::vector<std::pair<double, double>>& pts) { m_bgeControlPoints = pts; }

private:
    int m_width;
    int m_height;
    std::vector<float, AlignedAllocator<float, 32>> m_data;
    std::vector<std::pair<double, double>> m_bgeControlPoints;
};

using ImageBufferPtr = std::shared_ptr<ImageBuffer>;

} // namespace blastro

