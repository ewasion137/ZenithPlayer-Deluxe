#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#include "engine.hpp"
#include "fft.hpp"
#include "waveform.hpp"

#include <algorithm>
#include <cmath>
#include <QFile>

namespace Audio {

namespace {

enum class AudioFormatType {
    Unknown,
    Wav,
    Mp3,
    Flac,
    Ogg
};

AudioFormatType inspectAudioFormat(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return AudioFormatType::Unknown;
    }

    const QByteArray header = file.read(12);
    if (header.size() < 4) {
        return AudioFormatType::Unknown;
    }

    // RIFF....WAVE
    if (header.startsWith("RIFF") && header.mid(8, 4) == "WAVE") {
        return AudioFormatType::Wav;
    }

    // fLaC
    if (header.startsWith("fLaC")) {
        return AudioFormatType::Flac;
    }

    // OggS
    if (header.startsWith("OggS")) {
        return AudioFormatType::Ogg;
    }

    // ID3v2 container or sync word 0xFFE0..0xFFFF
    if (header.startsWith("ID3")) {
        return AudioFormatType::Mp3;
    }
    const unsigned char b0 = static_cast<unsigned char>(header[0]);
    const unsigned char b1 = static_cast<unsigned char>(header[1]);
    if (b0 == 0xFF && (b1 & 0xE0) == 0xE0) {
        return AudioFormatType::Mp3;
    }

    return AudioFormatType::Unknown;
}

constexpr double EQ_FREQUENCIES[7] = {60.0, 170.0, 350.0, 1000.0, 3500.0, 10000.0, 14000.0};
constexpr double EQ_Q = 1.41;

struct CustomEqNode {
    ma_node_base baseNode;
    ma_peak2 filters[7];
};

static float g_fftSamples[256] = {0.0f};
static size_t g_fftWritePos = 0;

void eqNodeProcess(ma_node* pNode, const float** ppFramesIn, ma_uint32* pFrameCountIn, float** ppFramesOut, ma_uint32* pFrameCountOut) {
    auto* pEq = reinterpret_cast<CustomEqNode*>(pNode);
    const ma_uint32 frameCount = *pFrameCountIn;

    ma_copy_pcm_frames(ppFramesOut[0], ppFramesIn[0], frameCount, ma_format_f32, 2);

    for (int i = 0; i < 7; ++i) {
        ma_peak2_process_pcm_frames(&pEq->filters[i], ppFramesOut[0], ppFramesOut[0], frameCount);
    }

    float* pOut = ppFramesOut[0];
    const ma_uint32 sampleCount = frameCount * 2;
    for (ma_uint32 s = 0; s < sampleCount; ++s) {
        if (pOut[s] > 1.0f) pOut[s] = 1.0f;
        else if (pOut[s] < -1.0f) pOut[s] = -1.0f;
    }

    // Буфер для FFT спектрограммы
    for (ma_uint32 i = 0; i < frameCount; ++i) {
        const float mono = (pOut[i * 2] + pOut[i * 2 + 1]) * 0.5f;
        g_fftSamples[g_fftWritePos] = mono;
        g_fftWritePos = (g_fftWritePos + 1) % 256;
    }

    *pFrameCountOut = frameCount;
}

static ma_node_vtable g_eqNodeVTable = {
    eqNodeProcess,
    nullptr,
    1,
    1,
    0
};

} // namespace

Engine::Engine(QObject* parent)
    : QObject(parent), m_eqGains(7, 0.0f) {
    m_engine = new ma_engine();
    ma_engine_config config = ma_engine_config_init();
    ma_result result = ma_engine_init(&config, m_engine);

    if (result != MA_SUCCESS) {
        delete m_engine;
        m_engine = nullptr;
        return;
    }

    m_sound = new ma_sound();
    initEqNode();

    connect(&m_tickTimer, &QTimer::timeout, this, &Engine::onTick);
    m_tickTimer.setInterval(30);
}

Engine::~Engine() {
    unloadSound(false);

    if (m_eqNodeMemory) {
        auto* pEq = reinterpret_cast<CustomEqNode*>(m_eqNodeMemory);
        for (int i = 0; i < 7; ++i) {
            ma_peak2_uninit(&pEq->filters[i], nullptr);
        }
        ma_node_uninit(&pEq->baseNode, nullptr);
        delete pEq;
        m_eqNodeMemory = nullptr;
    }

    if (m_engine) {
        ma_engine_uninit(m_engine);
        delete m_engine;
    }
    delete m_sound;
}

