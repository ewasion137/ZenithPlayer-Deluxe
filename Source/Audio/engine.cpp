#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#include "engine.hpp"
#include <algorithm>
#include <cmath>

namespace Audio {

Engine::Engine(QObject* parent)
    : QObject(parent) {
    m_engine = new ma_engine();
    ma_engine_config config = ma_engine_config_init();
    ma_result result = ma_engine_init(&config, m_engine);

    if (result != MA_SUCCESS) {
        delete m_engine;
        m_engine = nullptr;
        return;
    }

    m_sound = new ma_sound();

    connect(&m_tickTimer, &QTimer::timeout, this, &Engine::onTick);
    m_tickTimer.setInterval(30);
}

Engine::~Engine() {
    unloadSound(false);
    if (m_engine) {
        ma_engine_uninit(m_engine);
        delete m_engine;
    }
    delete m_sound;
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

    emit stateChanged();
    emit durationChanged();
    emit positionChanged();
}

void Engine::play(const QString& filePath) {
    if (!m_engine || filePath.isEmpty()) {
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

    float len = 0.0f;
    ma_sound_get_length_in_seconds(m_sound, &len);
    m_duration = static_cast<double>(len);
    m_playbackStartSec = 0.0;

    ma_sound_start(m_sound);
    m_isPlaying = true;
    m_tickTimer.start();

    emit stateChanged();
    emit durationChanged();
    emit positionChanged();
}

void Engine::togglePlayPause() {
    if (!m_soundLoaded) {
        return;
    }

    if (m_isPlaying) {
        pause();
    } else {
        m_playbackStartSec = position();
        ma_sound_start(m_sound);
        m_isPlaying = true;
        m_tickTimer.start();
        emit stateChanged();
    }
}

void Engine::stopWithReturn() {
    if (!m_soundLoaded) {
        return;
    }

    if (m_flStudioMode) {
        seek(m_playbackStartSec);
        pause();
    } else {
        togglePlayPause();
    }
}

void Engine::pause() {
    if (m_soundLoaded && m_isPlaying) {
        ma_sound_stop(m_sound);
        m_isPlaying = false;
        m_tickTimer.stop();
        emit stateChanged();
    }
}

void Engine::stop() {
    if (m_soundLoaded) {
        ma_sound_stop(m_sound);
        ma_sound_seek_to_pcm_frame(m_sound, 0);
        m_isPlaying = false;
        m_playbackStartSec = 0.0;
        m_tickTimer.stop();
        emit stateChanged();
        emit positionChanged();
    }
}

void Engine::seek(double seconds) {
    if (!m_soundLoaded) {
        return;
    }
    const double target = std::clamp(seconds, 0.0, m_duration);
    const auto frame = static_cast<ma_uint64>(target * ma_engine_get_sample_rate(m_engine));
    ma_sound_seek_to_pcm_frame(m_sound, frame);
    emit positionChanged();
}

void Engine::onTick() {
    if (!m_soundLoaded) {
        return;
    }

    if (ma_sound_at_end(m_sound)) {
        m_isPlaying = false;
        m_tickTimer.stop();
        emit stateChanged();
        emit trackEnded();
        return;
    }

    emit positionChanged();
}

} // namespace Audio