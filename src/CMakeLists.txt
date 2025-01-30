

# Recursively collect all header files from the install directory
file(GLOB_RECURSE HEADER_FILES_TOP  ${CMAKE_SOURCE_DIR}/include/jsonqml/*.h )

# Recursively collect all header files from the internal directory
file(GLOB_RECURSE HEADER_FILES  *.h )

# Recursively collect all source files from the current directory
file(GLOB_RECURSE SOURCE_FILES  *.cpp )

# Collect all qml files from the install directory
#file(GLOB JSQML_FILES  qml/*.qml )

# Collect all images files from the install directory
#file(GLOB JSIMAGE_FILES  ${CMAKE_SOURCE_DIR}/qml/images/*.png )

list(APPEND MODULE_HEADER_FILES_TOP
../include/jsonqml/arango_database.h
../include/jsonqml/arango_document.h
../include/jsonqml/models/base_model.h
../include/jsonqml/models/json_model.h
../include/jsonqml/models/schema_model.h
../include/jsonqml/models/select_model.h
../include/jsonqml/models/db_keys_model.h
../include/jsonqml/models/db_query_model.h
../include/jsonqml/models/csv_model.h
../include/jsonqml/clients/settings_client.h
../include/jsonqml/clients/json_client.h
../include/jsonqml/clients/vertex_client.h
../include/jsonqml/clients/edge_client.h
)

list(APPEND MODULE_HEADER_FILES
arango_database_p.h
arango_document_p.h
clients/json_client_p.h
clients/vertex_client_p.h
clients/edge_client_p.h
)

list(APPEND MODULE_SOURCE_FILES
arango_database.cpp
arango_document.cpp
models/json_model.cpp
models/schema_model.cpp
models/select_model.cpp
models/db_query_model.cpp
models/db_keys_model.cpp
models/csv_model.cpp
clients/edge_client.cpp
clients/settings_client.cpp
clients/vertex_client.cpp
clients/json_client.cpp
)

list(APPEND MODULE_QML_FILES
    qml/JsonView.qml
    qml/VertexKeys.qml
    qml/PreferencesForm.qml
    qml/VertexForm.qml
    qml/SelectTableView.qml
    qml/EdgeKeys.qml
    qml/EdgeForm.qml
)

list(APPEND MODULE_IMAGES
    qml/images/Incoming24.png
    qml/images/Outgoing24.png
    qml/images/jsonui-logo-icon.png
    qml/images/SaveCurrentRecordIcon24.png
    qml/images/ShowFilesIcon24.png
)


# Check if a shared library is to be built
if(BUILD_SHARED_LIBS)

    qt_add_library(jsonqml SHARED)
    #set_target_properties(jsonqml PROPERTIES AUTOMOC ON)
    target_link_libraries(jsonqml PUBLIC jsonio::jsonio)
    target_link_libraries(jsonqml PUBLIC Qt6::Gui Qt6::Quick)

    qt_add_qml_module(jsonqml
        URI jsonqml
        VERSION 1.0
        RESOURCE_PREFIX /qt/qml
        SOURCES
           ${SOURCE_FILES} ${HEADER_FILES_TOP} ${HEADER_FILES}
        QML_FILES
           ${MODULE_QML_FILES}
        RESOURCES
           ${MODULE_IMAGES}
     )

    add_library(jsonqml::jsonqml ALIAS jsonqml)

    #install( TARGETS jsonqml
    #     EXPORT jsonqmlTargets DESTINATION "lib" COMPONENT libraries)

endif()

                  
