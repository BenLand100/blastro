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

## 4. Stretching Controls (Histogram, GHS & Curves)

When using the Stretching Transformation panel, you have three mutually exclusive modes: Histogram Transform (HT), Generalized Hyperbolic Stretch (GHS), and Curves.

- **Histogram & GHS**:
  - **Adjusting Parameters**: Left-click and drag any of the vertical slider lines (e.g. Midpoint, Symmetry Point).
  - **Scrolling**: Right-click and drag anywhere on the histogram to pan/scroll horizontally.
  - **Zooming**: Scroll the mouse wheel up or down over the histogram to zoom in or out centered on your mouse cursor.
- **Curves**:
  - **Add/Move Points**: Left-click on the curve or background to add a control point, and drag to adjust its position.
  - **Remove Points**: Right-click directly on a control point to instantly remove it.
  - **Smooth Interpolation**: The curves use Monotone Cubic Spline (Fritsch-Carlson) interpolation to prevent unnatural overshoot and inversion.
  - **Channels**: Selecting "L" edits Luminance with color preservation. "S" targets Saturation (only). "K", "R", "G", "B" edit the individual or linked channel intensities.

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

The **Preprocessing Wizard** calibrates, normalizes the background, detects stars, registers, aligns, and stacks light frames with their corresponding calibration master frames.

- **Staged Workflow**: Execution is divided into Stage 1 (Calibration, Star Finding & Registration) and Stage 2 (Background Normalization, Alignment & Stacking), allowing you to review registered frames and adjust filtering constraints between stages.
- **Background Normalization**: When enabled, applies a modeled background subtraction to all target frames mapped from the reference frame's sample points before alignment.
- **Stacking Normalization**: Supports additive and multiplicative scaling to match the brightness and contrast of target frames against the reference frame on the fly during integration.
- **Output Settings (Control Tab)**:
  - **Open Calibration Stacks**: When checked (default), automatically opens the stacked master calibration frames (bias, dark, flat) in the workspace for inspection after Stage 1 completes.
  - **Open Light Masters**: When checked (default), automatically opens the final stacked light master image(s) after Stage 2 completes.
  - **Keep Intermediate Files**: When checked, generated intermediate files (calibrated flats, calibrated/debayered lights, and aligned lights) are moved from temporary storage into structured subfolders (`flat_calib`, `light_calib`, `light_align`) inside your output directory. Raw bias/darks are stacked directly and temporary frames unregistered to conserve space.

> [!TIP]
> **OSC CFA Drizzle**: When processing OSC images, you can achieve superior sharpness by bypassing standard debayer interpolation. In the Preprocessing Wizard, set the **Debayer Method** to `sparse` and the **Alignment Interpolation** to `drizzle`. This uses a mathematically rigorous drizzle technique directly on the native Bayer pattern data, and outputs a 6-plane FITS master file containing both the RGB data and weight maps used during stacking rejection.

---

## 7. Project Management & Session Persistence

BLastro provides tools to save your workspace progress and automatically remember your tool window configuration.

### Project Save / Load
- **Saving a Project**: Select **File -> Save Project** or **Save Project As**. A project is stored as a directory containing a `project.json` metadata file (describing MDI window layout, open tool windows, and image parameters) and any workspace files.
- **Copying References**: When saving a project, you can opt to copy all referenced images/batches from their external locations directly into the project directory to make the project folder self-contained and portable.
- **Intermediate Outputs**: Wizard outputs and temporary directories are automatically created inside `{CWD}/process`. Opening or saving a project updates the application's working directory (`CWD`), redirecting intermediate files to the project's folder.

### Session Auto-Restore & Startup Options
By default, closing BLastro saves your tool window layout, dialog parameter settings, and main window geometry to `~/.config/blastro/last_session.json` to restore it on the next launch.

You can customize startup behavior via CLI flags:
- **`--no-restore`**: Bypasses loading the last session for a clean startup workspace.
- **`--project <path>`**: Automatically opens the specified project directory at startup.
- **`--session <path>`**: Recovers layout/dialog configurations from a specific custom session JSON file.
