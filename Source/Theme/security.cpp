#include "security.hpp"

#include <QRegularExpression>

namespace Theme {

namespace {
constexpr qint64 MAX_IMAGE_SIZE = 15 * 1024 * 1024; // 15 MB
}

bool Security::isPathSafe(const QString& pathOrUrl) {
    if (pathOrUrl.trimmed().isEmpty()) return false;

    // 1. Unicode Right-to-Left Override (RLO) and directional spoofing check
    for (const QChar& c : pathOrUrl) {
        const ushort u = c.unicode();
        if ((u >= 0x202A && u <= 0x202E) || (u >= 0x2066 && u <= 0x2069)) {
            return false;
        }
    }

    // 2. Reject executable or script extensions
    static const QRegularExpression forbiddenExt(
        R"(\.(exe|dll|bat|cmd|vbs|vbe|js|jse|wsf|wsh|msc|scr|pif|com|msi|ps1|reg|hta)(\?|#|$))",
        QRegularExpression::CaseInsensitiveOption
    );
    if (forbiddenExt.match(pathOrUrl).hasMatch()) {
        return false;
    }

    // 3. Reject WebP as explicitly requested
    static const QRegularExpression webpExt(
        R"(\.webp(\?|#|$))",
        QRegularExpression::CaseInsensitiveOption
    );
    if (webpExt.match(pathOrUrl).hasMatch()) {
        return false;
    }

    return true;
}

bool Security::validateImageBytes(const QByteArray& data) {
    if (data.size() < 8 || data.size() > MAX_IMAGE_SIZE) {
        return false;
    }

    // Block Windows PE/EXE binaries (Magic bytes: 'M', 'Z')
    if (data.startsWith("MZ")) {
        return false;
    }

    // Block WebP container
    if (data.startsWith("RIFF") && data.mid(8, 4) == "WEBP") {
        return false;
    }

    // Accept strictly JPEG (\xFF\xD8\xFF)
    if (static_cast<uchar>(data[0]) == 0xFF &&
        static_cast<uchar>(data[1]) == 0xD8 &&
        static_cast<uchar>(data[2]) == 0xFF) {
        return true;
    }

    // Accept strictly PNG (\x89PNG\r\n\x1a\n)
    if (data.startsWith("\x89PNG\x0D\x0A\x1A\x0A")) {
        return true;
    }

    return false;
}

QString Security::sanitizeString(const QString& input) {
    QString out;
    out.reserve(input.size());
    for (const QChar& c : input) {
        const ushort u = c.unicode();
        // Исключаем RLO, LRO, PDF и скрытые управляющие directional-символы Unicode
        if (!((u >= 0x202A && u <= 0x202E) || (u >= 0x2066 && u <= 0x2069))) {
            out.append(c);
        }
    }
    return out.trimmed();
}
} // namespace Theme