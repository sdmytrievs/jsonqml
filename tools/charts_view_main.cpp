#include <QtGui/QGuiApplication>
#include <QtQml/QQmlApplicationEngine>
#include <QtQml/QQmlContext>
#include <QtGui/QIcon>
#include "jsonqml/clients/chart_client.h"
#include "jsonqml/clients/settings_client.h"

int main(int argc, char *argv[])
{
    //qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));
    jsonqml::Preferences::use_database = false;
    jsonqml::Preferences::use_schemas = false;
    jsonio::JsonioSettings::settingsFileName = "jsonqml-config.json";

    QGuiApplication app(argc, argv);
    app.setWindowIcon(QIcon("qrc:///resources/images/jsonui-logo-icon.png"));
    jsonqml::ChartClient client;

    QQmlApplicationEngine engine;
    qmlRegisterSingletonInstance("Qt.jsonqml.qobjectPreferences", 1, 0, "Preferences", &jsonqml::uiSettings());

    const QUrl url("qrc:/qt/qml/tools3/charts_view_main.qml");
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);


    engine.rootContext()->setContextProperty("client", &client);
    engine.load(url);

    return app.exec();
}
