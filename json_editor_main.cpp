#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QIcon>
#include "include/jsonqml/clients/json_client.h"
#include "include/jsonqml/clients/settings_client.h"

int main(int argc, char *argv[])
{
    //qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));

    QGuiApplication app(argc, argv);
    app.setWindowIcon(QIcon("qrc:///resources/images/jsonui-logo-icon.png.png"));
    jsonqml::JSonClient client;

    QQmlApplicationEngine engine;

    qmlRegisterSingletonInstance("Qt.jsonqml.qobjectPreferences", 1, 0, "Preferences", &jsonqml::uiSettings());

    const QUrl url("qrc:///qml/json_main.qml");
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);


    engine.rootContext()->setContextProperty("client", &client);
    engine.load(url);

    return app.exec();
}
