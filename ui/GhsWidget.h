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
#include "HistogramBaseWidget.h"
#include <array>

namespace blastro {

class GhsWidget : public HistogramBaseWidget {
    Q_OBJECT
public:
    explicit GhsWidget(QWidget* parent = nullptr);
    ~GhsWidget() override = default;

    void setGhsParams(const std::array<double, 6>& sp, const std::array<double, 6>& d);
    void setGhsProtections(const std::array<double, 6>& shadowProtect, const std::array<double, 6>& highlightProtect);

signals:
    void ghsParamsChanged(const std::array<double, 6>& sp, const std::array<double, 6>& d);
    void ghsProtectionsChanged(const std::array<double, 6>& shadowProtect, const std::array<double, 6>& highlightProtect);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

private:
    struct DragTarget {
        enum Type {
            None,
            SymmetryPoint,
            ShadowProtect,
            HighlightProtect
        };
        Type type = None;
        int channel = 0;
    };

    DragTarget getCloseLine(const QPoint& pos) const;

    std::array<double, 6> m_spPoint = {0.5, 0.5, 0.5, 0.5, 0.5, 0.5};
    std::array<double, 6> m_stretchFactor = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    std::array<double, 6> m_shadowProtect = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    std::array<double, 6> m_highlightProtect = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0};

    DragTarget m_dragTarget;
};

} // namespace blastro
