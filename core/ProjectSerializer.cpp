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

#include "core/ProjectSerializer.h"
#include "core/Preferences.h"
#include "core/GrayscaleImage.h"
#include "core/RGBImage.h"
#include "core/ImageBatch.h"
#include "ui/StretchingDialog.h"
#include "ui/BackgroundExtractionDialog.h"
#include "ui/StackingDialog.h"
#include "ui/RegisterDialog.h"
#include "ui/AlignDialog.h"
#include "ui/DebayerDialog.h"
#include "ui/CalibrationDialog.h"
#include "ui/PixelMathDialog.h"
#include "ui/PreprocessingWizardDialog.h"
#include "ui/PlatesolveDialog.h"
#include "ui/WorkspaceImageWindow.h"
#include "ui/LogWindow.h"
#include <QMainWindow>
#include <QMdiSubWindow>
#include <QMdiArea>
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

// ─── helpers ─────────────────────────────────────────────────────────────────

/// Converts an absolute path to a path relative to projectDir, if possible.
/// Returns absolute path with type "absolute" otherwise.
static QJsonObject pathEntry(const QString& absPath, const QString& projectDir) {
    QJsonObject obj;
    QDir projDir(projectDir);
    QString rel = projDir.relativeFilePath(absPath);
    // relativeFilePath returns an absolute path if on different drive / can't be made relative
    if (rel.startsWith("..") || QFileInfo(rel).isAbsolute()) {
        obj["path"] = absPath;
        obj["path_type"] = "absolute";
    } else {
        obj["path"] = rel;
        obj["path_type"] = "relative";
    }
    return obj;
}

/// Resolves a path entry back to an absolute path.
static QString resolvePath(const QJsonObject& entry, const QString& projectDir) {
    QString path = entry["path"].toString();
    QString type = entry["path_type"].toString();
    if (type == "relative") {
        return QDir(projectDir).absoluteFilePath(path);
    }
    return path; // absolute
}

// ─── dialog serialization ─────────────────────────────────────────────────────

QJsonObject ProjectSerializer::serializeDialogs(const DialogSet& dialogs) {
    QJsonObject obj;
    if (dialogs.stretching)  obj["stretching"]            = dialogs.stretching->serializeState();
    if (dialogs.bge)         obj["background_extraction"] = dialogs.bge->serializeState();
    if (dialogs.stacking)    obj["stacking"]              = dialogs.stacking->serializeState();
    if (dialogs.registerDlg) obj["register"]              = dialogs.registerDlg->serializeState();
    if (dialogs.align)       obj["align"]                 = dialogs.align->serializeState();
    if (dialogs.debayer)     obj["debayer"]               = dialogs.debayer->serializeState();
    if (dialogs.calibration) obj["calibration"]           = dialogs.calibration->serializeState();
    if (dialogs.pixelMath)   obj["pixel_math"]            = dialogs.pixelMath->serializeState();
    if (dialogs.ppw)         obj["ppw_control"]           = dialogs.ppw->serializeControlState();
    if (dialogs.platesolve)  obj["platesolve"]            = dialogs.platesolve->serializeState();
    return obj;
}

void ProjectSerializer::restoreDialogs(const QJsonObject& obj, const DialogSet& dialogs) {
    if (dialogs.stretching  && obj.contains("stretching"))
        dialogs.stretching->restoreState(obj["stretching"].toObject());
    if (dialogs.bge         && obj.contains("background_extraction"))
        dialogs.bge->restoreState(obj["background_extraction"].toObject());
    if (dialogs.stacking    && obj.contains("stacking"))
        dialogs.stacking->restoreState(obj["stacking"].toObject());
    if (dialogs.registerDlg && obj.contains("register"))
        dialogs.registerDlg->restoreState(obj["register"].toObject());
    if (dialogs.align       && obj.contains("align"))
        dialogs.align->restoreState(obj["align"].toObject());
    if (dialogs.debayer     && obj.contains("debayer"))
        dialogs.debayer->restoreState(obj["debayer"].toObject());
    if (dialogs.calibration && obj.contains("calibration"))
        dialogs.calibration->restoreState(obj["calibration"].toObject());
    if (dialogs.pixelMath   && obj.contains("pixel_math"))
        dialogs.pixelMath->restoreState(obj["pixel_math"].toObject());
    if (dialogs.ppw         && obj.contains("ppw_control"))
        dialogs.ppw->restoreControlState(obj["ppw_control"].toObject());
    if (dialogs.platesolve  && obj.contains("platesolve"))
        dialogs.platesolve->restoreState(obj["platesolve"].toObject());
}

// ─── tool window serialization ────────────────────────────────────────────────

struct ToolWindowDef {
    QString type;
    QWidget* widget;
};

