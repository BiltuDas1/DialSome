#include "backend.h"
#include <QDebug>

Backend::Backend(QObject *parent) : QObject(parent)
{
    // Constructor logic if needed
}

void Backend::performAction()
{
    qDebug() << "Action performed in C++ source file!";
}
