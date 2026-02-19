#include <QGuiApplication>
#include <QQuickStyle>
#include <QQmlApplicationEngine>
#include <QIcon>
#include <QSslSocket>
#include <QDebug>

void checkSslSupport() {
    qDebug() << "SSL Library Build Version:" << QSslSocket::sslLibraryBuildVersionString();
    qDebug() << "SSL Library Runtime Version:" << QSslSocket::sslLibraryVersionString();
    qDebug() << "Supports SSL:" << QSslSocket::supportsSsl();

    if (!QSslSocket::supportsSsl()) {
        qDebug() << "Error: OpenSSL libraries were not found at runtime!";
    } else {
        qDebug() << "Success: OpenSSL is working!";
    }
}

int main(int argc, char *argv[])
{
    QQuickStyle::setStyle("Material");
    QGuiApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/icons/logo.png"));

    QQmlApplicationEngine engine;
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("DialSome", "Main");

    return app.exec();
}
