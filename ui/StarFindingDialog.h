/*
 * BLastro - Astronomical Image Processing Software
 * Copyright (C) 2026 Benjamin Land
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#pragma once
#include "AlgorithmDialog.h"
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QPushButton>

namespace blastro {

class StarFindingDialog : public AlgorithmDialog {
    Q_OBJECT
public:
    StarFindingDialog(WorkspaceRegistry& workspace, QWidget* parent = nullptr);
    ~StarFindingDialog() override = default;

    std::map<std::string, std::string> getConfig() const override;
    std::string algorithmName() const override { return "StarFinding"; }
    QJsonObject serializeState() const override;
    void restoreState(const QJsonObject& obj) override;
    void refreshWorkspaceElements() override;

private slots:
    void onRunClicked();
    void onPrefsClicked();

private:
    QComboBox* m_targetInputCombo;
    QComboBox* m_methodCombo;
    QDoubleSpinBox* m_snrSpin;
    QDoubleSpinBox* m_minFwhmSpin;

    // Advanced parameters
    int m_maxStars = 500;
    double m_maxEccentricity = 0.9;
    int m_threads = -1;
};

} // namespace blastro
