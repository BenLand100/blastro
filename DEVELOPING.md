# BLastro Developer & Architecture Guide

Welcome to the BLastro developer documentation. This project uses the **Open Knowledge Format (OKF)** standard to package developer documentation and architectural rules for both human contributors and AI assistants.

---

## AI Tool & Agent Orientation Guide

> **For AI Assistants (Antigravity, Claude Code, Cursor, GitHub Copilot, etc.)**:
> Use this section to quickly orient yourself with BLastro's repository organization, module boundaries, architectural invariants, and build commands before reading or writing code.

### Project Layout & Module Responsibilities

| Module Directory | Target Name | Primary Responsibility | Dependencies Allowed |
| :--- | :--- | :--- | :--- |
| `core/` | `blastro_core` | Data structures (`ImageBuffer`, `GrayscaleImage`, `RGBImage`, `ImageBatch`), `WorkspaceRegistry`, `PCLBridge`, `MathUtils`, Logger, Preferences | Qt6::Core, Qt6::Gui, Qt6::Widgets, system libs |
| `io/` | `blastro_io` | FITS and standard image readers/writers (`FitsIO`, `QtImageIO`, `ImageReader`, `ImageWriter`) | `blastro_core`, CCfits, cfitsio, Qt6::Gui |
| `algorithms/` | `blastro_algorithms` | Astronomical algorithms (Calibration, Debayer, Alignment, Stacking, Stretching, StarFinder, Platesolve, PreprocessingPipeline, PixelMath) | `blastro_core`, `blastro_io`, OpenMP |
| `ui/` | `blastro_ui` | Qt MDI Workspace, Viewports (`ImageView`), Dialogs (`AlgorithmDialog` subclasses), Preprocessing Wizard, Log Window | `blastro_core`, `blastro_algorithms`, `blastro_io`, Qt6::Widgets, Qt6::Gui, Qt6::Core |
| `app/` | `blastro` (executable) | `main.cpp` entry point, command-line option parsing (`--project`, `--load-image`), application bootstrap | All libraries above |
| `tests/` | `test_algorithms` | C++ unit and algorithm integration test suite | `blastro_core`, `blastro_algorithms`, `blastro_ui`, Qt6 |

### Critical Architectural Invariants (DO NOT VIOLATE)

1. **Strict Dependency Hierarchy**:
   - `core` MUST NEVER depend on `algorithms`, `io`, or `ui`.
   - `algorithms` and `io` depend on `core`, but MUST NEVER depend on `ui`.
   - `ui` depends on `core`, `io`, and `algorithms`.
2. **Include Path Convention**:
   - Include headers starting with the module folder name: `#include "core/GrayscaleImage.h"`, `#include "io/ImageReader.h"`, `#include "algorithms/StarFinder.h"`.
   - Build system exports project root as target include directory.
3. **High-Performance Pixel Loops**:
   - NEVER call bounds-checked `buffer->pixel(x, y)` in nested full-image processing loops.
   - ALWAYS get raw float data pointer `float* data = buffer->data();` and iterate contiguously with OpenMP: `#pragma omp parallel for`.
4. **Workspace Registry Mutex & Thread Safety**:
   - `WorkspaceRegistry` methods use internal mutex locks (`std::lock_guard<std::mutex>`).
   - Callback notifications (`elementRegistered`, `elementUnregistered`) MUST be invoked **outside** registry locks to prevent recursive deadlocks.
   - GUI state updates triggered by registry events MUST be marshaled to the main thread via `QMetaObject::invokeMethod` using queued connections (`Qt::QueuedConnection`).
5. **MDI Subwindow Lifecycle**:
   - **Persistent Dialogs** (e.g. `AlgorithmDialog` subclasses, `PreprocessingWizardDialog`): Created at startup with `WA_DeleteOnClose = false`. Reusing instances across open/close cycles requires calling `show()` on **both** inner widget and subwindow wrapper (`m_dialog->show(); m_subWindow->show();`) to prevent the hidden-child gray-box bug.
   - **Transient Windows** (e.g. `WorkspaceImageWindow`): Created with `WA_DeleteOnClose = true` and destroyed on close.
6. **Documentation & UI Synchronization (Core Project Drive)**:
   - Keeping the Open Knowledge Format (OKF) documentation in `docs/` accurate and up to date is a **core project drive**, equal in priority to keeping configuration UIs (manual algorithm dialogs and Preprocessing Wizard controls) in sync with underlying algorithm changes. Whenever algorithm parameters, execution stages, or architecture models are extended, the corresponding OKF documentation and UIs MUST be updated simultaneously.

---

## Build System & Environment

BLastro uses CMake 3.16+ with target-based library dependencies and Qt6 standard setup.

### Building & Testing

```bash
# Configure and build
mkdir -p build && cd build
cmake ..
make -j$(nproc)

# Run the test suite
./test_algorithms

# Launch the application
./blastro
```

### Compile Commands Export
`set(CMAKE_EXPORT_COMPILE_COMMANDS ON)` is configured in the root CMakeLists.txt. When building, CMake outputs `build/compile_commands.json` for indexers and AI language tools.

---

## Open Knowledge Format (OKF) Bundle Index

Detailed subsystem documentation is structured according to the Open Knowledge Format (OKF) specification within the [`docs/`](docs/) directory.

| Knowledge File | Type | Description |
| :--- | :--- | :--- |
| [`docs/architecture.md`](docs/architecture.md) | `architecture` | Core data structures (`ImageBuffer`), memory alignment, OpenMP concurrency rules, and `WorkspaceRegistry` thread safety. |
| [`docs/ui_guidelines.md`](docs/ui_guidelines.md) | `ui` | MDI subwindow lifecycles, Live LUT & Manual preview recipes, dialog sizing rules, and base class helpers. |
| [`docs/algorithms.md`](docs/algorithms.md) | `algorithm` | High-performance star finder, Stacking & Drizzle `NaN` handling rules, affine alignment, Astrometry WCS metadata, and PixelMath rules. |
| [`docs/preprocessing_pipeline.md`](docs/preprocessing_pipeline.md) | `pipeline` | Automated Preprocessing Pipeline (PPW) staged execution, sensor gain matching, mutual stack alignment, and suffix auto-generation. |
| [`docs/pcl_bridge.md`](docs/pcl_bridge.md) | `plugin` | PixInsight PCL C++ ABI bridge, dynamic stubs, ANSI terminal output parsing, and third-party repository package manager. |

---

## Summary Checklist for Code Contributions

When submitting changes or extending BLastro:
- [ ] Header includes use canonical module prefixes (e.g. `#include "core/..."`).
- [ ] Heavy loops use raw `float*` pointers parallelized with OpenMP `#pragma omp parallel for`.
- [ ] No `core/` files depend on `ui/` or `algorithms/`.
- [ ] Async background thread calls do not touch Qt GUI widgets directly.
- [ ] New algorithm parameters are exposed with sane defaults in both standard dialogs and Preprocessing Wizard controls.
- [ ] OKF documentation files in `docs/` are updated to reflect any new parameters, architectural changes, or algorithm behavior updates.
- [ ] `make -j$(nproc)` compiles with 0 warnings, and `./test_algorithms` passes.
