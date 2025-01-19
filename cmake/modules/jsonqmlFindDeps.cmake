find_package(jsonarango REQUIRED)
if(NOT jsonarango_FOUND)
    message(FATAL_ERROR "jsonarango library not found")
endif()

if(USE_SPDLOG_PRECOMPILED)
   if(NOT TARGET spdlog::spdlog)
       find_package(spdlog CONFIG REQUIRED)
       if(NOT spdlog_FOUND)
           message(FATAL_ERROR "spdlog not found")
       endif()
   endif()
endif()

find_package(jsonio17 REQUIRED)
if(NOT jsonio17_FOUND)
    message(FATAL_ERROR "jsonio17 library not found")
endif()

find_package(jsonimpex17 REQUIRED)
if(NOT jsonimpex17_FOUND)
    message(FATAL_ERROR "jsonimpex17 library not found")
endif()

if(JSONUI_NO_IMPEX MATCHES ON OR JSONUI_NO_QWEBENGINE MATCHES ON)
  find_library(MARKDOWN_LIB markdown)
  if(NOT MARKDOWN_LIB)
    message(FATAL_ERROR "markdown library not found")
  endif()
endif()

