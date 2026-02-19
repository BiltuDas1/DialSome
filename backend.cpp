#include "backend.h"

#include <QJniObject>
#include <QGuiApplication>
#include <QCoreApplication>
#include <QDebug>

Backend::Backend(QObject *parent)
    : QObject(parent)
{
}

QString Backend::message() const
{
    return m_message;
}

void Backend::setMessage(const QString &msg)
{
    if (m_message == msg)
        return;

    m_message = msg;
    emit messageChanged();
}

void Backend::startWebRTCTest()
{
#ifdef Q_OS_ANDROID

    setMessage("Initializing WebRTC...");

    // Get Android activity context
    QJniObject context =
        QNativeInterface::QAndroidApplication::context();

    if (!context.isValid()) {
        setMessage("Android context not found!");
        return;
    }

    // Create Java WebRTCManager object
    QJniObject webrtc(
        "com/github/biltudas1/dialsome/WebRTCManager",
        "()V"
    );

    if (!webrtc.isValid()) {
        setMessage("WebRTCManager not found!");
        return;
    }

    // Call init(Context)
    webrtc.callMethod<void>(
        "init",
        "(Landroid/content/Context;)V",
        context.object<jobject>()
    );

    // Create peer connection
    webrtc.callMethod<void>("createPeerConnection");

    // Create offer
    webrtc.callMethod<void>("createOffer");

    setMessage("WebRTC started");

#else
    setMessage("WebRTC only supported on Android");
#endif
}
