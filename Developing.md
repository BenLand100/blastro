# BLastro Developer Guide

Welcome to the BLastro developer documentation! This guide covers the architectural design principles of the BLastro astronomical image processing toolkit and how to extend it.

## Architecture Overview

BLastro is built using modern C++17, Qt6, and OpenMP, prioritizing performance and extensibility. The project is divided into four main modules:

1. **`core`**: Contains the foundational data structures (`ImageBuffer`, `GrayscaleImage`, `RGBImage`, `ImageBatch`) and the central `WorkspaceRegistry`. It also includes the `PCLBridge` for PixInsight compatibility.
2. **`algorithms`**: Implements astronomical image processing routines (e.g., Calibration, Debayer, Alignment, Stacking, Stretching). These are derived from the design patterns of `oscdeeppy`.
3. **`io`**: Handles reading and writing of image formats, with a specialized wrapper for FITS files using CCfits/cfitsio, and standard Qt-supported formats.
4. **`ui`**: Provides a robust Multiple Document Interface (MDI) for the workspace, allowing multiple images and algorithm dialogs to be managed seamlessly.

## Design Principles

### The Workspace Registry

The core concept in BLastro is the `WorkspaceRegistry`. All loaded images and image batches exist within this registry. 
- Elements are defined as a `std::variant<GrayscaleImagePtr, RGBImagePtr, ImageBatchPtr>`.
- Algorithms execute against elements in the workspace, modifying them in place or creating new elements.

### Interacting with the ImageBuffer

`ImageBuffer` is BLastro's primary pixel data container. Understanding how to interact with it is key to writing high-performance algorithms:

1. **Memory Layout**:
   - Pixel data is stored contiguously in a `std::vector<float, AlignedAllocator<float, 32>>`.
   - The memory is 32-byte aligned to allow for AVX/SIMD vectorization.

2. **Pixel Access**:
   - **Safe/Bounds-Checked**: Use `buffer->pixel(x, y)` and `buffer->setPixel(x, y, val)`. Out-of-bounds coordinates return `0.0f` safely or are ignored. Excellent for localized operations like interpolation or sampling.
   - **Direct Pointer (High Performance)**: Use `buffer->data()` to get the raw `float*` pointer. In critical loops, iterate directly over the contiguous array to avoid function call overhead and bounds checking.

3. **Performance Guidelines**:
   - Parallelize loop iterations over raw pixel pointers using OpenMP:
     ```cpp
     float* pixels = buffer->data();
     int numPixels = buffer->width() * buffer->height();
     #pragma omp parallel for
     for (int i = 0; i < numPixels; ++i) {
         pixels[i] = process(pixels[i]);
     }
     ```

4. **Thread Safety**:
   - `ImageBuffer` does not implement locks. Mutating buffer data on a background thread while the UI is rendering will cause visual artifacts or race conditions.
   - Ensure the UI suspends rendering during thread execution by calling `setUpdatesSuspended(true)` (this displays a "Processing..." screen) and releases it afterwards.

### Adding an Algorithm

To add a new processing algorithm:

1. **Create the Algorithm Class:**
   - In `algorithms/`, create a new class inheriting from `blastro::Algorithm`.
   - Implement `name()` to return a unique identifier.
   - Implement `execute(WorkspaceRegistry& workspace, const std::map<std::string, std::string>& config, ProgressCallback progress)` where you parse the config, interact with the workspace, and apply your processing.

2. **Create the UI Dialog (Optional but recommended):**
   - In `ui/`, create a dialog class inheriting from `blastro::AlgorithmDialog`.
   - Provide the UI elements necessary to configure your algorithm.
   - Implement `getConfig()` to map UI states to string-based configuration key-value pairs.
   - Implement `algorithmName()` to match your core algorithm.

3. **Wire it Up:**
    - Register your dialog in `MainWindow.cpp` so it can be launched from the main menu. The base `AlgorithmDialog` class handles the execution signal that triggers the backend algorithm processing.

### In-Place vs. New Image Algorithms

When developing or modifying algorithms in BLastro, it is critical to handle image lifecycle and UI expectations correctly depending on whether the algorithm mutates existing images or creates new ones:

