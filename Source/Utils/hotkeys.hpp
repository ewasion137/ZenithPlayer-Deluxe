#pragma once

#include <QObject>

namespace Utils {

class GlobalHotkeys : public QObject {
    Q_OBJECT

public:
    explicit GlobalHotkeys(QObject* parent = nullptr);
    ~GlobalHotkeys() override;

signals:
    void playPauseTriggered();
    void nextTriggered();
    void prevTriggered();

private:
    void registerKeys();
    void unregisterKeys();
};

} // namespace Utils