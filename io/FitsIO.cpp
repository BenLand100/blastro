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

#include "FitsIO.h"
#include <CCfits/CCfits>
#include <valarray>
#include <stdexcept>
#include <algorithm>
#include <iostream>
#include <QFileInfo>

namespace blastro {

const double PI = 3.14159265358979323846;

static void populateMetadata(CCfits::FITS* pInfile, ImageMetadata& metadata) {
    fitsfile* fptr = pInfile->fitsPointer();
    int nkeys = 0;
    int status = 0;
    fits_get_hdrspace(fptr, &nkeys, nullptr, &status);
    for (int i = 1; i <= nkeys; ++i) {
        char keyname[FLEN_KEYWORD] = {0};
        char value[FLEN_VALUE] = {0};
        char comment[FLEN_COMMENT] = {0};
        fits_read_keyn(fptr, i, keyname, value, comment, &status);
        if (status == 0) {
            std::string name = keyname;
            if (name == "SIMPLE" || name == "BITPIX" || name == "NAXIS" || name == "NAXIS1" || name == "NAXIS2" || 
                name == "NAXIS3" || name == "EXTEND" || name == "BSCALE" || name == "BZERO" || name == "COMMENT" || name == "HISTORY") {
                continue;
            }
            std::string valStr = value;
            if (valStr.size() >= 2 && valStr.front() == '\'' && valStr.back() == '\'') {
                valStr = valStr.substr(1, valStr.size() - 2);
                while (!valStr.empty() && valStr.back() == ' ') {
                    valStr.pop_back();
                }
            }
            metadata.fitsKeywords[name] = valStr;
        } else {
            status = 0;
        }
    }

    auto getDouble = [&](const std::string& key, double& outVal) {
        auto it = metadata.fitsKeywords.find(key);
        if (it != metadata.fitsKeywords.end()) {
            try {
                outVal = std::stod(it->second);
                return true;
            } catch (...) {}
        }
        return false;
    };

    getDouble("EXPTIME", metadata.exposureTime);
    getDouble("GAIN", metadata.gain);

    auto itF = metadata.fitsKeywords.find("FILTER");
    if (itF != metadata.fitsKeywords.end()) {
        metadata.filter = itF->second;
    } else {
        itF = metadata.fitsKeywords.find("FILTNAM");
        if (itF != metadata.fitsKeywords.end()) {
            metadata.filter = itF->second;
        }
    }

    double crval1 = 0.0, crval2 = 0.0;
    if (getDouble("CRVAL1", crval1) && getDouble("CRVAL2", crval2)) {
        metadata.wcsSolved = true;
        metadata.wcsRaCenter = crval1;
        metadata.wcsDecCenter = crval2;

        double cd11 = 0.0, cd12 = 0.0, cd21 = 0.0, cd22 = 0.0;
        bool hasCD = getDouble("CD1_1", cd11) && getDouble("CD1_2", cd12) &&
                     getDouble("CD2_1", cd21) && getDouble("CD2_2", cd22);
        if (hasCD) {
            metadata.wcsPixelScale = std::sqrt(cd11 * cd11 + cd21 * cd21) * 3600.0;
            metadata.wcsRotation = std::atan2(cd12, cd11) * 180.0 / PI;
        } else {
            double cdelt1 = 0.0;
            if (getDouble("CDELT1", cdelt1)) {
                metadata.wcsPixelScale = std::abs(cdelt1) * 3600.0;
            }
            getDouble("CROTA2", metadata.wcsRotation);
        }
    } else {
        metadata.wcsSolved = false;
    }
}

static void writeMetadata(CCfits::PHDU& phdu, const ImageMetadata& metadata) {
    for (const auto& pair : metadata.fitsKeywords) {
        const std::string& name = pair.first;
        const std::string& val = pair.second;

        if (name == "SIMPLE" || name == "BITPIX" || name == "NAXIS" || name == "NAXIS1" || name == "NAXIS2" || 
            name == "NAXIS3" || name == "EXTEND" || name == "BSCALE" || name == "BZERO") {
            continue;
        }

        if (name == "EXPTIME" || name == "GAIN" || name == "FILTER" || name == "FILTNAM" ||
            name == "CRVAL1" || name == "CRVAL2" || name == "CD1_1" || name == "CD1_2" ||
            name == "CD2_1" || name == "CD2_2" || name == "CDELT1" || name == "CDELT2" ||
            name == "CROTA2" || name == "WCS_SLVD") {
            continue;
        }

        try {
            if (val == "T" || val == "F" || val == "true" || val == "false") {
                bool bVal = (val == "T" || val == "true");
                phdu.addKey(name, bVal, "");
            } else {
                size_t idx = 0;
                int iVal = std::stoi(val, &idx);
                if (idx == val.size()) {
                    phdu.addKey(name, iVal, "");
                } else {
                    double dVal = std::stod(val, &idx);
                    if (idx == val.size()) {
                        phdu.addKey(name, dVal, "");
                    } else {
                        phdu.addKey(name, val, "");
                    }
                }
            }
        } catch (...) {
            phdu.addKey(name, val, "");
        }
    }

    if (metadata.exposureTime > 0.0) {
        phdu.addKey("EXPTIME", metadata.exposureTime, "Exposure time in seconds");
    }
    if (metadata.gain > 0.0) {
        phdu.addKey("GAIN", metadata.gain, "Detector gain");
    }
    if (metadata.filter != "None" && !metadata.filter.empty()) {
        phdu.addKey("FILTER", metadata.filter, "Filter used");
    }

    if (metadata.wcsSolved) {
        phdu.addKey("WCS_SLVD", 1, "WCS solved by BLastro");
        phdu.addKey("CRVAL1", metadata.wcsRaCenter, "RA at reference pixel");
        phdu.addKey("CRVAL2", metadata.wcsDecCenter, "DEC at reference pixel");

        double scaleDeg = metadata.wcsPixelScale / 3600.0;
        double rotRad = metadata.wcsRotation * PI / 180.0;
        double cd11 = scaleDeg * std::cos(rotRad);
        double cd12 = scaleDeg * std::sin(rotRad);
        double cd21 = -scaleDeg * std::sin(rotRad);
        double cd22 = scaleDeg * std::cos(rotRad);

        phdu.addKey("CD1_1", cd11, "WCS CD matrix 1_1");
        phdu.addKey("CD1_2", cd12, "WCS CD matrix 1_2");
        phdu.addKey("CD2_1", cd21, "WCS CD matrix 2_1");
        phdu.addKey("CD2_2", cd22, "WCS CD matrix 2_2");
    }
}

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
            populateMetadata(pInfile.get(), grayImg->metadata());
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
            int drizzled = 0;
            try {
                image.readKey("DRIZZLED", drizzled);
            } catch (...) {
                drizzled = 0;
            }

