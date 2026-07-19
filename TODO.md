# BLastro Roadmap & TODO

## Staged & Upcoming Tasks

* **Color Calibration**: Implement manual color calibration (e.g. custom channel coefficients, background neutralization) and star-based calibration (requiring platesolved WCS data to perform stellar color indices lookup).
* **Shutdown Stability**: Trace down and resolve the occasional segmentation fault occurring on application shutdown.
* **Startup Visual Polish**: Smooth out startup window geometry transitions (preventing the visual layout "jump" when restoring last-session configuration).

## Completed Milestones

* [x] **PixelMath Optimization**: Reworked pixel loop using OpenMP and raw float pointer arithmetic to execute under 1 second (previously 15–30s).
* [x] **Curves Tool**: Implemented interactive Spline-based Curves graph stretching to complement Histogram Transform and GHS.
* [x] **Astrometry & Platesolving**: Integrated ASTAP and Solve-Field external solving and `.wcs` FITS keyword preservation.
* [x] **Drizzle Warping and Stacking**: Implemented mathematically rigorous Drizzle forward-mapping alignment alongside backward-mapping (Bilinear, Bicubic, Lanczos) interpolators and NaN-safe Stacking.
* [x] **UI Consistency**: Standardized alignment and drizzle mode/drop size options between the individual dialogs and the Preprocessing Wizard control panels.
