#include "backend.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QCoreApplication>
#include <QDebug>
#include <QPermission>
#include <QStandardPaths>
#include <QDir>
#include <QFile>

static Backend* s_instance = nullptr;

Backend::Backend(QObject *parent) : QObject(parent) {
    s_instance = this;

    connect(this, &Backend::settingsLoaded, this, [this]() {
        QString hostUrl = "https://" + this->m_settings->value("Server/host").toString();
        QUrl url(hostUrl);
        QNetworkRequest request(url);

        qDebug() << "Attempting to connect to the server..." + hostUrl;

        QNetworkReply *reply = m_networkManager.head(request);
        connect(reply, &QNetworkReply::finished, this, [this, reply]() {
            if (reply->error() == QNetworkReply::NoError) {
                qDebug() << "Server Connected";
                this->m_serverConnected = true;
                emit this->serverConnectionChanged();
            } else {
                qDebug() << "Server Disconnected";
            }
            reply->deleteLater();
        });

    });

    connect(this, &Backend::dataCollectionFinished, this, [this](const QString &email, const QString &displayName, const QString &idToken, const QString &userID) {
        QString hostUrl = "https://" + this->m_settings->value("Server/host").toString() + "/users/register";
        QUrl url(hostUrl);
        QNetworkRequest request(url);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        request.setRawHeader("Authorization", "Bearer " + idToken.toUtf8());

        QNetworkReply *reply = m_networkManager.post(request, QByteArray());
        connect(reply, &QNetworkReply::finished, this, [this, reply, idToken]() {
            int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
            qDebug() << "HTTP Status Code:" << statusCode;

            if (statusCode == 201 || statusCode == 409) {
                QByteArray responseData = reply->readAll();

                QJsonParseError parseError;
                QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData, &parseError);

                if (parseError.error == QJsonParseError::NoError && jsonDoc.isObject()) {
                    QJsonObject jsonObj = jsonDoc.object();

                    if (jsonObj.contains("status")) {
                        bool statusValue = jsonObj.value("status").toBool();
                        qDebug() << "Server Status:" << statusValue;
                        emit this->registerFinished(idToken);
                    }
                } else {
                    qDebug() << "JSON Parse Error:" << parseError.errorString();
                    emit this->loginError("Network Error");
                }
            } else {
                qDebug() << "Network Error:" << reply->errorString();
                emit this->loginError("Network Error");
            }
            reply->deleteLater();
        });
    });

    connect(this, &Backend::registerFinished, this, [this](const QString &idToken) {
        QString hostUrl = "https://" + this->m_settings->value("Server/host").toString() + "/users/login";
        QUrl url(hostUrl);
        QNetworkRequest request(url);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        request.setRawHeader("Authorization", "Bearer " + idToken.toUtf8());

        QNetworkReply *reply = m_networkManager.post(request, QByteArray());
        connect(reply, &QNetworkReply::finished, this, [this, reply]() {
            int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
            qDebug() << "HTTP Status Code:" << statusCode;

            if (reply->error() == QNetworkReply::NoError && statusCode == 200) {
                QByteArray responseData = reply->readAll();

                QJsonParseError parseError;
                QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData, &parseError);

                if (parseError.error == QJsonParseError::NoError && jsonDoc.isObject()) {
                    QJsonObject jsonObj = jsonDoc.object();

                    if (jsonObj.contains("status")) {
                        bool statusValue = jsonObj.value("status").toBool();
                        qDebug() << "Server Status:" << statusValue;
                        
                        if (statusValue && jsonObj.contains("data")) {
                            QJsonObject dataJson = jsonObj.value("data").toObject();
                            if (dataJson.contains("id") && dataJson.contains("email") && dataJson.contains("firstname") && dataJson.contains("lastname")) {
                                QString id = dataJson.value("id").toString();
                                QString email = dataJson.value("email").toString();
                                QString firstname = dataJson.value("firstname").toString();

                                if (dataJson.contains("jwt")) {
                                    QJsonObject jwtJson = dataJson.value("jwt").toObject();
                                    if (jwtJson.contains("refresh_token") && jwtJson.contains("access_token")) {
                                        QString refresh_token = jwtJson.value("refresh_token").toString();
                                        this->m_jwtAccessToken = jwtJson.value("access_token").toString();
                                        emit this->loginFinished(email, firstname, id, refresh_token);
                                    }
                                }
                            }
                        }
                    }
                } else {
                    qDebug() << "JSON Parse Error:" << parseError.errorString();
                    emit this->loginError("Network Error");
                }
            } else {
                qDebug() << "Network Error:" << reply->errorString();
                emit this->loginError("Network Error");
            }
            reply->deleteLater();
        });
    });

    connect(this, &Backend::loginFinished, this, [this](const QString &email, const QString &displayName, const QString &userID, const QString &refresh_token) {
        this->m_storage.saveRefreshToken(refresh_token);
    });

    connect(&m_webSocket, &QWebSocket::connected, this, &Backend::onConnected);
    connect(&m_webSocket, &QWebSocket::textMessageReceived, this, &Backend::onTextMessageReceived);
}