            if (drizzled) {
                if (depth == 2) {
                    std::valarray<float> contents;
                    std::vector<long> fp = { 1, 1, 1 };
                    std::vector<long> lp = { width, height, 1 };
                    std::vector<long> stride = { 1, 1, 1 };
                    image.read(contents, fp, lp, stride);

                    auto grayImg = std::make_shared<GrayscaleImage>(width, height);
                    populateMetadata(pInfile.get(), grayImg->metadata());
                    auto buffer = grayImg->buffer();
                    float scale = determineScaleFactor(image.bitpix(), contents);
                    for (int y = 0; y < height; ++y) {
                        for (int x = 0; x < width; ++x) {
                            buffer->setPixel(x, y, contents[y * width + x] / scale);
                        }
                    }
                    return grayImg;
                } else if (depth == 6) {
                    std::valarray<float> rPlane;
                    std::vector<long> r_fp = { 1, 1, 1 };
                    std::vector<long> r_lp = { width, height, 1 };
                    std::vector<long> stride = { 1, 1, 1 };
                    image.read(rPlane, r_fp, r_lp, stride);

                    std::valarray<float> gPlane;
                    std::vector<long> g_fp = { 1, 1, 3 };
                    std::vector<long> g_lp = { width, height, 3 };
                    image.read(gPlane, g_fp, g_lp, stride);

                    std::valarray<float> bPlane;
                    std::vector<long> b_fp = { 1, 1, 5 };
                    std::vector<long> b_lp = { width, height, 5 };
                    image.read(bPlane, b_fp, b_lp, stride);

                    auto rgbImg = std::make_shared<RGBImage>(width, height);
                    populateMetadata(pInfile.get(), rgbImg->metadata());
                    auto bufR = rgbImg->r()->buffer();
                    auto bufG = rgbImg->g()->buffer();
                    auto bufB = rgbImg->b()->buffer();

                    float scaleR = determineScaleFactor(image.bitpix(), rPlane);
                    float scaleG = determineScaleFactor(image.bitpix(), gPlane);
                    float scaleB = determineScaleFactor(image.bitpix(), bPlane);

                    for (int y = 0; y < height; ++y) {
                        for (int x = 0; x < width; ++x) {
                            long idx = y * width + x;
                            bufR->setPixel(x, y, rPlane[idx] / scaleR);
                            bufG->setPixel(x, y, gPlane[idx] / scaleG);
                            bufB->setPixel(x, y, bPlane[idx] / scaleB);
                        }
                    }
                    return rgbImg;
                } else {
                    throw std::runtime_error("Drizzled FITS cube must have depth of 2 or 6");
                }
            } else {
                std::valarray<float> contents;
                image.read(contents);

                if (depth == 3) {
                    // Read as RGB image
                    auto rgbImg = std::make_shared<RGBImage>(width, height);
                    populateMetadata(pInfile.get(), rgbImg->metadata());
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
                
                // Read registration and WCS metadata from primary HDU header if present
                ImageMetadata mainMeta;
                populateMetadata(pInfile.get(), mainMeta);

                for (int i = 0; i < depth; ++i) {
                    std::string idxStr = std::to_string(i + 1);
                    FrameMetadata meta = batch->frameMetadata(i);
                    meta.baseMetadata = mainMeta;

                    try {
                        int reg = 0;
                        image.readKey("R" + idxStr + "REG", reg);
                        if (reg) {
                            meta.registered = true;
                            image.readKey("R" + idxStr + "DX", meta.dx);
                            image.readKey("R" + idxStr + "DY", meta.dy);
                            image.readKey("R" + idxStr + "TH", meta.theta);
                            image.readKey("R" + idxStr + "ST", meta.starCount);
                            image.readKey("R" + idxStr + "FW", meta.fwhm);
                            image.readKey("R" + idxStr + "SN", meta.snr);
                            
                            int sel = 1;
                            try {
                                image.readKey("R" + idxStr + "SL", sel);
                            } catch (...) {}
                            batch->setFrameSelected(i, sel != 0);
                        }
                    } catch (...) {}

                    // Read WCS and base metadata overrides
                    try {
                        int wcs = 0;
                        image.readKey("R" + idxStr + "WCS", wcs);
                        if (wcs) {
                            meta.baseMetadata.wcsSolved = true;
                            image.readKey("R" + idxStr + "WRA", meta.baseMetadata.wcsRaCenter);
                            image.readKey("R" + idxStr + "WDEC", meta.baseMetadata.wcsDecCenter);
                            image.readKey("R" + idxStr + "WSCL", meta.baseMetadata.wcsPixelScale);
                            image.readKey("R" + idxStr + "WROT", meta.baseMetadata.wcsRotation);
                        }
                    } catch (...) {}
                    try { image.readKey("R" + idxStr + "EXP", meta.baseMetadata.exposureTime); } catch (...) {}
                    try { image.readKey("R" + idxStr + "GAIN", meta.baseMetadata.gain); } catch (...) {}
                    try {
                        std::string filt;
                        image.readKey("R" + idxStr + "FILT", filt);
                        meta.baseMetadata.filter = filt;
                    } catch (...) {}

                    batch->setFrameMetadata(i, meta);
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
    
    // Read registration and WCS metadata from each individual file header if present
    for (size_t i = 0; i < filepaths.size(); ++i) {
        try {
            std::unique_ptr<CCfits::FITS> pInfile(new CCfits::FITS(filepaths[i], CCfits::Read, true));
            CCfits::PHDU& image = pInfile->pHDU();
            
            FrameMetadata meta = batch->frameMetadata(i);
            populateMetadata(pInfile.get(), meta.baseMetadata);

            int reg = 0;
            try {
                image.readKey("REG_REG", reg);
            } catch (...) {
                try { image.readKey("REG", reg); } catch (...) {}
            }
            if (reg) {
                meta.registered = true;
                try { image.readKey("REG_DX", meta.dx); } catch (...) {}
                try { image.readKey("REG_DY", meta.dy); } catch (...) {}
                try { image.readKey("REG_TH", meta.theta); } catch (...) {}
                try { image.readKey("REG_ST", meta.starCount); } catch (...) {}
                try { image.readKey("REG_FW", meta.fwhm); } catch (...) {}
                try { image.readKey("REG_SN", meta.snr); } catch (...) {}
                
                int sel = 1;
                try { image.readKey("REG_SEL", sel); } catch (...) {}
                batch->setFrameSelected(i, sel != 0);
            }
            batch->setFrameMetadata(i, meta);
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

        if (std::holds_alternative<GrayscaleImagePtr>(image)) {
            writeMetadata(phdu, std::get<GrayscaleImagePtr>(image)->metadata());
        } else {
            writeMetadata(phdu, std::get<RGBImagePtr>(image)->metadata());
        }

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
        // Write primary batch metadata
        if (count > 0) {
            FrameMetadata meta0 = batch->frameMetadata(0);
            writeMetadata(phdu, meta0.baseMetadata);
        }

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
            phdu.addKey("R" + idxStr + "SL", static_cast<int>(selected), "Is frame selected");

            // Write frame specific metadata
            phdu.addKey("R" + idxStr + "EXP", meta.baseMetadata.exposureTime, "Exposure time");
            phdu.addKey("R" + idxStr + "GAIN", meta.baseMetadata.gain, "Gain");
            phdu.addKey("R" + idxStr + "FILT", meta.baseMetadata.filter, "Filter");
            if (meta.baseMetadata.wcsSolved) {
                phdu.addKey("R" + idxStr + "WCS", 1, "WCS solved");
                phdu.addKey("R" + idxStr + "WRA", meta.baseMetadata.wcsRaCenter, "WCS RA");
                phdu.addKey("R" + idxStr + "WDEC", meta.baseMetadata.wcsDecCenter, "WCS DEC");
                phdu.addKey("R" + idxStr + "WSCL", meta.baseMetadata.wcsPixelScale, "WCS Pixel Scale");
                phdu.addKey("R" + idxStr + "WROT", meta.baseMetadata.wcsRotation, "WCS Rotation");
            }
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

bool FitsIO::writeDrizzleImage(const std::string& filepath, GrayscaleImagePtr data, GrayscaleImagePtr weight) {
    try {
        std::string writePath = "!" + filepath;
        
        if (!data || !weight) return false;
        
        long width = data->width();
        long height = data->height();
        
        if (weight->width() != width || weight->height() != height) {
            return false;
        }

        long numAxes = 3;
        std::vector<long> naxes = { width, height, 2 };

        std::unique_ptr<CCfits::FITS> pOutfile(new CCfits::FITS(writePath, FLOAT_IMG, numAxes, naxes.data()));
        CCfits::PHDU& phdu = pOutfile->pHDU();

        writeMetadata(phdu, data->metadata());
        phdu.addKey("DRIZZLED", 1, "Image contains Drizzle Data and Weight planes");

        long planeSize = width * height;
        std::valarray<float> arrayData(planeSize * 2);

        auto dataBuf = data->buffer();
        auto weightBuf = weight->buffer();
        
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                arrayData[y * width + x] = dataBuf->pixel(x, y);
                arrayData[planeSize + y * width + x] = weightBuf->pixel(x, y);
            }
        }

        phdu.write(1, planeSize * 2, arrayData);
        return true;
    } catch (CCfits::FitsException& ex) {
        std::cerr << "FITS write drizzle error: " << ex.message() << std::endl;
        return false;
    }
}

bool FitsIO::writeDrizzleRGBImage(const std::string& filepath, 
                                  GrayscaleImagePtr rData, GrayscaleImagePtr rWeight,
                                  GrayscaleImagePtr gData, GrayscaleImagePtr gWeight,
                                  GrayscaleImagePtr bData, GrayscaleImagePtr bWeight) {
    try {
        std::string writePath = "!" + filepath;
        
        if (!rData || !rWeight || !gData || !gWeight || !bData || !bWeight) return false;
        
        long width = rData->width();
        long height = rData->height();
        
        if (rWeight->width() != width || rWeight->height() != height ||
            gData->width() != width || gData->height() != height ||
            gWeight->width() != width || gWeight->height() != height ||
            bData->width() != width || bData->height() != height ||
            bWeight->width() != width || bWeight->height() != height) {
            return false;
        }

        long numAxes = 3;
        std::vector<long> naxes = { width, height, 6 };

        std::unique_ptr<CCfits::FITS> pOutfile(new CCfits::FITS(writePath, FLOAT_IMG, numAxes, naxes.data()));
        CCfits::PHDU& phdu = pOutfile->pHDU();

        writeMetadata(phdu, rData->metadata());
        phdu.addKey("DRIZZLED", 1, "Image contains Drizzle Data and Weight planes");

        long planeSize = width * height;
        std::valarray<float> arrayData(planeSize * 6);

        auto rDataBuf = rData->buffer();
        auto rWeightBuf = rWeight->buffer();
        auto gDataBuf = gData->buffer();
        auto gWeightBuf = gWeight->buffer();
        auto bDataBuf = bData->buffer();
        auto bWeightBuf = bWeight->buffer();
        
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                arrayData[y * width + x] = rDataBuf->pixel(x, y);
                arrayData[planeSize + y * width + x] = rWeightBuf->pixel(x, y);
                arrayData[2 * planeSize + y * width + x] = gDataBuf->pixel(x, y);
                arrayData[3 * planeSize + y * width + x] = gWeightBuf->pixel(x, y);
                arrayData[4 * planeSize + y * width + x] = bDataBuf->pixel(x, y);
                arrayData[5 * planeSize + y * width + x] = bWeightBuf->pixel(x, y);
            }
        }

        phdu.write(1, planeSize * 6, arrayData);
        return true;
    } catch (CCfits::FitsException& ex) {
        std::cerr << "FITS write drizzle RGB error: " << ex.message() << std::endl;
        return false;
    }
}

void FitsIO::clearCache() {
    m_readCache.clear();
}

ImageVariant FitsIO::readImagePatch(const std::string& filepath, int xStart, int yStart, int patchW, int patchH, int planeIndex) {
    try {
        std::shared_ptr<CCfits::FITS> pInfile;
        auto it = m_readCache.find(filepath);
        if (it != m_readCache.end()) {
            pInfile = it->second;
        } else {
            pInfile = std::make_shared<CCfits::FITS>(filepath, CCfits::Read, false);
            m_readCache[filepath] = pInfile;
        }
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
                // 3D FITS cube plane/frame patch
                if (planeIndex < 0 || planeIndex >= depth) {
                    throw std::runtime_error("FITS cube plane index out of range");
                }
                std::valarray<float> contents;
                std::vector<long> fp = { xStart + 1, yStart + 1, planeIndex + 1 };
                std::vector<long> lp = { xStart + patchW, yStart + patchH, planeIndex + 1 };
                std::vector<long> stride = { 1, 1, 1 };
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

bool FitsIO::readHeaderInfo(const std::string& filepath, FitsHeaderInfo& info) {
    try {
        std::unique_ptr<CCfits::FITS> pInfile(new CCfits::FITS(filepath, CCfits::Read, false));
        CCfits::PHDU& image = pInfile->pHDU();

        info.width = static_cast<int>(image.axis(0));
        info.height = static_cast<int>(image.axis(1));

        // Read exposure time
        try {
            double exp = 0.0;
            image.readKey("EXPTIME", exp);
            info.exposureTime = exp;
        } catch (...) {
            try {
                double exp = 0.0;
                image.readKey("EXPOSURE", exp);
                info.exposureTime = exp;
            } catch (...) {
                info.exposureTime = 0.0;
            }
        }

        // Read filter
        try {
            std::string filter;
            image.readKey("FILTER", filter);
            info.filter = filter;
        } catch (...) {
            try {
                std::string filter;
                image.readKey("FILTNAM", filter);
                info.filter = filter;
            } catch (...) {
                info.filter = "None";
            }
        }

        // Read image type
        try {
            std::string imgType;
            image.readKey("IMAGETYP", imgType);
            std::transform(imgType.begin(), imgType.end(), imgType.begin(), ::tolower);
            if (imgType.find("bias") != std::string::npos) {
                info.imageType = "Bias";
            } else if (imgType.find("dark") != std::string::npos) {
                info.imageType = "Dark";
            } else if (imgType.find("flat") != std::string::npos) {
                info.imageType = "Flat";
            } else {
                info.imageType = "Light";
            }
        } catch (...) {
            info.imageType = "Light";
        }

        // Read binning
        try {
            int binX = 1;
            image.readKey("XBINNING", binX);
            info.binningX = binX;
        } catch (...) {
            info.binningX = 1;
        }
        try {
            int binY = 1;
            image.readKey("YBINNING", binY);
            info.binningY = binY;
        } catch (...) {
            info.binningY = 1;
        }

        // Read gain
        try {
            double gain = 0.0;
            image.readKey("GAIN", gain);
            info.gain = gain;
        } catch (...) {
            info.gain = 0.0;
        }

        // Read object/target name
        try {
            std::string obj;
            image.readKey("OBJECT", obj);
            info.objectName = obj;
        } catch (...) {
            info.objectName = "";
        }

        return true;
    } catch (...) {
        return false;
    }
}

} // namespace blastro
