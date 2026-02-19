#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include <QtQml/qqmlregistration.h>

class Backend : public QObject
{
    Q_OBJECT
    QML_ELEMENT
public:
    explicit Backend(QObject *parent = nullptr);

    // Declaration of the function
    Q_INVOKABLE void performAction();
};

#endif // BACKEND_H
