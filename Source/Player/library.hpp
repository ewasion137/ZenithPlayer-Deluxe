#pragma once

#include <QImage>
#include <QObject>
#include <QString>
#include <QVariantList>

namespace Player {

class Library : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantList folders READ folders NOTIFY foldersChanged)
    Q_PROPERTY(QString currentTrackPath READ currentTrackPath WRITE setCurrentTrackPath NOTIFY currentTrackPathChanged)
    Q_PROPERTY(QString currentTitle READ currentTitle NOTIFY metadataChanged)
    Q_PROPERTY(QString currentArtist READ currentArtist NOTIFY metadataChanged)
    Q_PROPERTY(QString currentAlbum READ currentAlbum NOTIFY metadataChanged)
    Q_PROPERTY(QString currentCoverUrl READ currentCoverUrl NOTIFY metadataChanged)
    Q_PROPERTY(QString searchQuery READ searchQuery WRITE setSearchQuery NOTIFY searchQueryChanged)

public:
    explicit Library(QObject* parent = nullptr);

    QVariantList folders() const;
    QString currentTrackPath() const;
    QString currentTitle() const;
    QString currentArtist() const;
    QString currentAlbum() const;
    QString currentCoverUrl() const;
    QString searchQuery() const;

    QImage getCoverImage(const QString& path) const;

    Q_INVOKABLE void setCurrentTrackPath(const QString& path);
    Q_INVOKABLE void selectFolder();
    Q_INVOKABLE void scanPath(const QString& rawPath);
    Q_INVOKABLE void setSearchQuery(const QString& query);
    Q_INVOKABLE QString getNextTrack() const;
    Q_INVOKABLE QString getPreviousTrack() const;
    QString currentOnlineCoverUrl() const { return m_currentOnlineCoverUrl; }
    Q_INVOKABLE void setWebMetadata(const QString& title, const QString& artist, const QString& coverPath, const QString& onlineUrl = "");

signals:
    void foldersChanged();
    void currentTrackPathChanged();
    void metadataChanged();
    void searchQueryChanged();

private:
    void updateMetadata(const QString& path);

    QVariantList m_rawFolders;
    QString m_currentTrackPath;
    QString m_currentTitle{"Select a track"};
    QString m_currentArtist{"Ready to play"};
    QString m_currentAlbum{""};
    QString m_currentCoverUrl{""};
    QString m_searchQuery{""};
    QImage m_cachedCover;
    QString m_currentOnlineCoverUrl{""};
};

} // namespace Player