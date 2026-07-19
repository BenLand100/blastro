import re

with open('ui/StretchingDialog.cpp', 'r') as f:
    content = f.read()

apply_stretch_code = """
ImageVariant StretchingDialog::applyCurrentStretch(const ImageVariant& baseImg) {
    if (baseImg.index() == 0 && std::get<0>(baseImg) == nullptr) return baseImg;

    if (std::holds_alternative<GrayscaleImagePtr>(baseImg)) {
        auto gray = std::get<GrayscaleImagePtr>(baseImg);
        auto cloned = cloneGrayscale(gray);

        if (m_mode == StretchMode::Curves) {
            std::vector<double> px, py;
            for (const auto& pt : m_curvePoints[0]) { px.push_back(pt.x()); py.push_back(pt.y()); }
            auto lut = MathUtils::computeCurvesLUT(px, py);
            return StretchingAlgorithm::stretchCurvesGrayscale(cloned, lut);
        } else if (m_mode == StretchMode::GHS) {
            return StretchingAlgorithm::stretchGhsGrayscale(cloned, 0.0, 1.0, m_spPoint[0], m_stretchFactor[0], m_shadowProtect[0], m_highlightProtect[0], 1);
        } else {
            return StretchingAlgorithm::stretchHistogramGrayscale(cloned, m_blackpoint[0], m_whitepoint[0], m_midpoint[0]);
        }
    } else if (std::holds_alternative<RGBImagePtr>(baseImg)) {
        auto rgb = std::get<RGBImagePtr>(baseImg);
        auto cloned = cloneRGB(rgb);

        if (m_mode == StretchMode::Curves) {
            auto isDefaultCurve = [](const std::vector<QPointF>& pts) {
                return pts.size() == 2 && pts[0] == QPointF(0,0) && pts[1] == QPointF(1,1);
            };

            // 1. R, G, B stretches
            if (m_channelsLinked) {
                if (!isDefaultCurve(m_curvePoints[0])) {
                    std::vector<double> px, py;
                    for (const auto& pt : m_curvePoints[0]) { px.push_back(pt.x()); py.push_back(pt.y()); }
                    auto lut = MathUtils::computeCurvesLUT(px, py);
                    std::array<std::vector<float>, 3> luts = {lut, lut, lut};
                    cloned = StretchingAlgorithm::stretchCurvesRGB(cloned, luts, false);
                }
            } else {
                bool rgbNeedsStretch = !isDefaultCurve(m_curvePoints[1]) || !isDefaultCurve(m_curvePoints[2]) || !isDefaultCurve(m_curvePoints[3]);
                if (rgbNeedsStretch) {
                    std::array<std::vector<float>, 3> luts;
                    for (int i = 0; i < 3; ++i) {
                        std::vector<double> px, py;
                        for (const auto& pt : m_curvePoints[i + 1]) { px.push_back(pt.x()); py.push_back(pt.y()); }
                        luts[i] = MathUtils::computeCurvesLUT(px, py);
                    }
                    cloned = StretchingAlgorithm::stretchCurvesRGB(cloned, luts, false);
                }
            }

            // 2. L stretch
            if (!isDefaultCurve(m_curvePoints[4])) {
                std::vector<double> px, py;
                for (const auto& pt : m_curvePoints[4]) { px.push_back(pt.x()); py.push_back(pt.y()); }
                auto lut = MathUtils::computeCurvesLUT(px, py);
                std::array<std::vector<float>, 3> luts = {lut, lut, lut};
                cloned = StretchingAlgorithm::stretchCurvesRGB(cloned, luts, true);
            }

            // 3. S stretch
            if (!isDefaultCurve(m_curvePoints[5])) {
                std::vector<double> px, py;
                for (const auto& pt : m_curvePoints[5]) { px.push_back(pt.x()); py.push_back(pt.y()); }
                auto lut = MathUtils::computeCurvesLUT(px, py);
                cloned = StretchingAlgorithm::stretchCurvesHSL(cloned, lut, true);
            }

        } else if (m_mode == StretchMode::GHS) {
            auto isDefaultGHS = [](double sp, double sh, double hl, double sf) {
                return std::abs(sp - 0.5) < 1e-4 && std::abs(sh - 0.0) < 1e-4 && std::abs(hl - 1.0) < 1e-4 && sf < 1e-4;
            };

            if (m_channelsLinked) {
                if (!isDefaultGHS(m_spPoint[0], m_shadowProtect[0], m_highlightProtect[0], m_stretchFactor[0])) {
                    std::array<double, 3> low = {0.0, 0.0, 0.0};
                    std::array<double, 3> high = {1.0, 1.0, 1.0};
                    cloned = StretchingAlgorithm::stretchGhsRGB(cloned, low, high, m_spPoint, m_stretchFactor, m_shadowProtect, m_highlightProtect, 1, false);
                }
            } else {
                bool rgbNeedsStretch = !isDefaultGHS(m_spPoint[0], m_shadowProtect[0], m_highlightProtect[0], m_stretchFactor[0]) ||
                                       !isDefaultGHS(m_spPoint[1], m_shadowProtect[1], m_highlightProtect[1], m_stretchFactor[1]) ||
                                       !isDefaultGHS(m_spPoint[2], m_shadowProtect[2], m_highlightProtect[2], m_stretchFactor[2]);
                if (rgbNeedsStretch) {
                    std::array<double, 3> low = {0.0, 0.0, 0.0};
                    std::array<double, 3> high = {1.0, 1.0, 1.0};
                    cloned = StretchingAlgorithm::stretchGhsRGB(cloned, low, high, m_spPoint, m_stretchFactor, m_shadowProtect, m_highlightProtect, 1, false);
                }
            }

            if (!isDefaultGHS(m_lSpPoint, m_lShadowProtect, m_lHighlightProtect, m_lStretchFactor)) {
                std::array<double, 3> low = {0.0, 0.0, 0.0};
                std::array<double, 3> high = {1.0, 1.0, 1.0};
                std::array<double, 3> sp = {m_lSpPoint, m_lSpPoint, m_lSpPoint};
                std::array<double, 3> sf = {m_lStretchFactor, m_lStretchFactor, m_lStretchFactor};
                std::array<double, 3> sh = {m_lShadowProtect, m_lShadowProtect, m_lShadowProtect};
                std::array<double, 3> hl = {m_lHighlightProtect, m_lHighlightProtect, m_lHighlightProtect};
                cloned = StretchingAlgorithm::stretchGhsRGB(cloned, low, high, sp, sf, sh, hl, 1, true);
            }

            if (!isDefaultGHS(m_sSpPoint, m_sShadowProtect, m_sHighlightProtect, m_sStretchFactor)) {
                cloned = StretchingAlgorithm::stretchGhsHSL(cloned, 0.0, 1.0, m_sSpPoint, m_sStretchFactor, m_sShadowProtect, m_sHighlightProtect, 1, true);
            }

        } else { // HT
            auto isDefaultHT = [](double b, double w, double m) {
                return std::abs(b - 0.0) < 1e-4 && std::abs(w - 1.0) < 1e-4 && std::abs(m - 0.5) < 1e-4;
            };

            if (m_channelsLinked) {
                if (!isDefaultHT(m_blackpoint[0], m_whitepoint[0], m_midpoint[0])) {
                    cloned = StretchingAlgorithm::stretchHistogramRGB(cloned, m_blackpoint, m_whitepoint, m_midpoint, false);
                }
            } else {
                bool rgbNeedsStretch = !isDefaultHT(m_blackpoint[0], m_whitepoint[0], m_midpoint[0]) ||
                                       !isDefaultHT(m_blackpoint[1], m_whitepoint[1], m_midpoint[1]) ||
                                       !isDefaultHT(m_blackpoint[2], m_whitepoint[2], m_midpoint[2]);
                if (rgbNeedsStretch) {
                    cloned = StretchingAlgorithm::stretchHistogramRGB(cloned, m_blackpoint, m_whitepoint, m_midpoint, false);
                }
            }

            if (!isDefaultHT(m_lBlackpoint, m_lWhitepoint, m_lMidpoint)) {
                std::array<double, 3> bp = {m_lBlackpoint, m_lBlackpoint, m_lBlackpoint};
                std::array<double, 3> wp = {m_lWhitepoint, m_lWhitepoint, m_lWhitepoint};
                std::array<double, 3> mp = {m_lMidpoint, m_lMidpoint, m_lMidpoint};
                cloned = StretchingAlgorithm::stretchHistogramRGB(cloned, bp, wp, mp, true);
            }

            if (!isDefaultHT(m_sBlackpoint, m_sWhitepoint, m_sMidpoint)) {
                cloned = StretchingAlgorithm::stretchHistogramHSL(cloned, m_sBlackpoint, m_sWhitepoint, m_sMidpoint, true);
            }
        }
        return cloned;
    }
    return baseImg;
}

void StretchingDialog::updatePreview() {
    auto win = getActiveImageWindow();
    if (!win) return;

#ifdef _OPENMP
    int threads = m_threads > 0 ? m_threads : Preferences::instance().getThreadCount();
    if (threads > 0) {
        omp_set_num_threads(threads);
    }
#endif

    ImageVariant baseImg = win->originalImage();
    if (baseImg.index() == 0 && std::get<0>(baseImg) == nullptr) return;

    ImageVariant previewResult = applyCurrentStretch(baseImg);
    win->setPreviewImage(previewResult);
}

void StretchingDialog::onApplyClicked() {
    auto win = getActiveImageWindow();
    if (!win) {
        QMessageBox::warning(this, "Apply Error", "No active image found to apply stretching.");
        close();
        return;
    }

    ImageVariant baseImg = win->originalImage();
    ImageVariant finalResult = applyCurrentStretch(baseImg);

    if (finalResult.index() == 0 && std::get<0>(finalResult) == nullptr) return;

    if (m_previewChk->isChecked()) {
        win->commitPreviewImage(finalResult);
    } else {
        win->setPreviewImage(finalResult);
        win->commitPreviewImage(finalResult);
    }

    m_previewChk->blockSignals(true);
    m_previewChk->setChecked(false);
    m_previewChk->blockSignals(false);
}
"""

# Find void StretchingDialog::updatePreview() and slice
start_idx = content.find("void StretchingDialog::updatePreview()")
# Find void StretchingDialog::onPrefsClicked() and slice
end_idx = content.find("void StretchingDialog::onPrefsClicked()")

new_content = content[:start_idx] + apply_stretch_code + "\n" + content[end_idx:]

with open('ui/StretchingDialog.cpp', 'w') as f:
    f.write(new_content)
