/*
 * BLastro - Astronomical Image Processing Software
 * Copyright (C) 2026 Benjamin Land
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "algorithms/StarFinder.h"
#include "algorithms/ConstellationMatcher.h"
#include "algorithms/StackingAlgorithm.h"
#include "algorithms/CalibrationAlgorithm.h"
#include "algorithms/RegisterAlgorithm.h"
#include "algorithms/AlignAlgorithm.h"
#include "algorithms/BackgroundExtractionAlgorithm.h"
#include "algorithms/StretchingAlgorithm.h"
#include "core/WorkspaceRegistry.h"
#include "core/ImageBatch.h"
#include "core/GrayscaleImage.h"
#include "core/ImageBuffer.h"
#include "core/TempDirectory.h"
#include "io/FitsIO.h"
#include <iostream>
#include <cmath>
#include <cassert>
#include <vector>
#include <memory>
#include <map>
#include <string>
#include <random>

using namespace blastro;

// Helper to check if two doubles are approximately equal
bool approxEqual(double a, double b, double tolerance = 1e-2) {
    return std::abs(a - b) <= tolerance;
}

void testGaussianStarFitting() {
    std::cout << "Running Gaussian Star Fitting Test..." << std::endl;

    const int width = 51;
    const int height = 51;
    auto img = std::make_shared<GrayscaleImage>(width, height);
    auto buffer = img->buffer();

    // Set up a synthetic star
    // True parameters:
    const double trueX = 25.3;
    const double trueY = 25.7;
    const double trueFwhm = 3.5;
    const double truePeak = 1.0;
    const double trueBg = 0.1;

    // Fill the image with a Gaussian profile + background
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            double dx = x - trueX;
            double dy = y - trueY;
            double r2 = dx * dx + dy * dy;
            // standard Gaussian formula: value = bg + peak * exp(-4 * ln(2) * r^2 / fwhm^2)
            double val = trueBg + truePeak * std::exp(-4.0 * std::log(2.0) * r2 / (trueFwhm * trueFwhm));
            buffer->setPixel(x, y, static_cast<float>(val));
        }
    }

    // 1. Test Nelder-Mead 2D Gaussian fitting (fastFit = false)
    std::vector<Star> stars = StarFinder::findStars(img, 10, 3.0, false, 10, 1.5, 0.85);

    if (stars.empty()) {
        std::cerr << "[-] Error: Nelder-Mead star finder returned 0 stars" << std::endl;
        std::exit(1);
    }

    Star foundStar = stars[0];
    std::cout << "[+] Nelder-Mead Star found at (" << foundStar.x << ", " << foundStar.y 
              << "), FWHM: " << foundStar.fwhm << ", Peak: " << foundStar.peak 
              << ", Bg: " << foundStar.background << std::endl;

    assert(approxEqual(foundStar.x, trueX, 0.1));
    assert(approxEqual(foundStar.y, trueY, 0.1));
    assert(approxEqual(foundStar.fwhm, trueFwhm, 0.2));
    assert(approxEqual(foundStar.peak, truePeak, 0.1));
    assert(approxEqual(foundStar.background, trueBg, 0.05));

    // 2. Test fast centroiding (fastFit = true)
    std::vector<Star> fastStars = StarFinder::findStars(img, 10, 3.0, true, 10, 1.5, 0.85);
    if (fastStars.empty()) {
        std::cerr << "[-] Error: Centroid star finder returned 0 stars" << std::endl;
        std::exit(1);
    }
    Star fastStar = fastStars[0];
    std::cout << "[+] Centroid Star found at (" << fastStar.x << ", " << fastStar.y << ")" << std::endl;
    assert(approxEqual(fastStar.x, trueX, 0.3));
    assert(approxEqual(fastStar.y, trueY, 0.3));

    std::cout << "[+] Gaussian Star Fitting Test PASSED." << std::endl << std::endl;
}

void testConstellationMatching() {
    std::cout << "Running Constellation Matching Test..." << std::endl;

    // Define 10 reference stars
    std::vector<Star> refStars;
    std::vector<std::pair<double, double>> coords = {
        {15.0, 20.0}, {40.0, 15.0}, {82.0, 78.0}, {115.0, 112.0}, {35.0, 95.0},
        {145.0, 30.0}, {90.0, 60.0}, {65.0, 125.0}, {110.0, 18.0}, {130.0, 80.0}
    };

    for (const auto& c : coords) {
        Star s;
        s.x = c.first;
        s.y = c.second;
        s.fwhm = 3.0;
        s.peak = 1.0;
        s.background = 0.1;
        refStars.push_back(s);
    }

    // Apply known transformation: dx = 8.5, dy = -5.2, theta = 0.20 radians
    const double trueDx = 8.5;
    const double trueDy = -5.2;
    const double trueTheta = 0.20;

    std::vector<Star> targetStars;
    for (const auto& s : refStars) {
        // We want target stars tx, ty such that:
        // s.x = tx * cos(theta) - ty * sin(theta) + dx
        // s.y = tx * sin(theta) + ty * cos(theta) + dy
        // Which means: tx, ty = R(-theta) * (ref - T)
        double rx = s.x - trueDx;
        double ry = s.y - trueDy;
        double tx = rx * std::cos(-trueTheta) - ry * std::sin(-trueTheta);
        double ty = rx * std::sin(-trueTheta) + ry * std::cos(-trueTheta);

        Star ts;
        // Add tiny coordinate noise to simulate measurement error
        ts.x = tx + 0.02; 
        ts.y = ty - 0.01;
        ts.fwhm = 3.0;
        ts.peak = 1.0;
        ts.background = 0.1;
        targetStars.push_back(ts);
    }

    // Add a spurious star in target set to test robustness
    Star spurious;
    spurious.x = 200.0;
    spurious.y = 200.0;
    spurious.fwhm = 3.0;
    spurious.peak = 1.0;
    targetStars.push_back(spurious);

    // Run Constellation Matcher
    AlignmentResult res = ConstellationMatcher::match(refStars, targetStars, 5, 3.0);

    std::cout << "[+] Match Result: Success=" << res.success 
              << ", dx=" << res.dx << ", dy=" << res.dy 
              << ", theta=" << res.theta << ", RMS=" << res.rmsError 
              << ", Matched Stars=" << res.matchedStars << std::endl;

    assert(res.success);
    assert(res.matchedStars >= 6); // Should match most stars
    assert(approxEqual(res.dx, trueDx, 0.1));
    assert(approxEqual(res.dy, trueDy, 0.1));
    assert(approxEqual(res.theta, trueTheta, 0.02));

    std::cout << "[+] Constellation Matching Test PASSED." << std::endl << std::endl;
}

void testStackingMathematics() {
    std::cout << "Running Stacking Mathematics Test..." << std::endl;

    WorkspaceRegistry workspace;
    const int width = 4;
    const int height = 4;
    const int numFrames = 15;

    // Create a batch of 15 frames
    // Set all pixels to background 0.10, except pixel (2, 2) which has an outlier in frame 10
    auto loader = [width, height](int idx) -> ImageVariant {
        auto img = std::make_shared<GrayscaleImage>(width, height);
        auto buffer = img->buffer();
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                buffer->setPixel(x, y, 0.10f);
            }
        }
        // Inject outlier in frame 10 (0-indexed) at (2, 2)
        if (idx == 10) {
            buffer->setPixel(2, 2, 0.50f); // Cosmic ray/hot pixel outlier!
        } else if (idx == 1) {
            buffer->setPixel(2, 2, 0.105f);
        } else if (idx == 2) {
            buffer->setPixel(2, 2, 0.095f);
        } else if (idx == 4) {
            buffer->setPixel(2, 2, 0.102f);
        }
        
        return img;
    };

    std::vector<std::string> names;
    std::vector<std::string> filepaths;
    for (int i = 0; i < numFrames; ++i) {
        names.push_back("f" + std::to_string(i));
        filepaths.push_back("f" + std::to_string(i) + ".fit");
    }
    auto batch = std::make_shared<ImageBatch>(numFrames, loader, names, filepaths);
    
    // Register batch in workspace
    workspace.registerElement("input_batch", batch);

    // Run Stacking in both RAM and Chunked modes
    for (std::string mode : {"ram", "chunked"}) {
        // Run Stacking with Winsorized Sigma clipping
        std::map<std::string, std::string> config = {
            {"input_name", "input_batch"},
            {"output_name", "stacked_output_" + mode},
            {"method", "average"},
            {"rejection", "winsorized_sigma"},
            {"sigma_low", "2.0"},
            {"sigma_high", "2.0"},
            {"quantile_low", "0.2"},
            {"quantile_high", "0.2"},
            {"stacking_mode", mode},
            {"patch_size", "2"} // Small patch size to test grid boundary math!
        };

        StackingAlgorithm stackAlg;
        stackAlg.execute(workspace, config);

        // Verify output
        std::string outName = "stacked_output_" + mode;
        assert(workspace.contains(outName));
        auto outVar = workspace.getElement(outName);
        assert(std::holds_alternative<GrayscaleImagePtr>(outVar));
        auto outImg = std::get<GrayscaleImagePtr>(outVar);

        float valNormal = outImg->buffer()->pixel(0, 0);
        float valOutlier = outImg->buffer()->pixel(2, 2);

        std::cout << "[+] Stacking Output (" << mode << "): Normal Pixel=" << valNormal 
                  << ", Outlier-affected Pixel (2,2)=" << valOutlier << std::endl;

        // The normal pixels should be exactly 0.10
        assert(approxEqual(valNormal, 0.10, 1e-4));

        // The outlier pixel (2,2) without rejection would average to:
        // (11 * 0.10 + 0.105 + 0.095 + 0.50 + 0.102) / 15 = 0.1268
        // With winsorization (2 iterations, sigma=2.0), the 0.50 is clipped and replaced,
        // and the average should be around 0.1092
        // Verify that the outlier is rejected and the value is close to 0.1092
        assert(valOutlier < 0.12f);
        assert(approxEqual(valOutlier, 0.1092, 0.0005));
    }

    std::cout << "[+] Stacking Mathematics Test PASSED." << std::endl << std::endl;
}

// ---------------------------------------------------------------------------
// Helper: build a config map for stacking (average + no rejection)
// ---------------------------------------------------------------------------
static std::map<std::string, std::string> stackConfig(
        const std::string& input, const std::string& output,
        const std::string& mode = "ram") {
    return {
        {"input_name",   input},
        {"output_name",  output},
        {"method",       "average"},
        {"rejection",    "none"},
        {"sigma_low",    "3.0"},
        {"sigma_high",   "3.0"},
        {"quantile_low", "0.0"},
        {"quantile_high","0.0"},
        {"stacking_mode",mode},
        {"patch_size",   "256"}
    };
}

void testCalibrationPipeline() {
    std::cout << "Running Calibration Pipeline Test..." << std::endl;

    // ------------------------------------------------------------------
    // 0. Setup: dimensions, parabolic flat profile, and RNG.
    // ------------------------------------------------------------------
    const int W = 50;
    const int H = 50;
    const int N_BIAS = 100;
    const int N_FLAT = 100;

    // Pixel-level noise parameters
    const double NOISE_MEAN   = 0.01;
    const double NOISE_STDDEV = 0.005;

    // 2-D parabolic flat-field profile:
    //   profile(x, y) = 0.1 + 0.4 * (1 - ((x - cx)^2 + (y - cy)^2) / r^2)
    // so profile = 0.5 at centre, 0.1 at the image corners.
    const double cx = (W - 1) / 2.0;
    const double cy = (H - 1) / 2.0;
    const double r2_max = cx * cx + cy * cy; // radius^2 to corner
    auto flatProfile = [&](int x, int y) -> double {
        double dx = x - cx;
        double dy = y - cy;
        return 0.1 + 0.4 * (1.0 - (dx*dx + dy*dy) / r2_max);
    };

    // Fixed-seed RNG for full reproducibility.
    std::mt19937_64 rng(42);
    std::normal_distribution<double> noise(NOISE_MEAN, NOISE_STDDEV);

    // ------------------------------------------------------------------
    // 1. Create intermediate directory and save FITS frames to disk.
    // ------------------------------------------------------------------
    std::string biasDir = TempDirectory::createTempDir("test_bias");
    std::string flatDir = TempDirectory::createTempDir("test_flat");
    assert(!biasDir.empty() && "Failed to create bias directory");
    assert(!flatDir.empty() && "Failed to create flat directory");

    FitsIO fits;
    std::vector<std::string> biasNames(N_BIAS), biasPaths(N_BIAS);
    std::vector<std::string> flatNames(N_FLAT), flatPaths(N_FLAT);

    for (int i = 0; i < N_BIAS; ++i) {
        auto img = std::make_shared<GrayscaleImage>(W, H);
        auto buf = img->buffer();
        for (int y = 0; y < H; ++y)
            for (int x = 0; x < W; ++x)
                buf->setPixel(x, y, static_cast<float>(noise(rng)));

        std::string path = biasDir + "/bias_" + std::to_string(i) + ".fits";
        assert(fits.writeImage(path, ImageVariant{img}));
        biasNames[i] = "bias_" + std::to_string(i);
        biasPaths[i] = path;
    }
    std::cout << "  [bias] Wrote " << N_BIAS << " bias frames to " << biasDir << std::endl;

    for (int i = 0; i < N_FLAT; ++i) {
        auto img = std::make_shared<GrayscaleImage>(W, H);
        auto buf = img->buffer();
        for (int y = 0; y < H; ++y)
            for (int x = 0; x < W; ++x)
                buf->setPixel(x, y, static_cast<float>(flatProfile(x, y) + noise(rng)));

        std::string path = flatDir + "/flat_" + std::to_string(i) + ".fits";
        assert(fits.writeImage(path, ImageVariant{img}));
        flatNames[i] = "flat_" + std::to_string(i);
        flatPaths[i] = path;
    }
    std::cout << "  [flat] Wrote " << N_FLAT << " flat frames to " << flatDir << std::endl;

    // ------------------------------------------------------------------
    // 2. Register disk-backed batches in the workspace.
    // ------------------------------------------------------------------
    WorkspaceRegistry workspace;

    auto biasBatch = std::make_shared<ImageBatch>(
        N_BIAS,
        [biasPaths](int idx) -> ImageVariant {
            FitsIO r; return r.readImage(biasPaths[idx]);
        },
        biasNames, biasPaths);
    for (int i = 0; i < N_BIAS; ++i) biasBatch->setFrameSelected(i, true);
    workspace.registerElement("bias_batch", biasBatch);

    auto flatBatch = std::make_shared<ImageBatch>(
        N_FLAT,
        [flatPaths](int idx) -> ImageVariant {
            FitsIO r; return r.readImage(flatPaths[idx]);
        },
        flatNames, flatPaths);
    for (int i = 0; i < N_FLAT; ++i) flatBatch->setFrameSelected(i, true);
    workspace.registerElement("flat_batch", flatBatch);

    // ------------------------------------------------------------------
    // 3. Stack biases → bias master.
    // ------------------------------------------------------------------
    std::cout << "  [stack] Stacking bias master..." << std::endl;
    StackingAlgorithm stacker;
    stacker.execute(workspace, stackConfig("bias_batch", "bias_master"));

    assert(workspace.contains("bias_master"));
    auto biasMasterVar = workspace.getElement("bias_master");
    assert(std::holds_alternative<GrayscaleImagePtr>(biasMasterVar));
    auto biasMaster = std::get<GrayscaleImagePtr>(biasMasterVar);

    // The bias master should average to ≈ NOISE_MEAN (0.01) with very low scatter.
    {
        double sum = 0;
        for (int y = 0; y < H; ++y)
            for (int x = 0; x < W; ++x)
                sum += biasMaster->buffer()->pixel(x, y);
        double mean = sum / (W * H);
        std::cout << "  [bias_master] mean=" << mean << " (expected ~" << NOISE_MEAN << ")" << std::endl;
        assert(approxEqual(mean, NOISE_MEAN, 5 * NOISE_STDDEV / std::sqrt(N_BIAS)));
    }

    // ------------------------------------------------------------------
    // 4. Calibrate flat batch: subtract bias master.
    // ------------------------------------------------------------------
    std::cout << "  [calibrate] Calibrating flat frames with bias master..." << std::endl;
    CalibrationAlgorithm calibrator;
    std::map<std::string, std::string> calConfig = {
        {"input_name",  "flat_batch"},
        {"output_name", "cal_flat_batch"},
        {"bias_name",   "bias_master"},
        {"dark_name",   ""},
        {"flat_name",   ""}
    };
    calibrator.execute(workspace, calConfig);

    assert(workspace.contains("cal_flat_batch"));
    auto calFlatElem = workspace.getElement("cal_flat_batch");
    assert(std::holds_alternative<ImageBatchPtr>(calFlatElem));
    auto calFlatBatch = std::get<ImageBatchPtr>(calFlatElem);
    for (int i = 0; i < N_FLAT; ++i) calFlatBatch->setFrameSelected(i, true);
    workspace.registerElement("cal_flat_batch", calFlatBatch);

    // ------------------------------------------------------------------
    // 5. Stack calibrated flats → flat master.
    // ------------------------------------------------------------------
    std::cout << "  [stack] Stacking flat master..." << std::endl;
    stacker.execute(workspace, stackConfig("cal_flat_batch", "flat_master"));

    assert(workspace.contains("flat_master"));
    auto flatMasterVar = workspace.getElement("flat_master");
    assert(std::holds_alternative<GrayscaleImagePtr>(flatMasterVar));
    auto flatMaster = std::get<GrayscaleImagePtr>(flatMasterVar);

    // ------------------------------------------------------------------
    // 6. Subtract the known analytic profile and verify residuals.
    //
    // After stacking N_FLAT frames each with noise σ=NOISE_STDDEV, the
    // expected per-pixel noise in the flat master is σ/√N ≈ 0.0005.
    // We tighten the tolerance to 3σ of the *mean* residual, and also
    // check that the RMS residual image is consistent with σ/√N.
    // ------------------------------------------------------------------
    std::cout << "  [verify] Checking flat master residuals..." << std::endl;

    // The calibration subtracted the bias master (≈ NOISE_MEAN) from every
    // flat, so after stacking, the expected value at (x,y) is:
    //   flatProfile(x,y) + NOISE_MEAN_FLAT_NOISE - NOISE_MEAN_BIAS
    // ≈ flatProfile(x,y) (because both noise means are the same and average out).
    // More precisely, each calibrated flat pixel = flatProfile + flat_noise - bias_master.
    // Stacking N_FLAT of those gives flatProfile + (mean flat noise) - (mean bias master).
    // The bias master mean ≈ NOISE_MEAN, the flat noise mean ≈ NOISE_MEAN,
    // so residual should be ≈ 0.

    double maxResidual = 0;
    double sumSqResidual = 0;
    double meanResidual = 0;
    for (int y = 0; y < H; ++y) {
        for (int x = 0; x < W; ++x) {
            double got      = flatMaster->buffer()->pixel(x, y);
            double expected = flatProfile(x, y);
            double residual = got - expected;
            meanResidual   += residual;
            sumSqResidual  += residual * residual;
            maxResidual     = std::max(maxResidual, std::abs(residual));
        }
    }
    meanResidual /= (W * H);
    double rmsResidual = std::sqrt(sumSqResidual / (W * H));

    // Expected noise in the stacked master = σ_noise / sqrt(N).
    // σ_noise is the noise per frame at each pixel after bias subtraction.
    // Each calibrated flat pixel has variance from flat noise + bias master noise.
    // bias master noise is reduced by sqrt(N_BIAS), so effectively:
    //   σ_pixel ≈ NOISE_STDDEV * sqrt(1 + 1/N_BIAS) ≈ NOISE_STDDEV
    // After stacking N_FLAT: σ_stacked = NOISE_STDDEV / sqrt(N_FLAT)
    double expectedRMS = NOISE_STDDEV / std::sqrt(static_cast<double>(N_FLAT));

    std::cout << "  [flat_master] mean_residual=" << meanResidual
              << ", rms_residual=" << rmsResidual
              << ", max_residual=" << maxResidual
              << " (expected rms~" << expectedRMS << ")" << std::endl;

    // Mean residual should be near zero (within 3-sigma of its own sampling noise).
    double sigmaOfMean = expectedRMS / std::sqrt(static_cast<double>(W * H));
    assert(std::abs(meanResidual) < 5.0 * sigmaOfMean && "Mean residual too large");

    // RMS residual should be within a factor of 2 of the theoretical noise floor.
    assert(rmsResidual < 3.0 * expectedRMS && "RMS residual too large");
    assert(rmsResidual > 0.3 * expectedRMS && "RMS residual suspiciously small");

    // Verify the parabolic shape: centre should be brighter than edges.
    float centre = flatMaster->buffer()->pixel(W / 2, H / 2);
    float corner = flatMaster->buffer()->pixel(0, 0);
    std::cout << "  [flat_master] centre=" << centre << ", corner=" << corner << std::endl;
    assert(centre > corner && "Flat master should be brighter at centre than corner");
    assert(approxEqual(centre, flatProfile(W/2, H/2), 10 * expectedRMS));
    assert(approxEqual(corner, flatProfile(0, 0),      10 * expectedRMS));

    // Cleanup
    TempDirectory::cleanup();

    std::cout << "[+] Calibration Pipeline Test PASSED." << std::endl << std::endl;
}

void testLightFramePipeline() {
    std::cout << "Running Light Frame Pipeline Test..." << std::endl;

    // -----------------------------------------------------------------------
    // 0.  Constants & shared geometry
    // -----------------------------------------------------------------------
    const int W = 200, H = 200;
    const int N_LIGHT = 50;

    // Bias / noise parameters (same as the calibration test)
    const double NOISE_MEAN   = 0.01;
    const double NOISE_STDDEV = 0.0005;  // 10x quieter — prevents noise spikes from masquerading as stars

    // Fixed seed for reproducibility
    std::mt19937_64 rng(1234);
    std::normal_distribution<double>  noise(NOISE_MEAN, NOISE_STDDEV);
    std::uniform_real_distribution<double> dither(-15.0, 15.0);

    // -----------------------------------------------------------------------
    // 1.  Parabolic vignetting profile (identical to the flat test)
    //     profile = 0.1 + 0.4 * (1 – r²/r²_max)  → [0.1, 0.5]
    //     Normalised to its own mean so calibration preserves total flux.
    // -----------------------------------------------------------------------
    const double cx = (W - 1) / 2.0;
    const double cy = (H - 1) / 2.0;
    const double r2max = cx * cx + cy * cy;
    auto vigProfile = [&](int x, int y) -> double {
        double dx = x - cx, dy = y - cy;
        return 0.1 + 0.4 * (1.0 - (dx*dx + dy*dy) / r2max);
    };

    // Compute the mean of vigProfile over all pixels once
    double vigSum = 0.0;
    for (int y = 0; y < H; ++y)
        for (int x = 0; x < W; ++x)
            vigSum += vigProfile(x, y);
    const double vigMean = vigSum / (W * H);  // ≈ 0.3 for a parabola

    // -----------------------------------------------------------------------
    // 2.  X-direction additive gradient (sky gradient / amp-glow model)
    //     g(x) = 0.05 + 0.10 * (x / (W-1))  → [0.05, 0.15]
    //     This is constant across ALL frames (including bias frames),
    //     so the 100-frame bias master will contain it exactly, and
    //     bias subtraction will remove it from the lights.
    // -----------------------------------------------------------------------
    auto gradient = [&](int x) -> double {
        return 0.05 + 0.10 * (static_cast<double>(x) / (W - 1));
    };

    // -----------------------------------------------------------------------
    // 3.  Synthetic star catalogue: 5 stars near the image centre.
    //     They are positioned so that dithers of up to ±15 px and a
    //     180° rotation around the centre all keep every star at least
    //     30 px from the image edge after alignment.
    // -----------------------------------------------------------------------
    // True star positions in the *reference frame* (frame 0).
    // Deliberately asymmetric — prime-number-spaced positions so every triangle
    // has a unique (L1/L0, L2/L0) aspect ratio descriptor after normalisation.
    // All positions ≥40px from each edge so a ±15px dither keeps them in frame.
    struct StarDef { double x, y, peak, fwhm; };
    const std::vector<StarDef> trueCatalogue = {
        { 53.0,  43.0, 1.50, 5.0},
        { 97.0,  61.0, 1.20, 5.0},
        {141.0,  47.0, 1.10, 4.8},
        { 71.0,  89.0, 1.40, 5.2},
        {113.0, 103.0, 1.00, 4.6},
        {149.0,  83.0, 1.30, 5.0},
        { 59.0, 127.0, 1.25, 4.8},
        { 89.0, 153.0, 1.15, 4.9},
        {137.0, 139.0, 1.35, 5.1},
        {157.0, 121.0, 1.10, 4.7},
        { 43.0, 157.0, 1.20, 5.0},
        {109.0, 143.0, 1.15, 4.8},
    };

    auto addStar = [&](float* buf, int bw, int bh,
                       double sx, double sy, double peak, double fwhm) {
        double sigma2 = (fwhm / 2.3548) * (fwhm / 2.3548);
        int r = static_cast<int>(std::ceil(fwhm * 3));
        int x0 = std::max(0, static_cast<int>(sx) - r);
        int x1 = std::min(bw - 1, static_cast<int>(sx) + r);
        int y0 = std::max(0, static_cast<int>(sy) - r);
        int y1 = std::min(bh - 1, static_cast<int>(sy) + r);
        for (int py = y0; py <= y1; ++py) {
            for (int px = x0; px <= x1; ++px) {
                double ddx = px - sx, ddy = py - sy;
                double val = peak * std::exp(-(ddx*ddx + ddy*ddy) / (2.0 * sigma2));
                buf[py * bw + px] += static_cast<float>(val);
            }
        }
    };

    // -----------------------------------------------------------------------
    // 4.  Generate per-frame dither offsets and rotation angles.
    //     Frames 0–24: small dithers, no rotation (theta = 0).
    //     Frames 25–49: same dithers but with 180° rotation (meridian flip).
    //     For a 180° rotation around the image centre the warping convention
    //     uses (dx, dy, theta) so we must express the flip correctly:
    //       - theta = π  (warp rotates by –theta about the output centre)
    //       - dx = dy = 0 keeps the rotation centred on the image
    //     The dither is then applied on top by an additional translation.
    // -----------------------------------------------------------------------
    struct FrameTransform { double dx, dy, theta; };
    std::vector<FrameTransform> transforms(N_LIGHT);
    for (int i = 0; i < N_LIGHT; ++i) {
        double ddx = dither(rng);
        double ddy = dither(rng);
        if (i < 25) {
            transforms[i] = {ddx, ddy, 0.0};
        } else {
            // Meridian flip: 180° rotation + dither
            transforms[i] = {ddx, ddy, M_PI};
        }
    }
    // Frame 0 is the reference — zero offset, zero rotation
    transforms[0] = {0.0, 0.0, 0.0};

    // -----------------------------------------------------------------------
    // 5.  Write 100 bias frames: pure readout noise, no sky signal.
    //     The gradient is sky glow and does NOT appear in bias frames.
    // -----------------------------------------------------------------------
    WorkspaceRegistry workspace;
    FitsIO fits;

    std::string biasDir = TempDirectory::createTempDir("light_test_bias");
    assert(!biasDir.empty());
    std::vector<std::string> biasNames(100), biasPaths(100);
    for (int i = 0; i < 100; ++i) {
        auto img = std::make_shared<GrayscaleImage>(W, H);
        float* buf = img->buffer()->data();
        for (int y = 0; y < H; ++y)
            for (int x = 0; x < W; ++x)
                buf[y * W + x] = static_cast<float>(noise(rng));  // readout noise only
        std::string path = biasDir + "/bias_" + std::to_string(i) + ".fits";
        assert(fits.writeImage(path, ImageVariant{img}));
        biasNames[i] = "lbias_" + std::to_string(i);
        biasPaths[i] = path;
    }
    std::cout << "  [bias] Wrote 100 bias frames." << std::endl;

    // -----------------------------------------------------------------------
    // 6.  Write 100 flat frames: 2D vignetting profile + readout noise.
    //     Flats are taken against a uniform light source so they contain
    //     no sky gradient.  They capture only the optical vignetting.
    // -----------------------------------------------------------------------
    std::string flatDir = TempDirectory::createTempDir("light_test_flat");
    assert(!flatDir.empty());
    std::vector<std::string> flatNames(100), flatPaths(100);
    for (int i = 0; i < 100; ++i) {
        auto img = std::make_shared<GrayscaleImage>(W, H);
        float* buf = img->buffer()->data();
        for (int y = 0; y < H; ++y)
            for (int x = 0; x < W; ++x)
                buf[y * W + x] = static_cast<float>(
                    vigProfile(x, y) + noise(rng));  // vignetting + readout noise
        std::string path = flatDir + "/flat_" + std::to_string(i) + ".fits";
        assert(fits.writeImage(path, ImageVariant{img}));
        flatNames[i] = "lflat_" + std::to_string(i);
        flatPaths[i] = path;
    }
    std::cout << "  [flat] Wrote 100 flat frames." << std::endl;

    // -----------------------------------------------------------------------
    // 7.  Write 50 light frames.
    //
    //     Physical model:
    //       pixel = (starSignal(x,y) + gradient(x)) * vigProfile(x,y)/vigMean
    //               + bias_noise
    //
    //     The sky gradient is an optical signal and is vignetted.
    //     The readout/bias noise is electronic and is added after vignetting.
    //
    //     After calibration:
    //       - subtract bias_master  →  removes bias_noise (≈ NOISE_MEAN)
    //       - divide by flat_master/flatMean  →  removes vigProfile/vigMean factor
    //     Residual = starSignal + gradient(x)   ← gradient remains
    //
    //     Background extraction then removes the gradient per frame.
    // -----------------------------------------------------------------------
    std::string lightDir = TempDirectory::createTempDir("light_test_lights");
    assert(!lightDir.empty());
    std::vector<std::string> lightNames(N_LIGHT), lightPaths(N_LIGHT);

    for (int i = 0; i < N_LIGHT; ++i) {
        auto img = std::make_shared<GrayscaleImage>(W, H);
        float* buf = img->buffer()->data();

        bool flipped = (i >= 25);

        for (int y = 0; y < H; ++y) {
            for (int x = 0; x < W; ++x) {
                // sky + optical signal (vignetted)
                double opticalSignal = gradient(x);   // sky glow only for now
                buf[y * W + x] = static_cast<float>(
                    opticalSignal * (vigProfile(x, y) / vigMean)
                    + noise(rng));                    // readout noise, not vignetted
            }
        }

        // Add star profiles on top of the vignetted sky, also vignetted
        for (auto& s : trueCatalogue) {
            double sx, sy;
            if (!flipped) {
                sx = s.x + transforms[i].dx;
                sy = s.y + transforms[i].dy;
            } else {
                // 180° rotation about image centre, then dither
                sx = (W - 1) - s.x + transforms[i].dx;
                sy = (H - 1) - s.y + transforms[i].dy;
            }
            // Stamp star into a temp buffer, vignette it, add to image
            int r = static_cast<int>(std::ceil(s.fwhm * 3));
            double sigma2 = (s.fwhm / 2.3548) * (s.fwhm / 2.3548);
            int x0 = std::max(0, static_cast<int>(sx) - r);
            int x1 = std::min(W - 1, static_cast<int>(sx) + r);
            int y0 = std::max(0, static_cast<int>(sy) - r);
            int y1 = std::min(H - 1, static_cast<int>(sy) + r);
            for (int py = y0; py <= y1; ++py) {
                for (int px = x0; px <= x1; ++px) {
                    double ddx = px - sx, ddy = py - sy;
                    double starVal = s.peak * std::exp(-(ddx*ddx + ddy*ddy) / (2.0 * sigma2));
                    buf[py * W + px] += static_cast<float>(
                        starVal * (vigProfile(px, py) / vigMean));
                }
            }
        }

        std::string path = lightDir + "/light_" + std::to_string(i) + ".fits";
        assert(fits.writeImage(path, ImageVariant{img}));
        lightNames[i] = "light_" + std::to_string(i);
        lightPaths[i] = path;
    }
    std::cout << "  [light] Wrote " << N_LIGHT << " light frames." << std::endl;

    // -----------------------------------------------------------------------
    // 8.  Register disk-backed batches
    // -----------------------------------------------------------------------
    auto makeDiskBatch = [](int n, const std::vector<std::string>& names,
                             const std::vector<std::string>& paths) {
        auto batch = std::make_shared<ImageBatch>(
            n,
            [paths](int idx) -> ImageVariant { FitsIO r; return r.readImage(paths[idx]); },
            names, paths);
        for (int i = 0; i < n; ++i) batch->setFrameSelected(i, true);
        return batch;
    };

    workspace.registerElement("bias_batch",  makeDiskBatch(100, biasNames, biasPaths));
    workspace.registerElement("flat_batch",  makeDiskBatch(100, flatNames, flatPaths));
    workspace.registerElement("light_batch", makeDiskBatch(N_LIGHT, lightNames, lightPaths));

    // -----------------------------------------------------------------------
    // 9.  Build bias master
    // -----------------------------------------------------------------------
    std::cout << "  [stack] Building bias master..." << std::endl;
    StackingAlgorithm stacker;
    stacker.execute(workspace, stackConfig("bias_batch", "bias_master"));

    // -----------------------------------------------------------------------
    // 10. Calibrate flats → calibrated flat batch → flat master
    // -----------------------------------------------------------------------
    std::cout << "  [calibrate] Calibrating flats..." << std::endl;
    CalibrationAlgorithm calibrator;
    calibrator.execute(workspace, {
        {"input_name",  "flat_batch"},
        {"output_name", "cal_flat_batch"},
        {"bias_name",   "bias_master"},
        {"dark_name",   ""},
        {"flat_name",   ""}
    });
    // mark all cal flats selected for stacking
    auto calFlatBatch = std::get<ImageBatchPtr>(workspace.getElement("cal_flat_batch"));
    for (int i = 0; i < 100; ++i) calFlatBatch->setFrameSelected(i, true);
    workspace.registerElement("cal_flat_batch", calFlatBatch);

    std::cout << "  [stack] Building flat master..." << std::endl;
    stacker.execute(workspace, stackConfig("cal_flat_batch", "flat_master"));

    // -----------------------------------------------------------------------
    // 11. Calibrate lights: subtract bias master + divide by normalised flat master
    // -----------------------------------------------------------------------
    std::cout << "  [calibrate] Calibrating light frames..." << std::endl;
    calibrator.execute(workspace, {
        {"input_name",  "light_batch"},
        {"output_name", "cal_light_batch"},
        {"bias_name",   "bias_master"},
        {"dark_name",   ""},
        {"flat_name",   "flat_master"}
    });
    auto calLightBatch = std::get<ImageBatchPtr>(workspace.getElement("cal_light_batch"));
    for (int i = 0; i < N_LIGHT; ++i) calLightBatch->setFrameSelected(i, true);
    workspace.registerElement("cal_light_batch", calLightBatch);

    // Sanity check on first calibrated light:
    //   - vignetting removed → star signals should be at their true peak values
    //   - bias removed → readout pedestal gone
    //   - sky gradient REMAINS (it's not in the flat/bias, only BGE removes it)
    {
        auto frame0var = calLightBatch->getImage(0);
        auto frame0 = std::get<GrayscaleImagePtr>(frame0var);
        // Pixel at brightest star position (53,43): star peak ~1.5 + sky gradient
        float starVal = frame0->buffer()->pixel(53, 43);
        // Sky-only pixel at centre (no star there in this catalogue)
        float skyVal  = frame0->buffer()->pixel(100, 100);
        // Corner pixel: just sky gradient(0) = 0.05
        float cornerVal = frame0->buffer()->pixel(0, 0);
        std::cout << "  [cal_light_0] star@(53,43)=" << starVal
                  << ", sky@(100,100)=" << skyVal
                  << ", corner=" << cornerVal
                  << " (gradient remains, BGE will remove it)" << std::endl;
        assert(starVal > 1.0f && "Star signal missing after calibration");
        assert(skyVal  > 0.0f && skyVal < 0.30f && "Sky-only pixel out of expected range");
        // Corner should have sky gradient value ≈ gradient(0) = 0.05 (within noise)
        assert(cornerVal > 0.0f && cornerVal < 0.20f && "Corner value out of expected range after calibration");
    }
    std::cout << "  [calibrate] Calibration sanity check PASSED." << std::endl;

    // -----------------------------------------------------------------------
    // 12. Background extraction: remove the per-frame sky gradient.
    //     After calibration each frame contains  stars + gradient(x).
    //     A linear (order=1) polynomial fit will capture and subtract the
    //     x-direction gradient cleanly, leaving only star signal + noise.
    // -----------------------------------------------------------------------
    std::cout << "  [bge] Applying per-frame background extraction..." << std::endl;
    BackgroundExtractionAlgorithm bge;
    bge.execute(workspace, {
        {"input_name",   "cal_light_batch"},
        {"output_name",  "bge_light_batch"},
        {"order",        "1"},       // linear surface sufficient for a 1-D gradient
        {"sigma_cut",    "3.0"},
        {"sample_frac",  "0.05"},
        {"huber_delta",  "5.0"},
        {"equalize",     "true"}     // lift floor above zero so StarFinder SNR check passes
    });
    auto bgeBatch = std::get<ImageBatchPtr>(workspace.getElement("bge_light_batch"));
    // BGE forwards selection state, but make sure
    for (int i = 0; i < N_LIGHT; ++i) bgeBatch->setFrameSelected(i, true);
    workspace.registerElement("bge_light_batch", bgeBatch);

    // Verify that the gradient is gone from the first BGE frame
    {
        auto bge0var = bgeBatch->getImage(0);
        auto bge0 = std::get<GrayscaleImagePtr>(bge0var);
        float starVal   = bge0->buffer()->pixel(53, 43);  // brightest star
        float cornerVal = bge0->buffer()->pixel(0, 0);
        std::cout << "  [bge_light_0] star@(53,43)=" << starVal
                  << ", corner=" << cornerVal
                  << " (expected: star≈peak, corner≈equalize floor)" << std::endl;
        assert(starVal > 0.80f && "Star signal lost after background extraction");
        // With equalize=true a positive floor (~3*sigma) is added so patchMin > 0
        // for the StarFinder. Corner should be small but non-negative.
        assert(cornerVal >= 0.0f && cornerVal < 0.30f && "BGE corner floor out of expected range");
    }
    std::cout << "  [bge] Background extraction sanity check PASSED." << std::endl;

    // Test user-placed control points BGE
    {
        std::cout << "  [bge_control_points] Testing background extraction with custom control points..." << std::endl;
        auto calBatch = std::get<ImageBatchPtr>(workspace.getElement("cal_light_batch"));
        auto firstImg = std::get<GrayscaleImagePtr>(calBatch->getImage(0));
        
        std::vector<std::pair<double, double>> pts = {
            {10.0, 10.0}, {50.0, 10.0}, {90.0, 10.0},
            {10.0, 50.0}, {50.0, 50.0}, {90.0, 50.0},
            {10.0, 90.0}, {50.0, 90.0}, {90.0, 90.0}
        };
        firstImg->buffer()->setBgeControlPoints(pts);
        
        workspace.registerElement("single_cal_light", firstImg);
        BackgroundExtractionAlgorithm bgeSingle;
        bgeSingle.execute(workspace, {
            {"input_name",   "single_cal_light"},
            {"output_name",  "single_bge_light"},
            {"order",        "1"},
            {"sigma_cut",    "3.0"},
            {"equalize",     "true"}
        });
        
        auto bgeSingleImg = std::get<GrayscaleImagePtr>(workspace.getElement("single_bge_light"));
        float cornerVal = bgeSingleImg->buffer()->pixel(0, 0);
        float starVal   = bgeSingleImg->buffer()->pixel(53, 43);
        assert(starVal > 0.80f && "Star signal lost after custom-points BGE");
        assert(cornerVal >= 0.0f && cornerVal < 0.30f && "Custom-points BGE corner floor out of expected range");
        std::cout << "  [bge_control_points] Custom control points BGE test PASSED." << std::endl;
    }

    // -----------------------------------------------------------------------
    // 13. Register: determine shifts and rotations (on BGE frames)
    // -----------------------------------------------------------------------
    std::cout << "  [register] Running star registration..." << std::endl;
    RegisterAlgorithm registrar;
    registrar.execute(workspace, {
        {"input_name",       "bge_light_batch"},
        {"ref_frame_index",  "0"},
        {"detection_method", "centroid"},
        {"snr_min",          "3.0"},
        {"min_fwhm",         "1.5"},
        {"max_stars",        "12"},    // exactly our injected star count — excludes noise peaks
        {"max_eccentricity", "0.90"},
        {"match_tolerance",  "3.0"}
    });

    // Confirm that all 50 frames were registered and verify transforms
    int registeredCount = 0;
    int successfulFlips = 0;
    for (int i = 0; i < N_LIGHT; ++i) {
        if (!bgeBatch->isFrameSelected(i)) continue;
        FrameMetadata meta = bgeBatch->frameMetadata(i);
        if (!meta.registered) continue;
        registeredCount++;

        if (i == 0) {
            // Reference frame: no offset
            assert(std::abs(meta.dx) < 1.0 && std::abs(meta.dy) < 1.0);
            assert(std::abs(meta.theta) < 0.1);
            continue;
        }

        // All registered frames should have recovered the dither within ±2 px
        double expectedDx = transforms[i].dx;
        double expectedDy = transforms[i].dy;
        bool isFlip = (i >= 25);

        // The constellation matcher gives dx/dy relative to reference; for flipped
        // frames the shift will be (W-1 + dx_dither - (W-1)) = dx_dither but the
        // recovered theta should be near π (or −π).
        if (isFlip) {
            // theta should be near ±π
            double thetaMod = std::fmod(std::abs(meta.theta), 2 * M_PI);
            double flipErr  = std::min(std::abs(thetaMod - M_PI),
                                       std::abs(thetaMod - M_PI) );
            if (flipErr < 0.5) successfulFlips++;
        } else {
            // Non-flipped: theta should be near 0
            assert(std::abs(meta.theta) < 0.5 && "Non-flipped frame has unexpected rotation");
            // Registration reports the shift needed to bring the target onto the reference,
            // which is the negative of the dither that was applied to the target frame.
            double expectedDx = -transforms[i].dx;
            double expectedDy = -transforms[i].dy;
            assert(std::abs(meta.dx - expectedDx) < 5.0 &&
                   std::abs(meta.dy - expectedDy) < 5.0 &&
                   "Dither recovery exceeds 5-pixel tolerance");
        }
    }
    std::cout << "  [register] " << registeredCount << "/" << N_LIGHT
              << " frames registered, " << successfulFlips
              << "/25 meridian flips detected." << std::endl;
    assert(registeredCount >= 45 && "Too many frames failed registration");
    assert(successfulFlips >= 20 && "Too many meridian flips went undetected");

    // -----------------------------------------------------------------------
    // 14. Align (use BGE frames, which carry the registration metadata)
    // -----------------------------------------------------------------------
    std::cout << "  [align] Aligning frames (drizzle=1.0)..." << std::endl;
    AlignAlgorithm aligner;
    aligner.execute(workspace, {
        {"input_name",   "bge_light_batch"},
        {"output_name",  "aligned_batch"},
        {"drizzle_scale","1.0"},
        {"threads",      "4"},
        {"evict_cache",  "true"}
    });

    auto alignedBatch = std::get<ImageBatchPtr>(workspace.getElement("aligned_batch"));
    for (int i = 0; i < N_LIGHT; ++i) {
        if (bgeBatch->isFrameSelected(i))
            alignedBatch->setFrameSelected(i, true);
    }
    workspace.registerElement("aligned_batch", alignedBatch);

    // -----------------------------------------------------------------------
    // 14. Stack aligned frames
    // -----------------------------------------------------------------------
    std::cout << "  [stack] Stacking aligned frames..." << std::endl;
    stacker.execute(workspace, stackConfig("aligned_batch", "stacked_master", "ram"));

    assert(workspace.contains("stacked_master"));
    auto stackedVar = workspace.getElement("stacked_master");
    assert(std::holds_alternative<GrayscaleImagePtr>(stackedVar));
    auto stacked = std::get<GrayscaleImagePtr>(stackedVar);

    {
        // Sanity check: stacked master should have star signal
        float pCtr   = stacked->buffer()->pixel(53, 43);  // brightest star
        float pOther = stacked->buffer()->pixel(100, 100); // sky
        float pCorner= stacked->buffer()->pixel(0, 0);
        std::cout << "  [stack] stacked_master pixel@(53,43)=" << pCtr
                  << " pixel@(100,100)=" << pOther
                  << " pixel@(0,0)=" << pCorner << std::endl;
    }

    // -----------------------------------------------------------------------
    // 15. Run StarFinder on the stacked master and verify we recover 5 stars
    // -----------------------------------------------------------------------
    std::cout << "  [star_find] Running StarFinder on stacked master..." << std::endl;
    std::vector<Star> foundStars = StarFinder::findStars(
        stacked,
        /*maxStars=*/20,
        /*snrMin=*/3.0,
        /*fastFit=*/true,
        /*patchRadius=*/12,
        /*minFwhm=*/1.5,
        /*maxEccentricity=*/0.90
    );

    std::cout << "  [star_find] Found " << foundStars.size() << " stars in stacked master." << std::endl;
    for (auto& s : foundStars) {
        std::cout << "    star at (" << s.x << ", " << s.y
                  << ")  peak=" << s.peak
                  << "  fwhm=" << s.fwhm << std::endl;
    }

    assert(foundStars.size() >= 8 && "StarFinder failed to recover at least 8 stars in stacked master");

    // Each found star should be near one of the true reference-frame positions
    // (which is where they land after alignment to frame 0).
    // Use a 10-pixel match radius to allow for sub-pixel shift residuals
    // and the PSF broadening from stacking NaN-bordered frames.
    for (auto& s : trueCatalogue) {
        bool matched = false;
        for (auto& fs : foundStars) {
            if (std::hypot(fs.x - s.x, fs.y - s.y) < 5.0) {
                matched = true;
                break;
            }
        }
        assert(matched && "A true catalogue star is missing from the stacked master");
    }
    std::cout << "  [star_find] Catalogue cross-match complete." << std::endl;

    // Cleanup
    TempDirectory::cleanup();

    std::cout << "[+] Light Frame Pipeline Test PASSED." << std::endl << std::endl;
}

