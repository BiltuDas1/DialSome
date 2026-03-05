#ifndef APISERVICE_H
#define APISERVICE_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QPointer>
#include <QJsonObject>
#include <QJsonDocument>
#include "apiconstrains.h"
#include "settings.h"
#include "securestorage.h"

class APIService : public QObject {
    Q_OBJECT
    
public:
    explicit APIService(QPointer<Settings> settings, QPointer<SecureStorage> storage, QObject *parent = nullptr);
    void update_fcm(QString token, QString accessToken);
    void refreshToken();

signals:
    void tokenRefreshed(QString accessToken, QString refreshToken);
    void tokenRefreshError(QString error);
    void invalidSession();

private:
    QPointer<Settings> m_settings;
    QPointer<SecureStorage> m_storage;
    QNetworkAccessManager m_networkManager;
};

#endif // APISERVICE_H