extern "C" {
JNIEXPORT void JNICALL Java_com_github_biltudas1_dialsome_WebRTCManager_onLocalIceCandidate(
    JNIEnv* env, jobject, jstring sdp, jstring mid, jint index) {
    if (!s_instance) return;
    QString sdpStr = QJniObject(sdp).toString();
    QString midStr = QJniObject(mid).toString();
    QMetaObject::invokeMethod(s_instance, [=]() {
        QJsonObject json;
        json["type"] = "candidate";
        json["sdp"] = sdpStr;
        json["sdpMid"] = midStr;
        json["sdpMLineIndex"] = (int)index;
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

JNIEXPORT void JNICALL Java_com_github_biltudas1_dialsome_WebRTCManager_onCallEstablished(JNIEnv*, jobject) {
    if (!s_instance) return;
    QMetaObject::invokeMethod(s_instance, [=]() {
        s_instance->setMessage("Call Connected! Audio is live.");
    }, Qt::QueuedConnection);
}
}

void Backend::startCall(const QString &roomId) {
#ifdef Q_OS_ANDROID
    QMicrophonePermission micPermission;
    qApp->requestPermission(micPermission, [this, roomId](const QPermission &permission) {
        if (permission.status() != Qt::PermissionStatus::Granted) {
            setMessage("Microphone permission denied!");
            return;
        }

        setMessage("Connecting to Room: " + roomId);
        // Ensure this IP matches your local server IP
        QString wsURL = "wss://" + this->m_settings->value("Server/host").toString() + "/ws/" + roomId;
        m_webSocket.open(QUrl(wsURL));

        QJniObject context = QNativeInterface::QAndroidApplication::context();
        m_webrtc = QJniObject("com/github/biltudas1/dialsome/WebRTCManager");

        if (m_webrtc.isValid()) {
            m_webrtc.callMethod<void>("init", "(Landroid/content/Context;)V", context.object());
            // Pre-initialize PC so tracks are ready
            m_webrtc.callMethod<void>("createPeerConnection");
        }
    });
#endif
}

void Backend::onConnected() {
    setMessage("Signaling connected. Waiting for peer...");
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

void Backend::fetchStartupData() {
    qDebug() << "App started: Fetching initial data...";
    QString directory = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    QDir dir;
    if (!dir.exists(directory)) {
        dir.mkpath(directory);
    }

    QString filePath = directory + "/settings.ini";

    if (!QFile::exists(filePath)) {
        QUrl url(GITHUB_CONFIGURATIONS);
        QNetworkRequest request(url);

        QNetworkReply *reply = m_networkManager.get(request);
        connect(reply, &QNetworkReply::finished, this, [this, reply, filePath]() {
            if (reply->error() == QNetworkReply::NoError) {
                QByteArray rawData = reply->readAll();

                QFile file(filePath);
                if (file.open(QIODevice::WriteOnly)) {
                    file.write(rawData);
                    file.close();
                    qDebug() << "File saved successfully at:" << filePath;

                    this->m_settings.reset(new QSettings(filePath, QSettings::IniFormat));
                    emit this->settingsLoaded();
                } else {
                    qDebug() << "Unable to save file: " << filePath;
                }
            } else {
                qDebug() << "Unable to fetch settings from " << reply->errorString();
            }
            reply->deleteLater();
        });
    } else {
        qDebug() << "Loading existing settings from:" << filePath;
        this->m_settings.reset(new QSettings(filePath, QSettings::IniFormat));
        emit this->settingsLoaded();
    }
}

bool Backend::serverConnected() const { return m_serverConnected; }


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

void Backend::loginWithGoogle(const QString &webClientId) {
#ifdef Q_OS_ANDROID
    QJniObject context = QNativeInterface::QAndroidApplication::context();
    m_googleLogin = QJniObject("com/github/biltudas1/dialsome/GoogleLoginManager");

    bool autoLogin = this->m_storage.exists("refresh_token");
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

void Backend::showToast(const QString &message) {
#ifdef Q_OS_ANDROID
    // Retrieve the current Android activity instance from the Qt framework
    QJniObject activity = QNativeInterface::QAndroidApplication::context();
    
    if (activity.isValid()) {
        QJniObject javaMessage = QJniObject::fromString(message);
        
        // Call the static Java method in your new AndroidUtils class
        QJniObject::callStaticMethod<void>(
            "com/github/biltudas1/dialsome/AndroidUtils",
            "showToast",
            "(Landroid/app/Activity;Ljava/lang/String;)V",
            activity.object<jobject>(),
            javaMessage.object<jstring>()
        );
    }
#else
    // Fallback for desktop testing
    qDebug() << "Toast message:" << message;
#endif
}

void Backend::createFile(const QString &fileName, const QString &content) {
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/" + fileName;
    QFile file(path);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << content;
        file.close();
    }
}

bool Backend::isLoggedIn() {
    if (this->m_storage.exists("refresh_token")) {
        return true;
    } else {
        return false;
    }
}
