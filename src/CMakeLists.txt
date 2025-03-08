

# Recursively collect all header files from the install directory
file(GLOB_RECURSE HEADER_FILES_TOP  ${JSONQML_HEADER_DIR}/jsonqml/*.h )

# Recursively collect all header files from the internal directory
file(GLOB_RECURSE HEADER_FILES  *.h )

# Recursively collect all source files from the current directory
file(GLOB_RECURSE SOURCE_FILES  *.cpp )

list(APPEND MODULE_QML_FILES
    qml/JsonView.qml
    qml/JsonEditor.qml
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
    target_link_libraries(jsonqml PUBLIC jsonio::jsonio)
    target_link_libraries(jsonqml PUBLIC Qt6::Gui Qt6::Quick)

    install( TARGETS jsonqml
         EXPORT jsonqmlTargets DESTINATION "lib" COMPONENT libraries)
    install( TARGETS jsonqmlplugin
              EXPORT jsonqmlTargets DESTINATION "lib" COMPONENT libraries)

endif()

# Check if a static library is to be built
if(BUILD_STATIC_LIBS)

   qt_add_library(jsonqml-static STATIC)
   qt_add_qml_module(jsonqml-static
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

   add_library(jsonqml::jsonqml-static ALIAS jsonqml-static)
   target_link_libraries(jsonqml-static PUBLIC jsonio::jsonio-static)
   target_link_libraries(jsonqml-static PUBLIC Qt6::Gui Qt6::Quick)

if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
     target_link_libraries( jsonqml-static PRIVATE libcurl)
else()
     target_link_libraries( jsonqml-static PRIVATE curl)
endif()
if(USE_SPDLOG_PRECOMPILED)
    target_compile_definitions(jsonqml-static PUBLIC USE_SPDLOG_PRECOMPILED)
    target_link_libraries(jsonqml-static PUBLIC spdlog::spdlog )
endif()

   install( TARGETS jsonqml-static_resources_1
       EXPORT jsonqmlTargets DESTINATION "lib" COMPONENT libraries)
   install( TARGETS jsonqml-static_resources_2
       EXPORT jsonqmlTargets DESTINATION "lib" COMPONENT libraries)
   install( TARGETS jsonqml-static
       EXPORT jsonqmlTargets DESTINATION "lib" COMPONENT libraries)
   install( TARGETS jsonqml-staticplugin_init
        EXPORT jsonqmlTargets DESTINATION "lib" COMPONENT libraries)
   install( TARGETS jsonqml-staticplugin
        EXPORT jsonqmlTargets DESTINATION "lib" COMPONENT libraries)

endif()

install(
    DIRECTORY   "${JSONQML_HEADER_DIR}/jsonqml"
    DESTINATION include
)
