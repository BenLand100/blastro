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

#include "ui/ProjectUISerializer.h"
#include "core/ProjectDataSerializer.h"
#include "core/Preferences.h"
#include "ui/MainWindow.h"
#include "ui/WorkspaceImageWindow.h"
#include "ui/StretchingDialog.h"
#include "ui/BackgroundExtractionDialog.h"
#include "ui/StackingDialog.h"
#include "ui/RegisterDialog.h"
#include "ui/StarFindingDialog.h"
#include "ui/AlignDialog.h"
#include "ui/DebayerDialog.h"
#include "ui/CalibrationDialog.h"
#include "ui/PixelMathDialog.h"
#include "ui/PreprocessingWizardDialog.h"
#include "ui/PlatesolveDialog.h"
#include "ui/LogWindow.h"

#include <QMdiSubWindow>
#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDir>
#include <QDateTime>
#include <QDebug>
#include <QFileInfo>

namespace blastro {

// ─── dialog serialization ────────────────────────────────────────────────────

static QJsonObject serializeDialogs(const UIDialogSet& dialogs) {
    QJsonObject obj;
    if (dialogs.stretching)  obj["stretching"]            = dialogs.stretching->serializeState();
    if (dialogs.bge)         obj["background_extraction"] = dialogs.bge->serializeState();
    if (dialogs.stacking)    obj["stacking"]              = dialogs.stacking->serializeState();
    if (dialogs.registerDlg) obj["register"]              = dialogs.registerDlg->serializeState();
    if (dialogs.starFinding) obj["star_finding"]          = dialogs.starFinding->serializeState();
    if (dialogs.align)       obj["align"]                 = dialogs.align->serializeState();
    if (dialogs.debayer)     obj["debayer"]               = dialogs.debayer->serializeState();
    if (dialogs.calibration) obj["calibration"]           = dialogs.calibration->serializeState();
    if (dialogs.pixelMath)   obj["pixel_math"]            = dialogs.pixelMath->serializeState();
    if (dialogs.ppw)         obj["ppw_control"]           = dialogs.ppw->serializeControlState();
    if (dialogs.platesolve)  obj["platesolve"]            = dialogs.platesolve->serializeState();
    return obj;
}

static void restoreDialogs(const QJsonObject& obj, const UIDialogSet& dialogs) {
    if (dialogs.stretching  && obj.contains("stretching"))
        dialogs.stretching->restoreState(obj["stretching"].toObject());
    if (dialogs.bge         && obj.contains("background_extraction"))
        dialogs.bge->restoreState(obj["background_extraction"].toObject());
    if (dialogs.stacking    && obj.contains("stacking"))
        dialogs.stacking->restoreState(obj["stacking"].toObject());
    if (dialogs.registerDlg && obj.contains("register"))
        dialogs.registerDlg->restoreState(obj["register"].toObject());
    if (dialogs.starFinding && obj.contains("star_finding"))
        dialogs.starFinding->restoreState(obj["star_finding"].toObject());
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

static QList<ToolWindowDef> toolWindowDefs(const UIDialogSet& dialogs) {
    return {
        { "StretchingDialog",          dialogs.stretching },
        { "BackgroundExtractionDialog",dialogs.bge },
        { "StackingDialog",            dialogs.stacking },
        { "RegisterDialog",            dialogs.registerDlg },
        { "StarFindingDialog",         dialogs.starFinding },
        { "AlignDialog",               dialogs.align },
        { "DebayerDialog",             dialogs.debayer },
        { "CalibrationDialog",         dialogs.calibration },
        { "PixelMathDialog",           dialogs.pixelMath },
        { "PreprocessingWizardDialog", dialogs.ppw },
        { "PlatesolveDialog",          dialogs.platesolve },
        { "LogWindow",                 dialogs.logWindow }
    };
}

static QJsonArray serializeMdiToolWindows(QMdiArea* mdi, const UIDialogSet& dialogs) {
    QJsonArray arr;
    if (!mdi) return arr;
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

static void restoreMdiToolWindows(const QJsonArray& arr,
                                   QMdiArea* mdi,
                                   const UIDialogSet& dialogs) {
    if (!mdi) return;
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

// ─── saveProject ─────────────────────────────────────────────────────────────

bool ProjectUISerializer::saveProject(const QString& projectDir,
                                       WorkspaceRegistry& workspace,
                                       QMdiArea* mdi,
                                       const UIDialogSet& dialogs,
                                       MainWindow* mainWin) {
    QDir dir(projectDir);
    if (!dir.exists() && !dir.mkpath(".")) {
        qWarning() << "[ProjectUISerializer] Cannot create project directory:" << projectDir;
        return false;
    }

    QJsonObject root;
    root["version"] = 1;
    root["saved_at"] = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);
    root["process_folder"] = QString::fromStdString(Preferences::instance().getProcessFolderName());

    // Workspace elements
    root["workspace"] = ProjectDataSerializer::serializeWorkspace(projectDir, workspace);

    // MDI image windows
    QJsonArray mdiWindows;
    if (mdi) {
        for (auto* sub : mdi->subWindowList(QMdiArea::StackingOrder)) {
            if (!sub->widget()) continue;
            auto* win = qobject_cast<WorkspaceImageWindow*>(sub->widget());
            if (win && !win->name().isEmpty()) {
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
        }
    }
    root["mdi_windows"] = mdiWindows;

    // Tool windows & dialogs
    root["tool_windows"] = serializeMdiToolWindows(mdi, dialogs);
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
        qWarning() << "[ProjectUISerializer] Cannot write project.json:" << jsonPath;
        return false;
    }
    f.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
    f.close();
    qDebug() << "[ProjectUISerializer] Project saved to" << jsonPath;
    return true;
}

// ─── loadProject ─────────────────────────────────────────────────────────────

bool ProjectUISerializer::loadProject(const QString& projectDir,
                                       WorkspaceRegistry& workspace,
                                       QMdiArea* mdi,
                                       MainWindow* mainWin,
                                       const UIDialogSet& dialogs) {
    QString jsonPath = QDir(projectDir).filePath("project.json");
    QFile f(jsonPath);
    if (!f.open(QIODevice::ReadOnly)) {
        qWarning() << "[ProjectUISerializer] Cannot open project.json:" << jsonPath;
        return false;
    }
    QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
    f.close();
    if (doc.isNull()) {
        qWarning() << "[ProjectUISerializer] Malformed project.json";
        return false;
    }

    QJsonObject root = doc.object();

    // 1. Restore workspace data elements (load batch paths & image files via MainWindow)
    if (root.contains("workspace") && mainWin) {
        QJsonArray elements = root["workspace"].toObject()["elements"].toArray();
        for (const auto& val : elements) {
            QJsonObject elem = val.toObject();
            QString name = elem["name"].toString();
            QString type = elem["type"].toString();
            QString note = elem["note"].toString();
            if (note == "not_persisted") continue;

            if (type == "GrayscaleImage" || type == "RGBImage") {
                QString relPath = elem["path"].toString();
                QString absPath = (elem["path_type"].toString() == "relative")
                                      ? QDir(projectDir).absoluteFilePath(relPath)
                                      : relPath;
                if (!absPath.isEmpty()) {
                    mainWin->loadImageDirectly(absPath, name);
                }
            } else if (type == "ImageBatch") {
                QStringList paths;
                QJsonArray frames = elem["frames"].toArray();
                for (const auto& fval : frames) {
                    QJsonObject frame = fval.toObject();
                    QString relPath = frame["path"].toString();
                    QString absPath = (frame["path_type"].toString() == "relative")
                                          ? QDir(projectDir).absoluteFilePath(relPath)
                                          : relPath;
                    if (!absPath.isEmpty()) paths << absPath;
                }
                if (!paths.isEmpty()) {
                    mainWin->loadBatchPaths(paths, name);
                }
            }
        }
    }

    // Process queued window creation events so windows exist before setting geometry
    QCoreApplication::processEvents();

    // 2. Restore MDI image window geometry and view states
    if (root.contains("mdi_windows") && mdi) {
        QMap<QString, QMdiSubWindow*> subMap;
        for (auto* sub : mdi->subWindowList()) {
            if (!sub->widget()) continue;
            auto* win = qobject_cast<WorkspaceImageWindow*>(sub->widget());
            if (win && !win->name().isEmpty()) {
                subMap[win->name()] = sub;
            }
        }
        QMdiSubWindow* activeSub = nullptr;
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
                if (sw["minimized"].toBool()) {
                    sub->showMinimized();
                } else if (sw["maximized"].toBool()) {
                    sub->showMaximized();
                    activeSub = sub;
                } else {
                    sub->show();
                    sub->raise();
                    activeSub = sub;
                }
            }
            if (win && entry.contains("window_state")) {
                win->restoreWindowState(entry["window_state"].toObject());
            }
        }
        if (activeSub) {
            mdi->setActiveSubWindow(activeSub);
        }
    }

    // 3. Tool windows
    if (root.contains("tool_windows")) {
        restoreMdiToolWindows(root["tool_windows"].toArray(), mdi, dialogs);
    }

    // 4. Algorithm dialog states
    if (root.contains("algorithm_dialogs")) {
        restoreDialogs(root["algorithm_dialogs"].toObject(), dialogs);
    }

    // 5. Main window geometry
    if (mainWin && root.contains("main_window")) {
        QJsonObject geom = root["main_window"].toObject();
        if (geom["maximized"].toBool()) {
            mainWin->showMaximized();
        } else {
            mainWin->move(geom["x"].toInt(), geom["y"].toInt());
            mainWin->resize(geom["width"].toInt(), geom["height"].toInt());
        }
    }

    qDebug() << "[ProjectUISerializer] Project loaded from" << jsonPath;
    return true;
}

// ─── saveSession / loadSession ───────────────────────────────────────────────

bool ProjectUISerializer::saveSession(const QString& sessionPath,
                                       QMdiArea* mdi,
                                       const UIDialogSet& dialogs,
                                       MainWindow* mainWin) {
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

    QFileInfo fi(sessionPath);
    fi.dir().mkpath(".");

    QFile f(sessionPath);
    if (!f.open(QIODevice::WriteOnly)) {
        qWarning() << "[ProjectUISerializer] Cannot write session file:" << sessionPath;
        return false;
    }
    f.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
    f.close();
    qDebug() << "[ProjectUISerializer] Session saved to" << sessionPath;
    return true;
}

bool ProjectUISerializer::loadSession(const QString& sessionPath,
                                       QMdiArea* mdi,
                                       const UIDialogSet& dialogs,
                                       MainWindow* mainWin) {
    QFile f(sessionPath);
    if (!f.exists()) return false;
    if (!f.open(QIODevice::ReadOnly)) {
        qWarning() << "[ProjectUISerializer] Cannot open session file:" << sessionPath;
        return false;
    }
    QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
    f.close();
    if (doc.isNull()) {
        qWarning() << "[ProjectUISerializer] Malformed session file";
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
    qDebug() << "[ProjectUISerializer] Session loaded from" << sessionPath;
    return true;
}

} // namespace blastro
