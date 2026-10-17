#include "waveform.hpp"
#include "miniaudio.h"

#include <algorithm>
#include <cmath>

namespace Audio {

std::vector<float> generateWaveformPeaks(const QString& filePath, int totalBars) {
    if (filePath.isEmpty() || totalBars <= 0) {
        return std::vector<float>(totalBars, 0.1f);
    }

    ma_decoder decoder;
    ma_decoder_config config = ma_decoder_config_init(ma_format_f32, 1, 22050);

    ma_result res;
#if defined(_WIN32)
    res = ma_decoder_init_file_w(filePath.toStdWString().c_str(), &config, &decoder);
#else
    res = ma_decoder_init_file(filePath.toUtf8().constData(), &config, &decoder);
#endif

    if (res != MA_SUCCESS) {
        return std::vector<float>(totalBars, 0.1f);
    }

    ma_uint64 totalFrames = 0;
    ma_decoder_get_length_in_pcm_frames(&decoder, &totalFrames);

    if (totalFrames == 0) {
        ma_decoder_uninit(&decoder);
        return std::vector<float>(totalBars, 0.1f);
    }

    const ma_uint64 framesPerBar = totalFrames / static_cast<ma_uint64>(totalBars);
    std::vector<float> peaks(totalBars, 0.08f);
    std::vector<float> buffer(4096);

    for (int bar = 0; bar < totalBars; ++bar) {
        ma_uint64 framesRemaining = framesPerBar;
        float maxVal = 0.0f;

        while (framesRemaining > 0) {
            const ma_uint64 toRead = std::min(framesRemaining, static_cast<ma_uint64>(buffer.size()));
            ma_uint64 framesRead = 0;
            ma_decoder_read_pcm_frames(&decoder, buffer.data(), toRead, &framesRead);

            if (framesRead == 0) break;

            for (ma_uint64 i = 0; i < framesRead; i += 4) {
                const float val = std::abs(buffer[i]);
                if (val > maxVal) maxVal = val;
            }

            framesRemaining -= framesRead;
        }

        peaks[bar] = std::clamp(maxVal, 0.08f, 1.0f);
    }

    ma_decoder_uninit(&decoder);
    return peaks;
}

} // namespace Audio