void Engine::initEqNode() {
    if (!m_engine) {
        return;
    }

    auto* pEq = new CustomEqNode();
    m_eqNodeMemory = pEq;

    ma_uint32 channels = 2;
    ma_node_config baseConfig = ma_node_config_init();
    baseConfig.vtable = &g_eqNodeVTable;
    baseConfig.pInputChannels = &channels;
    baseConfig.pOutputChannels = &channels;

    ma_node_init(ma_engine_get_node_graph(m_engine), &baseConfig, nullptr, &pEq->baseNode);

    const ma_uint32 sampleRate = ma_engine_get_sample_rate(m_engine);

    for (int i = 0; i < 7; ++i) {
        ma_peak2_config filterConfig = ma_peak2_config_init(
            ma_format_f32,
            channels,
            sampleRate,
            0.0,
            EQ_Q,
            EQ_FREQUENCIES[i]
        );
        ma_peak2_init(&filterConfig, nullptr, &pEq->filters[i]);
    }

    ma_node_attach_output_bus(&pEq->baseNode, 0, ma_engine_get_endpoint(m_engine), 0);
}

bool Engine::isPlaying() const {
    return m_isPlaying;
}

double Engine::position() const {
    if (!m_soundLoaded) {
        return 0.0;
    }
    float pos = 0.0f;
    ma_sound_get_cursor_in_seconds(m_sound, &pos);
    return static_cast<double>(pos);
}

double Engine::duration() const {
    return m_duration;
}

float Engine::volume() const {
    return m_volume;
}

float Engine::speed() const {
    return m_speed;
}

bool Engine::flStudioMode() const {
    return m_flStudioMode;
}

QVariantList Engine::eqGains() const {
    QVariantList list;
    for (float g : m_eqGains) {
        list.append(g);
    }
    return list;
}

QVariantList Engine::spectrum() const {
    return m_spectrum;
}

QVariantList Engine::waveform() const {
    return m_waveform;
}

void Engine::setVolume(float vol) {
    m_volume = std::clamp(vol, 0.0f, 2.0f);
    if (m_soundLoaded) {
        ma_sound_set_volume(m_sound, m_volume);
    }
    emit volumeChanged();
}

void Engine::setSpeed(float spd) {
    m_speed = std::clamp(spd, 0.5f, 2.0f);
    if (m_soundLoaded) {
        ma_sound_set_pitch(m_sound, m_speed);
    }
    emit speedChanged();
}

void Engine::setFlStudioMode(bool enabled) {
    if (m_flStudioMode != enabled) {
        m_flStudioMode = enabled;
        emit flStudioModeChanged();
    }
}

void Engine::setEqGain(int index, float gainDb) {
    if (index < 0 || index >= 7 || !m_eqNodeMemory) {
        return;
    }

    gainDb = std::clamp(gainDb, -12.0f, 12.0f);
    m_eqGains[index] = gainDb;

    auto* pEq = reinterpret_cast<CustomEqNode*>(m_eqNodeMemory);
    const ma_uint32 sampleRate = ma_engine_get_sample_rate(m_engine);

    ma_peak2_config filterConfig = ma_peak2_config_init(
        ma_format_f32,
        2,
        sampleRate,
        static_cast<double>(gainDb),
        EQ_Q,
        EQ_FREQUENCIES[index]
    );

    ma_peak2_reinit(&filterConfig, &pEq->filters[index]);
    emit eqGainsChanged();
}

void Engine::setAllEqGains(const QVariantList& gains) {
    if (gains.size() != 7) {
        return;
    }
    for (int i = 0; i < 7; ++i) {
        setEqGain(i, gains[i].toFloat());
    }
}

void Engine::resetEq() {
    for (int i = 0; i < 7; ++i) {
        setEqGain(i, 0.0f);
    }
}

QString Engine::formatTime(double seconds) const {
    if (seconds < 0.0 || std::isnan(seconds)) {
        seconds = 0.0;
    }
    const int totalSec = static_cast<int>(seconds);
    const int m = totalSec / 60;
    const int s = totalSec % 60;
    return QString("%1:%2").arg(m, 2, 10, QChar('0')).arg(s, 2, 10, QChar('0'));
}

void Engine::unloadSound(bool fade) {
    if (!m_soundLoaded) {
        return;
    }

    m_tickTimer.stop();

    if (fade && m_isPlaying) {
        ma_sound_set_fade_in_milliseconds(m_sound, m_volume, 0.0f, 120);
        ma_sleep(120);
    }

    ma_sound_stop(m_sound);
    ma_sound_uninit(m_sound);

    m_soundLoaded = false;
    m_isPlaying = false;
    m_duration = 0.0;
    m_playbackStartSec = 0.0;
    m_spectrum.clear();
    m_waveform.clear();
    m_smoothedSpectrum.clear();

    emit stateChanged();
    emit durationChanged();
    emit positionChanged();
    emit spectrumChanged();
    emit waveformChanged();
}

