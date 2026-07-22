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

#include "core/ProjectDataSerializer.h"
#include "core/Preferences.h"
#include "core/GrayscaleImage.h"
#include "core/RGBImage.h"
#include "core/ImageBatch.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDir>
#include <QDateTime>
#include <QDebug>
#include <QFileInfo>
#include <variant>

namespace blastro {

// ─── path resolution helpers ──────────────────────────────────────────────────

static QJsonObject pathEntry(const QString& absPath, const QString& projectDir) {
    QJsonObject obj;
    QDir projDir(projectDir);
    QString rel = projDir.relativeFilePath(absPath);
    if (rel.startsWith("..") || QFileInfo(rel).isAbsolute()) {
        obj["path"] = absPath;
        obj["path_type"] = "absolute";
    } else {
        obj["path"] = rel;
        obj["path_type"] = "relative";
    }
    return obj;
}

static QString resolvePath(const QJsonObject& entry, const QString& projectDir) {
    QString path = entry["path"].toString();
    QString type = entry["path_type"].toString();
    if (type == "relative") {
        return QDir(projectDir).absoluteFilePath(path);
    }
    return path;
}

// ─── workspace serialization ─────────────────────────────────────────────────

QJsonObject ProjectDataSerializer::serializeWorkspace(const QString& projectDir,
                                                       const WorkspaceRegistry& workspace) {
    QJsonArray elements;
    for (const auto& name : workspace.elementNames()) {
        WorkspaceElement elem = workspace.getElement(name);
        QJsonObject elemObj;
        elemObj["name"] = QString::fromStdString(name);

        if (std::holds_alternative<GrayscaleImagePtr>(elem)) {
            elemObj["type"] = "GrayscaleImage";
            QString relPath = QString("images/%1.fits").arg(QString::fromStdString(name));
            QString absPath = QDir(projectDir).absoluteFilePath(relPath);
            auto pe = pathEntry(absPath, projectDir);
            elemObj["path"] = pe["path"];
            elemObj["path_type"] = pe["path_type"];
        } else if (std::holds_alternative<RGBImagePtr>(elem)) {
            elemObj["type"] = "RGBImage";
            QString relPath = QString("images/%1.fits").arg(QString::fromStdString(name));
            QString absPath = QDir(projectDir).absoluteFilePath(relPath);
            auto pe = pathEntry(absPath, projectDir);
            elemObj["path"] = pe["path"];
            elemObj["path_type"] = pe["path_type"];
        } else if (std::holds_alternative<ImageBatchPtr>(elem)) {
            auto batch = std::get<ImageBatchPtr>(elem);
            elemObj["type"] = "ImageBatch";
            if (batch) {
                QJsonArray frames;
                for (int i = 0; i < batch->count(); ++i) {
                    QJsonObject frame;
                    frame["name"] = QString::fromStdString(batch->frameName(i));
                    auto pe = pathEntry(QString::fromStdString(batch->frameFilepath(i)), projectDir);
                    frame["path"] = pe["path"];
                    frame["path_type"] = pe["path_type"];
                    frame["selected"] = batch->isFrameSelected(i);

                    FrameMetadata meta = batch->frameMetadata(i);
                    QJsonObject metaObj;
                    metaObj["registered"] = meta.registered;
                    metaObj["dx"] = meta.dx;
                    metaObj["dy"] = meta.dy;
                    metaObj["theta"] = meta.theta;
                    QJsonArray transArr;
                    for (int k = 0; k < 6; ++k) {
                        transArr.append(meta.transform[k]);
                    }
                    metaObj["transform"] = transArr;
                    metaObj["star_count"] = meta.starCount;
                    metaObj["fwhm"] = meta.fwhm;
                    metaObj["snr"] = meta.snr;

                    QJsonObject wcsObj;
                    wcsObj["solved"] = meta.baseMetadata.wcsSolved;
                    wcsObj["ra"] = meta.baseMetadata.wcsRaCenter;
                    wcsObj["dec"] = meta.baseMetadata.wcsDecCenter;
                    wcsObj["scale"] = meta.baseMetadata.wcsPixelScale;
                    wcsObj["rotation"] = meta.baseMetadata.wcsRotation;
                    wcsObj["exposure"] = meta.baseMetadata.exposureTime;
                    wcsObj["gain"] = meta.baseMetadata.gain;
                    wcsObj["filter"] = QString::fromStdString(meta.baseMetadata.filter);
                    QJsonObject kwObj;
                    for (const auto& pair : meta.baseMetadata.fitsKeywords) {
                        kwObj[QString::fromStdString(pair.first)] = QString::fromStdString(pair.second);
                    }
                    wcsObj["keywords"] = kwObj;
                    metaObj["wcs"] = wcsObj;

                    frame["metadata"] = metaObj;
                    frames.append(frame);
                }
                elemObj["frames"] = frames;
            }
        }
        elements.append(elemObj);
    }
    QJsonObject obj;
    obj["elements"] = elements;
    return obj;
}

// ─── restoreWorkspaceData ────────────────────────────────────────────────────

void ProjectDataSerializer::restoreWorkspaceData(const QJsonObject& obj,
                                                 const QString& projectDir,
                                                 WorkspaceRegistry& workspace) {
    QJsonArray elements = obj["elements"].toArray();
    for (const auto& val : elements) {
        QJsonObject elem = val.toObject();
        std::string name = elem["name"].toString().toStdString();
        QString type = elem["type"].toString();
        QString note = elem["note"].toString();
        if (note == "not_persisted") continue;

        if (type == "ImageBatch") {
            std::vector<std::string> paths;
            std::vector<std::string> names;
            QJsonArray frames = elem["frames"].toArray();
            for (const auto& fval : frames) {
                QJsonObject frame = fval.toObject();
                QString absPath = resolvePath(frame, projectDir);
                if (!absPath.isEmpty()) {
                    paths.push_back(absPath.toStdString());
                    names.push_back(frame["name"].toString().toStdString());
                }
            }
            if (!paths.empty()) {
                int count = static_cast<int>(paths.size());
                ImageBatchPtr batch = std::make_shared<ImageBatch>(count, [](int) -> ImageVariant { return GrayscaleImagePtr(); }, names, paths);
                for (int i = 0; i < batch->count() && i < frames.size(); ++i) {
                    QJsonObject frame = frames[i].toObject();
                    if (frame.contains("selected")) {
                        batch->setFrameSelected(i, frame["selected"].toBool());
                    }
                    if (frame.contains("metadata")) {
                        QJsonObject metaObj = frame["metadata"].toObject();
                        FrameMetadata meta = batch->frameMetadata(i);
                        meta.registered = metaObj["registered"].toBool();
                        meta.dx = metaObj["dx"].toDouble();
                        meta.dy = metaObj["dy"].toDouble();
                        meta.theta = metaObj["theta"].toDouble();
                        meta.starCount = metaObj["star_count"].toInt();
                        meta.fwhm = metaObj["fwhm"].toDouble();
                        meta.snr = metaObj["snr"].toDouble();
                        if (metaObj.contains("transform")) {
                            QJsonArray transArr = metaObj["transform"].toArray();
                            for (int k = 0; k < 6 && k < transArr.size(); ++k) {
                                meta.transform[k] = transArr[k].toDouble();
                            }
                        }
                        if (metaObj.contains("wcs")) {
                            QJsonObject wcsObj = metaObj["wcs"].toObject();
                            meta.baseMetadata.wcsSolved = wcsObj["solved"].toBool();
                            meta.baseMetadata.wcsRaCenter = wcsObj["ra"].toDouble();
                            meta.baseMetadata.wcsDecCenter = wcsObj["dec"].toDouble();
                            meta.baseMetadata.wcsPixelScale = wcsObj["scale"].toDouble();
                            meta.baseMetadata.wcsRotation = wcsObj["rotation"].toDouble();
                            meta.baseMetadata.exposureTime = wcsObj["exposure"].toDouble();
                            meta.baseMetadata.gain = wcsObj["gain"].toDouble();
                            meta.baseMetadata.filter = wcsObj["filter"].toString().toStdString();
                            if (wcsObj.contains("keywords")) {
                                QJsonObject kwObj = wcsObj["keywords"].toObject();
                                for (auto it = kwObj.begin(); it != kwObj.end(); ++it) {
                                    meta.baseMetadata.fitsKeywords[it.key().toStdString()] = it.value().toString().toStdString();
                                }
                            }
                        }
                        batch->setFrameMetadata(i, meta);
                    }
                }
                workspace.registerElement(name, batch);
            }
        }
    }
}

// ─── saveProjectData ─────────────────────────────────────────────────────────

bool ProjectDataSerializer::saveProjectData(const QString& projectDir,
                                             const WorkspaceRegistry& workspace) {
    QDir dir(projectDir);
    if (!dir.exists() && !dir.mkpath(".")) {
        qWarning() << "[ProjectDataSerializer] Cannot create project directory:" << projectDir;
        return false;
    }

    QJsonObject root;
    root["version"] = 1;
    root["saved_at"] = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);
    root["process_folder"] = QString::fromStdString(Preferences::instance().getProcessFolderName());
    root["workspace"] = serializeWorkspace(projectDir, workspace);

