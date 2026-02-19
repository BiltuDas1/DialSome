#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "backend.h"

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);

    // This registers your C++ class so it can be used in Main.qml
    qmlRegisterType<Backend>("com.github.biltudas1.dialsome", 1, 0, "Backend");

    QQmlApplicationEngine engine;
    const QUrl url(u"qrc:/qt/qml/DialSome/Main.qml"_qs);
    engine.load(url);

    return app.exec();
}