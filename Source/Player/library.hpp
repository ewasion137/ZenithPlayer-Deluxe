#pragma once

#include <QObject>
#include <QString>
#include <QVariantList>

namespace Player {

class Library : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantList folders READ folders NOTIFY foldersChanged)
    Q_PROPERTY(QString currentTrackPath READ currentTrackPath WRITE setCurrentTrackPath NOTIFY currentTrackPathChanged)
    Q_PROPERTY(QString currentTrackName READ currentTrackName NOTIFY currentTrackNameChanged)

public:
    explicit Library(QObject* parent = nullptr);

    QVariantList folders() const;
    QString currentTrackPath() const;
    QString currentTrackName() const;

    Q_INVOKABLE void setCurrentTrackPath(const QString& path);
    Q_INVOKABLE void selectFolder();
    Q_INVOKABLE void scanPath(const QString& rawPath);

signals:
    void foldersChanged();
    void currentTrackPathChanged();
    void currentTrackNameChanged();

private:
    QVariantList m_folders;
    QString m_currentTrackPath;
    QString m_currentTrackName{"Select a track"};
};

} // namespace Player