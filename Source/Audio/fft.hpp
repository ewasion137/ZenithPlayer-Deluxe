#pragma once

#include <algorithm>
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

inline std::vector<float> calculateFftMagnitudes(const float* monoSamples, size_t sampleCount, size_t outBins = 48) {
    if (sampleCount < 256) return std::vector<float>(outBins, 0.0f);

    std::vector<std::complex<float>> data(256);
    for (size_t i = 0; i < 256; ++i) {
        // Hann window
        const float win = 0.5f * (1.0f - std::cos(2.0f * static_cast<float>(M_PI) * i / 255.0f));
        data[i] = std::complex<float>(monoSamples[i] * win, 0.0f);
    }

    cooleyTukeyFft(data);

    std::vector<float> magnitudes(outBins, 0.0f);

    // Логарифмическое распределение полос от ~40 Гц до ~20 кГц (пропуская нулевой бин DC)
    for (size_t i = 0; i < outBins; ++i) {
        const float t0 = static_cast<float>(i) / static_cast<float>(outBins);
        const float t1 = static_cast<float>(i + 1) / static_cast<float>(outBins);

        const size_t startBin = 1 + static_cast<size_t>(std::pow(t0, 1.9f) * 126.0f);
        const size_t endBin = std::max(startBin + 1, 1 + static_cast<size_t>(std::pow(t1, 1.9f) * 126.0f));

        float peakVal = 0.0f;
        for (size_t k = startBin; k < endBin && k < 128; ++k) {
            const float mag = std::abs(data[k]) / 128.0f;
            if (mag > peakVal) peakVal = mag;
        }

        // Акустическая компенсация высоких частот (Treble compensation)
        const float trebleBoost = 1.0f + (static_cast<float>(i) / static_cast<float>(outBins)) * 3.5f;
        const float boosted = peakVal * trebleBoost;

        // Логарифмический перевод в децибелы: диапазон от -55 dB до 0 dB
        const float db = 20.0f * std::log10(std::max(1e-4f, boosted));
        const float normalized = std::clamp((db + 55.0f) / 55.0f, 0.0f, 1.0f);

        magnitudes[i] = normalized;
    }

    return magnitudes;
}

} // namespace Audio