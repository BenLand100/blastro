---
type: pipeline
title: Automated Preprocessing Pipeline
description: Staged execution rules, sensor gain matching, batch normalization, mutual stack alignment, and suffix auto-generation.
tags: [pipeline, preprocessing, batch, calibration, registration]
---

# Automated Preprocessing Pipeline (PPW)

This document covers the execution phases, frame matching, and batch alignment mechanisms in `PreprocessingPipeline`.

---

## 1. Staged Execution Architecture

The pipeline is split into two primary asynchronous execution phases to support user frame review:
1. **Stage 1 (`calibrate_register`)**:
   - Groups calibration frames (bias, darks, flats) and light frames.
   - Matches calibration frames to light frames by sensor `GAIN`, image dimensions, exposure time, binning, and filter.
   - Executes calibration and star detection.
2. **Stage 2 (`align_stack`)**:
   - Executes mutual alignment, batch background normalization, and final master stacking.

---

## 2. Optimized Mutual Stack Alignment

During batch alignment, frames across all batches are aligned to a single global reference batch (the batch with the highest SNR).

- **Matrix Composition**: For each frame, a combined transformation matrix is computed as:
  $$T_{\text{final}} = T_{\text{mutual}} \times T_{\text{local\_inv}} \times T_{\text{original}}$$
  where $T_{\text{local\_inv}}$ is the inverse transform of the batch's local best frame, and $T_{\text{mutual}}$ maps the local best frame to the global reference batch.
- **Single-Pass Warping**: Applies local frame registration and mutual batch-to-batch alignment in a single combined warping pass.

---

## 3. Batch Background Normalization

Background extraction is executed after registration is finalized. By using the combined affine transformations, sample grids defined on the reference frame are re-projected onto target frames using:
$$T_{\text{ref} \to \text{target}} = T_{\text{target}}^{-1} \times T_{\text{ref}}$$

---

## 4. Suffix Auto-generation for Output Naming

To prevent overwriting previous runs when re-executing pipelines or stacking passes, `PreprocessingPipeline` checks for naming collisions in the `WorkspaceRegistry` and automatically appends an increasing numeric suffix (e.g. `_1`, `_2`).
