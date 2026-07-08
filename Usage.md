# BLastro User Guide & Navigation Controls 🌌

This guide documents the usage quirks, keyboard shortcuts, mouse controls, and workspace tools available in BLastro.

---

## 1. Image View Navigation & Mouse Controls

When viewing single images or frame batches, you can navigate the viewport using the following controls:

| Control | Action | Details |
| :--- | :--- | :--- |
| **Left Click / Middle Click + Drag** | **Pan Viewport** | Pans/scrolls the image inside the viewport. |
| **Scroll Wheel** | **Zoom In / Out** | Zooms the image centered directly under the mouse pointer. |
| **Shift + Left Click & Drag** | **Create Region Selection** | Draws a rectangular crop/selection area. Renders a stencil (dark overlay outside selected area). |
| **Shift + Hover / Drag Edges** | **Tweak / Resize Selection** | Hovering near selection edges or corners (with `Shift` held) displays resize cursors. Click and drag to adjust. |
| **Escape Key** | **Clear Selection** | Dismisses/clears any active region selection. |

---

## 2. Image Batch Controls

If your active window is an **Image Batch** (a list of raw or processed frames), the following keyboard shortcuts are active when the viewport is focused:

- **Right Arrow Key**: Advances to the next frame in the batch.
- **Left Arrow Key**: Returns to the previous frame in the batch.
- **Spacebar**: Toggles the check/selection status of the current frame (used during alignment, registration, or stacking filtering).

---

## 3. Image Menu & Operations

All simple image operations apply to the **active single image MDI window**:

- **Undo (`Ctrl + Z`)**: Reverts the last image mutation (mutations include Flip, Mirror, Rotate, and Crop).
- **Redo (`Ctrl + Y`)**: Reapplies the last undone mutation.
- **Flip Vertical**: Flips the image top-to-bottom.
- **Mirror Horizontal**: Flips the image left-to-right.
- **Rotate 90° CW**: Rotates the image 90 degrees clockwise (adjusts MDI window dimensions).
- **Rotate 90° CCW**: Rotates the image 90 degrees counter-clockwise (adjusts MDI window dimensions).
- **Rotate 180°**: Rotates the image 180 degrees.
- **Crop (`Ctrl + K`)**: Crops the image to the active selection region (only enabled when a selection box is drawn on the image).

> [!NOTE]
> **Undo/Redo Depth**: The undo stack keeps up to **10 history states** in memory to avoid running out of RAM. These edits apply only to single images, not to whole batches.

---

## 4. Histogram Stretching & GHS Controls

When using the histogram panel (manual stretching or Generalized Hyperbolic Stretch - GHS), the widget has several interactive features:

- **Adjusting Parameters**: Left-click and drag any of the vertical slider lines:
  - **Manual Stretch**: Blackpoint, Midpoint, and Whitepoint.
  - **GHS Stretch**: Symmetry Point (SP), Shadow Protection, and Highlight Protection.
- **Scrolling the Histogram**: Right-click and drag anywhere on the histogram to pan/scroll horizontally.
- **Zooming the Histogram**: Scroll the mouse wheel up or down over the histogram to zoom in or out centered on your mouse cursor.

---

## 5. Background Subtraction & Control Points

The **Background Extraction** algorithm removes background gradients (such as sky glow or light pollution) from raw or calibrated images.

- **Phase 1: Placing Control Points**: When the **Algorithms -> Background Extraction** dialog is open, use **Ctrl + Left Click** on the image to place control points in regions that contain only background sky (avoid placing them on stars, nebulosity, or galaxies). Use **Ctrl + Right Click** near an existing control point to remove it. Standard left-click and drag can still be used to pan/scroll the viewport.
- **Phase 2: Execution**: The algorithm fits a 2D polynomial surface to these points and subtracts it.
- **Verification / Visualization**: Since the image viewer automatically stretches previews to center the background at a specific grey level, the background before and after extraction might visually look similar. To verify the extraction:
  1. Inspect the pixel values at different areas (e.g. corners) before and after subtraction to see if the values have been neutralized.
  2. Notice the improvement in target contrast (nebulae/galaxies) when a fresh auto-stretch is applied, as the dynamic range is no longer dominated by the gradient.

---

## 6. Preprocessing Wizard (PPW)

The **Preprocessing Wizard** calibrates, registers, aligns, and stacks light frames with their corresponding calibration master frames.

- **Staged Workflow**: Execution is divided into Stage 1 (Calibration & Registration) and Stage 2 (Alignment & Stacking), allowing you to review registered frames and adjust filtering constraints between stages.
- **Output Settings (Control Tab)**:
  - **Open Calibration Stacks**: When checked (default), automatically opens the stacked master calibration frames (bias, dark, flat) in the workspace for inspection after Stage 1 completes.
  - **Open Light Masters**: When checked (default), automatically opens the final stacked light master image(s) after Stage 2 completes.
  - **Keep Intermediate Files**: When checked, generated intermediate files (calibrated flats, calibrated/debayered lights, and aligned lights) are moved from temporary storage into structured subfolders (`flat_calib`, `light_calib`, `light_align`) inside your output directory. Raw bias/darks are stacked directly and temporary frames unregistered to conserve space.