static QList<ToolWindowDef> toolWindowDefs(const DialogSet& dialogs) {
    return {
        { "StretchingDialog",          dialogs.stretching },
        { "BackgroundExtractionDialog",dialogs.bge },
        { "StackingDialog",            dialogs.stacking },
        { "RegisterDialog",            dialogs.registerDlg },
        { "AlignDialog",               dialogs.align },
        { "DebayerDialog",             dialogs.debayer },
        { "CalibrationDialog",         dialogs.calibration },
        { "PixelMathDialog",           dialogs.pixelMath },
        { "PreprocessingWizardDialog", dialogs.ppw },
        { "PlatesolveDialog",          dialogs.platesolve },
        { "LogWindow",                 LogWindow::instance() }
    };
}

QJsonArray ProjectSerializer::serializeMdiToolWindows(QMdiArea* mdi, const DialogSet& dialogs) {
    QJsonArray arr;
    auto defs = toolWindowDefs(dialogs);
    for (const auto& def : defs) {
        if (!def.widget) continue;
        QMdiSubWindow* sub = nullptr;
        for (auto* s : mdi->subWindowList()) {
            if (s->widget() == def.widget) { sub = s; break; }
        }
        QJsonObject entry;
        entry["type"] = def.type;
        bool open = (sub && sub->isVisible());
        entry["open"] = open;
        if (open && sub) {
            QJsonObject sw;
            sw["x"] = sub->x(); sw["y"] = sub->y();
            sw["width"]  = sub->width(); sw["height"] = sub->height();
            sw["minimized"] = sub->isMinimized();
            sw["maximized"] = sub->isMaximized();
            entry["subwindow"] = sw;
        }
        arr.append(entry);
    }
    return arr;
}

void ProjectSerializer::restoreMdiToolWindows(const QJsonArray& arr,
                                               QMdiArea* mdi,
                                               const DialogSet& dialogs) {
    auto defs = toolWindowDefs(dialogs);
    for (const auto& val : arr) {
        QJsonObject entry = val.toObject();
        QString type = entry["type"].toString();
        bool open = entry["open"].toBool();

        QWidget* widget = nullptr;
        for (const auto& def : defs) {
            if (def.type == type) { widget = def.widget; break; }
        }
        if (!widget) continue;

        // Find existing sub-window
        QMdiSubWindow* sub = nullptr;
        for (auto* s : mdi->subWindowList()) {
            if (s->widget() == widget) { sub = s; break; }
        }

        if (open) {
            if (!sub) {
                sub = mdi->addSubWindow(widget);
            }
            if (entry.contains("subwindow")) {
                QJsonObject sw = entry["subwindow"].toObject();
                sub->move(sw["x"].toInt(), sw["y"].toInt());
                sub->resize(sw["width"].toInt(), sw["height"].toInt());
                if (sw["minimized"].toBool()) {
                    sub->showMinimized();
                } else if (sw["maximized"].toBool()) {
                    sub->showMaximized();
                } else {
                    sub->show();
                }
            } else {
                sub->show();
            }
        } else {
            if (sub) sub->hide();
        }
    }
}

// ─── workspace serialization ─────────────────────────────────────────────────

