#pragma once

#include <QObject>
#include <QString>
#include <QTimer>

struct ma_engine;
struct ma_sound;

namespace Audio {

class Engine : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool isPlaying READ isPlaying NOTIFY stateChanged)
    Q_PROPERTY(double position READ position NOTIFY positionChanged)
    Q_PROPERTY(double duration READ duration NOTIFY durationChanged)
    Q_PROPERTY(float volume READ volume WRITE setVolume NOTIFY volumeChanged)

public:
    explicit Engine(QObject* parent = nullptr);
    ~Engine() override;

    bool isPlaying() const;
    double position() const;
    double duration() const;
    float volume() const;

    Q_INVOKABLE void play(const QString& filePath);
    Q_INVOKABLE void togglePlayPause();
    Q_INVOKABLE void pause();
    Q_INVOKABLE void stop();
    Q_INVOKABLE void seek(double seconds);
    Q_INVOKABLE void setVolume(float volume);

signals:
    void stateChanged();
    void positionChanged();
    void durationChanged();
    void volumeChanged();
    void trackEnded();

private slots:
    void onTick();

private:
    void unloadSound();

    ma_engine* m_engine{nullptr};
    ma_sound* m_sound{nullptr};
    bool m_soundLoaded{false};
    bool m_isPlaying{false};
    double m_duration{0.0};
    float m_volume{1.0f};
    QTimer m_tickTimer;
};

} // namespace Audio