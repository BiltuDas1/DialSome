#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QString>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QByteArray>
#include <QIODevice>
#include <QSettings>
#include <QVariant>
#include <QAnyStringView>

class Settings : public QObject {
    Q_OBJECT

public:
    explicit Settings(QObject *parent = nullptr);
    void loadSettings();
    void downloadSettings(QString filePath);
    QString getHost() const;
    QString getHttpProtocol() const;
    QString getWSProtocol() const;

signals:
    void settingsDownloaded();
    void settingsDownloadError(QString error);
    void settingsReady();

private:
    QString m_settingsUrl;
    QNetworkAccessManager m_networkManager;
    bool m_settingsLoaded = false;
    QScopedPointer<QSettings> m_settings;
};

#endif // SETTINGS_H