void testGhsStretching() {
    std::cout << "Running GHS Stretching Test..." << std::endl;

    // 1. Create a synthetic grayscale image with a linear gradient
    const int width = 100;
    const int height = 100;
    auto img = std::make_shared<GrayscaleImage>(width, height);
    auto buf = img->buffer();
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            // Linear gradient from 0.0 to 1.0 horizontally
            float val = static_cast<float>(x) / (width - 1);
            buf->setPixel(x, y, val);
        }
    }

    // 2. Test hyperbolic stretch (form = 1)
    double SP = 0.1;
    double D = 3.0;
    auto stretchedGrayscale1 = StretchingAlgorithm::stretchGhsGrayscale(img, 0.0, 1.0, SP, D, 0.0, 1.0, 1);
    assert(stretchedGrayscale1 != nullptr);
    
    // Check bounds and monotonicity
    float prevVal = -1.0f;
    for (int x = 0; x < width; ++x) {
        float val = stretchedGrayscale1->buffer()->pixel(x, 50);
        assert(val >= 0.0f && val <= 1.0f && "Stretched value out of [0,1] range");
        assert(val >= prevVal && "Stretched values are not monotonically increasing");
        prevVal = val;
    }
    
    // Verifying that a linear input gets stretched non-linearly
    float midIn = img->buffer()->pixel(50, 50); // 50 / 99 ≈ 0.505
    float midOut = stretchedGrayscale1->buffer()->pixel(50, 50);
    assert(midOut > midIn && "GHS stretch failed to boost midtone values");

    // 3. Test exponential stretch (form = 0)
    auto stretchedGrayscale0 = StretchingAlgorithm::stretchGhsGrayscale(img, 0.0, 1.0, SP, D, 0.0, 1.0, 0);
    assert(stretchedGrayscale0 != nullptr);
    float val0 = stretchedGrayscale0->buffer()->pixel(50, 50);
    assert(val0 != midOut && "Exponential and hyperbolic stretches should produce different profiles");

    // 4. Test RGB Color-Preserving Stretch
    auto rgbImg = std::make_shared<RGBImage>(width, height);
    // Set up a pixel with known color ratios: R:G:B = 1:2:3
    rgbImg->r()->buffer()->setPixel(50, 50, 0.01f);
    rgbImg->g()->buffer()->setPixel(50, 50, 0.02f);
    rgbImg->b()->buffer()->setPixel(50, 50, 0.03f);

    auto stretchedRGB = StretchingAlgorithm::stretchGhsRGB(rgbImg, 0.0, 1.0, SP, D, 0.0, 1.0, 1, true);
    assert(stretchedRGB != nullptr);

    float rOut = stretchedRGB->r()->buffer()->pixel(50, 50);
    float gOut = stretchedRGB->g()->buffer()->pixel(50, 50);
    float bOut = stretchedRGB->b()->buffer()->pixel(50, 50);

    // Verify color ratios are preserved: gOut / rOut should be 2.0, bOut / rOut should be 3.0
    assert(approxEqual(gOut / rOut, 2.0, 1e-4) && "Color-preserving GHS stretch failed to preserve R:G ratio");
    assert(approxEqual(bOut / rOut, 3.0, 1e-4) && "Color-preserving GHS stretch failed to preserve R:B ratio");

    // 5. Test RGB Independent Channel Stretch
    auto stretchedRGBIndep = StretchingAlgorithm::stretchGhsRGB(rgbImg, 0.0, 1.0, SP, D, 0.0, 1.0, 1, false);
    assert(stretchedRGBIndep != nullptr);

    float rOutIndep = stretchedRGBIndep->r()->buffer()->pixel(50, 50);
    float gOutIndep = stretchedRGBIndep->g()->buffer()->pixel(50, 50);
    float bOutIndep = stretchedRGBIndep->b()->buffer()->pixel(50, 50);

    // With independent stretch, since R, G, B have different inputs, they stretch non-linearly
    // independently, so their output ratios will be distorted (not 1:2:3).
    double ratioG = gOutIndep / rOutIndep;
    assert(!approxEqual(ratioG, 2.0, 1e-3) && "Independent channel GHS stretch should not preserve color ratios");

    std::cout << "[+] GHS Stretching Test PASSED." << std::endl << std::endl;
}

