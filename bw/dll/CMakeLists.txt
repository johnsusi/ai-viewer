cmake_minimum_required (VERSION 2.8)

include(GenerateExportHeader)

project(ai-viewer/bw)

add_library(BroadcastModule SHARED BroadcastModule.cpp BroadcastModule.h Dll.cpp)

GENERATE_EXPORT_HEADER( BroadcastModule
             BASE_NAME BroadcastModule
             EXPORT_MACRO_NAME BroadcastModule_EXPORT
             EXPORT_FILE_NAME BroadcastModule_Export.h
             STATIC_DEFINE BroadcastModule_BUILT_AS_STATIC
)
