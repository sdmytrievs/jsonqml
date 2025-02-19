message(STATUS "Building test suite: ${BuildTests}")
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/testschemas" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/")
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/testdata"    DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/")


# Unit Test
include(CTest)
enable_testing()

qt_add_executable(models_tester  test.cpp )
target_link_libraries(models_tester PRIVATE jsonqml::jsonqml Qt6::Test)

# Resources:
set(model_resource_files
   "testschemas/schema_test.schema.json"
   "testdata/data.SimpleSchemaTest.json"
   "testdata/example.ComplexSchemaTest.json"
   "testdata/qq-plot-data.csv"
)

qt_add_resources(models_tester "treemodel"
    PREFIX
        "/"
    FILES
        ${model_resource_files}
)

add_test(NAME models_tester
         COMMAND models_tester)

