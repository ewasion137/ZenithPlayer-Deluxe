#pragma once

#include <QObject>
#include <QString>
#include <QTimer>
#include <QVariantList>
#include <vector>

struct ma_engine;
struct ma_sound;

namespace Audio {

class Engine : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool isPlaying READ isPlaying NOTIFY stateChanged)
    Q_PROPERTY(double position READ position NOTIFY positionChanged)
    Q_PROPERTY(double duration READ duration NOTIFY durationChanged)
    Q_PROPERTY(float volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(float speed READ speed WRITE setSpeed NOTIFY speedChanged)
    Q_PROPERTY(bool flStudioMode READ flStudioMode WRITE setFlStudioMode NOTIFY flStudioModeChanged)
    Q_PROPERTY(QVariantList eqGains READ eqGains NOTIFY eqGainsChanged)

public:
    explicit Engine(QObject* parent = nullptr);
    ~Engine() override;

    bool isPlaying() const;
    double position() const;
    double duration() const;
    float volume() const;
    float speed() const;
    bool flStudioMode() const;
    QVariantList eqGains() const;

    Q_INVOKABLE void play(const QString& filePath);
    Q_INVOKABLE void togglePlayPause();
    Q_INVOKABLE void stopWithReturn();
    Q_INVOKABLE void pause();
    Q_INVOKABLE void stop();
    Q_INVOKABLE void seek(double seconds);
    Q_INVOKABLE void setVolume(float volume);
    Q_INVOKABLE void setSpeed(float speed);
    Q_INVOKABLE void setFlStudioMode(bool enabled);
    Q_INVOKABLE void setEqGain(int index, float gainDb);
    Q_INVOKABLE void setAllEqGains(const QVariantList& gains);
    Q_INVOKABLE void resetEq();
    Q_INVOKABLE QString formatTime(double seconds) const;

signals:
    void stateChanged();
    void positionChanged();
    void durationChanged();
    void volumeChanged();
    void speedChanged();
    void flStudioModeChanged();
    void eqGainsChanged();
    void trackEnded();

private slots:
    void onTick();

private:
    void unloadSound(bool fade = false);
    void initEqNode();

    ma_engine* m_engine{nullptr};
    ma_sound* m_sound{nullptr};
    void* m_eqNodeMemory{nullptr};

    bool m_soundLoaded{false};
    bool m_isPlaying{false};
    bool m_flStudioMode{true};
    double m_duration{0.0};
    double m_playbackStartSec{0.0};
    float m_volume{1.0f};
    float m_speed{1.0f};
    std::vector<float> m_eqGains;
    QTimer m_tickTimer;
};

} // namespace Audio