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

class Backend : public QObject {
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QString message READ message NOTIFY messageChanged)

public:
    explicit Backend(QObject *parent = nullptr);
    QString message() const;
    void setMessage(const QString &msg);
    Q_INVOKABLE void startCall(const QString &roomId);
    void handleLocalIce(const QJsonObject &json);
    void handleLocalSdp(const QJsonObject &json);
    Q_INVOKABLE void fetchStartupData();

signals:
    void messageChanged();

private slots:
    void onTextMessageReceived(const QString &message);
    void onConnected();

private:
    QString m_message = "Ready";
    QNetworkAccessManager m_networkManager;
    QWebSocket m_webSocket;
    QJniObject m_webrtc;
    QScopedPointer<QSettings> m_settings;
};

#endif
