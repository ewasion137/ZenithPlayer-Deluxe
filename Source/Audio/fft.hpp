#pragma once

#include <cmath>
#include <complex>
#include <vector>

namespace Audio {

inline void cooleyTukeyFft(std::vector<std::complex<float>>& x) {
    const size_t n = x.size();
    if (n <= 1) return;

    std::vector<std::complex<float>> even(n / 2);
    std::vector<std::complex<float>> odd(n / 2);
    for (size_t i = 0; i < n / 2; ++i) {
        even[i] = x[i * 2];
        odd[i] = x[i * 2 + 1];
    }

    cooleyTukeyFft(even);
    cooleyTukeyFft(odd);

    for (size_t k = 0; k < n / 2; ++k) {
        const float angle = -2.0f * static_cast<float>(M_PI) * static_cast<float>(k) / static_cast<float>(n);
        const std::complex<float> t = std::polar(1.0f, angle) * odd[k];
        x[k] = even[k] + t;
        x[k + n / 2] = even[k] - t;
    }
}

inline std::vector<float> calculateFftMagnitudes(const float* monoSamples, size_t sampleCount, size_t outBins = 64) {
    if (sampleCount < 256) return std::vector<float>(outBins, 0.0f);

    std::vector<std::complex<float>> data(256);
    for (size_t i = 0; i < 256; ++i) {
        // Hann window to reduce spectral leakage
        const float multiplier = 0.5f * (1.0f - std::cos(2.0f * static_cast<float>(M_PI) * i / 255.0f));
        data[i] = std::complex<float>(monoSamples[i] * multiplier, 0.0f);
    }

    cooleyTukeyFft(data);

    std::vector<float> magnitudes(outBins, 0.0f);
    const size_t binsToAverage = (128 / outBins);

    for (size_t i = 0; i < outBins; ++i) {
        float sum = 0.0f;
        for (size_t j = 0; j < binsToAverage; ++j) {
            const size_t idx = i * binsToAverage + j;
            if (idx < 128) {
                sum += std::abs(data[idx]);
            }
        }
        magnitudes[i] = std::clamp((sum / static_cast<float>(binsToAverage)) * 0.08f, 0.0f, 1.0f);
    }

    return magnitudes;
}

} // namespace Audio