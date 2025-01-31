#include <QtGui/QGuiApplication>
#include <QtQml/QQmlApplicationEngine>
#include <QtQml/QQmlContext>
#include <QtGui/QIcon>
#include "jsonqml/clients/edge_client.h"
#include "jsonqml/clients/settings_client.h"

int main(int argc, char *argv[])
{
    //qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));

    QGuiApplication app(argc, argv);
    app.setWindowIcon(QIcon("qrc:/qt/qml/jsonqml/qml/images/jsonui-logo-icon.png"));

    jsonqml::VertexClient vertexClient;
    jsonqml::EdgeClient edgeClient;

    QQmlApplicationEngine engine;

    qmlRegisterSingletonInstance("Qt.jsonqml.qobjectPreferences", 1, 0, "Preferences", &jsonqml::uiSettings());

    const QUrl url("qrc:/qt/qml/tools/db_view_main.qml");
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);


    engine.rootContext()->setContextProperty("vertexClient", &vertexClient);
    engine.rootContext()->setContextProperty("edgeClient", &edgeClient);
    engine.load(url);

    return app.exec();
}
