#include "QtImageIO.h"
#include <QFileInfo>
#include <QImageReader>
#include <QImageWriter>
#include <stdexcept>

namespace blastro {

bool QtImageIO::supportsExtension(const std::string& ext) const {
    QString qext = QString::fromStdString(ext).toLower();
    if (qext.startsWith(".")) {
        qext = qext.mid(1);
    }
    
    // Check if Qt can read or write this format
    QList<QByteArray> formats = QImageReader::supportedImageFormats();
    for (const auto& fmt : formats) {
        if (QString::fromLatin1(fmt).toLower() == qext) {
            return true;
        }
    }
    return false;
}

ImageVariant QtImageIO::readImage(const std::string& filepath) {
    QImage img(QString::fromStdString(filepath));
    if (img.isNull()) {
        throw std::runtime_error("Failed to load image: " + filepath);
    }

    int width = img.width();
    int height = img.height();

    // Check if the image is grayscale
    if (img.isGrayscale()) {
        auto grayImg = std::make_shared<GrayscaleImage>(width, height);
        auto buffer = grayImg->buffer();
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                QRgb pixel = img.pixel(x, y);
                float val = qGray(pixel) / 255.0f;
                buffer->setPixel(x, y, val);
            }
        }
        return grayImg;
    } else {
        auto rgbImg = std::make_shared<RGBImage>(width, height);
        auto bufR = rgbImg->r()->buffer();
        auto bufG = rgbImg->g()->buffer();
        auto bufB = rgbImg->b()->buffer();

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                QRgb pixel = img.pixel(x, y);
                bufR->setPixel(x, y, qRed(pixel) / 255.0f);
                bufG->setPixel(x, y, qGreen(pixel) / 255.0f);
                bufB->setPixel(x, y, qBlue(pixel) / 255.0f);
            }
        }
        return rgbImg;
    }
}

ImageBatchPtr QtImageIO::readBatch(const std::vector<std::string>& filepaths) {
    std::vector<std::string> names(filepaths.size());
    for (size_t i = 0; i < filepaths.size(); ++i) {
        names[i] = QFileInfo(QString::fromStdString(filepaths[i])).fileName().toStdString();
    }
    
    auto loader = [filepaths](int index) -> ImageVariant {
        QtImageIO reader;
        return reader.readImage(filepaths[index]);
    };
    return std::make_shared<ImageBatch>(filepaths.size(), loader, names, filepaths);
}

bool QtImageIO::writeImage(const std::string& filepath, const ImageVariant& image) {
    int width = 0;
    int height = 0;

    if (std::holds_alternative<GrayscaleImagePtr>(image)) {
        auto img = std::get<GrayscaleImagePtr>(image);
        width = img->width();
        height = img->height();
    } else {
        auto img = std::get<RGBImagePtr>(image);
        width = img->width();
        height = img->height();
    }

    QImage qimg(width, height, QImage::Format_RGB32);

    if (std::holds_alternative<GrayscaleImagePtr>(image)) {
        auto img = std::get<GrayscaleImagePtr>(image);
        auto buffer = img->buffer();
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                float val = buffer->pixel(x, y);
                int grayVal = qBound(0, static_cast<int>(val * 255.0f), 255);
                qimg.setPixel(x, y, qRgb(grayVal, grayVal, grayVal));
            }
        }
    } else {
        auto img = std::get<RGBImagePtr>(image);
        auto bufR = img->r()->buffer();
        auto bufG = img->g()->buffer();
        auto bufB = img->b()->buffer();
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                int rVal = qBound(0, static_cast<int>(bufR->pixel(x, y) * 255.0f), 255);
                int gVal = qBound(0, static_cast<int>(bufG->pixel(x, y) * 255.0f), 255);
                int bVal = qBound(0, static_cast<int>(bufB->pixel(x, y) * 255.0f), 255);
                qimg.setPixel(x, y, qRgb(rVal, gVal, bVal));
            }
        }
    }

    return qimg.save(QString::fromStdString(filepath));
}

} // namespace blastro
