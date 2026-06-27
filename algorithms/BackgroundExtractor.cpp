#include "BackgroundExtractor.h"
#include <cmath>
#include <random>
#include <algorithm>
#include <numeric>
#include <stdexcept>
#include <iostream>

namespace blastro {

// Helper: Solve A * x = B using Cholesky decomposition A = L * L^T
static std::vector<double> solveCholesky(const std::vector<double>& A, const std::vector<double>& B, int m) {
    std::vector<double> L(m * m, 0.0);
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j <= i; ++j) {
            double sum = 0.0;
            for (int k = 0; k < j; ++k) {
                sum += L[i * m + k] * L[j * m + k];
            }
            if (i == j) {
                double val = A[i * m + i] - sum;
                if (val <= 0.0) return {}; // Not positive definite
                L[i * m + j] = std::sqrt(val);
            } else {
                L[i * m + j] = (A[i * m + j] - sum) / L[j * m + j];
            }
        }
    }

    // Solve L * y = B (Forward substitution)
    std::vector<double> y(m);
    for (int i = 0; i < m; ++i) {
        double sum = 0.0;
        for (int k = 0; k < i; ++k) {
            sum += L[i * m + k] * y[k];
        }
        y[i] = (B[i] - sum) / L[i * m + i];
    }

    // Solve L^T * x = y (Backward substitution)
    std::vector<double> x(m);
    for (int i = m - 1; i >= 0; --i) {
        double sum = 0.0;
        for (int k = i + 1; k < m; ++k) {
            sum += L[k * m + i] * x[k];
        }
        x[i] = (y[i] - sum) / L[i * m + i];
    }

    // Fallback: If Cholesky fails or becomes unstable, we can return empty or try a simple ridge regularizer.
    return x;
}

struct FitPoint {
    double x_norm;
    double y_norm;
    double z;
};

// Computes the polynomial terms for a given coordinate
static std::vector<double> getPolynomialTerms(double x, double y, int order) {
    std::vector<double> terms;
    // Terms are x^i * y^j where i + j <= order
    for (int i = 0; i <= order; ++i) {
        for (int j = 0; j <= order; ++j) {
            if (i + j <= order) {
                terms.push_back(std::pow(x, i) * std::pow(y, j));
            }
        }
    }
    return terms;
}

GrayscaleImagePtr BackgroundExtractor::extractGrayscale(GrayscaleImagePtr src,
                                                       int order,
                                                       double sigmaCut,
                                                       double sampleFrac,
                                                       double huberDelta,
                                                       bool equalize) {
    if (!src) return nullptr;

    int w = src->width();
    int h = src->height();
    int numPixels = w * h;
    const float* data = src->buffer()->data();

    // 1. Compute median and standard deviation of the channel
    std::vector<float> sortedData(data, data + numPixels);
    std::sort(sortedData.begin(), sortedData.end());
    double median = sortedData[numPixels / 2];

    double sumSqDiff = 0.0;
    for (int i = 0; i < numPixels; ++i) {
        double diff = data[i] - median;
        sumSqDiff += diff * diff;
    }
    double stddev = std::sqrt(sumSqDiff / numPixels);

    // 2. Select background sample points within sigmaCut of median
    double lowBound = median - sigmaCut * stddev;
    double highBound = median + sigmaCut * stddev;

    std::vector<FitPoint> samples;
    std::minstd_rand rng(1337); // Deterministic seed for reproducibility
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            float val = data[y * w + x];
            if (val >= lowBound && val <= highBound) {
                // Probabilistic sampling based on sampleFrac
                if (dist(rng) < sampleFrac) {
                    FitPoint pt;
                    // Normalize coordinates to [-1, 1] to prevent numerical overflow at high orders
                    pt.x_norm = (2.0 * x - w) / w;
                    pt.y_norm = (2.0 * y - h) / h;
                    pt.z = val;
                    samples.push_back(pt);
                }
            }
        }
    }

    if (samples.empty()) {
        throw std::runtime_error("No valid background sample points found");
    }

    // Number of polynomial terms
    int numTerms = 0;
    for (int i = 0; i <= order; ++i) {
        for (int j = 0; j <= order; ++j) {
            if (i + j <= order) numTerms++;
        }
    }

    // 3. Solve robust polynomial fit using Iteratively Reweighted Least Squares (IRLS)
    std::vector<double> coeffs(numTerms, 0.0);
    std::vector<double> weights(samples.size(), 1.0);

    const int maxIRLSIterations = 8;
    for (int iter = 0; iter < maxIRLSIterations; ++iter) {
        // Build Normal Equations: A * c = B
        // A is a symmetric positive definite matrix of size (numTerms x numTerms)
        // B is a vector of size (numTerms)
        std::vector<double> A(numTerms * numTerms, 0.0);
        std::vector<double> B(numTerms, 0.0);

        // Add small ridge regularization to diagonal to guarantee positive definiteness
        for (int i = 0; i < numTerms; ++i) {
            A[i * numTerms + i] = 1e-6; 
        }

        for (size_t p = 0; p < samples.size(); ++p) {
            const auto& pt = samples[p];
            double wVal = weights[p];
            std::vector<double> terms = getPolynomialTerms(pt.x_norm, pt.y_norm, order);

            for (int i = 0; i < numTerms; ++i) {
                for (int j = 0; j < numTerms; ++j) {
                    A[i * numTerms + j] += terms[i] * terms[j] * wVal;
                }
                B[i] += terms[i] * pt.z * wVal;
            }
        }

        // Solve linear system via Cholesky
        auto nextCoeffs = solveCholesky(A, B, numTerms);
        if (nextCoeffs.empty()) {
            break; // Cholesky failed, keep previous coefficients
        }
        coeffs = nextCoeffs;

        // Update residuals and weights using robust Huber loss influence
        for (size_t p = 0; p < samples.size(); ++p) {
            const auto& pt = samples[p];
            std::vector<double> terms = getPolynomialTerms(pt.x_norm, pt.y_norm, order);
            
            double fitVal = 0.0;
            for (int i = 0; i < numTerms; ++i) {
                fitVal += terms[i] * coeffs[i];
            }

            double residual = pt.z - fitVal;
            double absRes = std::abs(residual);

            // Huber Weight Function
            if (absRes <= huberDelta) {
                weights[p] = 1.0;
            } else {
                weights[p] = huberDelta / absRes;
            }
        }
    }

    // 4. Evaluate fitted background polynomial and subtract from source
    auto outBuffer = std::make_shared<ImageBuffer>(w, h);
    float* outData = outBuffer->data();

    // Cache terms mapping for rows/cols in full image
    std::vector<double> x_norms(w);
    for (int x = 0; x < w; ++x) x_norms[x] = (2.0 * x - w) / w;

    #pragma omp parallel for
    for (int y = 0; y < h; ++y) {
        double y_norm = (2.0 * y - h) / h;
        for (int x = 0; x < w; ++x) {
            double x_norm = x_norms[x];
            std::vector<double> terms = getPolynomialTerms(x_norm, y_norm, order);
            
            double bkgVal = 0.0;
            for (int i = 0; i < numTerms; ++i) {
                bkgVal += terms[i] * coeffs[i];
            }

            outData[y * w + x] = data[y * w + x] - static_cast<float>(bkgVal);
        }
    }

    // 5. Shift background-subtracted channel to a safe floor (neutral background peak)
    if (equalize) {
        std::vector<float> outSorted(outData, outData + numPixels);
        std::sort(outSorted.begin(), outSorted.end());
        double outMedian = outSorted[numPixels / 2];

        double outSumSq = 0.0;
        for (int i = 0; i < numPixels; ++i) {
            double diff = outData[i] - outMedian;
            outSumSq += diff * diff;
        }
        double outStd = std::sqrt(outSumSq / numPixels);

        // Safe floor offset
        float offset = -outMedian + sigmaCut * outStd;
        
        #pragma omp parallel for
        for (int i = 0; i < numPixels; ++i) {
            outData[i] += offset;
        }
    }

    return std::make_shared<GrayscaleImage>(outBuffer);
}

