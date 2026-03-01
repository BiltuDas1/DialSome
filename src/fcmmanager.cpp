#include "fcmmanager.h"
#include <QDebug>
#include <QJniObject>
#include <QJsonObject>
#include <QJsonDocument>
#include <QNetworkReply>

FCMManager* FCMManager::s_instance = nullptr;

FCMManager::FCMManager(SecureStorage *storage, QString hostUrl, QObject *parent) : QObject(parent) {
    s_instance = this;
    this->m_storage = storage;
    this->hostUrl = hostUrl;
}

FCMManager* FCMManager::instance() {
    return s_instance;
}

void FCMManager::processIncomingSignal(const QString &type, const QString &roomId, const QString &email) {
    qDebug() << "FCMManager: Received" << type << "from" << email;
    emit callSignalReceived(type, roomId, email);
}

// JNI Bridge: Matches the native method in MyFirebaseMessagingService.java
extern "C" {
    JNIEXPORT void JNICALL Java_com_github_biltudas1_dialsome_MyFirebaseMessagingService_onFCMMessageReceived(
        JNIEnv* env, jobject, jstring type, jstring roomId, jstring email) {
        
        FCMManager* manager = FCMManager::instance();
        if (!manager) {
            qDebug() << "FCMManager instance not initialized!";
            return;
        }

        QString typeStr = QJniObject(type).toString();
        QString roomStr = QJniObject(roomId).toString();
        QString emailStr = QJniObject(email).toString();

        // Invoke on the main thread to safely emit signals and update UI
        QMetaObject::invokeMethod(manager, [=]() {
            manager->processIncomingSignal(typeStr, roomStr, emailStr);
        }, Qt::QueuedConnection);
    }
}

extern "C" {
    JNIEXPORT void JNICALL Java_com_github_biltudas1_dialsome_MyFirebaseMessagingService_notifyNewToken(
        JNIEnv* env, jobject, jstring token) {
        
        QString tokenStr = QJniObject(token).toString();
        QMetaObject::invokeMethod(FCMManager::instance(), [=]() {
            FCMManager::instance()->updateTokenOnBackend(tokenStr);
        }, Qt::QueuedConnection);
    }
}

void FCMManager::updateTokenOnBackend(const QString &token) {
    this->m_storage->save("fcm_token", token);
    QString userId = this->m_storage->get("id");
    if (userId.isEmpty()) {
        qDebug() << "User not logged in yet. FCM token cached locally.";
        return;
    }

    QUrl url(this->hostUrl + "/users/fcm/update");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    qDebug() << "FCM Token:" << token;

    QJsonObject json;
    json["id"] = userId;
    json["fcm_token"] = token;

    QNetworkReply *reply = m_networkManager.post(request, QJsonDocument(json).toJson(QJsonDocument::Compact));
    connect(reply, &QNetworkReply::finished, this, [reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            qDebug() << "FCM Token updated successfully";
        } else {
            qDebug() << "FCM Update Failed:" << reply->errorString();
        }
        reply->deleteLater();
    });
}
