#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include <QString>
#include <QJniObject>      // Fixed: Missing this caused the header error
#include <QJsonObject>     // Needed for JSON signaling
#include <QtWebSockets/QWebSocket>
#include <QtQmlIntegration/qqmlintegration.h>

class Backend : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QString message READ message NOTIFY messageChanged)

public:
    explicit Backend(QObject *parent = nullptr);
    QString message() const;

    Q_INVOKABLE void startCall(const QString &roomId);

    // Internal methods for JNI callbacks
    void handleLocalIce(const QJsonObject &json);
    void handleLocalSdp(const QJsonObject &json);

signals:
    void messageChanged();

private slots:
    void onTextMessageReceived(const QString &message);
    void onConnected();

private:
    void setMessage(const QString &msg);
    QString m_message = "Ready";
    QWebSocket m_webSocket;
    QJniObject m_webrtc;
};

#endif
