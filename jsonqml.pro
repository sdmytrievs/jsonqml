TARGET = jsonqml-test
TEMPLATE = app

CONFIG += thread
CONFIG += c++20

QT += quick
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
    include/jsonqml/models/base_model.h \
    include/jsonqml/models/json_model.h \
    include/jsonqml/models/schema_model.h \
    include/jsonqml/models/select_model.h \
    include/jsonqml/models/db_query_model.h \
    include/jsonqml/models/db_keys_model.h \
    include/jsonqml/models/csv_model.h \
    include/jsonqml/clients/settings_client.h \
    include/jsonqml/clients/json_client.h \
    include/jsonqml/clients/vertex_client.h \
    include/jsonqml/clients/edge_client.h \
    src/clients/json_client_p.h \
    src/clients/vertex_client_p.h \
    src/clients/edge_client_p.h \

SOURCES += \
    #json_editor_main.cpp \
    main.cpp \
    src/arango_database.cpp \
    src/arango_document.cpp \
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

#resources.files = main.qml
resources.prefix = /$${TARGET}

RESOURCES += resources\
             jsonqml.qrc

win32:LIBS +=  -larango-cpp-static
!win32:LIBS += -larango-cpp

OBJECTS_DIR   = obj

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
    qml/JsonView.qml \
    qml/json_main.qml


