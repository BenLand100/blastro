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
   - **Viewport Navigation Coexistence**: To allow standard panning (click-and-drag) and zooming even while edit modes are open, coordinate placement must require a modifier key (e.g. **Ctrl + Left Click** to add, **Ctrl + Right Click** to remove).
   - **Visibility States**: Control point visibility should remain generally off by default to avoid cluttering the viewport, turning on automatically only when active user interaction occurs (such as a Ctrl+click or triggering a grid generator). Visibility should turn off immediately when the algorithm successfully executes.
2. **Algorithm Execution**: The backend algorithm consumes the coordinates stored in the active image's property to sample local pixel values and run calculations (e.g. surface fitting). Because the control points are stored on the `ImageBuffer`, they are automatically deep-copied and restored by the Undo/Redo checkpoint system, enabling the user to undo a subtraction, tweak the points on the original image, and re-apply.
   - **Post-Execution Cleanup**: Running BGE outputs a new image buffer that contains no control points. This naturally clears the points in the active view, while the undo stack preserves the original buffer with its control points intact.

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
- **Background Subtraction Verification**: Graphical user interfaces frequently apply auto-stretches that dynamically scale pixel value ranges. As a result, a raw gradient and a neutralized background might look visually similar on-screen. To verify that subtraction occurred, compare numerical pixel values across corners or run synthetic tests (e.g. `tests/generate_gradient_fits.py` paired with `testBackgroundExtractionOnGradient()` in `tests/test_algorithms.cpp`).

### 3. PCL Module & Downloader Integration
- **TensorFlow Runtime & Lib Preloading**: Stars removal and noise reduction PCL modules require the PixInsight TensorFlow runtime library. This is installed and loaded strictly within the configured PCL lib directory (`Preferences::instance().getPclLibFolder()`). The option to preload all libraries recursively scans the lib directory on startup to resolve symbol dependencies.
- **File Collision & Refcounting**: The package manager tracks all files extracted from update packages in `QSettings`. To prevent package uninstallation from breaking shared dependencies, track reference counts for files and skip deletion if a file is owned by another active package.
- **Safe Extraction Scans**: Always verify the contents of download packages using dry-run zip/tar file list queries (`unzip -Z -1` or `tar -tf`) before extraction, warning the user of potential file collisions and allowing them to uninstall conflicting modules.

### 4. Image Stacking & NaN Handling

- **NaN as Masked/Missing Data**: In BLastro, `NaN` (Not a Number) values represent masked or missing pixel data rather than arithmetic errors. This includes sparse data patterns (like raw bayer channels before debayering), aligned frame edges (due to dithering and rotation shifts), or mosaic overlaps. Stacking algorithms must treat `NaN`s as missing observations, not outliers to reject.
- **Dynamic Normalization**: At each stacked pixel location, stacking algorithms must only calculate statistics (means, medians, and sigmas) using non-NaN pixel values. When normalizing (averaging) at a pixel location, the summed intensity of valid pixels is divided by the dynamic count of non-NaN frames contributing at that specific location, rather than the constant total frame count. E.g., if an average stack has 50 frames, 5 NaN and 45 with value 0.15, the output is `(45 * 0.15) / 45 = 0.15`, not `(45 * 0.15) / 50`.
- **All-NaN Pixel Propagation**: If every contributing frame has a `NaN` value at a given pixel, the stacked output pixel must be set to `NaN` (to indicate that no data exists at that location), rather than falling back to a arbitrary numeric value like `0.0f`.
- **NaN Handling in Other Algorithms**: Other algorithms (like Stretching, Background Extraction, Calibration, or Histogram calculation) must safely propagate or ignore `NaN` values. Any sorting of pixel values (e.g., finding medians or quantiles for background fitting or auto-stretching) must filter out `NaN`s beforehand to prevent undefined behavior in `std::sort`.

### 5. UI/UX Cascading, Renaming, Cleanup & Dialog Layout
- **Window Cascading**: When opening new image windows, offset their initial MDI position slightly sequentially in X and Y (e.g. 20-30 pixels) to avoid stacking them perfectly on top of each other, cascading them naturally over time.
- **Output Filename Patterning**: All algorithm dialogs should support output name patterning. By default, suggest naming patterns like `{input}_calib`, `{input}_aligned`, or `{input}_registered` to automatically base the output filename on the active input element name.
- **Window Cleanup**: When an MDI image window is closed, clean up all references, active timers, and menu hooks immediately to prevent dangling pointer errors and menu list desynchronizations.
- **Middle-Elided Titles**: Image window titles and tabs should be elided in the middle (`...`) using `QFontMetrics::elidedText` with `Qt::ElideMiddle` so they do not take up excessive width, while permitting tab expansion when workspace views are collapsed/expanded.
- **Disabled UI Element Contrast**: Ensure that all custom-styled widgets (especially `QPushButton` in dialogs) have highly distinct disabled states. When overriding styles dynamically (e.g. coloring the cancel button or execution buttons), explicitly define the `:disabled` selector (e.g. background `#222222`, text `#555555`, border `#2a2a2a`) to ensure disabled controls are visually muted and distinct from enabled states in dark mode.

