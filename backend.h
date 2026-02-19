#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include <QString>
#include <QtQmlIntegration/qqmlintegration.h>

class Backend : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QString message READ message NOTIFY messageChanged)

public:
    explicit Backend(QObject *parent = nullptr);

    QString message() const;

    Q_INVOKABLE void startWebRTCTest();

signals:
    void messageChanged();

private:
    void setMessage(const QString &msg);

private:
    QString m_message = "Ready";
};

#endif // BACKEND_H
