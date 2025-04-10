if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
find_library(CURL_LIB libcurl)
else()
find_library(CURL_LIB curl)
endif()
if(NOT CURL_LIB)
  message(FATAL_ERROR "curl library not found")
endif()

find_package(arango-cpp REQUIRED)
if(NOT arango-cpp_FOUND)
    message(FATAL_ERROR "arango-cpp library not found")
endif()

if(USE_SPDLOG_PRECOMPILED)
#   if(NOT TARGET spdlog::spdlog)
#       find_package(spdlog CONFIG REQUIRED)
#   endif()
   if(NOT TARGET fmt::fmt)
       find_package(fmt CONFIG REQUIRED)
   endif()
endif()

find_package(jsonio REQUIRED)
if(NOT jsonio_FOUND)
    message(FATAL_ERROR "jsonio library not found")
endif()

