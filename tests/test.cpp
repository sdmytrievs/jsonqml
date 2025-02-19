
#include <QObject>
#include <QAbstractItemModelTester>
#include <QTest>
#include <QFile>

#include "jsonqml/clients/settings_client.h"
#include "jsonqml/models/json_model.h"
#include "jsonqml/models/schema_model.h"
#include "jsonqml/models/csv_model.h"

using namespace Qt::StringLiterals;

class TestModels : public QObject
{
    Q_OBJECT

    QStringList header_names{"key", "value", "comment"};

public:
    TestModels(QObject* parent = nullptr):
        QObject(parent)
    {
        my_init();
    }

    void my_init();

private slots:
    void testJsonModel();
    void testJsonShemaModel();
    void testJsonModelComplex();
    void testJsonShemaModelComplex();
    void testCSVModel();
};

void TestModels::my_init()
{
    jsonqml::Preferences::use_database = false;
    jsonqml::uiSettings().changeScemasPath("testschemas");

    auto std_schema_list = jsonio::ioSettings().Schema().getStructs(false);
    QStringList new_list;
    std::transform(std_schema_list.begin(), std_schema_list.end(),
                   std::back_inserter(new_list),
                   [](const std::string &v){ return QString::fromStdString(v); });
    qDebug() << new_list;
}

void TestModels::testJsonModel()
{
    QString current_schema_name{};
    jsonqml::JsonFreeModel model(header_names);

    constexpr auto file_name = ":/testdata/data.SimpleSchemaTest.json"_L1;
    QFile file(file_name);
    QVERIFY2(file.open(QIODevice::ReadOnly | QIODevice::Text),
             qPrintable(file_name + " cannot be opened: "_L1 + file.errorString()));

    QByteArray ba = file.readAll();
    model.setupModelData(ba.toStdString(), current_schema_name);
    QAbstractItemModelTester tester(&model);
}

void TestModels::testJsonShemaModel()
{
    QString current_schema_name{"SimpleSchemaTest"};
    jsonqml::JsonSchemaModel model(current_schema_name, header_names);

    constexpr auto file_name = ":/testdata/data.SimpleSchemaTest.json"_L1;
    QFile file(file_name);
    QVERIFY2(file.open(QIODevice::ReadOnly | QIODevice::Text),
             qPrintable(file_name + " cannot be opened: "_L1 + file.errorString()));

    QByteArray ba = file.readAll();
    model.setupModelData(ba.toStdString(), current_schema_name);
    QAbstractItemModelTester tester(&model);
}

void TestModels::testJsonModelComplex()
{
    QString current_schema_name{};
    jsonqml::JsonFreeModel model(header_names);

    constexpr auto file_name = ":/testdata/example.ComplexSchemaTest.json"_L1;
    QFile file(file_name);
    QVERIFY2(file.open(QIODevice::ReadOnly | QIODevice::Text),
             qPrintable(file_name + " cannot be opened: "_L1 + file.errorString()));

    QByteArray ba = file.readAll();
    model.setupModelData(ba.toStdString(), current_schema_name);
    QAbstractItemModelTester tester(&model);
}

void TestModels::testJsonShemaModelComplex()
{
    QString current_schema_name{"ComplexSchemaTest"};
    jsonqml::JsonSchemaModel model(current_schema_name, header_names);

    constexpr auto file_name = ":/testdata/example.ComplexSchemaTest.json"_L1;
    QFile file(file_name);
    QVERIFY2(file.open(QIODevice::ReadOnly | QIODevice::Text),
             qPrintable(file_name + " cannot be opened: "_L1 + file.errorString()));

    QByteArray ba = file.readAll();
    model.setupModelData(ba.toStdString(), current_schema_name);
    QAbstractItemModelTester tester(&model);
}

void TestModels::testCSVModel()
{
    constexpr auto file_name = ":/testdata/qq-plot-data.csv"_L1;
    QFile file(file_name);
    QVERIFY2(file.open(QIODevice::ReadOnly | QIODevice::Text),
             qPrintable(file_name + " cannot be opened: "_L1 + file.errorString()));

    QByteArray ba = file.readAll();
    jsonqml::CSVModel model(ba.toStdString());
    QAbstractItemModelTester tester(&model);
}


QTEST_APPLESS_MAIN(TestModels)

#include "test.moc"

