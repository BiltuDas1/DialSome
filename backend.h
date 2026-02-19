#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include <QtQml/qqmlregistration.h>
#include <QString> // Include types you need

class Backend : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    // The Bridge: links 'message' to C++ functions
    Q_PROPERTY(QString message READ message WRITE setMessage NOTIFY messageChanged)

public:
    explicit Backend(QObject *parent = nullptr);

    // The Getter (required for QML to read the value)
    QString message() const { return m_message; }

    // The Setter (this is where you modify the value)
    void setMessage(const QString &newMessage) {
        if (m_message == newMessage) return;
        m_message = newMessage;
        emit messageChanged(); // This tells QML to refresh
    }

    Q_INVOKABLE void performAction();

signals:
    // This signal must match the NOTIFY name in Q_PROPERTY
    void messageChanged();

private:
    QString m_message;
};

#endif // BACKEND_H