    QString jsonPath = dir.filePath("project.json");
    QFile f(jsonPath);
    if (!f.open(QIODevice::WriteOnly)) {
        qWarning() << "[ProjectDataSerializer] Cannot write project.json:" << jsonPath;
        return false;
    }
    f.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
    f.close();
    qDebug() << "[ProjectDataSerializer] Project data saved to" << jsonPath;
    return true;
}

// ─── loadProjectData ─────────────────────────────────────────────────────────

bool ProjectDataSerializer::loadProjectData(const QString& projectDir,
                                             WorkspaceRegistry& workspace) {
    QString jsonPath = QDir(projectDir).filePath("project.json");
    QFile f(jsonPath);
    if (!f.open(QIODevice::ReadOnly)) {
        qWarning() << "[ProjectDataSerializer] Cannot open project.json:" << jsonPath;
        return false;
    }
    QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
    f.close();
    if (doc.isNull()) {
        qWarning() << "[ProjectDataSerializer] Malformed project.json";
        return false;
    }

    QJsonObject root = doc.object();
    if (root.contains("workspace")) {
        restoreWorkspaceData(root["workspace"].toObject(), projectDir, workspace);
    }
    qDebug() << "[ProjectDataSerializer] Project data loaded from" << jsonPath;
    return true;
}

// ─── external references ─────────────────────────────────────────────────────

QStringList ProjectDataSerializer::externalReferences(const QString& projectDir,
                                                           const WorkspaceRegistry& workspace) {
    QStringList external;
    QDir dir(projectDir);
    for (const auto& name : workspace.elementNames()) {
        WorkspaceElement elem = workspace.getElement(name);
        if (std::holds_alternative<ImageBatchPtr>(elem)) {
            auto batch = std::get<ImageBatchPtr>(elem);
            if (!batch) continue;
            for (int i = 0; i < batch->count(); ++i) {
                QString path = QString::fromStdString(batch->frameFilepath(i));
                if (path.isEmpty()) continue;
                QString rel = dir.relativeFilePath(path);
                if (rel.startsWith("..") || QFileInfo(rel).isAbsolute()) {
                    external << path;
                }
            }
        }
    }
    return external;
}

bool ProjectDataSerializer::copyReferencesIntoProject(const QString& projectDir,
                                                         WorkspaceRegistry& workspace) {
    Q_UNUSED(projectDir); Q_UNUSED(workspace);
    qDebug() << "[ProjectDataSerializer] copyReferencesIntoProject: not yet implemented";
    return true;
}

} // namespace blastro
