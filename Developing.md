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

## Build Requirements

- C++17 compliant compiler
- CMake 3.16+
- Qt6 (Core, Gui, Widgets)
- OpenMP
- CCfits & cfitsio

Happy coding!
