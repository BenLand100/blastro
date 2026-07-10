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

#include "PlatesolveAlgorithm.h"
#include "core/GrayscaleImage.h"
#include "core/RGBImage.h"
#include "core/ImageBatch.h"
#include "core/TempDirectory.h"
#include "core/Preferences.h"
#include "core/Logger.h"
#include "io/FitsIO.h"
#include <QProcess>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <CCfits/CCfits>
#include <cmath>
#include <stdexcept>

namespace blastro {

static bool readWcsFile(const std::string& wcsPath, ImageMetadata& metadata) {
    try {
        std::unique_ptr<CCfits::FITS> pInfile(new CCfits::FITS(wcsPath, CCfits::Read, false));
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
                metadata.wcsRotation = std::atan2(cd12, cd11) * 180.0 / 3.14159265358979323846;
            } else {
                double cdelt1 = 0.0;
                if (getDouble("CDELT1", cdelt1)) {
                    metadata.wcsPixelScale = std::abs(cdelt1) * 3600.0;
                }
                getDouble("CROTA2", metadata.wcsRotation);
            }
            return true;
        }
    } catch (...) {}
    return false;
}

void PlatesolveAlgorithm::execute(WorkspaceRegistry& workspace, 
                                 const std::map<std::string, std::string>& config, 
                                 ProgressCallback progress) {
    std::string inputName = config.at("input_name");
    std::string solver = config.count("solver") ? config.at("solver") : "astap";
    
    double raHint = config.count("ra_hint") ? std::stod(config.at("ra_hint")) : -1.0;
    double decHint = config.count("dec_hint") ? std::stod(config.at("dec_hint")) : -99.0;
    double focalLength = config.count("focal_length") ? std::stod(config.at("focal_length")) : 0.0;
    double pixelSize = config.count("pixel_size") ? std::stod(config.at("pixel_size")) : 0.0;

    WorkspaceElement elem = workspace.getElement(inputName);

    std::string binaryPath;
    if (solver == "astap") {
        binaryPath = Preferences::instance().getAstapBinaryPath();
    } else {
        binaryPath = Preferences::instance().getSolveFieldBinaryPath();
    }

    if (binaryPath.empty() || !QFile::exists(QString::fromStdString(binaryPath))) {
        throw std::runtime_error("Platesolver binary not found at path: " + binaryPath);
    }

    auto solveFile = [&](const std::string& fitsPath, ImageMetadata& metadata) -> bool {
        QFileInfo fi(QString::fromStdString(fitsPath));
        QString tempDir = fi.absolutePath();
        QString baseName = fi.completeBaseName();
        QString wcsPath = QDir(tempDir).filePath(baseName + ".wcs");

        QFile::remove(wcsPath);

        QStringList args;
        if (solver == "astap") {
            args << "-f" << QString::fromStdString(fitsPath);
            if (raHint >= 0.0 && decHint >= -90.0) {
                args << "-r" << QString::number(raHint) << "-d" << QString::number(decHint);
            }
            if (focalLength > 0.0 && pixelSize > 0.0) {
                FitsHeaderInfo headerInfo;
                if (FitsIO::readHeaderInfo(fitsPath, headerInfo)) {
                    double fov = (headerInfo.width * pixelSize / 1000.0) / focalLength * 57.2958;
                    args << "-fov" << QString::number(fov);
                }
            }
            args << "-z" << "0";
        } else {
            args << QString::fromStdString(fitsPath)
                 << "--overwrite"
                 << "--no-plots"
                 << "--dir" << tempDir
                 << "--cpulimit" << "30";
            if (raHint >= 0.0 && decHint >= -90.0) {
                args << "--ra" << QString::number(raHint)
                     << "--dec" << QString::number(decHint)
                     << "--radius" << "5.0";
            }
            if (focalLength > 0.0 && pixelSize > 0.0) {
                double scale = (pixelSize / focalLength) * 206.265;
                args << "--scale-units" << "arcsecperpix"
                     << "--scale-low" << QString::number(scale * 0.9)
                     << "--scale-high" << QString::number(scale * 1.1);
            }
        }

        QProcess proc;
        proc.start(QString::fromStdString(binaryPath), args);
        if (!proc.waitForFinished(45000)) {
            proc.kill();
            Logger::error("Platesolve", "Platesolver timed out (45s limit)");
            return false;
        }

        if (proc.exitCode() != 0 && solver != "astap") {
            Logger::error("Platesolve", QString("Platesolver exited with code %1").arg(proc.exitCode()));
            return false;
        }

        if (QFile::exists(wcsPath)) {
            if (readWcsFile(wcsPath.toStdString(), metadata)) {
                QFile::remove(wcsPath);
                return true;
            }
        }
        return false;
    };

    if (std::holds_alternative<GrayscaleImagePtr>(elem)) {
        auto img = std::get<GrayscaleImagePtr>(elem);
        std::string tempDir = TempDirectory::createTempDir("platesolve");
        std::string tempFits = tempDir + "/target.fits";
        FitsIO writer;
        if (writer.writeImage(tempFits, img)) {
            ImageMetadata meta = img->metadata();
            if (solveFile(tempFits, meta)) {
                img->setMetadata(meta);
                Logger::success("Platesolve", "Successfully solved grayscale image!");
            } else {
                throw std::runtime_error("Platesolver failed to solve image.");
            }
        }
    } else if (std::holds_alternative<RGBImagePtr>(elem)) {
        auto img = std::get<RGBImagePtr>(elem);
        std::string tempDir = TempDirectory::createTempDir("platesolve");
        std::string tempFits = tempDir + "/target.fits";
        FitsIO writer;
        if (writer.writeImage(tempFits, img)) {
            ImageMetadata meta = img->metadata();
            if (solveFile(tempFits, meta)) {
                img->setMetadata(meta);
                Logger::success("Platesolve", "Successfully solved RGB image!");
            } else {
                throw std::runtime_error("Platesolver failed to solve image.");
            }
        }
    } else if (std::holds_alternative<ImageBatchPtr>(elem)) {
        auto batch = std::get<ImageBatchPtr>(elem);
        int count = batch->count();
        int solvedCount = 0;
        for (int i = 0; i < count; ++i) {
            if (batch->isFrameSelected(i)) {
                std::string path = batch->frameFilepath(i);
                bool isTemp = false;
                if (path.empty()) {
                    std::string tempDir = TempDirectory::createTempDir("platesolve");
                    path = tempDir + "/target_frame.fits";
                    FitsIO writer;
                    writer.writeImage(path, batch->getImage(i));
                    isTemp = true;
                }
                FrameMetadata meta = batch->frameMetadata(i);
                if (solveFile(path, meta.baseMetadata)) {
                    batch->setFrameMetadata(i, meta);
                    solvedCount++;
                }
                if (isTemp) {
                    QFile::remove(QString::fromStdString(path));
                }
            }
        }
        Logger::success("Platesolve", QString("Solved %1 / %2 frames in batch!").arg(solvedCount).arg(count));
    }
}

} // namespace blastro