void Engine::play(const QString& filePath) {
    if (!m_engine || filePath.isEmpty()) {
        return;
    }

    const AudioFormatType fmt = inspectAudioFormat(filePath);
    if (fmt == AudioFormatType::Unknown) {
        return;
    }

    unloadSound(true);

    ma_result res;
#if defined(_WIN32)
    res = ma_sound_init_from_file_w(
        m_engine,
        filePath.toStdWString().c_str(),
        MA_SOUND_FLAG_STREAM,
        nullptr,
        nullptr,
        m_sound
    );
#else
    res = ma_sound_init_from_file(
        m_engine,
        filePath.toUtf8().constData(),
        MA_SOUND_FLAG_STREAM,
        nullptr,
        nullptr,
        m_sound
    );
#endif

    if (res != MA_SUCCESS) {
        return;
    }

    m_soundLoaded = true;
    ma_sound_set_volume(m_sound, m_volume);
    ma_sound_set_pitch(m_sound, m_speed);

    if (m_eqNodeMemory) {
        auto* pEq = reinterpret_cast<CustomEqNode*>(m_eqNodeMemory);
        ma_node_attach_output_bus(m_sound, 0, &pEq->baseNode, 0);
    }

    float len = 0.0f;
    ma_sound_get_length_in_seconds(m_sound, &len);
    m_duration = static_cast<double>(len);
    m_playbackStartSec = 0.0;

    // Генерация 120 пиков формы волны
    const auto peaks = generateWaveformPeaks(filePath, 120);
    m_waveform.clear();
    m_waveform.reserve(static_cast<qsizetype>(peaks.size()));
    for (float p : peaks) {
        m_waveform.append(p);
    }
    emit waveformChanged();

    ma_sound_start(m_sound);
    m_isPlaying = true;
    m_tickTimer.start();

    emit stateChanged();
    emit durationChanged();
    emit positionChanged();
}

void Engine::setFpsCap(int fps) {
    const int clampedFps = std::clamp(fps, 15, 144);
    m_tickTimer.setInterval(1000 / clampedFps);
}

void Engine::stopWithReturn() {
    if (!m_soundLoaded) {
        return;
    }

    if (m_isPlaying) {
        if (m_flStudioMode) {
            seek(m_playbackStartSec);
        }
        pause();
    } else {
        if (m_flStudioMode) {
            m_playbackStartSec = position();
        }
        ma_sound_start(m_sound);
        m_isPlaying = true;
        m_tickTimer.start();
        emit stateChanged();
    }
}

void Engine::pause() {
    if (m_soundLoaded && m_isPlaying) {
        ma_sound_stop(m_sound);
        m_isPlaying = false;
        m_tickTimer.stop();
        m_spectrum.clear();
        emit stateChanged();
        emit spectrumChanged();
    }
}

void Engine::stop() {
    if (m_soundLoaded) {
        ma_sound_stop(m_sound);
        ma_sound_seek_to_pcm_frame(m_sound, 0);
        m_isPlaying = false;
        m_playbackStartSec = 0.0;
        m_tickTimer.stop();
        m_spectrum.clear();
        emit stateChanged();
        emit positionChanged();
        emit spectrumChanged();
    }
}

void Engine::seek(double seconds) {
    if (!m_soundLoaded) {
        return;
    }
    const double target = std::clamp(seconds, 0.0, m_duration);
    const auto frame = static_cast<ma_uint64>(target * ma_engine_get_sample_rate(m_engine));

    if (m_isPlaying) {
        ma_sound_seek_to_pcm_frame(m_sound, frame);
        ma_sound_set_fade_in_milliseconds(m_sound, 0.001f, m_volume, 20);
    } else {
        ma_sound_seek_to_pcm_frame(m_sound, frame);
    }

    m_playbackStartSec = target;
    emit positionChanged();
}

void Engine::togglePlayPause() {
    if (!m_soundLoaded) {
        return;
    }

    if (m_isPlaying) {
        pause();
        m_playbackStartSec = position();
    } else {
        ma_sound_start(m_sound);
        m_isPlaying = true;
        m_tickTimer.start();
        emit stateChanged();
    }
}

void Engine::onTick() {
    if (!m_soundLoaded) {
        return;
    }

    if (ma_sound_at_end(m_sound)) {
        m_isPlaying = false;
        m_tickTimer.stop();
        m_spectrum.clear();
        m_smoothedSpectrum.clear();
        emit stateChanged();
        emit spectrumChanged();
        emit trackEnded();
        return;
    }

    if (m_isPlaying) {
        float samples[256];
        for (size_t i = 0; i < 256; ++i) {
            samples[i] = g_fftSamples[(g_fftWritePos + i) % 256];
        }

        const auto rawMags = calculateFftMagnitudes(samples, 256, 48);

        if (m_smoothedSpectrum.size() != rawMags.size()) {
            m_smoothedSpectrum.assign(rawMags.size(), 0.0f);
        }

        m_spectrum.clear();
        m_spectrum.reserve(static_cast<qsizetype>(rawMags.size()));

        for (size_t i = 0; i < rawMags.size(); ++i) {
            const float target = rawMags[i];
            float current = m_smoothedSpectrum[i];

            if (target > current) {
                // Быстрый панчевый подъем на пиках
                current = current * 0.25f + target * 0.75f;
            } else {
                // Мягкое плавное падение без дрожания
                current = current * 0.85f;
            }

            if (current < 0.005f) current = 0.0f;

            m_smoothedSpectrum[i] = current;
            m_spectrum.append(current);
        }

        emit spectrumChanged();
    }

    emit positionChanged();
}

} // namespace Audio