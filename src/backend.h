#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include <QString>
#include <QJniObject>
#include <QJsonObject>
#include <QtWebSockets/QWebSocket>
#include <QtQmlIntegration/qqmlintegration.h>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSettings>
#include <QScopedPointer>
#include "lib/google.h"
#include "fcmmanager.h"

class Backend : public QObject {
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QString message READ message NOTIFY messageChanged)
    Q_PROPERTY(bool serverConnected READ serverConnected NOTIFY serverConnectionChanged)
    Q_PROPERTY(Google* google READ google CONSTANT)

public:
    explicit Backend(QObject *parent = nullptr);
    QString message() const;
    void setMessage(const QString &msg);
    Q_INVOKABLE void startCall(const QString &roomId);
    void handleLocalIce(const QJsonObject &json);
    void handleLocalSdp(const QJsonObject &json);
    Q_INVOKABLE void fetchStartupData();
    bool serverConnected() const;
    Google* google() const { return m_google; }

signals:
    void messageChanged();
    void settingsLoaded();
    void serverConnectionChanged();
    void registerFinished(const QString &idToken);
    void loginFinished(const QString &email, const QString &displayName, const QString &userID, const QString &refresh_token);
    void loginError(const QString &error);

private slots:
    void onTextMessageReceived(const QString &message);
    void onConnected();
    void onFCMCallReceived(const QString &type, const QString &sdp, const QString &email);

private:
    QString m_message = "Ready";
    QNetworkAccessManager m_networkManager;
    QWebSocket m_webSocket;
    QJniObject m_webrtc;
    QScopedPointer<QSettings> m_settings;
    bool m_serverConnected = false;
    QString m_jwtAccessToken = "";
    QPointer<Google> m_google;
    QPointer<SecureStorage> m_storage;
    QPointer<FCMManager> m_fcm;
};

#endif
