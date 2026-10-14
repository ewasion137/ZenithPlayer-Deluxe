#include "metadata.hpp"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <algorithm>

namespace Player {

namespace {

QImage findFolderCover(const QString& filePath) {
    const QDir dir = QFileInfo(filePath).dir();
    const QStringList filters = {
        "cover.jpg", "cover.png", "cover.jpeg",
        "folder.jpg", "folder.png", "folder.jpeg",
        "album.jpg", "album.png", "front.jpg", "front.png"
    };

    const QStringList files = dir.entryList(filters, QDir::Files);
    if (!files.isEmpty()) {
        return QImage(dir.absoluteFilePath(files.first()));
    }
    return {};
}

QImage parseId3v2Cover(QFile& file) {
    if (!file.seek(0)) {
        return {};
    }

    QByteArray header = file.read(10);
    if (header.size() < 10 || !header.startsWith("ID3")) {
        return {};
    }

    const int tagSize = ((header[6] & 0x7F) << 21) |
                        ((header[7] & 0x7F) << 14) |
                        ((header[8] & 0x7F) << 7) |
                        (header[9] & 0x7F);

    QByteArray tagData = file.read(std::min(tagSize, 10 * 1024 * 1024));
    int apicIndex = tagData.indexOf("APIC");
    if (apicIndex == -1) {
        return {};
    }

    int offset = apicIndex + 10;
    if (offset >= tagData.size()) {
        return {};
    }

    offset += 1;
    int mimeEnd = tagData.indexOf('\0', offset);
    if (mimeEnd == -1) {
        return {};
    }
    offset = mimeEnd + 2;

    int descEnd = tagData.indexOf('\0', offset);
    if (descEnd == -1) {
        return {};
    }
    offset = descEnd + 1;

    if (offset < tagData.size()) {
        QImage img;
        img.loadFromData(reinterpret_cast<const uchar*>(tagData.constData() + offset), tagData.size() - offset);
        return img;
    }

    return {};
}

QImage parseFlacCover(QFile& file) {
    if (!file.seek(0)) {
        return {};
    }

    QByteArray magic = file.read(4);
    if (magic != "fLaC") {
        return {};
    }

    while (!file.atEnd()) {
        QByteArray blockHeader = file.read(4);
        if (blockHeader.size() < 4) {
            break;
        }

        const bool isLast = (blockHeader[0] & 0x80) != 0;
        const int blockType = blockHeader[0] & 0x7F;
        const int blockLen = ((uchar)blockHeader[1] << 16) | ((uchar)blockHeader[2] << 8) | (uchar)blockHeader[3];

        if (blockType == 6) {
            QByteArray picBlock = file.read(blockLen);
            if (picBlock.size() >= 32) {
                int mimeLen = ((uchar)picBlock[4] << 24) | ((uchar)picBlock[5] << 16) | ((uchar)picBlock[6] << 8) | (uchar)picBlock[7];
                int offset = 8 + mimeLen;
                int descLen = ((uchar)picBlock[offset] << 24) | ((uchar)picBlock[offset + 1] << 16) | ((uchar)picBlock[offset + 2] << 8) | (uchar)picBlock[offset + 3];
                offset += 4 + descLen + 16;
                int dataLen = ((uchar)picBlock[offset] << 24) | ((uchar)picBlock[offset + 1] << 16) | ((uchar)picBlock[offset + 2] << 8) | (uchar)picBlock[offset + 3];
                offset += 4;

                if (offset + dataLen <= picBlock.size()) {
                    QImage img;
                    img.loadFromData(reinterpret_cast<const uchar*>(picBlock.constData() + offset), dataLen);
                    return img;
                }
            }
            break;
        }

        file.seek(file.pos() + blockLen);
        if (isLast) {
            break;
        }
    }

    return {};
}

} // namespace

TrackMetadata extractMetadata(const QString& filePath) {
    TrackMetadata meta;
    const QFileInfo info(filePath);
    meta.title = info.completeBaseName();
    meta.artist = info.dir().dirName();
    meta.album = "";

    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly)) {
        const QString ext = info.suffix().toLower();
        if (ext == "mp3") {
            meta.cover = parseId3v2Cover(file);
        } else if (ext == "flac") {
            meta.cover = parseFlacCover(file);
        }
    }

    if (meta.cover.isNull()) {
        meta.cover = findFolderCover(filePath);
    }

    return meta;
}

} // namespace Player