#### MDI Persistent Subwindow Lifecycle

BLastro distinguishes between two kinds of MDI subwindows:

- **Persistent** (`WA_DeleteOnClose = false`): Algorithm dialogs (all `AlgorithmDialog` subclasses), the Preprocessing Wizard, and the Process Console. These windows retain their state (form values, log history, open file lists) across open/close cycles. They must **never** be destroyed and reopened; the same widget instance is reused each time.
- **Transient** (`WA_DeleteOnClose = true`): Image viewer windows, Preferences, plugin sub-windows. These are created fresh each open and destroyed on close.

**The Hidden-Widget Trap**: When a `QMdiSubWindow` is closed, Qt sends a `QCloseEvent` to its inner child widget. If the child accepts the event (default `QWidget` and `QDialog` behaviour), Qt marks the child as hidden. Re-showing the subwindow wrapper via `sub->show()` restores the frame but leaves the child hidden, producing an empty gray box.

**The Fix**: When reopening or reshowing the persistent `QMdiSubWindow`, you must explicitly call `show()` on **both** the inner widget instance and the parent `QMdiSubWindow` wrapper. This ensures that the inner widget is shown again even if it accepted a close event previously and was hidden.

```cpp
// In MainWindow.cpp show slots:
m_pixelMathDlg->show(); // Show the inner widget first (un-hiding it)
m_pixelMathSub->show(); // Then show the subwindow wrapper
```

Persistent subwindows must be created with `WA_DeleteOnClose = false` (which `MainWindow` applies to all algorithm and wizard subwindows at startup). The `onOpen...` slots in `MainWindow` simply refresh contents, show the inner dialog, show the subwindow wrapper, raise it, and activate it.

#### Algorithm Dialog Layout Rules

All `AlgorithmDialog` subclasses follow the layout conventions established by the Preferences window, targeting a uniform resize behavior:

1. **No size locks**: Do not call `mainLayout->setSizeConstraint(QLayout::SetFixedSize)` or `setFixedSize(...)`. Dialogs must be freely resizable by the user.
2. **No hardcoded initial size**: Do not call `resize(w, h)` in the constructor. Let `sizeHint()` drive the initial size.
3. **Top-justify content, pin buttons to bottom**: Add `mainLayout->addStretch(1)` between the last content section and the button row. This ensures that form fields stay at the top and the button row stays pinned to the bottom as the dialog is resized taller.
4. **Horizontally expanding fields**: Wrap bare `QComboBox`, `QLineEdit`, or `QSlider` fields in a `QHBoxLayout` with `addWidget(field, 1)` stretch factor so they fill the available row width. Fields that should not expand (e.g. small spin boxes beside a slider) are exempt.
5. **Priority expand widget**: Dialogs that contain a large view widget (e.g. the histogram in `StretchingDialog`) should add that widget to the vertical layout with a stretch factor of `1` (`mainLayout->addWidget(histogram, 1)`), and add the controls section with stretch `0`. This way the view widget absorbs all extra vertical space on resize while the controls remain compact.

### 6. Log Infrastructure & PCL Progress Interception
- **Process Console Routing**: Do not route low-level debug warnings (e.g., Qt internal notifications, widget desync warnings) to the Process Console; let them output to the terminal (stdout). Only print high-level, actionable, formatted information (algorithm execution, load module confirmations, file actions) to the Rich HTML Process Console.
- **PCL Progress Interception**: PCL modules (like DeepSNR, StarXterminator, or BlurXterminator) output progress values as control sequences (such as `...%` percentages) to standard output. Intercept these console outputs, parse the percentage values, update the main window's status progress bar, and show the latest percent in the process console log.

### 7. Histogram & Local Histogram Equalization (CLAHE)
- **Interactive Histograms**: The histogram widgets rendered on image views must remain fully interactive (supporting mouse scroll-zooming and click-dragging) in normal (non-expanded) display mode.
- **Histogram Auto-Zoom Bounds**: When calculating auto-stretch limits, position the blackpoint at 5% of the histogram width and place the midpoint near the visual center to yield a balanced preview stretch.
- **Contrast Limiting (CLAHE)**: Local histogram equalization is highly sensitive to noise. To prevent posterization and pixel clipping:
  - Perform smooth bilinear interpolation between tile grids.
  - Limit the maximum contrast limit on the highest level (Hist H) to half of the theoretical maximum, and scale down the limit substantially for Hist M and Hist L.

