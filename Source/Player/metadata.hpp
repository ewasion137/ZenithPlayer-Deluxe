#pragma once

#include <QImage>
#include <QString>

namespace Player {

struct TrackMetadata {
    QString title;
    QString artist;
    QString album;
    QImage cover;
};

TrackMetadata extractMetadata(const QString& filePath);

} // namespace Player