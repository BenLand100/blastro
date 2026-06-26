#include "AlignAlgorithm.h"
#include "Warping.h"
#include "core/GrayscaleImage.h"
#include "core/RGBImage.h"
#include "core/ImageBatch.h"
#include "core/TempDirectory.h"
#include "io/FitsIO.h"
#include <stdexcept>
#include <iostream>
#include <omp.h>
#include <QDebug>

namespace blastro {

void AlignAlgorithm::execute(WorkspaceRegistry& workspace, 
                             const std::map<std::string, std::string>& config, 
                             ProgressCallback progress) {
    std::string inputName = config.at("input_name");
    std::string outputName = config.at("output_name");
    double drizzleScale = std::stod(config.at("drizzle_scale"));
    int threads = std::stoi(config.at("threads"));
    bool evictCache = config.at("evict_cache") == "true";

    qInfo() << "[Align] Starting execution. Input batch:" << QString::fromStdString(inputName) << ", output batch:" << QString::fromStdString(outputName) << ", drizzle scale:" << drizzleScale << ", threads:" << threads;

    // Set thread count for OpenMP parallel loops
    if (threads > 0) {
        omp_set_num_threads(threads);
    }

    WorkspaceElement inputElem = workspace.getElement(inputName);
    if (!std::holds_alternative<ImageBatchPtr>(inputElem)) {
        throw std::runtime_error("Image Alignment requires an Image Batch as input");
    }

    auto batch = std::get<ImageBatchPtr>(inputElem);
    int count = batch->count();

    // 1. Create a unique temporary directory to write the aligned FITS frames
    std::string tempDir = TempDirectory::createTempDir("aligned");
    if (tempDir.empty()) {
        throw std::runtime_error("Failed to create temporary directory for aligned frames");
    }

    std::vector<std::string> names(count);
    std::vector<std::string> filepaths(count);
    std::vector<FrameMetadata> finalMetadata(count);

    // 2. Warp each frame sequentially and write to disk
    for (int i = 0; i < count; ++i) {
        if (progress) {
            progress(static_cast<int>(100.0 * i / count));
        }

        // We only warp selected frames.
        if (!batch->isFrameSelected(i)) {
            names[i] = batch->frameName(i) + "_unselected";
            filepaths[i] = "";
            continue;
        }

        FrameMetadata meta = batch->frameMetadata(i);
        if (!meta.registered) {
            std::cerr << "[AlignAlgorithm] Warning: frame " << i << " is selected but not registered. Skipping." << std::endl;
            names[i] = batch->frameName(i) + "_unregistered";
            filepaths[i] = "";
            continue;
        }

        ImageVariant frame = batch->getImage(i);
        ImageVariant alignedFrame;

        // Perform warping (backward-mapping bilinear interpolation, applying drizzle)
        if (std::holds_alternative<GrayscaleImagePtr>(frame)) {
            auto gray = std::get<GrayscaleImagePtr>(frame);
            alignedFrame = Warping::warpGrayscale(gray, meta.dx, meta.dy, meta.theta, drizzleScale);
        } else if (std::holds_alternative<RGBImagePtr>(frame)) {
            auto rgb = std::get<RGBImagePtr>(frame);
            alignedFrame = Warping::warpRGB(rgb, meta.dx, meta.dy, meta.theta, drizzleScale);
        }

        // Save aligned frame to a temporary FITS file
        std::string origPath = batch->frameFilepath(i);
        std::string outFilename = TempDirectory::getIntermediateFileName(origPath, "_aligned", i);
        std::string fullOutPath = tempDir + "/" + outFilename;

        FitsIO writer;
        if (!writer.writeImage(fullOutPath, alignedFrame)) {
            throw std::runtime_error("Failed to write temporary aligned frame to " + fullOutPath);
        }

        names[i] = batch->frameName(i) + "_aligned";
        filepaths[i] = fullOutPath;
        finalMetadata[i] = meta;

        // Memory optimization: evict the loaded source frame from RAM
        if (evictCache) {
            batch->clearCache(i);
        }
    }

    // 3. Create a new disk-backed ImageBatch pointing to the aligned FITS files
    auto alignedBatch = std::make_shared<ImageBatch>(
        count,
        [filepaths](int index) -> ImageVariant {
            FitsIO reader;
            return reader.readImage(filepaths[index]);
        },
        names,
        filepaths
    );

    // Copy over the metadata to the new aligned batch
    for (int i = 0; i < count; ++i) {
        alignedBatch->setFrameMetadata(i, finalMetadata[i]);
        alignedBatch->setFrameSelected(i, batch->isFrameSelected(i));
    }

    if (progress) progress(100);
    
    if (workspace.contains(outputName)) {
        workspace.unregisterElement(outputName);
    }
    workspace.registerElement(outputName, alignedBatch);
    qInfo() << "[Align] Finished alignment. Aligned" << count << "frames into temporary directory:" << QString::fromStdString(tempDir) << ". Registered output batch:" << QString::fromStdString(outputName);
}

} // namespace blastro