1. **In-Place Mutation Algorithms**
   - **Definition**: Algorithms that modify the active image's pixels directly (e.g. Stretching, Pixel Math with output matching input, or Background Subtraction replacing the current view).
   - **Expectations**:
     - **Undo Checkpoint**: You **must** call `saveUndoState()` on the active `WorkspaceImageWindow` *before* the modification is executed. This deep-copies the original `ImageBuffer`s onto the undo stack.
     - **UI Refresh**: Once execution completes, update the active `WorkspaceImageWindow` via `setElement` to reload the mutated element.
     - **Zoom State**: Ensure zoom/pan/scroll boundaries remain persistent (zoom persistence) by utilizing `preserveZoom = true` in `ImageView::setImage`.

2. **New Image/Batch Creation Algorithms**
   - **Definition**: Algorithms that produce a completely new element in the workspace (e.g. Stacking generating a stacked master, Calibration creating a calibrated batch, or Star Alignment creating aligned frames).
   - **Expectations**:
     - **Workspace Registration**: The algorithm must register the new element (e.g. `GrayscaleImage`, `RGBImage`, or `ImageBatch`) under a new, unique name in the `WorkspaceRegistry`.
     - **Sub-Window Management**: The main application (`MainWindow`) intercepts the execution signal and creates a new MDI sub-window for the newly created element.
     - **Undo Isolation**: The original input images are left unchanged (no undo checkpoint needed on their windows), and the new window starts with a clean, independent undo history.

3. **Hybrid Algorithms**
   - **Definition**: Algorithms that support both replacement (output name equals input name) and creation (different output name).
   - **Expectations**:
     - Programmatically inspect the configured output name. If it matches the input name, follow the **In-Place** guidelines (checkpointing the existing window). Otherwise, follow the **New Image** guidelines (opening a separate sub-window).

### The PCL Bridge

BLastro has a unique subsystem called `PCLBridge` which allows loading compiled plugins meant for the PixInsight ecosystem (PCL modules).
- **How it works:** `PCLBridge` uses `dlopen`/`LoadLibrary` to load `.so`/`.dylib`/`.dll` PCL modules. It provides mock implementations of PCL objects (`PCLImageMock`, `PCLViewMock`, etc.) that wrap our native BLastro `ImageBuffer`s. 
- **Stubs:** A Python script `generate_stubs.py` generates ABI-compatible stubs that satisfy the PCL library's unresolved symbols. `PCLBridge` dynamically overrides these stubs with actual functional hooks to enable PixInsight processes to execute on BLastro data.
- **Console Output & Styling:** Standard ANSI terminal control codes (color/text style escape sequences) printed by PCL modules are intercepted and parsed into HTML `<span>` tags for rich log rendering. Log messages are prefixed with the loaded PCL module's name.
- **In-Place Modification & UI Suspension:** Many algorithms and PCL processes modify image buffer data in-place on background threads. To guarantee thread safety and prevent rendering of corrupt or intermediate state data, the GUI automatically suspends viewport updates, rendering a "Processing..." overlay until the thread completes.
- **Third-Party Updates & Repository Manager:** The `UpdateManagerDialog` supports fetching and installing third-party PCL modules using standard PixInsight update repository structures. It downloads repository-defined `updates.xri` XML manifests over HTTPS, parses `<platform>` target constraints, retrieves target files, and extracts them locally to the configured module folder.

### Undo/Redo System & Viewport State Persistence

BLastro implements an MDI-window-specific Undo/Redo history system and viewport state preservation system to facilitate non-destructive editing workflows:

1. **In-Memory Checkpointing**:
   - The undo history is maintained on a per-window basis (`WorkspaceImageWindow`).
   - Before any destructive action (such as crop/rotate operations in the Image menu, committing a stretch, or running in-place PCL plugin processes), the current `ImageVariant` is cloned (creating deep copies of the raw `ImageBuffer`s) and pushed to the window's undo stack.
   - Redo stacks are cleared upon performing new operations, and histories are capped at a maximum depth of 10 to limit RAM consumption.
   - When an algorithm replaces the active window's contents (e.g. Background Extraction with the same output name), instead of destroying the window, the old state is pushed to the undo stack, and the window's viewport element is replaced in-place via `setElement`.

