#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#include "engine.hpp"
#include <QFileInfo>

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
    m_tickTimer.setInterval(40);
}

Engine::~Engine() {
    unloadSound();
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

void Engine::setVolume(float vol) {
    m_volume = std::clamp(vol, 0.0f, 2.0f);
    if (m_soundLoaded) {
        ma_sound_set_volume(m_sound, m_volume);
    }
    emit volumeChanged();
}

void Engine::unloadSound() {
    if (m_soundLoaded) {
        m_tickTimer.stop();
        ma_sound_stop(m_sound);
        ma_sound_uninit(m_sound);
        m_soundLoaded = false;
        m_isPlaying = false;
        m_duration = 0.0;
        emit stateChanged();
        emit durationChanged();
        emit positionChanged();
    }
}

void Engine::play(const QString& filePath) {
    if (!m_engine || filePath.isEmpty()) {
        return;
    }

    unloadSound();

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

    float len = 0.0f;
    ma_sound_get_length_in_seconds(m_sound, &len);
    m_duration = static_cast<double>(len);

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
        emit stateChanged();
    }
}

void Engine::stop() {
    if (m_soundLoaded) {
        ma_sound_stop(m_sound);
        ma_sound_seek_to_pcm_frame(m_sound, 0);
        m_isPlaying = false;
        m_tickTimer.stop();
        emit stateChanged();
        emit positionChanged();
    }
}

void Engine::seek(double seconds) {
    if (!m_soundLoaded) {
        return;
    }
    ma_uint64 frame = static_cast<ma_uint64>(seconds * ma_engine_get_sample_rate(m_engine));
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