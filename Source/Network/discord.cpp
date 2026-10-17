#include "discord.hpp"

#include <QCoreApplication>
#include <QDataStream>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>

namespace Network {

namespace {
constexpr quint32 OP_HANDSHAKE = 0;
constexpr quint32 OP_FRAME = 1;
constexpr quint32 OP_CLOSE = 2;
} // namespace

DiscordRPC::DiscordRPC(QObject* parent)
    : QObject(parent) {
    connect(&m_socket, &QLocalSocket::connected, this, &DiscordRPC::onConnected);
    connect(&m_socket, &QLocalSocket::disconnected, this, &DiscordRPC::onDisconnected);
    connect(&m_socket, &QLocalSocket::readyRead, this, &DiscordRPC::onReadyRead);
    connect(&m_socket, &QLocalSocket::errorOccurred, this, &DiscordRPC::onError);
}

DiscordRPC::~DiscordRPC() {
    disconnectRpc();
}

bool DiscordRPC::isConnected() const {
    return m_isConnected;
}

QString DiscordRPC::statusText() const {
    return m_statusText;
}

QString DiscordRPC::clientId() const {
    return m_clientId;
}

void DiscordRPC::setClientId(const QString& id) {
    if (m_clientId != id) {
        m_clientId = id.trimmed();
        emit clientIdChanged();
    }
}

QString DiscordRPC::findIpcPipe() {
#if defined(_WIN32)
    for (int i = 0; i < 10; ++i) {
        const QString pipeName = QString("\\\\.\\pipe\\discord-ipc-%1").arg(i);
        QLocalSocket testSocket;
        testSocket.connectToServer(pipeName);
        if (testSocket.waitForConnected(50)) {
            testSocket.disconnectFromServer();
            return pipeName;
        }
    }
    return "\\\\.\\pipe\\discord-ipc-0";
#else
    const QString runtimeDir = QStandardPaths::writableLocation(QStandardPaths::RuntimeLocation);
    for (int i = 0; i < 10; ++i) {
        const QString pipePath = QString("%1/discord-ipc-%2").arg(runtimeDir).arg(i);
        if (QFile::exists(pipePath)) {
            return pipePath;
        }
    }
    return QString("%1/discord-ipc-0").arg(runtimeDir);
#endif
}

void DiscordRPC::connectRpc(const QString& customClientId) {
    if (!customClientId.isEmpty()) {
        setClientId(customClientId);
    }

    if (m_clientId.isEmpty()) {
        m_statusText = "Error: Client ID is empty";
        emit statusTextChanged();
        return;
    }

    if (m_socket.state() == QLocalSocket::ConnectedState) {
        disconnectRpc();
    }

    m_statusText = "Connecting to Discord...";
    emit statusTextChanged();

    const QString pipe = findIpcPipe();
    m_socket.connectToServer(pipe);
}

void DiscordRPC::disconnectRpc() {
    if (m_socket.state() != QLocalSocket::UnconnectedState) {
        clearActivity();
        sendPacket(OP_CLOSE, "{}");
        m_socket.disconnectFromServer();
    }
    m_isConnected = false;
    m_statusText = "Disconnected";
    emit connectionChanged();
    emit statusTextChanged();
}

void DiscordRPC::onConnected() {
    sendHandshake();
}

void DiscordRPC::onDisconnected() {
    m_isConnected = false;
    m_statusText = "Disconnected";
    emit connectionChanged();
    emit statusTextChanged();
}

void DiscordRPC::onError() {
    m_isConnected = false;
    m_statusText = "Error: Discord not running";
    emit connectionChanged();
    emit statusTextChanged();
}

void DiscordRPC::sendPacket(quint32 opcode, const QByteArray& json) {
    if (m_socket.state() != QLocalSocket::ConnectedState) {
        return;
    }

    QByteArray packet;
    QDataStream stream(&packet, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);

    stream << opcode;
    stream << static_cast<quint32>(json.size());
    packet.append(json);

    m_socket.write(packet);
    m_socket.flush();
}

void DiscordRPC::sendHandshake() {
    QJsonObject obj;
    obj["v"] = 1;
    obj["client_id"] = m_clientId;

    const QByteArray data = QJsonDocument(obj).toJson(QJsonDocument::Compact);
    sendPacket(OP_HANDSHAKE, data);
}

void DiscordRPC::onReadyRead() {
    while (m_socket.bytesAvailable() >= 8) {
        const QByteArray header = m_socket.peek(8);
        QDataStream stream(header);
        stream.setByteOrder(QDataStream::LittleEndian);

        quint32 opcode = 0;
        quint32 length = 0;
        stream >> opcode;
        stream >> length;

        if (m_socket.bytesAvailable() < static_cast<qint64>(8 + length)) {
            return;
        }

        m_socket.read(8);
        const QByteArray payload = m_socket.read(length);

        if (opcode == OP_FRAME) {
            const auto doc = QJsonDocument::fromJson(payload);
            const auto obj = doc.object();
            if (obj.value("evt").toString() == "READY") {
                m_isConnected = true;
                m_statusText = "Connected";
                emit connectionChanged();
                emit statusTextChanged();
            }
        }
    }
}

void DiscordRPC::updateActivity(const QString& title, const QString& artist, const QString& album,
                               double duration, double position, float speed, bool isPlaying,
                               const QString& coverUrl) {
    if (!m_isConnected) {
        return;
    }

    QJsonObject activity;
    activity["details"] = title.isEmpty() ? "Listening to Music" : title;
    activity["state"] = isPlaying ? (artist.isEmpty() ? "Playing" : artist) : "Paused";

    if (isPlaying && duration > 0.0) {
        const qint64 nowSec = QDateTime::currentSecsSinceEpoch();
        const double safeSpeed = std::max(0.5f, speed);
        const double remainingSec = (duration - position) / safeSpeed;
        const qint64 startEpoch = nowSec - static_cast<qint64>(position / safeSpeed);
        const qint64 endEpoch = nowSec + static_cast<qint64>(remainingSec);

        QJsonObject timestamps;
        timestamps["start"] = startEpoch;
        timestamps["end"] = endEpoch;
        activity["timestamps"] = timestamps;
    }

    QJsonObject assets;
    if (coverUrl.startsWith("http://") || coverUrl.startsWith("https://")) {
        assets["large_image"] = coverUrl;
    } else {
        assets["large_image"] = "icon";
    }
    assets["large_text"] = album.isEmpty() ? "Zenith Player Deluxe" : album;

    if (!isPlaying) {
        assets["small_image"] = "pause";
        assets["small_text"] = "Paused";
    }
    activity["assets"] = assets;

    QJsonObject args;
    args["pid"] = static_cast<qint64>(QCoreApplication::applicationPid());
    args["activity"] = activity;

    QJsonObject frame;
    frame["cmd"] = "SET_ACTIVITY";
    frame["args"] = args;
    frame["nonce"] = QString::number(QDateTime::currentMSecsSinceEpoch());

    const QByteArray data = QJsonDocument(frame).toJson(QJsonDocument::Compact);
    sendPacket(OP_FRAME, data);
}

void DiscordRPC::clearActivity() {
    if (!m_isConnected) {
        return;
    }

    QJsonObject args;
    args["pid"] = static_cast<qint64>(QCoreApplication::applicationPid());
    args["activity"] = QJsonValue::Null;

    QJsonObject frame;
    frame["cmd"] = "SET_ACTIVITY";
    frame["args"] = args;
    frame["nonce"] = QString::number(QDateTime::currentMSecsSinceEpoch());

    const QByteArray data = QJsonDocument(frame).toJson(QJsonDocument::Compact);
    sendPacket(OP_FRAME, data);
}

} // namespace Network