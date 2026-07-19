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

class HtWidget : public HistogramBaseWidget {
    Q_OBJECT
public:
    explicit HtWidget(QWidget* parent = nullptr);
    ~HtWidget() override = default;

    void setStretchParams(const std::array<double, 6>& b, const std::array<double, 6>& w, const std::array<double, 6>& m);
    void snapToBlackToMid();

signals:
    void stretchParamsChanged(const std::array<double, 6>& b, const std::array<double, 6>& w, const std::array<double, 6>& m);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

private:
    struct DragTarget {
        enum Type {
            None,
            Black,
            Mid,
            White
        };
        Type type = None;
        int channel = 0;
    };

    bool isChannelActive(int c) const;
    DragTarget getCloseLine(const QPoint& pos) const;

    std::array<double, 6> m_blackpoint = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    std::array<double, 6> m_whitepoint = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
    std::array<double, 6> m_midpoint = {0.5, 0.5, 0.5, 0.5, 0.5, 0.5}; // Relative [0, 1]

    DragTarget m_dragTarget;
};

} // namespace blastro
