#ifndef FCMMANAGER_H
#define FCMMANAGER_H

#include <QObject>
#include <QString>
#include <QNetworkAccessManager>
#include <QPointer>
#include "lib/securestorage.h"

class FCMManager : public QObject {
    Q_OBJECT
public:
    explicit FCMManager(SecureStorage *storage, QString hostUrl, QObject *parent = nullptr);
    void updateTokenOnBackend(const QString &token);
    static FCMManager* instance();
    // Method to be called by the JNI bridge
    void processIncomingSignal(const QString &type, const QString &sdp, const QString &email);

signals:
    // Signal to notify the Backend or UI of an incoming call
    void callSignalReceived(const QString &type, const QString &sdp, const QString &email);

private:
    static FCMManager* s_instance;
    QNetworkAccessManager m_networkManager;
    QPointer<SecureStorage> m_storage;
    QString hostUrl;
};

#endif // FCMMANAGER_H