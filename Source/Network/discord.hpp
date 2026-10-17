#pragma once

#include <QDateTime>
#include <QLocalSocket>
#include <QObject>
#include <QString>

namespace Network {

class DiscordRPC : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool isConnected READ isConnected NOTIFY connectionChanged)
    Q_PROPERTY(QString statusText READ statusText NOTIFY statusTextChanged)
    Q_PROPERTY(QString clientId READ clientId WRITE setClientId NOTIFY clientIdChanged)

public:
    explicit DiscordRPC(QObject* parent = nullptr);
    ~DiscordRPC() override;

    bool isConnected() const;
    QString statusText() const;
    QString clientId() const;

    void setClientId(const QString& id);

    Q_INVOKABLE void connectRpc(const QString& customClientId = "");
    Q_INVOKABLE void disconnectRpc();
    Q_INVOKABLE void updateActivity(const QString& title, const QString& artist, const QString& album,
                                    double duration, double position, float speed, bool isPlaying,
                                    const QString& coverUrl);
    Q_INVOKABLE void clearActivity();

signals:
    void connectionChanged();
    void statusTextChanged();
    void clientIdChanged();

private slots:
    void onConnected();
    void onDisconnected();
    void onReadyRead();
    void onError();

private:
    void sendPacket(quint32 opcode, const QByteArray& json);
    void sendHandshake();
    QString findIpcPipe();

    QLocalSocket m_socket;
    bool m_isConnected{false};
    QString m_statusText{"Disconnected"};
    QString m_clientId{""};
};

} // namespace Network