### 8. Automated Preprocessing Pipeline & Staged Execution (PPW)
- **Staged Design**: The Preprocessing Pipeline is split into two primary execution phases (`stage = "calibrate_register"` and `stage = "align_stack"`) to accommodate user-guided image review and filtering.
- **Sensor Gain Matching**: Calibration frames (bias, darks, flats, flat-darks) and light frames must be grouped and matched by sensor `GAIN` in addition to dimensions, exposure time, binning, and filter.
- **Asynchronous Execution & GUI Safety**: All pipeline processing must run asynchronously on a background `QThread` (using `PreprocessingWorker`) so the main thread remains responsive. Calls to `MainWindow::setProcessingState` suspend other image MDI windows and menus, but the Preprocessing Wizard and the Process Console are exempted from this lock so they can receive cancellation clicks and display real-time log updates.
- **Adaptive/Exposed Registration**: Default star finding SNR thresholds (e.g. `snr_min = 4.0`) must be kept low/adaptive to support narrow-band frames (like Oiii). These parameters should be exposed in the Control UI so they can be modified by the user.
- **Dynamic Tab UX**: The Frame Selection review tab should be omitted entirely on wizard startup, and dynamically inserted and focused only when Stage 1 completes successfully.
- **Workspace Synchronization & Master Opening**: Upon successful completion of calibration or stacking, the master calibration frames (bias, dark, flat) or final stacked light master images can be opened automatically in the workspace MDI layout for inspection, depending on the **"Open Calibration Stacks"** and **"Open Light Masters"** checkboxes in the Control tab.
- **Keep Intermediate Files**: When enabled, the pipeline relocates generated intermediate files to structured subdirectories (`flat_calib`, `light_calib`, `light_align`) in the output directory. It dynamically updates the batch filepaths and memory loader functions (`ImageBatch::setFrameLoader`) so subsequent steps load frames from their correct relocated paths. Raw bias and dark temporary batches are unregistered without copying.

### 9. High-Performance Star Detection & Refinement (SOTA)
- **Spatial Occupancy Grid**: Replaced $O(N \cdot M)$ pairwise distance checking loops inside the candidate peak loops with an $O(1)$ flat occupancy grid (marking a circle of size `patchRadius` around fitted stars). This prevents the star finder from hanging on large astronomical frames with thousands of candidates.
- **Centroid Pre-filtering**: Instead of running heavy 2D Gaussian Nelder-Mead fits on all raw local maxima (many of which are noise or hot pixels), SOTA first applies a fast, sub-microsecond moment-based centroid fit to all candidates.
- **OpenMP Parallel Refinement**: The centroid fits are sorted descending by peak brightness, and Nelder-Mead 2D Gaussian refinement is run *only* on the top `maxStars` (e.g. 500) brightest valid stars. This is parallelized using OpenMP (`#pragma omp parallel for schedule(dynamic)`), rendering the entire fitting phase extremely fast (< 10 ms).
- **Hot Pixel & Cosmic Ray Rejection**: Rejects isolated spikes by checking that the average amplitude of the 4 direct neighbors is at least 15% of the peak amplitude. Discards stars that fail to converge to valid FWHM/eccentricity constraints during refinement.
- **Bilinear Interpolation Optimization**: Avoids full-image allocations and full-image bilinear interpolation loops. Computes block background/noise grid on a 1/16th sampled subset of pixels using linear-time `std::nth_element` (100x speedup), and performs bilinear interpolation on the fly only for candidate peaks.
- **Simplex Convergence Stability**: When running Nelder-Mead on small sub-pixel centroid offsets, initializing the simplex offset from the integer patch center `(0.0, 0.0)` rather than the sub-pixel centroid offset prevents degenerate/collapsed simplex shapes, guaranteeing convergence for all stars.

### 10. Thread-Safe Workspace & Suffix Auto-generation
- **Thread-Safe Workspace Registry**: Since algorithms are executed asynchronously inside background threads (`AlgorithmWorker`), they may register or unregister workspace elements concurrently with the main thread reading them. To prevent race conditions and segfaults, `WorkspaceRegistry` uses a `std::lock_guard<std::mutex>` to lock all read and write operations.
- **Dynamic Suffix Auto-generation**: Rerunning preprocessing runs or final stacking passes can cause name collisions for output stacked masters (e.g. `preprocessed_lights_Ha_stacked`). To prevent overwriting and allow easy comparison of runs, the Preprocessing Pipeline checks for naming collisions and appends an increasing unique suffix (e.g. `_1`, `_2`, etc.). The wizard's completion callback automatically resolves and displays the highest suffix/most recent run.

## Build Requirements

- C++17 compliant compiler
- CMake 3.16+
- Qt6 (Core, Gui, Widgets)
- OpenMP
- CCfits & cfitsio

Happy coding!
