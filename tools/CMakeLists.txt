
message(STATUS "Building jsonqml tools: ${BuildTools}")
file(COPY "${CMAKE_SOURCE_DIR}/resources/data/schemas" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/")
file(COPY "${CMAKE_SOURCE_DIR}/resources/jsonqml-config.json" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/")

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
    #WIN32_EXECUTABLE TRUE
)

if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
  target_link_libraries(arangodb-view PRIVATE jsonqml::jsonqml-static)
else()
  target_link_libraries(arangodb-view PRIVATE jsonqml::jsonqml)
endif()

qt_add_executable(json_editor  json_editor_main.cpp)

qt_add_qml_module(json_editor
    URI tools2
    VERSION 1.0
    RESOURCE_PREFIX /qt/qml
    QML_FILES json_editor_main.qml
)

if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
  target_link_libraries(json_editor PRIVATE jsonqml::jsonqml-static)
else()
  target_link_libraries(json_editor PRIVATE jsonqml::jsonqml)
endif()

if(DeployApp)
  #need install not to usr/local
  qt_generate_deploy_qml_app_script(
      TARGET arangodb-view
      OUTPUT_SCRIPT deploy_script
  )

  install(SCRIPT ${deploy_script})
  install(TARGETS arangodb-view EXPORT jsonqmlTargets DESTINATION "bin")
  install(TARGETS json_editor EXPORT jsonqmlTargets DESTINATION "bin")

if(USE_SPDLOG_PRECOMPILED)
    set(dll_path "C:/Miniconda/envs/jsonqml/Library/bin")
    message(STATUS "dll path: ${dll_path}")
    message(STATUS "conda path: $ENV{CONDA_PREFIX}")
    install(FILES "${dll_path}/libcurl.dll" DESTINATION bin)
    install(FILES "${dll_path}/libpng16.dll" DESTINATION bin)
    install(FILES "${dll_path}/fmt.dll" DESTINATION bin)
    install(FILES "${dll_path}/libcrypto-3-x64.dll" DESTINATION bin)
    install(FILES "${dll_path}/libcrypto-3-x64.pdb" DESTINATION bin)
    install(FILES "${dll_path}/zlib.dll" DESTINATION bin)
    install(FILES "${dll_path}/zstd.dll" DESTINATION bin)

    install(FILES "${dll_path}/vcruntime140.dll" DESTINATION bin)
    install(FILES "${dll_path}/msvcp140.dll" DESTINATION bin)
    install(FILES "${dll_path}/opengl32sw.dll" DESTINATION bin)
    install(FILES "${dll_path}/icudt75.dll" DESTINATION bin)
    install(FILES "${dll_path}/icuin75.dll" DESTINATION bin)
    install(FILES "${dll_path}/icuuc75.dll" DESTINATION bin)
endif()
endif()


