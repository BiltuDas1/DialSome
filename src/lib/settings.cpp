#include "settings.h"

Settings::Settings(QObject *parent) : QObject(parent) {
    this->m_settingsUrl = GITHUB_CONFIGURATIONS;
}

void Settings::loadSettings() {
    QString directory = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString filePath = directory + "/settings.ini";
    
    QDir dir;
    if (!dir.exists(directory)) {
        dir.mkpath(directory);
    }

    if (!QFile::exists(filePath)) {
        this->downloadSettings(filePath);
    } else {
        // Call the download finish signal, cause it's already downloaded
        emit this->settingsDownloaded();
    }

    connect(this, &Settings::settingsDownloaded, this, [this, filePath]() {
        this->m_settings.reset(new QSettings(filePath, QSettings::IniFormat));
        emit this->settingsReady();
    });
}

void Settings::downloadSettings(QString filePath) {
    QUrl url(this->m_settingsUrl);
    QNetworkRequest request(url);
    QNetworkReply *reply = m_networkManager.get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply, filePath]() {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray rawData = reply->readAll();

            QFile file(filePath);
            if (file.open(QIODevice::WriteOnly)) {
                file.write(rawData);
                file.close();
                qDebug() << "File saved successfully at:" << filePath;
                emit this->settingsDownloaded();
            } else {
                qDebug() << "Unable to save file: " << filePath;
                emit this->settingsDownloadError("Unable to save file: " + filePath);
            }
        } else {
            qDebug() << "Unable to fetch settings from" << reply->errorString();
            emit this->settingsDownloadError("Unable to fetch settings from: " + reply->errorString());
        }
        reply->deleteLater();
    });
}

QString Settings::getHost() const {
    return this->m_settings->value("Server/host").toString();
}

QString Settings::getHttpProtocol() const {
    bool https = this->m_settings->value("Protocol/https").toBool();
    return https ? "https" : "http";
}

QString Settings::getWSProtocol() const {
    bool wss = this->m_settings->value("Protocol/wss").toBool();
    return wss ? "wss" : "ws";
}