QJsonObject ProjectSerializer::serializeWorkspace(const QString& projectDir,
                                                   const WorkspaceRegistry& workspace) {
    QJsonArray elements;
    for (const auto& name : workspace.elementNames()) {
        WorkspaceElement elem = workspace.getElement(name);
        QJsonObject elemObj;
        elemObj["name"] = QString::fromStdString(name);

        if (std::holds_alternative<GrayscaleImagePtr>(elem)) {
            // Single images don't have stored source paths in WorkspaceRegistry.
            // They will be omitted from project serialization; user must re-open.
            elemObj["type"] = "GrayscaleImage";
            elemObj["note"] = "not_persisted";
        } else if (std::holds_alternative<RGBImagePtr>(elem)) {
            elemObj["type"] = "RGBImage";
            elemObj["note"] = "not_persisted";
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
                    // Serialize key numeric metadata
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

// ─── saveProject ─────────────────────────────────────────────────────────────

bool ProjectSerializer::saveProject(const QString& projectDir,
                                     WorkspaceRegistry& workspace,
                                     QMdiArea* mdi,
                                     const DialogSet& dialogs,
                                     QMainWindow* mainWin) {
    QDir dir(projectDir);
    if (!dir.exists() && !dir.mkpath(".")) {
        qWarning() << "[ProjectSerializer] Cannot create project directory:" << projectDir;
        return false;
    }

    QJsonObject root;
    root["version"] = 1;
    root["saved_at"] = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);
    root["process_folder"] = QString::fromStdString(Preferences::instance().getProcessFolderName());

    // Workspace
    root["workspace"] = serializeWorkspace(projectDir, workspace);

    // MDI image windows
    QJsonArray mdiWindows;
    for (auto* sub : mdi->subWindowList()) {
        auto* win = qobject_cast<WorkspaceImageWindow*>(sub->widget());
        if (!win) continue;
        QJsonObject entry;
        entry["element_name"] = win->name();
        QJsonObject sw;
        sw["x"] = sub->x(); sw["y"] = sub->y();
        sw["width"] = sub->width(); sw["height"] = sub->height();
        sw["minimized"] = sub->isMinimized();
        sw["maximized"] = sub->isMaximized();
        entry["subwindow"] = sw;
        entry["window_state"] = win->serializeWindowState();
        mdiWindows.append(entry);
    }
    root["mdi_windows"] = mdiWindows;

    // Tool windows
    root["tool_windows"] = serializeMdiToolWindows(mdi, dialogs);

    // Algorithm dialogs
    root["algorithm_dialogs"] = serializeDialogs(dialogs);

    // Main window geometry
    if (mainWin) {
        QJsonObject geom;
        geom["x"] = mainWin->x(); geom["y"] = mainWin->y();
        geom["width"] = mainWin->width(); geom["height"] = mainWin->height();
        geom["maximized"] = mainWin->isMaximized();
        root["main_window"] = geom;
    }

    // Write project.json
    QString jsonPath = dir.filePath("project.json");
    QFile f(jsonPath);
    if (!f.open(QIODevice::WriteOnly)) {
        qWarning() << "[ProjectSerializer] Cannot write project.json:" << jsonPath;
        return false;
    }
    f.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
    f.close();
    qDebug() << "[ProjectSerializer] Project saved to" << jsonPath;
    return true;
}

// ─── loadProject ─────────────────────────────────────────────────────────────

bool ProjectSerializer::loadProject(const QString& projectDir,
                                     WorkspaceRegistry& workspace,
                                     QMdiArea* mdi,
                                     QMainWindow* mainWin,
                                     const DialogSet& dialogs) {
    QString jsonPath = QDir(projectDir).filePath("project.json");
    QFile f(jsonPath);
    if (!f.open(QIODevice::ReadOnly)) {
        qWarning() << "[ProjectSerializer] Cannot open project.json:" << jsonPath;
        return false;
    }
    QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
    f.close();
    if (doc.isNull()) {
        qWarning() << "[ProjectSerializer] Malformed project.json";
        return false;
    }

    QJsonObject root = doc.object();

    // Workspace elements
    if (root.contains("workspace")) {
        restoreWorkspace(root["workspace"].toObject(), workspace, mdi, mainWin);
    }

    // Restore MDI image window geometry and view state (after elements are in workspace)
    if (root.contains("mdi_windows")) {
        // Mapping from element name to sub-window
        QMap<QString, QMdiSubWindow*> subMap;
        for (auto* sub : mdi->subWindowList()) {
            auto* win = qobject_cast<WorkspaceImageWindow*>(sub->widget());
            if (win) subMap[win->name()] = sub;
        }
        for (const auto& val : root["mdi_windows"].toArray()) {
            QJsonObject entry = val.toObject();
            QString name = entry["element_name"].toString();
            if (!subMap.contains(name)) continue;
            QMdiSubWindow* sub = subMap[name];
            auto* win = qobject_cast<WorkspaceImageWindow*>(sub->widget());
            if (entry.contains("subwindow")) {
                QJsonObject sw = entry["subwindow"].toObject();
                sub->move(sw["x"].toInt(), sw["y"].toInt());
                sub->resize(sw["width"].toInt(), sw["height"].toInt());
                if (sw["minimized"].toBool()) sub->showMinimized();
                else if (sw["maximized"].toBool()) sub->showMaximized();
                else sub->show();
            }
            if (win && entry.contains("window_state")) {
                win->restoreWindowState(entry["window_state"].toObject());
            }
        }
    }

    // Tool windows
    if (root.contains("tool_windows")) {
        restoreMdiToolWindows(root["tool_windows"].toArray(), mdi, dialogs);
    }

    // Algorithm dialogs
    if (root.contains("algorithm_dialogs")) {
        restoreDialogs(root["algorithm_dialogs"].toObject(), dialogs);
    }

    // Main window geometry
    if (mainWin && root.contains("main_window")) {
        QJsonObject geom = root["main_window"].toObject();
        if (geom["maximized"].toBool()) {
            mainWin->showMaximized();
        } else {
            mainWin->move(geom["x"].toInt(), geom["y"].toInt());
            mainWin->resize(geom["width"].toInt(), geom["height"].toInt());
        }
    }

    qDebug() << "[ProjectSerializer] Project loaded from" << jsonPath;
    return true;
}

// ─── restoreWorkspace ────────────────────────────────────────────────────────

void ProjectSerializer::restoreWorkspace(const QJsonObject& obj,
                                          WorkspaceRegistry& /*workspace*/,
                                          QMdiArea* /*mdi*/,
                                          QMainWindow* mainWin) {
    // Workspace restoration triggers MainWindow to reload batch files.
    // MainWindow must expose a slot "loadBatchPaths(QStringList, QString)"
    // (paths, workspace element name). We invoke it via Qt's meta-object system
    // to keep ProjectSerializer decoupled from the concrete MainWindow type.
    QJsonArray elements = obj["elements"].toArray();
    for (const auto& val : elements) {
        QJsonObject elem = val.toObject();
        QString name = elem["name"].toString();
        QString type = elem["type"].toString();
        QString note = elem["note"].toString();
        if (note == "not_persisted") continue;

        if (type == "ImageBatch") {
            QStringList paths;
            QJsonArray frames = elem["frames"].toArray();
            for (const auto& fval : frames) {
                QJsonObject frame = fval.toObject();
                QString absPath = resolvePath(frame, QDir::currentPath());
                if (!absPath.isEmpty()) paths << absPath;
            }
            if (!paths.isEmpty() && mainWin) {
                QMetaObject::invokeMethod(mainWin, "loadBatchPaths",
                    Qt::QueuedConnection,
                    Q_ARG(QStringList, paths),
                    Q_ARG(QString, name));
            }
        }
    }
}

// ─── saveSession ─────────────────────────────────────────────────────────────

bool ProjectSerializer::saveSession(const QString& sessionPath,
                                     QMdiArea* mdi,
                                     const DialogSet& dialogs,
                                     QMainWindow* mainWin) {
    QJsonObject root;
    root["version"] = 1;
    root["saved_at"] = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);

    root["tool_windows"]       = serializeMdiToolWindows(mdi, dialogs);
    root["algorithm_dialogs"]  = serializeDialogs(dialogs);

    if (mainWin) {
        QJsonObject geom;
        geom["x"] = mainWin->x(); geom["y"] = mainWin->y();
        geom["width"] = mainWin->width(); geom["height"] = mainWin->height();
        geom["maximized"] = mainWin->isMaximized();
        root["main_window"] = geom;
    }

    // Ensure parent directories exist
    QFileInfo fi(sessionPath);
    fi.dir().mkpath(".");

    QFile f(sessionPath);
    if (!f.open(QIODevice::WriteOnly)) {
        qWarning() << "[ProjectSerializer] Cannot write session file:" << sessionPath;
        return false;
    }
    f.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
    f.close();
    qDebug() << "[ProjectSerializer] Session saved to" << sessionPath;
    return true;
}

// ─── loadSession ─────────────────────────────────────────────────────────────

bool ProjectSerializer::loadSession(const QString& sessionPath,
                                     QMdiArea* mdi,
                                     const DialogSet& dialogs,
                                     QMainWindow* mainWin) {
    QFile f(sessionPath);
    if (!f.exists()) return false; // silent — no session yet
    if (!f.open(QIODevice::ReadOnly)) {
        qWarning() << "[ProjectSerializer] Cannot open session file:" << sessionPath;
        return false;
    }
    QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
    f.close();
    if (doc.isNull()) {
        qWarning() << "[ProjectSerializer] Malformed session file";
        return false;
    }

    QJsonObject root = doc.object();
    if (root.contains("tool_windows"))
        restoreMdiToolWindows(root["tool_windows"].toArray(), mdi, dialogs);
    if (root.contains("algorithm_dialogs"))
        restoreDialogs(root["algorithm_dialogs"].toObject(), dialogs);

    if (mainWin && root.contains("main_window")) {
        QJsonObject geom = root["main_window"].toObject();
        if (geom["maximized"].toBool()) {
            mainWin->showMaximized();
        } else {
            mainWin->move(geom["x"].toInt(), geom["y"].toInt());
            mainWin->resize(geom["width"].toInt(), geom["height"].toInt());
        }
    }
    qDebug() << "[ProjectSerializer] Session loaded from" << sessionPath;
    return true;
}

// ─── external references ─────────────────────────────────────────────────────

QStringList ProjectSerializer::externalReferences(const QString& projectDir,
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

bool ProjectSerializer::copyReferencesIntoProject(const QString& projectDir,
                                                  WorkspaceRegistry& workspace) {
    Q_UNUSED(projectDir); Q_UNUSED(workspace);
    qDebug() << "[ProjectSerializer] copyReferencesIntoProject: not yet implemented";
    return true;
}

} // namespace blastro
