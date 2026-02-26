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
#include "securestorage.h"

class Backend : public QObject {
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QString message READ message NOTIFY messageChanged)
    Q_PROPERTY(bool serverConnected READ serverConnected NOTIFY serverConnectionChanged)

public:
    explicit Backend(QObject *parent = nullptr);
    QString message() const;
    void setMessage(const QString &msg);
    Q_INVOKABLE void startCall(const QString &roomId);
    void handleLocalIce(const QJsonObject &json);
    void handleLocalSdp(const QJsonObject &json);
    Q_INVOKABLE void fetchStartupData();
    bool serverConnected() const;
    Q_INVOKABLE void loginWithGoogle(const QString &webClientId);
    Q_INVOKABLE void showToast(const QString &message);
    Q_INVOKABLE void createFile(const QString &fileName, const QString &content);
    Q_INVOKABLE bool isLoggedIn();

signals:
    void messageChanged();
    void settingsLoaded();
    void serverConnectionChanged();
    void dataCollectionFinished(const QString &email, const QString &displayName, const QString &idToken, const QString &userID);
    void dataCollectionError(const QString &error);
    void registerFinished(const QString &idToken);
    void loginFinished(const QString &email, const QString &displayName, const QString &userID, const QString &refresh_token);
    void loginError(const QString &error);

private slots:
    void onTextMessageReceived(const QString &message);
    void onConnected();

private:
    QString m_message = "Ready";
    QNetworkAccessManager m_networkManager;
    QWebSocket m_webSocket;
    QJniObject m_webrtc;
    QScopedPointer<QSettings> m_settings;
    bool m_serverConnected = false;
    QJniObject m_googleLogin;
    SecureStorage m_storage;
    QString m_jwtAccessToken = "";
};

#endif
