#include "securestorage.h"
#include <QCoreApplication>
#include <QJniEnvironment>

SecureStorage::SecureStorage() {
    // Get the Android Context from the Qt Activity
    QJniObject context = QNativeInterface::QAndroidApplication::context();

    if (context.isValid()) {
        // Initialize our Java SecureStorage class
        m_javaObject = QJniObject("com/github/biltudas1/dialsome/SecureStorage",
                                  "(Landroid/content/Context;)V",
                                  context.object());
    }
}

bool SecureStorage::isValid() const {
    return m_javaObject.isValid();
}

void SecureStorage::save(const QString &key, const QString &value) {
    if (!isValid()) return;

    m_javaObject.callMethod<void>("saveData",
                                  "(Ljava/lang/String;Ljava/lang/String;)V",
                                  QJniObject::fromString(key).object(),
                                  QJniObject::fromString(value).object());
}

QString SecureStorage::get(const QString &key) {
    if (!isValid()) return QString();

    QJniObject result = m_javaObject.callMethod<QJniObject>("getData",
                                                            "(Ljava/lang/String;)Ljava/lang/String;",
                                                            QJniObject::fromString(key).object());
    return result.toString();
}

void SecureStorage::clear() {
    if (isValid()) {
        m_javaObject.callMethod<void>("clearData");
    }
}

// Dialsome-specific Helpers
void SecureStorage::saveRefreshToken(const QString &token) {
    save("refresh_token", token);
}

QString SecureStorage::getRefreshToken() {
    return get("refresh_token");
}

bool SecureStorage::exists(const QString &key) {
    if (!isValid()) return false;

    // We call the 'hasKey' method and expect a boolean return
    return m_javaObject.callMethod<jboolean>("hasKey", 
                                            "(Ljava/lang/String;)Z", 
                                            QJniObject::fromString(key).object());
}
