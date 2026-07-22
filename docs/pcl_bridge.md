---
type: plugin
title: PixInsight PCL Bridge and Package Manager
description: Dynamic C++ ABI plugin loading, PCLImageMock wrappers, Python stub generation, ANSI log stream parsing, and third-party repository updates.
tags: [pcl, plugins, dynamic-loading, update-manager, stubs]
---

# PixInsight PCL Bridge Subsystem

This document covers `PCLBridge`, dynamic symbol stubbing, and third-party package management.

---

## 1. How PCL Bridge Works

`PCLBridge` enables loading compiled dynamic plugins (`.so`, `.dll`, `.dylib`) built for the PixInsight C++ Development Library (PCL) environment:

- **Dynamic Loading**: Uses `dlopen`/`LoadLibrary` to inspect and load PCL modules at runtime.
- **Mock Interfaces**: Wraps BLastro `ImageBuffer` objects inside C++ mock classes (`PCLImageMock`, `PCLViewMock`) that present PCL-compatible method signatures to loaded modules.
- **ABI Stub Overrides**: Python script `core/generate_stubs.py` parses PCL header declarations to generate C++ ABI stubs. `PCLBridge` dynamically overrides these stubs at runtime with BLastro engine functions.

---

## 2. Terminal ANSI Console Output Parsing

Console text emitted by PCL modules containing ANSI control codes (color and text formatting escape sequences) is intercepted by `PCLBridge`, parsed into standard HTML `<span>` tags with CSS inline colors, and rendered in the Process Console (`LogWindow`).

---

## 3. Third-Party Package Manager (`UpdateManagerDialog`)

- **XML Feed Manifests**: Queries PixInsight-compatible repository feeds (e.g. DeepSNR or RC-Astro) over HTTPS, downloading and parsing `updates.xri` XML manifests.
- **Platform Constraints**: Filters updates by target platform (`Linux/x64`).
- **Local Extraction**: Securely extracts installed packages into structured local directories under `plugins/`:
  - Shared library modules (`.so` / `.dll`) -> `plugins/bin/`
  - Auxiliary runtime libraries (TensorFlow) -> `plugins/lib/`
  - Neural network models (`.pb` files) -> `plugins/library/`
- **Library Preloading**: Scans `plugins/lib/` on startup to preload auxiliary libraries before loading dependent PCL modules.