2. **Viewport Zoom & Pan Persistence**:
   - Operations like changing frames in a batch (`BatchImageWidget`), applying stretches, or performing Undo/Redo should not reset the user's viewport zoom and panning boundaries.
   - To achieve this, the `ImageView::setImage` method supports a `preserveZoom` parameter. When `true`, it bypasses `fitToWindow()` auto-scaling, maintaining the graphics scene's transformation matrix and scroll states.

### Interactive Control Point Algorithms

Algorithms that require user-guided region/coordinate selection (such as Background Subtraction/Extraction) must split their workflow into two phases:
1. **Interactive Placement**: User places, drags, or clears control points represented as a persistent property of the image's data structure (`ImageBuffer`). These coordinates are drawn directly as overlays on the `ImageView` using interactive edit modes.
2. **Algorithm Execution**: The backend algorithm consumes the coordinates stored in the active image's property to sample local pixel values and run calculations (e.g. surface fitting). Because the control points are stored on the `ImageBuffer`, they are automatically deep-copied and restored by the Undo/Redo checkpoint system, enabling the user to undo a subtraction, tweak the points on the original image, and re-apply.

## Performance & Implementation Insights (Astro Use Case)

To ensure BLastro remains extremely performant and reliable under typical astronomical imaging workloads (which involve processing dozens of high-resolution FITS/RAW frames, often exceeding 50 megapixels per frame), follow these implementation insights:

### 1. High-Performance Pixel Processing & Concurrency
- **Avoid Loop Overhead**: Never use bounds-checked pixel accessors like `pixel(x, y)` inside nested loops for whole-image processing. Get the raw float data pointer using `buffer->data()` and iterate contiguously.
- **OpenMP Parallelization**: Almost all whole-image processing (stretching, calibration, subtraction) should be parallelized using `#pragma omp parallel for`. Ensure loops are free of thread-unsafe shared resource access.
- **Memory Allocation**: Astronomical batches allocate gigabytes of RAM. Minimize temporary image copies and buffer allocations. Where possible, perform computations in-place or reuse pre-allocated intermediate buffers.

### 2. Robust Mathematical Solvers
- **Outlier Mitigation**: Raw astronomical frames contain stars, hot pixels, and cosmic rays. Standard Least Squares fitting is highly sensitive to these spikes. Always use robust estimators (like Huber loss or RANSAC) combined with Iteratively Reweighted Least Squares (IRLS) for surface fitting.
- **Coordinate Normalization**: When solving polynomial surfaces (e.g. background models up to order 5), raw pixel coordinates $(x, y)$ can lead to numerical overflow and matrix singularity. Normalize all coordinates to $[-1.0, 1.0]$ relative to image dimensions before building design matrices.
- **Local Neighborhood Sampling**: When evaluating image properties at discrete coordinate points (like user-placed control points or star centroids), sample a small local region (e.g. 5x5 box average or median) to mitigate the impact of random noise or localized stellar signal.

### 3. PCL Module & Downloader Integration
- **TensorFlow Runtime Fallback**: Stars removal and noise reduction PCL modules require the PixInsight TensorFlow runtime library. If missing, configure BLastro to locate it or download it from the configured preference URL (`https://download.starnetastro.com/pixinsight/legacy/tensorflow-runtime/pixinsight_tensorflow_runtime_linux_TF_x64.zip`) and unpack it locally.
- **File Collision & Refcounting**: The package manager tracks all files extracted from update packages in `QSettings`. To prevent package uninstallation from breaking shared dependencies, track reference counts for files and skip deletion if a file is owned by another active package.
- **Safe Extraction Scans**: Always verify the contents of download packages using dry-run zip/tar file list queries (`unzip -Z -1` or `tar -tf`) before extraction, warning the user of potential file collisions and allowing them to uninstall conflicting modules.

## Build Requirements

- C++17 compliant compiler
- CMake 3.16+
- Qt6 (Core, Gui, Widgets)
- OpenMP
- CCfits & cfitsio

Happy coding!
