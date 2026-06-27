#include "FitsIO.h"
#include <CCfits/CCfits>
#include <valarray>
#include <stdexcept>
#include <algorithm>
#include <iostream>
#include <QFileInfo>

namespace blastro {

static float determineScaleFactor(int bitpix, const std::valarray<float>& contents) {
    float scaleFactor = 1.0f;
    if (bitpix == 8) {
        scaleFactor = 255.0f;
    } else if (bitpix == 16) {
        scaleFactor = 65535.0f;
    } else if (bitpix == 32) {
        scaleFactor = 4294967295.0f;
    }
    // bitpix < 0 means floating-point FITS (FLOAT_IMG / DOUBLE_IMG).
    // Values are stored as true floats — no integer-range normalisation needed.
    return scaleFactor;
}

bool FitsIO::supportsExtension(const std::string& ext) const {
    std::string lowerExt = ext;
    std::transform(lowerExt.begin(), lowerExt.end(), lowerExt.begin(), ::tolower);
    return (lowerExt == "fits" || lowerExt == "fit" || lowerExt == ".fits" || lowerExt == ".fit");
}

ImageVariant FitsIO::readImage(const std::string& filepath) {
    try {
        // Open FITS file for reading
        std::unique_ptr<CCfits::FITS> pInfile(new CCfits::FITS(filepath, CCfits::Read, true));
        CCfits::PHDU& image = pInfile->pHDU();

        int axes = image.axes();
        if (axes < 2) {
            throw std::runtime_error("FITS file must have at least 2 dimensions");
        }

        long width = image.axis(0);
        long height = image.axis(1);

        if (axes == 2) {
            // Grayscale image
            std::valarray<float> contents;
            image.read(contents);

            auto grayImg = std::make_shared<GrayscaleImage>(width, height);
            auto buffer = grayImg->buffer();
            float scale = determineScaleFactor(image.bitpix(), contents);
            for (int y = 0; y < height; ++y) {
                for (int x = 0; x < width; ++x) {
                    // FITS pixel storage: row-major where axis(0) varies fastest
                    buffer->setPixel(x, y, contents[y * width + x] / scale);
                }
            }
            return grayImg;
        } else if (axes == 3) {
            long depth = image.axis(2);
            std::valarray<float> contents;
            image.read(contents);

            if (depth == 3) {
                // Read as RGB image
                auto rgbImg = std::make_shared<RGBImage>(width, height);
                auto bufR = rgbImg->r()->buffer();
                auto bufG = rgbImg->g()->buffer();
                auto bufB = rgbImg->b()->buffer();

                float scale = determineScaleFactor(image.bitpix(), contents);
                long planeSize = width * height;
                for (int y = 0; y < height; ++y) {
                    for (int x = 0; x < width; ++x) {
                        long idx = y * width + x;
                        bufR->setPixel(x, y, contents[idx] / scale);
                        bufG->setPixel(x, y, contents[planeSize + idx] / scale);
                        bufB->setPixel(x, y, contents[2 * planeSize + idx] / scale);
                    }
                }
                return rgbImg;
            } else {
                // 3D FITS cube that is not 3 planes is treated as a Batch of grayscale images
                throw std::runtime_error("3D FITS cube with depth != 3 must be loaded as a Batch");
            }
        } else {
            throw std::runtime_error("Unsupported FITS dimensions");
        }
    } catch (CCfits::FitsException& ex) {
        throw std::runtime_error(std::string("CCfits Error: ") + ex.message());
    }
}

ImageBatchPtr FitsIO::readBatch(const std::vector<std::string>& filepaths) {
    if (filepaths.empty()) {
        throw std::invalid_argument("Filepath list is empty");
    }

    // Check if the first file is a FITS cube (3D where depth > 3 or just any 3D)
    // If it's a single file and it's a 3D cube, we can make a batch out of its planes.
    if (filepaths.size() == 1) {
        const std::string& filepath = filepaths[0];
        try {
            std::unique_ptr<CCfits::FITS> pInfile(new CCfits::FITS(filepath, CCfits::Read, true));
            CCfits::PHDU& image = pInfile->pHDU();
            int axes = image.axes();
            if (axes == 3) {
                long width = image.axis(0);
                long height = image.axis(1);
                long depth = image.axis(2);

                // Precompute plane names
                std::vector<std::string> names(depth);
                std::string baseName = QFileInfo(QString::fromStdString(filepath)).fileName().toStdString();
                for (int i = 0; i < depth; ++i) {
                    names[i] = baseName + " (Plane " + std::to_string(i + 1) + ")";
                }

                std::vector<std::string> filepaths(depth, filepath);
                auto loader = [filepath, width, height, depth](int index) -> ImageVariant {
                    try {
                        std::unique_ptr<CCfits::FITS> pInfileInner(new CCfits::FITS(filepath, CCfits::Read, true));
                        CCfits::PHDU& imageInner = pInfileInner->pHDU();
                        
                        std::valarray<float> plane;
                        std::vector<long> fp = { 1, 1, index + 1 };
                        std::vector<long> lp = { width, height, index + 1 };
                        std::vector<long> stride = { 1, 1, 1 };
                        imageInner.read(plane, fp, lp, stride);

                        auto grayImg = std::make_shared<GrayscaleImage>(width, height);
                        auto buffer = grayImg->buffer();
                        float scale = determineScaleFactor(imageInner.bitpix(), plane);
                        for (int y = 0; y < height; ++y) {
                            for (int x = 0; x < width; ++x) {
                                buffer->setPixel(x, y, plane[y * width + x] / scale);
                            }
                        }
                        return grayImg;
                    } catch (CCfits::FitsException& ex) {
                        throw std::runtime_error(std::string("CCfits Error in batch loader: ") + ex.message());
                    }
                };
                auto batch = std::make_shared<ImageBatch>(depth, loader, names, filepaths);
                
                // Read registration metadata from primary HDU header if present
                for (int i = 0; i < depth; ++i) {
                    std::string idxStr = std::to_string(i + 1);
                    try {
                        int reg = 0;
                        image.readKey("R" + idxStr + "REG", reg);
                        if (reg) {
                            FrameMetadata meta;
                            meta.registered = true;
                            image.readKey("R" + idxStr + "DX", meta.dx);
                            image.readKey("R" + idxStr + "DY", meta.dy);
                            image.readKey("R" + idxStr + "TH", meta.theta);
                            image.readKey("R" + idxStr + "ST", meta.starCount);
                            image.readKey("R" + idxStr + "FW", meta.fwhm);
                            image.readKey("R" + idxStr + "SN", meta.snr);
                            image.readKey("R" + idxStr + "QL", meta.qualityScore);
                            batch->setFrameMetadata(i, meta);
                            
                            int sel = 1;
                            try {
                                image.readKey("R" + idxStr + "SL", sel);
                            } catch (...) {}
                            batch->setFrameSelected(i, sel != 0);
                        }
                    } catch (...) {
                        // Keyword not found, skip
                    }
                }
                return batch;
            }
        } catch (...) {
            // Fall back to treating it as a single 2D FITS file batch
        }
    }

    // Otherwise, treat as a list of independent FITS files
    std::vector<std::string> names(filepaths.size());
    for (size_t i = 0; i < filepaths.size(); ++i) {
        names[i] = QFileInfo(QString::fromStdString(filepaths[i])).fileName().toStdString();
    }

    auto loader = [filepaths](int index) -> ImageVariant {
        FitsIO reader;
        return reader.readImage(filepaths[index]);
    };
    auto batch = std::make_shared<ImageBatch>(filepaths.size(), loader, names, filepaths);
    
    // Read registration metadata from each individual file header if present
    for (size_t i = 0; i < filepaths.size(); ++i) {
        try {
            std::unique_ptr<CCfits::FITS> pInfile(new CCfits::FITS(filepaths[i], CCfits::Read, true));
            CCfits::PHDU& image = pInfile->pHDU();
            int reg = 0;
            try {
                image.readKey("REG_REG", reg);
            } catch (...) {
                try { image.readKey("REG", reg); } catch (...) {}
            }
            if (reg) {
                FrameMetadata meta;
                meta.registered = true;
                try { image.readKey("REG_DX", meta.dx); } catch (...) {}
                try { image.readKey("REG_DY", meta.dy); } catch (...) {}
                try { image.readKey("REG_TH", meta.theta); } catch (...) {}
                try { image.readKey("REG_ST", meta.starCount); } catch (...) {}
                try { image.readKey("REG_FW", meta.fwhm); } catch (...) {}
                try { image.readKey("REG_SN", meta.snr); } catch (...) {}
                try { image.readKey("REG_QL", meta.qualityScore); } catch (...) {}
                batch->setFrameMetadata(i, meta);
                
                int sel = 1;
                try { image.readKey("REG_SEL", sel); } catch (...) {}
                batch->setFrameSelected(i, sel != 0);
            }
        } catch (...) {}
    }
    return batch;
}

bool FitsIO::writeImage(const std::string& filepath, const ImageVariant& image) {
    try {
        // Create new FITS file, overwrite if exists
        std::string writePath = "!" + filepath; // '!' tells cfitsio to overwrite existing file
        
        long width = 0;
        long height = 0;
        int planes = 0;

        if (std::holds_alternative<GrayscaleImagePtr>(image)) {
            auto img = std::get<GrayscaleImagePtr>(image);
            width = img->width();
            height = img->height();
            planes = 1;
        } else {
            auto img = std::get<RGBImagePtr>(image);
            width = img->width();
            height = img->height();
            planes = 3;
        }

        long numAxes = (planes == 1) ? 2 : 3;
        std::vector<long> naxes(numAxes);
        naxes[0] = width;
        naxes[1] = height;
        if (planes > 1) {
            naxes[2] = planes;
        }

        // CCfits::FITS constructor creates the file
        std::unique_ptr<CCfits::FITS> pOutfile(new CCfits::FITS(writePath, FLOAT_IMG, numAxes, naxes.data()));
        CCfits::PHDU& phdu = pOutfile->pHDU();

        long planeSize = width * height;
        std::valarray<float> arrayData(planeSize * planes);

        if (planes == 1) {
            auto img = std::get<GrayscaleImagePtr>(image);
            auto buffer = img->buffer();
            for (int y = 0; y < height; ++y) {
                for (int x = 0; x < width; ++x) {
                    arrayData[y * width + x] = buffer->pixel(x, y);
                }
            }
        } else {
            auto img = std::get<RGBImagePtr>(image);
            auto bufR = img->r()->buffer();
            auto bufG = img->g()->buffer();
            auto bufB = img->b()->buffer();
            for (int y = 0; y < height; ++y) {
                for (int x = 0; x < width; ++x) {
                    long idx = y * width + x;
                    arrayData[idx] = bufR->pixel(x, y);
                    arrayData[planeSize + idx] = bufG->pixel(x, y);
                    arrayData[2 * planeSize + idx] = bufB->pixel(x, y);
                }
            }
        }

        // Write the data to FITS primary HDU
        phdu.write(1, planeSize * planes, arrayData);
        return true;
    } catch (CCfits::FitsException& ex) {
        std::cerr << "FITS write error: " << ex.message() << std::endl;
        return false;
    }
}

bool FitsIO::writeBatch(const std::string& filepath, ImageBatchPtr batch) {
    try {
        std::string writePath = "!" + filepath;
        int count = batch->count();
        if (count <= 0) return false;

        // Get dimensions from the first frame
        ImageVariant firstImg = batch->getImage(0);
        long width = 0;
        long height = 0;
        if (std::holds_alternative<GrayscaleImagePtr>(firstImg)) {
            auto img = std::get<GrayscaleImagePtr>(firstImg);
            width = img->width();
            height = img->height();
        } else {
            auto img = std::get<RGBImagePtr>(firstImg);
            width = img->width();
            height = img->height();
        }

        long numAxes = 3;
        std::vector<long> naxes(3);
        naxes[0] = width;
        naxes[1] = height;
        naxes[2] = count;

        std::unique_ptr<CCfits::FITS> pOutfile(new CCfits::FITS(writePath, FLOAT_IMG, numAxes, naxes.data()));
        CCfits::PHDU& phdu = pOutfile->pHDU();

        // Write registration metadata to the primary HDU header
        for (int i = 0; i < count; ++i) {
            FrameMetadata meta = batch->frameMetadata(i);
            bool selected = batch->isFrameSelected(i);
            std::string idxStr = std::to_string(i + 1);
            
            phdu.addKey("R" + idxStr + "REG", static_cast<int>(meta.registered), "Is registered");
            phdu.addKey("R" + idxStr + "DX", meta.dx, "dx offset");
            phdu.addKey("R" + idxStr + "DY", meta.dy, "dy offset");
            phdu.addKey("R" + idxStr + "TH", meta.theta, "theta offset");
            phdu.addKey("R" + idxStr + "ST", meta.starCount, "Star count");
            phdu.addKey("R" + idxStr + "FW", meta.fwhm, "Average FWHM");
            phdu.addKey("R" + idxStr + "SN", meta.snr, "SNR value");
            phdu.addKey("R" + idxStr + "QL", meta.qualityScore, "Quality score");
            phdu.addKey("R" + idxStr + "SL", static_cast<int>(selected), "Is frame selected");
        }

        long planeSize = width * height;
        std::valarray<float> arrayData(planeSize * count);

        for (int i = 0; i < count; ++i) {
            ImageVariant frameImg = batch->getImage(i);
            if (std::holds_alternative<GrayscaleImagePtr>(frameImg)) {
                auto img = std::get<GrayscaleImagePtr>(frameImg);
                auto buffer = img->buffer();
                for (int y = 0; y < height; ++y) {
                    for (int x = 0; x < width; ++x) {
                        arrayData[i * planeSize + y * width + x] = buffer->pixel(x, y);
                    }
                }
            } else {
                auto img = std::get<RGBImagePtr>(frameImg);
                auto bufR = img->r()->buffer();
                auto bufG = img->g()->buffer();
                auto bufB = img->b()->buffer();
                for (int y = 0; y < height; ++y) {
                    for (int x = 0; x < width; ++x) {
                        float gray = (bufR->pixel(x, y) + bufG->pixel(x, y) + bufB->pixel(x, y)) / 3.0f;
                        arrayData[i * planeSize + y * width + x] = gray;
                    }
                }
            }
        }

        phdu.write(1, planeSize * count, arrayData);
        return true;
    } catch (CCfits::FitsException& ex) {
        std::cerr << "FITS write batch error: " << ex.message() << std::endl;
        return false;
    }
}

ImageVariant FitsIO::readImagePatch(const std::string& filepath, int xStart, int yStart, int patchW, int patchH) {
    try {
        std::unique_ptr<CCfits::FITS> pInfile(new CCfits::FITS(filepath, CCfits::Read, true));
        CCfits::PHDU& image = pInfile->pHDU();

        int axes = image.axes();
        if (axes < 2) {
            throw std::runtime_error("FITS file must have at least 2 dimensions");
        }

        if (axes == 2) {
            // Grayscale image patch
            std::valarray<float> contents;
            std::vector<long> fp = { xStart + 1, yStart + 1 };
            std::vector<long> lp = { xStart + patchW, yStart + patchH };
            std::vector<long> stride = { 1, 1 };
            image.read(contents, fp, lp, stride);

            auto grayImg = std::make_shared<GrayscaleImage>(patchW, patchH);
            auto buffer = grayImg->buffer();
            float scale = determineScaleFactor(image.bitpix(), contents);
            for (int y = 0; y < patchH; ++y) {
                for (int x = 0; x < patchW; ++x) {
                    buffer->setPixel(x, y, contents[y * patchW + x] / scale);
                }
            }
            return grayImg;
        } else if (axes == 3) {
            long depth = image.axis(2);
            if (depth == 3) {
                // RGB image patch
                std::valarray<float> contents;
                std::vector<long> fp = { xStart + 1, yStart + 1, 1 };
                std::vector<long> lp = { xStart + patchW, yStart + patchH, 3 };
                std::vector<long> stride = { 1, 1, 1 };
                image.read(contents, fp, lp, stride);

                auto rgbImg = std::make_shared<RGBImage>(patchW, patchH);
                auto bufR = rgbImg->r()->buffer();
                auto bufG = rgbImg->g()->buffer();
                auto bufB = rgbImg->b()->buffer();

                float scale = determineScaleFactor(image.bitpix(), contents);
                long planeSize = patchW * patchH;
                for (int y = 0; y < patchH; ++y) {
                    for (int x = 0; x < patchW; ++x) {
                        long idx = y * patchW + x;
                        bufR->setPixel(x, y, contents[idx] / scale);
                        bufG->setPixel(x, y, contents[planeSize + idx] / scale);
                        bufB->setPixel(x, y, contents[2 * planeSize + idx] / scale);
                    }
                }
                return rgbImg;
            } else {
                throw std::runtime_error("Unsupported FITS dimensions for patch read");
            }
        } else {
            throw std::runtime_error("Unsupported FITS dimensions for patch read");
        }
    } catch (CCfits::FitsException& ex) {
        throw std::runtime_error(std::string("CCfits Error: ") + ex.message());
    }
}

bool FitsIO::writeImagePatch(const std::string& filepath, const ImageVariant& patch, int xStart, int yStart) {
    try {
        std::unique_ptr<CCfits::FITS> pOutfile(new CCfits::FITS(filepath, CCfits::Write));
        CCfits::PHDU& phdu = pOutfile->pHDU();

        long patchW = 0;
        long patchH = 0;
        int planes = 0;

        if (std::holds_alternative<GrayscaleImagePtr>(patch)) {
            auto img = std::get<GrayscaleImagePtr>(patch);
            patchW = img->width();
            patchH = img->height();
            planes = 1;
        } else {
            auto img = std::get<RGBImagePtr>(patch);
            patchW = img->width();
            patchH = img->height();
            planes = 3;
        }

        long planeSize = patchW * patchH;
        std::valarray<float> arrayData(planeSize * planes);

        if (planes == 1) {
            auto img = std::get<GrayscaleImagePtr>(patch);
            auto buffer = img->buffer();
            for (int y = 0; y < patchH; ++y) {
                for (int x = 0; x < patchW; ++x) {
                    arrayData[y * patchW + x] = buffer->pixel(x, y);
                }
            }
            std::vector<long> fp = { xStart + 1, yStart + 1 };
            std::vector<long> lp = { xStart + patchW, yStart + patchH };
            std::vector<long> stride = { 1, 1 };
            phdu.write(fp, lp, stride, arrayData);
        } else {
            auto img = std::get<RGBImagePtr>(patch);
            auto bufR = img->r()->buffer();
            auto bufG = img->g()->buffer();
            auto bufB = img->b()->buffer();
            for (int y = 0; y < patchH; ++y) {
                for (int x = 0; x < patchW; ++x) {
                    long idx = y * patchW + x;
                    arrayData[idx] = bufR->pixel(x, y);
                    arrayData[planeSize + idx] = bufG->pixel(x, y);
                    arrayData[2 * planeSize + idx] = bufB->pixel(x, y);
                }
            }
            std::vector<long> fp = { xStart + 1, yStart + 1, 1 };
            std::vector<long> lp = { xStart + patchW, yStart + patchH, 3 };
            std::vector<long> stride = { 1, 1, 1 };
            phdu.write(fp, lp, stride, arrayData);
        }

        return true;
    } catch (CCfits::FitsException& ex) {
        std::cerr << "FITS write patch error: " << ex.message() << std::endl;
        return false;
    }
}

} // namespace blastro
