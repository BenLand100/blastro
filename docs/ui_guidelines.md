---
type: ui
title: User Interface and Viewport Guidelines
description: MDI subwindow lifecycle rules, live LUT and manual preview recipes, viewport zoom persistence, and AlgorithmDialog base class patterns.
tags: [ui, qt, mdi, preview, widgets]
---

# User Interface & Viewport Guidelines

This document specifies UI patterns, window lifecycles, dialog sizing rules, and viewport preview recipes in `blastro_ui`.

---

## 1. MDI Subwindow Lifecycle Rules

BLastro distinguishes strictly between persistent and transient subwindows:

### Persistent Subwindows (`WA_DeleteOnClose = false`)
- **Applies to**: `AlgorithmDialog` subclasses, `PreprocessingWizardDialog`, `LogWindow`.
- **Behavior**: Created once at startup and reused across open/close cycles.
- **The Hidden-Widget Fix**: When a `QMdiSubWindow` is closed, Qt sends a `QCloseEvent` to its inner child widget, marking it hidden. To reopen the subwindow without showing an empty gray box, call `show()` on **both** the inner child widget and the subwindow wrapper:
  ```cpp
  m_pixelMathDlg->show(); // Un-hide inner widget
  m_pixelMathSub->show(); // Show subwindow wrapper
  ```

### Transient Subwindows (`WA_DeleteOnClose = true`)
- **Applies to**: `WorkspaceImageWindow`, `PreferencesWindow`.
- **Behavior**: Instantiated fresh on each open request and destroyed on close.

---

## 2. UI Preview Recipes & Performance Strategy

### Recipe 1: Live Previews (Fast Algorithms < 100ms)
- **Use Case**: Histogram Transform, Generalized Hyperbolic Stretch (GHS), Curves.
- **Pattern**:
  - Connect slider parameter changes to a single-shot debounce timer (`QTimer* m_previewTimer`).
  - Precompute 1D LUTs (65536 entries for RGB/L and Saturation) and pass them to the viewport via `win->imageView()->setLivePreview(liveLut, liveSatLut)`.
  - Clear live preview via `win->restoreOriginalImage()` when unchecked or dialog closes.
  - **Signal Loop Protection**: Set an `m_ignoreImageUpdates = true` flag during preview clear to prevent recursive signal cascades with `imageUpdated()`.

### Recipe 2: Manual Update Previews (Computationally Intense Algorithms)
- **Use Case**: Background Extraction, Denoising, Star Finding.
- **Pattern**:
  - Provide a "Show Preview" checkbox and an "Update Preview" button.
  - Parameter changes do not auto-trigger execution. Clicking "Update Preview" executes the calculation once.
  - Pass the calculated preview buffer to `win->setPreviewImage(previewResult)` for display-only rendering without mutating workspace pointers.

### Global Preview Lifecycle Rule
All `AlgorithmDialog` subclasses implementing previews must override:
- `bool hasActivePreview() const`
- `void clearPreview()`
- `QMdiSubWindow* getTargetWindow() const`

`MainWindow` monitors window activations (`subWindowActivated`) and automatically calls `clearPreview()` if a dialog's target window loses focus.

---

## 3. Base Class Helpers in `AlgorithmDialog`

Never duplicate Qt widget tree traversals in dialog subclasses. Use the inherited helpers:
- `WorkspaceArea* findWorkspaceArea() const`: Walks the parent chain to locate the workspace area.
- `WorkspaceImageWindow* getActiveImageWindow() const`: Returns the currently active image window, automatically filtering out windows displaying active previews.

---

## 4. Dialog Layout and Resizing Rules

All algorithm dialogs follow these sizing rules:
1. **No size locks**: Do not call `setFixedSize()` or `QLayout::SetFixedSize`.
2. **Top-justify form controls**: Add `mainLayout->addStretch(1)` between form controls and the bottom button row.
3. **Horizontally expanding controls**: Wrap `QComboBox`, `QLineEdit`, and `QSlider` in layouts with stretch factor `1`.
4. **Priority view widgets**: View widgets (e.g. histogram displays) absorb vertical resize space using a stretch factor of `1`.
