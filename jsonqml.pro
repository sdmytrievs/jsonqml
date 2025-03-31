TARGET = jsonqml-test
TEMPLATE = app

CONFIG += thread
CONFIG += c++20

QT += quick
QT += charts printsupport svg
CONFIG += qmltypes
QML_IMPORT_NAME = Scanner
QML_IMPORT_MAJOR_VERSION = 1

# Define the directory where jsonio source code is located
JSONIO_DIR =  $$PWD/../jsonio/src
JSONIO_HEADERS_DIR =  $$PWD/../jsonio/include
JSONQML_DIR =  $$PWD/src
JSONQML_HEADERS_DIR =  $$PWD/include

DEPENDPATH   += $$JSONIO_HEADERS_DIR
DEPENDPATH   += $$JSONQML_HEADERS_DIR

INCLUDEPATH   += $$JSONIO_HEADERS_DIR
INCLUDEPATH   += $$JSONQML_HEADERS_DIR

HEADERS += \
    include/jsonqml/arango_database.h \
    include/jsonqml/arango_document.h \
    include/jsonqml/charts/chart_model.h \
    include/jsonqml/charts/chart_view.h \
    include/jsonqml/charts/graph_data.h \
    include/jsonqml/charts/legend_data.h \
    include/jsonqml/charts/legend_model.h \
    include/jsonqml/clients/table_client.h \
    include/jsonqml/clients/chart_client.h \
    include/jsonqml/models/base_model.h \
    include/jsonqml/models/json_model.h \
    include/jsonqml/models/schema_model.h \
    include/jsonqml/models/select_model.h \
    include/jsonqml/models/db_keys_model.h \
    include/jsonqml/models/db_query_model.h \
    include/jsonqml/models/csv_model.h \
    include/jsonqml/clients/settings_client.h \
    include/jsonqml/clients/json_client.h \
    include/jsonqml/clients/vertex_client.h \
    include/jsonqml/clients/edge_client.h \
    src/arango_database_p.h \
    src/arango_document_p.h \
    src/charts/markershapes.h \
    src/clients/json_client_p.h \
    src/clients/vertex_client_p.h \
    src/clients/edge_client_p.h \
    src/clients/table_client_p.h \

SOURCES += \
    #main.cpp \
    src/arango_database.cpp \
    src/arango_document.cpp \
    src/charts/chart_model.cpp \
    src/charts/chart_view.cpp \
    src/charts/graph_data.cpp \
    src/charts/legend_data.cpp \
    src/charts/legend_model.cpp \
    src/charts/markershapes.cpp \
    src/clients/table_client.cpp \
    src/clients/chart_client.cpp \
    src/models/base_model.cpp \
    src/models/json_model.cpp \
    src/models/schema_model.cpp \
    src/models/select_model.cpp \
    src/models/db_query_model.cpp \
    src/models/db_keys_model.cpp \
    src/models/csv_model.cpp \
    src/clients/edge_client.cpp \
    src/clients/settings_client.cpp \
    src/clients/vertex_client.cpp \
    src/clients/json_client.cpp \
    #tools/db_view_main.cpp \
    tools/charts_view_main.cpp \
    #tools/json_editor_main.cpp

#resources.files = main.qml
resources.prefix = /$${TARGET}

RESOURCES += src/jsonqml.qrc \
             tools/tools.qrc

win32:LIBS +=  -larango-cpp-static
!win32:LIBS += -larango-cpp

OBJECTS_DIR   = obj
MOC_DIR = tmp
UI_DIR  = $$MOC_DIR

include($$JSONIO_DIR/jsonio.pri)


# Additional import path used to resolve QML modules in Qt Creator's code model
# QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
# QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# https://github.com/RicardoRagel/qt-qml-sample-app/tree/master/qml
# https://doc.qt.io/qt-6/qml-qtquick-treeview.html

DISTFILES += \
    Form.qml \
    FormForm.ui.qml \
    resources/tmp/ChartSettings.qml \
    src/qml/SymbolForm.qml



