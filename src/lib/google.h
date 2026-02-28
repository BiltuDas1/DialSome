#ifndef GOOGLE_H
#define GOOGLE_H

#include <QObject>
#include <QJniObject>
#include <QString>
#include <QtQmlIntegration/qqmlintegration.h>
#include <QCoreApplication>
#include "securestorage.h"
#include <QPointer>

class Google : public QObject {
    Q_OBJECT
    QML_ELEMENT

public:
    explicit Google(QObject *parent = nullptr, SecureStorage *storage = nullptr);
    Q_INVOKABLE void loginWithGoogle(const QString &webClientId);
    Q_INVOKABLE bool isLoggedIn();

signals:
    void dataCollectionFinished(const QString &email, const QString &displayName, const QString &idToken, const QString &userID);
    void dataCollectionError(const QString &error);

private:
    QJniObject m_googleLogin;
    QPointer<SecureStorage> m_storage;
};

#endif // GOOGLE_H
