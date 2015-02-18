cmake_minimum_required (VERSION 2.8)

include(GenerateExportHeader)

project(bw)

if (WIN32)
  include_directories(
      "C:/bwapi/include"
      "C:/zmq/include"
      "C:/curl/include")
  link_directories(
      "C:/bwapi/lib"
      "C:/zmq/lib"
      "C:/curl/lib")
endif()

add_library(BroadcastModule SHARED
    Dll.cpp
    BroadcastModule.cpp
    BroadcastModule.h)

target_link_libraries(BroadcastModule
    BWAPI.lib
    libzmq-v120-mt-4_0_4.lib)

GENERATE_EXPORT_HEADER( BroadcastModule
             BASE_NAME BroadcastModule
             EXPORT_MACRO_NAME BroadcastModule_EXPORT
             EXPORT_FILE_NAME BroadcastModule_Export.h
             STATIC_DEFINE BroadcastModule_BUILT_AS_STATIC
)
