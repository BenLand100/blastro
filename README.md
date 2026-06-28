# BLastro 🌌

![BLastro Logo](ui/icons/bl_spacey_icon.png)

**BLastro** is an astronomical image processing toolkit. It provides the necessary features for processing both OSC (One Shot Color) and Mono images from raw data to a final result. 

Built with performance in mind, BLastro leverages C++17, Qt6, and OpenMP, utilizing algorithms based on the `oscdeeppy` library.

## Key Features

- **Full Processing Pipeline:** Calibration, Debayering, Background Extraction, Alignment, Registration, and Stacking.
- **Advanced Adjustments:** Stretching, PixelMath, and filtering.
- **PixInsight Plugin Compatibility:** A first-of-its-kind `PCLBridge` allows you to load and execute select plugins and modules designed for the PixInsight ecosystem directly inside BLastro. Output console messages from loaded modules are formatted with native terminal ANSI color sequence parsing, and logged under the module's own name for a cleaner developer log.
- **Workspace:** An MDI (Multiple Document Interface) workspace allows you to work with multiple images, cubes, and image batches simultaneously. During in-place background processing by native algorithms or PCL modules, updates to the UI viewport are automatically suspended to ensure thread safety.
- **Third-Party Repositories Manager:** Configure and query PixInsight-compatible repository feeds (like DeepSNR or SetiAstro) directly. The update manager parses `updates.xri` XML manifests, filters by target platform (Linux/x64), downloads packages securely, and automatically extracts/installs plugins locally.
- **FITS Support:** Comprehensive support for FITS files via CCfits/cfitsio, as well as common image formats.

## Getting Started

### Prerequisites

You will need the following dependencies installed on your system to build BLastro:

- CMake (3.16+)
- Qt6 (Core, Gui, Widgets)
- OpenMP
- CCfits & cfitsio

### Building from Source

```bash
mkdir build
cd build
cmake ..
make -j$(nproc)
```

### Running

To launch the GUI, simply run:
```bash
./blastro
```

You can also preload images directly from the command line:
```bash
./blastro --load-image /path/to/image1.fits --load-image /path/to/image2.fits
```

## Third-Party Plugins Setup (DeepSNR & RC-Astro)

BLastro features a built-in package installer and dynamic loader designed to work with PixInsight-compatible third-party update repositories.

### 1. Adding Repository Feeds
Open **Preferences** (wrench icon or menu) and navigate to the **Update Repositories** tab. You can add the following official feed URLs:
- **DeepSNR**: `https://pixinsight.deepsnrastro.com/` (Configured by default)
- **BlurXTerminator**: `https://www.rc-astro.com/BlurXTerminator/PixInsight/`
- **StarXTerminator**: `https://www.rc-astro.com/StarXTerminator/PixInsight/`
- **TensorFlow CPU dependency**: `https://www.rc-astro.com/TensorFlow/PixInsight/CPU/` (Required for running RC-Astro neural networks)

### 2. Installing Packages
1. Open **Algorithms** -> **Install from Repo...**
2. Click **Check** to fetch and parse the packages from the configured repositories.
3. Check the packages you want to install (e.g., `DeepSNR`, `BlurXTerminator`, `StarXTerminator`, and `TensorFlow CPU`).
4. Click **Download & Install**. BLastro will securely download the archives and extract them directly into the local `plugins/` structure:
   - Module shared libraries (`.so` / `.dll` / `.dylib`) -> `plugins/bin/`
   - Auxiliary core libraries (TensorFlow) -> `plugins/lib/`
   - AI neural network models (`.pb` files) -> `plugins/library/`

### 3. Startup & Execution
- **Dynamic Library Preloading**: Open **Preferences** (General Settings tab) and check **Preload all libraries in PCL lib folder on startup** so the neural network and auxiliary library dependencies are loaded correctly.
- **Autoloading**: Restart BLastro. It recursively scans the `plugins/bin/` directory on startup, loads all installed PCL modules, and registers them directly in the **Algorithms** menu.

---

## Contributing

We welcome contributions! If you're interested in adding new algorithms or improving the PCL Bridge, please check out our [Developing Guide](Developing.md) for an architectural overview and instructions.

## License

This software is licensed under the terms of the GNU General Public License version 3.0 (GPL v3). See the [LICENSE](LICENSE) file for the full license text.
