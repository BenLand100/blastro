---
type: architecture
title: Core Architecture and Workspace Registry
description: Comprehensive technical guidelines for blastro_core, ImageBuffer memory layout, OpenMP concurrency, and WorkspaceRegistry thread safety.
tags: [core, memory, threading, concurrency, openmp]
---

# Core Architecture & Workspace Registry

This document details the core data structures, memory layout, and concurrency patterns governing the `blastro_core` library.

---

## 1. The Workspace Registry

The `WorkspaceRegistry` is the central repository for all loaded image data in BLastro.

- **Data Representation**: Elements are stored in a `std::variant<GrayscaleImagePtr, RGBImagePtr, ImageBatchPtr>`.
- **Execution Model**: Algorithms query or mutate workspace elements directly in place or register newly generated elements under unique names.
- **Reactive Workspace Event Callbacks**: 
  - Supports registering callbacks (`setElementRegisteredCallback`, `setElementUnregisteredCallback`, `setElementRenamedCallback`).
  - **Thread Safety Constraint**: Callbacks MUST be invoked **outside** the registry's internal mutex locks (`std::lock_guard<std::mutex>`) to prevent recursive deadlocks.
  - **GUI Marshaling Constraint**: Actions that update Qt GUI state from registry events MUST be marshaled back to the main thread via `QMetaObject::invokeMethod` using queued connections (`Qt::QueuedConnection`).

---

## 2. ImageBuffer Memory Layout and Access

`ImageBuffer` is the fundamental container for 32-bit floating-point single-channel pixel data.

### Contiguous Memory Allocation
- Pixel data is stored contiguously in a `std::vector<float, AlignedAllocator<float, 32>>`.
- The memory is 32-byte aligned to allow AVX/SIMD vectorization.

### Pixel Access Patterns
1. **Safe/Bounds-Checked Access**:
   - `buffer->pixel(x, y)` and `buffer->setPixel(x, y, val)`.
   - Out-of-bounds coordinates return `0.0f` safely or are ignored.
   - Recommended for localized operations such as interpolation, sampling, or stencil filters.
2. **Direct Raw Pointer (High Performance)**:
   - `buffer->data()` yields the raw `float*` data pointer.
   - In full-image processing loops, iterate directly over the contiguous array to avoid function call overhead.

### OpenMP Concurrency Guidelines
All full-image calculations (stretching, calibration, subtraction, math operations) must be parallelized over the raw pointer using OpenMP:

```cpp
float* pixels = buffer->data();
const int numPixels = buffer->width() * buffer->height();

#pragma omp parallel for
for (int i = 0; i < numPixels; ++i) {
    pixels[i] = processPixel(pixels[i]);
}
```

### Viewport Safety During Background Mutating Loops
`ImageBuffer` does not contain internal locks. Mutating buffer data on a background thread while the UI is actively rendering causes race conditions or visual artifacts. Always call `setUpdatesSuspended(true)` on the target view during execution to display a processing overlay.