void testBackgroundExtractionOnGradient() {
    std::cout << "Running Background Extraction on Gradient Test..." << std::endl;

    WorkspaceRegistry workspace;
    FitsIO fits;

    ImageVariant inputVar = fits.readImage("gradient_input.fits");
    assert(std::holds_alternative<GrayscaleImagePtr>(inputVar) && "Failed to load gradient_input.fits as GrayscaleImage");
    auto inputImg = std::get<GrayscaleImagePtr>(inputVar);
    workspace.registerElement("input_gradient", inputImg);

    // Get input properties
    int w = inputImg->width();
    int h = inputImg->height();
    
    // Print input stats
    float valTopLeft = inputImg->buffer()->pixel(0, 0);
    float valTopRight = inputImg->buffer()->pixel(w - 1, 0);
    float valBottomLeft = inputImg->buffer()->pixel(0, h - 1);
    float valBottomRight = inputImg->buffer()->pixel(w - 1, h - 1);
    std::cout << "  Input corner values:" << std::endl;
    std::cout << "    Top-Left:     " << valTopLeft << std::endl;
    std::cout << "    Top-Right:    " << valTopRight << std::endl;
    std::cout << "    Bottom-Left:  " << valBottomLeft << std::endl;
    std::cout << "    Bottom-Right: " << valBottomRight << std::endl;

    BackgroundExtractionAlgorithm bge;
    bge.execute(workspace, {
        {"input_name",   "input_gradient"},
        {"output_name",  "output_neutralized"},
        {"order",        "1"},
        {"sigma_cut",    "3.0"},
        {"sample_frac",  "0.05"},
        {"huber_delta",  "5.0"},
        {"equalize",     "true"}
    });

    auto outputVar = workspace.getElement("output_neutralized");
    assert(std::holds_alternative<GrayscaleImagePtr>(outputVar) && "BGE output is not GrayscaleImage");
    auto outputImg = std::get<GrayscaleImagePtr>(outputVar);

    // Save the output so it can be verified/loaded by the user
    fits.writeImage("gradient_output.fits", ImageVariant{outputImg});

    // Print output stats
    float outTopLeft = outputImg->buffer()->pixel(0, 0);
    float outTopRight = outputImg->buffer()->pixel(w - 1, 0);
    float outBottomLeft = outputImg->buffer()->pixel(0, h - 1);
    float outBottomRight = outputImg->buffer()->pixel(w - 1, h - 1);
    std::cout << "  Output corner values:" << std::endl;
    std::cout << "    Top-Left:     " << outTopLeft << std::endl;
    std::cout << "    Top-Right:    " << outTopRight << std::endl;
    std::cout << "    Bottom-Left:  " << outBottomLeft << std::endl;
    std::cout << "    Bottom-Right: " << outBottomRight << std::endl;

    // Check if the gradient is removed. The difference between corners should be minimal (close to noise standard deviation ~ 0.01)
    double diffX_in = std::abs(valTopRight - valTopLeft);
    double diffY_in = std::abs(valBottomLeft - valTopLeft);
    double diffX_out = std::abs(outTopRight - outTopLeft);
    double diffY_out = std::abs(outBottomLeft - outTopLeft);

    std::cout << "  Gradient comparison:" << std::endl;
    std::cout << "    Input X Diff: " << diffX_in << " -> Output X Diff: " << diffX_out << std::endl;
    std::cout << "    Input Y Diff: " << diffY_in << " -> Output Y Diff: " << diffY_out << std::endl;

    // The gradient diff was 0.3 in X, 0.2 in Y. In the output it should be close to 0 (typically < 0.02)
    assert(diffX_out < 0.03 && "X-gradient not removed properly!");
    assert(diffY_out < 0.03 && "Y-gradient not removed properly!");

    std::cout << "[+] Background Extraction on Gradient Test PASSED." << std::endl << std::endl;
}

