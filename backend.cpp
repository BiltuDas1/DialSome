#include "backend.h"
#include <QTime>

Backend::Backend(QObject *parent) : QObject(parent), m_message("") {}

void Backend::performAction() {
    // Logic happens here...

    // Changing this automatically updates the UI!
    setMessage(QTime::currentTime().toString());
}
