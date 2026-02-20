#include "backend.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QCoreApplication>
#include <QDebug>

static Backend* s_instance = nullptr;

Backend::Backend(QObject *parent) : QObject(parent) {
    s_instance = this;
    connect(&m_webSocket, &QWebSocket::connected, this, &Backend::onConnected);
    connect(&m_webSocket, &QWebSocket::textMessageReceived, this, &Backend::onTextMessageReceived);
}

// JNI Callbacks from Java
extern "C" {
JNIEXPORT void JNICALL Java_com_github_biltudas1_dialsome_WebRTCManager_onLocalIceCandidate(
    JNIEnv* env, jobject, jstring sdp, jstring mid, jint index) {
    if (!s_instance) return;

    QString sdpStr = QJniObject(sdp).toString();
    QString midStr = QJniObject(mid).toString();
    int mLineIndex = (int)index;

    QMetaObject::invokeMethod(s_instance, [=]() {
        QJsonObject json;
        json["type"] = "candidate";
        json["sdp"] = sdpStr;
        json["sdpMid"] = midStr;
        json["sdpMLineIndex"] = mLineIndex;
        s_instance->handleLocalIce(json);
    }, Qt::QueuedConnection);
}

JNIEXPORT void JNICALL Java_com_github_biltudas1_dialsome_WebRTCManager_onLocalSdp(
    JNIEnv* env, jobject, jstring sdp, jstring type) {
    if (!s_instance) return;

    QString sdpStr = QJniObject(sdp).toString();
    QString typeStr = QJniObject(type).toString();

    QMetaObject::invokeMethod(s_instance, [=]() {
        QJsonObject json;
        json["type"] = typeStr;
        json["sdp"] = sdpStr;
        s_instance->handleLocalSdp(json);
    }, Qt::QueuedConnection);
}
}

void Backend::startCall(const QString &roomId) {
#ifdef Q_OS_ANDROID
    setMessage("Connecting to Room: " + roomId);
    // Ensure this IP matches your server
    m_webSocket.open(QUrl("ws://192.168.31.130:8000/ws/" + roomId));

    QJniObject context = QNativeInterface::QAndroidApplication::context();
    m_webrtc = QJniObject("com/github/biltudas1/dialsome/WebRTCManager");

    if (m_webrtc.isValid()) {
        m_webrtc.callMethod<void>("init", "(Landroid/content/Context;)V", context.object());
        m_webrtc.callMethod<void>("createPeerConnection");
    }
#endif
}

void Backend::onConnected() {
    setMessage("Signaling connected. Waiting for peer...");
    // Instead of creating an offer, notify the room that you joined
    QJsonObject joinJson;
    joinJson["type"] = "join";
    m_webSocket.sendTextMessage(QJsonDocument(joinJson).toJson(QJsonDocument::Compact));
}

void Backend::handleLocalIce(const QJsonObject &json) {
    if (m_webSocket.isValid())
        m_webSocket.sendTextMessage(QJsonDocument(json).toJson(QJsonDocument::Compact));
}

void Backend::handleLocalSdp(const QJsonObject &json) {
    if (m_webSocket.isValid())
        m_webSocket.sendTextMessage(QJsonDocument(json).toJson(QJsonDocument::Compact));
}

void Backend::onTextMessageReceived(const QString &message) {
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());
    QJsonObject json = doc.object();
    QString type = json["type"].toString();

    if (type == "join") {
        // Someone else joined! Now we can safely initiate the offer
        setMessage("Peer joined. Initiating WebRTC...");
        m_webrtc.callMethod<void>("createOffer");
    } else if (type == "offer" || type == "answer") {
        m_webrtc.callMethod<void>("handleRemoteSdp",
                                  "(Ljava/lang/String;Ljava/lang/String;)V",
                                  QJniObject::fromString(json["sdp"].toString()).object(),
                                  QJniObject::fromString(type).object());
    } else if (type == "candidate") {
        m_webrtc.callMethod<void>("addRemoteIceCandidate",
                                  "(Ljava/lang/String;Ljava/lang/String;I)V",
                                  QJniObject::fromString(json["sdp"].toString()).object(),
                                  QJniObject::fromString(json["sdpMid"].toString()).object(),
                                  json["sdpMLineIndex"].toInt());
    }
}

QString Backend::message() const { return m_message; }
void Backend::setMessage(const QString &msg) { m_message = msg; emit messageChanged(); }