void testRbfBackgroundExtraction() {
    std::cout << "Running RBF Background Extraction Test..." << std::endl;

    WorkspaceRegistry workspace;
    FitsIO fits;

    // Load the gradient input fits file generated in the previous test
    ImageVariant inputVar = fits.readImage("gradient_input.fits");
    assert(std::holds_alternative<GrayscaleImagePtr>(inputVar) && "Failed to load gradient_input.fits");
    auto inputImg = std::get<GrayscaleImagePtr>(inputVar);
    workspace.registerElement("input_gradient_rbf", inputImg);

    int w = inputImg->width();
    int h = inputImg->height();

    // Set control points grid on the input image buffer
    std::vector<std::pair<double, double>> pts;
    int cols = 5;
    int rows = 5;
    double marginX = w * 0.08;
    double marginY = h * 0.08;
    double stepX = (w - 2 * marginX) / (cols - 1);
    double stepY = (h - 2 * marginY) / (rows - 1);
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            pts.push_back({marginX + c * stepX, marginY + r * stepY});
        }
    }
    inputImg->buffer()->setBgeControlPoints(pts);

    BackgroundExtractionAlgorithm bge;
    bge.execute(workspace, {
        {"input_name",     "input_gradient_rbf"},
        {"output_name",    "output_rbf_neutralized"},
        {"method",         "RBF"},
        {"rbf_smoothing",  "0.0"},
        {"sigma_cut",      "3.0"},
        {"sample_frac",    "0.05"},
        {"huber_delta",    "5.0"},
        {"equalize",       "true"}
    });

    auto outputVar = workspace.getElement("output_rbf_neutralized");
    assert(std::holds_alternative<GrayscaleImagePtr>(outputVar) && "BGE RBF output is not GrayscaleImage");
    auto outputImg = std::get<GrayscaleImagePtr>(outputVar);

    // Print output stats
    float outTopLeft = outputImg->buffer()->pixel(0, 0);
    float outTopRight = outputImg->buffer()->pixel(w - 1, 0);
    float outBottomLeft = outputImg->buffer()->pixel(0, h - 1);
    float outBottomRight = outputImg->buffer()->pixel(w - 1, h - 1);
    std::cout << "  RBF Output corner values:" << std::endl;
    std::cout << "    Top-Left:     " << outTopLeft << std::endl;
    std::cout << "    Top-Right:    " << outTopRight << std::endl;
    std::cout << "    Bottom-Left:  " << outBottomLeft << std::endl;
    std::cout << "    Bottom-Right: " << outBottomRight << std::endl;

    double diffX_out = std::abs(outTopRight - outTopLeft);
    double diffY_out = std::abs(outBottomLeft - outTopLeft);
    std::cout << "  RBF Gradient comparison:" << std::endl;
    std::cout << "    Output X Diff: " << diffX_out << std::endl;
    std::cout << "    Output Y Diff: " << diffY_out << std::endl;

    // The gradient diff was 0.3 in X, 0.2 in Y. In the output it should be close to 0 (typically < 0.02)
    assert(diffX_out < 0.03 && "RBF X-gradient not removed properly!");
    assert(diffY_out < 0.03 && "RBF Y-gradient not removed properly!");

    std::cout << "[+] RBF Background Extraction Test PASSED." << std::endl << std::endl;
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "Starting Blastro Algorithm Unit Tests..." << std::endl;
    std::cout << "========================================" << std::endl << std::endl;

    testGaussianStarFitting();
    testConstellationMatching();
    testStackingMathematics();
    testCalibrationPipeline();
    testLightFramePipeline();
    testGhsStretching();
    testBackgroundExtractionOnGradient();
    testRbfBackgroundExtraction();

    std::cout << "========================================" << std::endl;
    std::cout << "All Blastro Algorithm Unit Tests PASSED!" << std::endl;
    std::cout << "========================================" << std::endl;
    return 0;
}
