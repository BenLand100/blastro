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

#include "ConstellationMatcher.h"
#include <cmath>
#include <algorithm>
#include <map>
#include <set>
#include <QDebug>

namespace blastro {

std::vector<Triangle> ConstellationMatcher::buildTriangles(const std::vector<Star>& stars, int kNearest) {
    std::vector<Triangle> triangles;
    int numStars = stars.size();
    if (numStars < 3) return triangles;

    // For each star, find its k-nearest neighbors
    for (int i = 0; i < numStars; ++i) {
        const Star& p = stars[i];

        // Compute distances to all other stars
        std::vector<std::pair<double, int>> neighbors;
        neighbors.reserve(numStars - 1);
        for (int j = 0; j < numStars; ++j) {
            if (i == j) continue;
            const Star& other = stars[j];
            double dx = p.x - other.x;
            double dy = p.y - other.y;
            double distSq = dx * dx + dy * dy;
            neighbors.push_back({distSq, j});
        }

        // Sort neighbors by distance
        std::sort(neighbors.begin(), neighbors.end());

        // Select up to kNearest neighbors
        int limit = std::min(kNearest, static_cast<int>(neighbors.size()));
        std::vector<Star> localStars;
        localStars.reserve(limit);
        for (int k = 0; k < limit; ++k) {
            localStars.push_back(stars[neighbors[k].second]);
        }

        // Form triangles among neighbors (combinations of 2)
        int numLocal = localStars.size();
        for (int aIdx = 0; aIdx < numLocal; ++aIdx) {
            for (int bIdx = aIdx + 1; bIdx < numLocal; ++bIdx) {
                const Star& a = localStars[aIdx];
                const Star& b = localStars[bIdx];

                // Side lengths
                double d_ab = std::sqrt((a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y));
                double d_bp = std::sqrt((b.x - p.x)*(b.x - p.x) + (b.y - p.y)*(b.y - p.y));
                double d_pa = std::sqrt((p.x - a.x)*(p.x - a.x) + (p.y - a.y)*(p.y - a.y));

                // Canonically associate opposite vertices
                // Side L0 is d_ab, opposite is p
                // Side L1 is d_bp, opposite is a
                // Side L2 is d_pa, opposite is b
                std::vector<std::pair<double, Star>> sides = {
                    {d_ab, p},
                    {d_bp, a},
                    {d_pa, b}
                };

                // Sort sides by distance descending
                std::sort(sides.begin(), sides.end(), [](const auto& lhs, const auto& rhs) {
                    return lhs.first > rhs.first;
                });

                // Skip degenerate triangles or extremely small ones
                if (sides[0].first < 5.0) continue;

                Triangle tri;
                double maxSide = sides[0].first;
                tri.dists[0] = 1.0;                          // longest side normalised to 1
                tri.dists[1] = sides[1].first / maxSide;     // ratio ∈ (0,1]
                tri.dists[2] = sides[2].first / maxSide;     // ratio ∈ (0,1], ≤ dists[1]
                tri.abc[0] = sides[0].second;
                tri.abc[1] = sides[1].second;
                tri.abc[2] = sides[2].second;
                triangles.push_back(tri);
            }
        }
    }

    return triangles;
}

AlignmentResult ConstellationMatcher::match(const std::vector<Star>& refStars,
                                             const std::vector<Star>& targetStars,
                                             int kNearest,
                                             double matchTolerance,
                                             bool useAffine) {
    AlignmentResult result;
    if (refStars.size() < 3 || targetStars.size() < 3) {
        return result;
    }

    // -----------------------------------------------------------------------
    // Helper: run iterative Procrustes + RANSAC on a set of (ref, target)
    // star pairs and populate result if at least 3 inliers are found.
    // -----------------------------------------------------------------------
    struct StarPair { Star ref; Star target; };

    auto ransac = [&](std::vector<StarPair>& pairs) -> AlignmentResult {
        AlignmentResult r;
        if (pairs.size() < 3) return r;

        double dx = 0.0, dy = 0.0, theta = 0.0;
        std::array<double, 6> trans = {1.0, 0.0, 0.0, 0.0, 1.0, 0.0};
        std::vector<bool> inliers(pairs.size(), true);
        int numInliers = pairs.size();
        bool localUseAffine = useAffine;

        for (int iter = 0; iter < 15; ++iter) {
            if (numInliers < 3) break;

            if (localUseAffine) {
                double sum_x = 0, sum_y = 0, sum_xx = 0, sum_xy = 0, sum_yy = 0;
                double sum_rx = 0, sum_ry = 0;
                double sum_x_rx = 0, sum_y_rx = 0;
                double sum_x_ry = 0, sum_y_ry = 0;
                double n = 0;

                for (size_t i = 0; i < pairs.size(); ++i) {
                    if (!inliers[i]) continue;
                    double tx = pairs[i].target.x;
                    double ty = pairs[i].target.y;
                    double rx = pairs[i].ref.x;
                    double ry = pairs[i].ref.y;

                    sum_x += tx;
                    sum_y += ty;
                    sum_xx += tx * tx;
                    sum_yy += ty * ty;
                    sum_xy += tx * ty;
                    sum_rx += rx;
                    sum_ry += ry;
                    sum_x_rx += tx * rx;
                    sum_y_rx += ty * rx;
                    sum_x_ry += tx * ry;
                    sum_y_ry += ty * ry;
                    n += 1.0;
                }

                double M00 = sum_xx, M01 = sum_xy, M02 = sum_x;
                double M10 = sum_xy, M11 = sum_yy, M12 = sum_y;
                double M20 = sum_x,  M21 = sum_y,  M22 = n;

                double det = M00*(M11*M22 - M12*M21) - M01*(M10*M22 - M12*M20) + M02*(M10*M21 - M11*M20);
                if (std::abs(det) < 1e-9) {
                    localUseAffine = false;
                } else {
                    double invDet = 1.0 / det;
                    double I00 = (M11*M22 - M12*M21) * invDet;
                    double I01 = (M02*M21 - M01*M22) * invDet;
                    double I02 = (M01*M12 - M02*M11) * invDet;
                    
                    double I10 = (M12*M20 - M10*M22) * invDet;
                    double I11 = (M00*M22 - M02*M20) * invDet;
                    double I12 = (M02*M10 - M00*M12) * invDet;
                    
                    double I20 = (M10*M21 - M11*M20) * invDet;
                    double I21 = (M01*M20 - M00*M21) * invDet;
                    double I22 = (M00*M11 - M01*M10) * invDet;

                    double a  = I00 * sum_x_rx + I01 * sum_y_rx + I02 * sum_rx;
                    double b  = I10 * sum_x_rx + I11 * sum_y_rx + I12 * sum_rx;
                    double tx = I20 * sum_x_rx + I21 * sum_y_rx + I22 * sum_rx;

                    double c  = I00 * sum_x_ry + I01 * sum_y_ry + I02 * sum_ry;
                    double d  = I10 * sum_x_ry + I11 * sum_y_ry + I12 * sum_ry;
                    double ty = I20 * sum_x_ry + I21 * sum_y_ry + I22 * sum_ry;

                    trans = {a, b, tx, c, d, ty};
                    dx = tx;
                    dy = ty;
                    theta = std::atan2(c - b, a + d);
                }
            }

            if (!localUseAffine) {
                double mrx = 0, mry = 0, mtx = 0, mty = 0;
                for (size_t i = 0; i < pairs.size(); ++i) {
                    if (!inliers[i]) continue;
                    mrx += pairs[i].ref.x;   mry += pairs[i].ref.y;
                    mtx += pairs[i].target.x; mty += pairs[i].target.y;
                }
                mrx /= numInliers; mry /= numInliers;
                mtx /= numInliers; mty /= numInliers;

                double Sxx=0, Sxy=0, Syx=0, Syy=0;
                for (size_t i = 0; i < pairs.size(); ++i) {
                    if (!inliers[i]) continue;
                    double rx = pairs[i].ref.x    - mrx, ry = pairs[i].ref.y    - mry;
                    double tx = pairs[i].target.x - mtx, ty = pairs[i].target.y - mty;
                    Sxx += tx*rx; Sxy += tx*ry; Syx += ty*rx; Syy += ty*ry;
                }
                theta = std::atan2(Sxy - Syx, Sxx + Syy);
                double cosT = std::cos(theta), sinT = std::sin(theta);
                dx = mrx - (mtx*cosT - mty*sinT);
                dy = mry - (mty*cosT + mtx*sinT);
                trans = {cosT, -sinT, dx, sinT, cosT, dy};
            }

            double iterThresh = matchTolerance;
            if (iter >= 3) iterThresh = std::min(iterThresh, 1.5);
            if (iter >= 6) iterThresh = std::min(iterThresh, 0.8);
            
            int nextN = 0;
            std::vector<bool> next(pairs.size(), false);
            for (size_t i = 0; i < pairs.size(); ++i) {
                double tx = pairs[i].target.x, ty = pairs[i].target.y;
                double px = trans[0]*tx + trans[1]*ty + trans[2];
                double py = trans[3]*tx + trans[4]*ty + trans[5];
                double res = std::hypot(pairs[i].ref.x - px,
                                       pairs[i].ref.y - py);
                if (res < iterThresh) { next[i] = true; nextN++; }
            }
            if (next == inliers && iter >= 6) break;
            inliers = next; numInliers = nextN;
        }

        if (numInliers < 3) return r;

        double sse = 0;
        for (size_t i = 0; i < pairs.size(); ++i) {
            if (!inliers[i]) continue;
            double tx = pairs[i].target.x, ty = pairs[i].target.y;
            double px = trans[0]*tx + trans[1]*ty + trans[2];
            double py = trans[3]*tx + trans[4]*ty + trans[5];
            double ex = pairs[i].ref.x - px;
            double ey = pairs[i].ref.y - py;
            sse += ex*ex + ey*ey;
        }
        r.success = true; r.dx = dx; r.dy = dy; r.theta = theta; r.transform = trans;
        r.rmsError = std::sqrt(sse / numInliers); r.matchedStars = numInliers;
        return r;
    };

    // -----------------------------------------------------------------------
    // PASS 1: Vote-based Hough transform for translation (and 180° flip).
    //   For each (ref_i, target_j) pair, vote for the implied translation.
    //   Also vote for the 180°-flipped translation using the estimated image
    //   centre derived from the bounding box of ref+target star positions.
    // -----------------------------------------------------------------------

    // Estimate image centre from star bounding boxes
    double maxX = 0, maxY = 0;
    for (const auto& s : refStars)    { maxX = std::max(maxX, s.x); maxY = std::max(maxY, s.y); }
    for (const auto& s : targetStars) { maxX = std::max(maxX, s.x); maxY = std::max(maxY, s.y); }
    // Add a small margin to get approximate image size
    double estW = maxX + 50.0;  // approximate W-1
    double estH = maxY + 50.0;  // approximate H-1

    const double binW = (matchTolerance > 0.5) ? matchTolerance : 3.0;

    struct VoteKey {
        int bx, by, flipped;
        bool operator<(const VoteKey& o) const {
            if (bx != o.bx) return bx < o.bx;
            if (by != o.by) return by < o.by;
            return flipped < o.flipped;
        }
    };

    std::map<VoteKey, std::vector<std::pair<int,int>>> votes;

    for (int i = 0; i < (int)refStars.size(); ++i) {
        const Star& r = refStars[i];
        for (int j = 0; j < (int)targetStars.size(); ++j) {
            const Star& t = targetStars[j];
            // Normal translation vote
            votes[{(int)std::floor((r.x - t.x)/binW),
                   (int)std::floor((r.y - t.y)/binW), 0}].emplace_back(i, j);
            // 180°-flip vote: unflip target first → (estW-t.x, estH-t.y)
            votes[{(int)std::floor((r.x - (estW - t.x))/binW),
                   (int)std::floor((r.y - (estH - t.y))/binW), 1}].emplace_back(i, j);
        }
    }

    int bestCount = 0; VoteKey bestKey{0,0,0};
    for (const auto& [key, pairs] : votes) {
        if ((int)pairs.size() > bestCount) { bestCount = pairs.size(); bestKey = key; }
    }

    if (bestCount >= 3) {
        // Collect pairs from winning bin + adjacent bins
        std::vector<StarPair> cands;
        std::set<std::pair<int,int>> used;
        for (int dbx = -1; dbx <= 1; ++dbx) {
            for (int dby = -1; dby <= 1; ++dby) {
                VoteKey k{bestKey.bx+dbx, bestKey.by+dby, bestKey.flipped};
                auto it = votes.find(k);
                if (it == votes.end()) continue;
                for (auto [i, j] : it->second) {
                    if (used.count({i,j})) continue;
                    used.insert({i,j});
                    Star ts = targetStars[j];
                    if (bestKey.flipped) {
                        ts.x = estW - ts.x;
                        ts.y = estH - ts.y;
                    }
                    cands.push_back({refStars[i], ts});
                }
            }
        }
        auto r = ransac(cands);
        if (r.success) {
            // For a flip, the reported theta should be ≈ π
            if (bestKey.flipped) {
                r.theta = M_PI + r.theta;   // fold back the π offset
                r.dx = r.dx + estW;
                r.dy = r.dy + estH;
                // Correct transform matrix for flip
                double new_tx = r.transform[0] * estW + r.transform[1] * estH + r.transform[2];
                double new_ty = r.transform[3] * estW + r.transform[4] * estH + r.transform[5];
                r.transform[0] = -r.transform[0];
                r.transform[1] = -r.transform[1];
                r.transform[2] = new_tx;
                r.transform[3] = -r.transform[3];
                r.transform[4] = -r.transform[4];
                r.transform[5] = new_ty;
            }
            return r;
        }
    }

    // -----------------------------------------------------------------------
    // PASS 2: Triangle-ratio matching for arbitrary rotations (fallback).
    //   Normalise side lengths by longest side to get shape descriptors.
    // -----------------------------------------------------------------------
    std::vector<Triangle> refTris    = buildTriangles(refStars,    kNearest);
    std::vector<Triangle> targetTris = buildTriangles(targetStars, kNearest);

    if (refTris.empty() || targetTris.empty()) return result;

    double tolSq = 0.01 * 0.01;   // 0.01 ratio tolerance for normalised side ratios
    std::map<int, std::vector<int>> refToTarget, targetToRef;
    for (size_t i = 0; i < refTris.size(); ++i) {
        for (size_t j = 0; j < targetTris.size(); ++j) {
            double d1 = refTris[i].dists[1] - targetTris[j].dists[1];
            double d2 = refTris[i].dists[2] - targetTris[j].dists[2];
            if (d1*d1 + d2*d2 < tolSq) {
                refToTarget[i].push_back(j);
                targetToRef[j].push_back(i);
            }
        }
    }

    std::vector<StarPair> cands;
    std::set<std::pair<int,int>> usedPairs;
    auto addUnique = [&](const Star& r, const Star& t) {
        for (auto& p : cands) {
            if (std::hypot(p.ref.x - r.x, p.ref.y - r.y) < 0.1 &&
                std::hypot(p.target.x - t.x, p.target.y - t.y) < 0.1) return;
        }
        cands.push_back({r, t});
    };

    for (const auto& [rIdx, tList] : refToTarget) {
        if (tList.size() != 1) continue;
        int tIdx = tList[0];
        if (targetToRef[tIdx].size() != 1) continue;
        const auto& rt = refTris[rIdx];
        const auto& tt = targetTris[tIdx];
        for (int c = 0; c < 3; ++c) addUnique(rt.abc[c], tt.abc[c]);
    }

    return ransac(cands);
}

} // namespace blastro
