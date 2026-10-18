#pragma once

#include <QByteArray>
#include <QString>

namespace Theme {

class Security {
public:
    static bool isPathSafe(const QString& pathOrUrl);
    static bool validateImageBytes(const QByteArray& data);
    static QString sanitizeString(const QString& input);
};

} // namespace Theme