#include "FitsIO.h"
#include <CCfits/CCfits>
#include <valarray>
#include <stdexcept>
#include <algorithm>
#include <iostream>

namespace blastro {

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
            for (int y = 0; y < height; ++y) {
                for (int x = 0; x < width; ++x) {
                    // FITS pixel storage: row-major where axis(0) varies fastest
                    buffer->setPixel(x, y, contents[y * width + x]);
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

                long planeSize = width * height;
                for (int y = 0; y < height; ++y) {
                    for (int x = 0; x < width; ++x) {
                        long idx = y * width + x;
                        bufR->setPixel(x, y, contents[idx]);
                        bufG->setPixel(x, y, contents[planeSize + idx]);
                        bufB->setPixel(x, y, contents[2 * planeSize + idx]);
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

                auto loader = [filepath, width, height, depth](int index) -> ImageVariant {
                    try {
                        std::unique_ptr<CCfits::FITS> pInfileInner(new CCfits::FITS(filepath, CCfits::Read, true));
                        CCfits::PHDU& imageInner = pInfileInner->pHDU();
                        
                        std::valarray<float> plane;
                        long planeSize = width * height;
                        // Read only the specific plane (index starts at 1 for pixel coordinates in FITS)
                        long firstPixel = index * planeSize + 1;
                        long lastPixel = (index + 1) * planeSize;
                        imageInner.read(plane, firstPixel, lastPixel);

                        auto grayImg = std::make_shared<GrayscaleImage>(width, height);
                        auto buffer = grayImg->buffer();
                        for (int y = 0; y < height; ++y) {
                            for (int x = 0; x < width; ++x) {
                                buffer->setPixel(x, y, plane[y * width + x]);
                            }
                        }
                        return grayImg;
                    } catch (CCfits::FitsException& ex) {
                        throw std::runtime_error(std::string("CCfits Error in batch loader: ") + ex.message());
                    }
                };
                return std::make_shared<ImageBatch>(depth, loader);
            }
        } catch (...) {
            // Fall back to treating it as a single 2D FITS file batch
        }
    }

    // Otherwise, treat as a list of independent FITS files
    auto loader = [filepaths](int index) -> ImageVariant {
        FitsIO reader;
        return reader.readImage(filepaths[index]);
    };
    return std::make_shared<ImageBatch>(filepaths.size(), loader);
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

} // namespace blastro
