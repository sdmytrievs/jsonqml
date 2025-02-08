
message(STATUS "Building jsonqml tools: ${BuildTools}")

qt_add_executable(arangodb-view  db_view_main.cpp)

qt_add_qml_module(arangodb-view
    URI tools
    VERSION 1.0
    RESOURCE_PREFIX /qt/qml
    QML_FILES db_view_main.qml
)

set_target_properties(arangodb-view PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.jsonqml.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(arangodb-view PRIVATE jsonqml::jsonqml)

qt_generate_deploy_qml_app_script(
    TARGET arangodb-view
    OUTPUT_SCRIPT deploy_script
)
#need install not to usr/local
#install(SCRIPT ${deploy_script})

qt_add_executable(json_editor  json_editor_main.cpp)

qt_add_qml_module(json_editor
    URI tools2
    VERSION 1.0
    RESOURCE_PREFIX /qt/qml
    QML_FILES json_editor_main.qml
)

target_link_libraries(json_editor PRIVATE jsonqml::jsonqml)