RGBImagePtr BackgroundExtractor::extractRGB(RGBImagePtr src,
                                            int order,
                                            double sigmaCut,
                                            double sampleFrac,
                                            double huberDelta,
                                            bool equalize) {
    if (!src) return nullptr;

    // 1. Process each channel independently WITHOUT individual equalization
    auto extR = extractGrayscale(src->r(), order, sigmaCut, sampleFrac, huberDelta, false);
    auto extG = extractGrayscale(src->g(), order, sigmaCut, sampleFrac, huberDelta, false);
    auto extB = extractGrayscale(src->b(), order, sigmaCut, sampleFrac, huberDelta, false);

    // 2. If equalize is true, apply coordinated equalization to neutralize the background
    if (equalize) {
        int w = src->width();
        int h = src->height();
        int numPixels = w * h;

        auto getStats = [numPixels](GrayscaleImagePtr img, double& median, double& stddev) {
            const float* data = img->buffer()->data();
            std::vector<float> sorted(data, data + numPixels);
            std::sort(sorted.begin(), sorted.end());
            median = sorted[numPixels / 2];

            double sumSq = 0.0;
            for (int i = 0; i < numPixels; ++i) {
                double diff = data[i] - median;
                sumSq += diff * diff;
            }
            stddev = std::sqrt(sumSq / numPixels);
        };

        double medR, stdR;
        double medG, stdG;
        double medB, stdB;

        getStats(extR, medR, stdR);
        getStats(extG, medG, stdG);
        getStats(extB, medB, stdB);

        // Common pedestal based on the maximum noise standard deviation of the channels
        double maxStd = std::max({stdR, stdG, stdB});
        float commonPedestal = static_cast<float>(sigmaCut * maxStd);

        // Coordinated offsets to shift all channel medians to the exact same pedestal
        float offsetR = static_cast<float>(-medR + commonPedestal);
        float offsetG = static_cast<float>(-medG + commonPedestal);
        float offsetB = static_cast<float>(-medB + commonPedestal);

        float* dataR = extR->buffer()->data();
        float* dataG = extG->buffer()->data();
        float* dataB = extB->buffer()->data();

        #pragma omp parallel for
        for (int i = 0; i < numPixels; ++i) {
            dataR[i] += offsetR;
            dataG[i] += offsetG;
            dataB[i] += offsetB;
        }
    }

    return std::make_shared<RGBImage>(extR, extG, extB);
}

} // namespace blastro
