---
type: algorithm
title: Astronomical Processing Algorithms and Mathematics
description: Star detection grid optimization, stacking NaN propagation, alignment warping, astrometry WCS preservation, and PixelMath expression parsing.
tags: [algorithms, star-finder, stacking, alignment, astrometry, pixelmath]
---

# Astronomical Algorithms & Mathematics

This document details mathematical formulations, outlier rejection, star detection optimizations, and metadata preservation in `blastro_algorithms`.

---

## 1. High-Performance Star Detection & Refinement

To process large astronomical frames with thousands of stars quickly, `StarFinder` implements an adaptive multi-phase approach:

1. **Spatial Occupancy Grid**: Replaces pairwise distance checks with an $O(1)$ flat occupancy grid (marking a circular radius around fitted stars), preventing execution hangs on dense star fields.
2. **Centroid Pre-filtering**: Sub-microsecond moment-based centroid fitting pre-filters candidate local maxima before executing non-linear fits.
3. **OpenMP Parallel Nelder-Mead Fits**: 2D Gaussian Nelder-Mead refinement is executed only on the top brightest candidates (e.g. top 500) using dynamic OpenMP scheduling (`#pragma omp parallel for schedule(dynamic)`).
4. **Bilinear Background Sampling Optimization**: Block background/noise grids are computed on a 1/16th sampled subset of pixels using `std::nth_element`, avoiding full-image interpolation overhead.

---

## 2. Image Stacking, Drizzle, and NaN Handling

`NaN` (Not a Number) values represent masked or missing pixel data (dithered frame boundaries, un-debayered sparse CFA points, mosaic overlaps).

### Handling Rules
- **NaN is Masked Data**: Treat `NaN` as missing observations, not outliers to reject.
- **Dynamic Normalization**: Pixel statistics (mean, median, sigma) are calculated strictly across valid non-NaN values. Summed intensity is divided by the count of valid contributing frames at that coordinate (`sum / valid_count`), not the total frame count.
- **All-NaN Pixel Propagation**: If every frame contains `NaN` at a pixel coordinate, the output pixel must be set to `NaN`.
- **Sorting Safety**: Always filter out `NaN` values before passing arrays to `std::sort` for median or quantile evaluation.

---

## 3. High-Performance Image Alignment & Interpolation

- **Affine Transformation Solving**: RANSAC constellation matching fits a 6-element affine matrix mapping target coordinates to reference coordinates.
- **Matrix Composition**: Sub-pixel relative alignments multiply target transformation matrices by the inverse of reference transformation matrices ($T_{\text{target}} \times T_{\text{ref}}^{-1}$).
- **Advanced Interpolators**: Supports Bilinear, Bicubic, Lanczos-3, Lanczos-4, and mathematically rigorous Drizzle (mapping source pixel droplets to target grids tracking intensity and coverage weight maps).

---

## 4. Astrometry & WCS Coordinates Preservation

- **`ImageMetadata`**: Stores WCS solution flags, RA/Dec centers, pixel scales, rotations, exposure parameters, and raw FITS header keywords.
- **1:1 Transformations**: Operations such as Debayering, Alignment, Calibration, and Background Subtraction deep-copy `ImageMetadata` from input to output.
- **Integration Operations**: Stacking copies metadata from the reference frame, updates `EXPTIME` with the total aggregated exposure time, and writes the `STACKCNT` keyword.
- **Platesolving**: `PlatesolveAlgorithm` executes external solvers (`solve-field` or ASTAP) asynchronously, parsing resulting `.wcs` files via `CCfits` to populate WCS header keys.

---

## 5. PixelMath Expression Evaluation

PixelMath evaluates arbitrary math expressions on workspace images using ExprTk:
- **Symbol Precedence**: Active image names take top precedence over coordinates (`x`, `y`, `w`, `h`) or constants.
- **Dimension Protection**: All referenced images must be verified to have identical dimensions (width and height) prior to execution.
- **Suffix Namespaces**: Single-character channel suffixes use underscores (`_R`, `_G`, `_B`, `_K`).
