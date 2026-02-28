#include "google.h"

static Google* s_instance = nullptr;

Google::Google(QObject *parent, SecureStorage *storage) : QObject(parent) {
  s_instance = this;
  if (storage == nullptr) {
      this->m_storage = new SecureStorage();
  } else {
      this->m_storage = storage;
  }
}

extern "C" {
JNIEXPORT void JNICALL Java_com_github_biltudas1_dialsome_GoogleLoginManager_onLoginSuccess(
    JNIEnv* env, jobject, jstring idToken, jstring email, jstring displayName, jstring userID) {
    if (!s_instance) return;
    QString token = QJniObject(idToken).toString();
    QString mail = QJniObject(email).toString();
    QString name = QJniObject(displayName).toString();
    QString userid = QJniObject(userID).toString();

    qDebug() << "================ GOOGLE LOGIN SUCCESS ================";
    qDebug() << "Name: " << name;
    qDebug() << "UserID: " << userid;
    qDebug() << "Mail: " << mail;
    qDebug() << "Token (first 15 chars): " << token.left(15) << "...";
    qDebug() << "======================================================";

    if (!s_instance) {
        qDebug() << "ERROR: s_instance is null in onLoginSuccess!";
        return;
    }
    
    QMetaObject::invokeMethod(s_instance, [=]() {
        emit s_instance->dataCollectionFinished(mail, name, token, userid);
    }, Qt::QueuedConnection);
}

JNIEXPORT void JNICALL Java_com_github_biltudas1_dialsome_GoogleLoginManager_onLoginError(
    JNIEnv* env, jobject, jstring error) {

    QString errorStr = QJniObject(error).toString();
    qDebug() << "GOOGLE LOGIN ERROR: " << errorStr;
    
    if (!s_instance) return;
    QMetaObject::invokeMethod(s_instance, [=]() {
        emit s_instance->dataCollectionError(errorStr);
    }, Qt::QueuedConnection);
}
}

void Google::loginWithGoogle(const QString &webClientId) {
#ifdef Q_OS_ANDROID
    QJniObject context = QNativeInterface::QAndroidApplication::context();
    m_googleLogin = QJniObject("com/github/biltudas1/dialsome/GoogleLoginManager");

    bool autoLogin = this->m_storage->exists("refresh_token");
    if (m_googleLogin.isValid()) {
        m_googleLogin.callMethod<void>("signIn", 
            "(Landroid/content/Context;Ljava/lang/String;Z)V", 
            context.object(), 
            QJniObject::fromString(webClientId).object(),
            autoLogin
        );
    }
#endif
}

bool Google::isLoggedIn() {
    if (this->m_storage->exists("refresh_token")) {
        return true;
    } else {
        return false;
    }
}
