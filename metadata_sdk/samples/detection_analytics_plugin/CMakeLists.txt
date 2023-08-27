## Copyright 2018-present Network Optix, Inc. Licensed under MPL 2.0: www.mozilla.org/MPL/2.0/

cmake_minimum_required(VERSION 3.3.2)
project(detection_analytics_plugin)

set(metadataSdkDir "" CACHE PATH "Path to unpacked VMS Metadata SDK zip.")
if(metadataSdkDir STREQUAL "")
    set(metadataSdkDir ${CMAKE_CURRENT_LIST_DIR}/../..) #< Assume building samples inside the SDK.
    if(NOT EXISTS ${metadataSdkDir}/src/nx/sdk OR NOT EXISTS ${metadataSdkDir}/nx_kit/src/nx/kit)
        message(FATAL_ERROR "Define metadataSdkDir cache variable to point to the unzipped SDK.")
    endif()
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")


#isntall conan
execute_process(
   COMMAND conan install ${CMAKE_SOURCE_DIR} --build missing
   WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

#Include Conan-generated CMake configuration
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

#################################

if(UNIX)
    # In Linux, for the plugin .so library, set `rpath` to "$ORIGIN" and do not set `runpath`, thus
    # enabling the lookup of the dependencies in the plugin dir first.
    string(APPEND CMAKE_SHARED_LINKER_FLAGS " -Wl,--disable-new-dtags")
endif()
set(CMAKE_SKIP_BUILD_RPATH ON)
set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)
set(CMAKE_INSTALL_RPATH "$ORIGIN")

if(WIN32)
    string(APPEND CMAKE_CXX_FLAGS " /MP") #< Use all CPU cores by MSVC.
    # Do not create separate .pdb files for object files (workaround for mspdbsrv.exe bug).
    add_compile_options("/Z7")
endif()

if(WIN32)
    set(API_EXPORT_MACRO "__declspec(dllexport)")
else()
    set(API_EXPORT_MACRO "__attribute__((visibility(\"default\")))")
endif()

#--------------------------------------------------------------------------------------------------
# Define nx_kit lib, static.

set(nxKitLibraryType "STATIC" CACHE STRING "" FORCE)
set(nxKitWithTests "NO" CACHE STRING "" FORCE)

add_subdirectory(${metadataSdkDir}/nx_kit ${CMAKE_CURRENT_BINARY_DIR}/nx_kit)

#--------------------------------------------------------------------------------------------------
# Define nx_sdk lib, static, depends on nx_kit.

set(SDK_SRC_DIR ${metadataSdkDir}/src)
file(GLOB_RECURSE SDK_SRC CONFIGURE_DEPENDS ${SDK_SRC_DIR}/*)

add_library(nx_sdk STATIC ${SDK_SRC})
target_include_directories(nx_sdk PUBLIC ${SDK_SRC_DIR})
target_link_libraries(nx_sdk PRIVATE nx_kit)

target_compile_definitions(nx_sdk PRIVATE NX_PLUGIN_API=${API_EXPORT_MACRO}) #< for nxLibContext()

#--------------------------------------------------------------------------------------------------
# Define detection_analytics_plugin lib, dynamic, depends on nx_kit and nx_sdk.

set(DETECTION_ANALYTICS_PLUGIN_SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
file(GLOB_RECURSE DETECTION_ANALYTICS_PLUGIN_SRC CONFIGURE_DEPENDS ${DETECTION_ANALYTICS_PLUGIN_SRC_DIR}/*)

add_library(detection_analytics_plugin SHARED ${DETECTION_ANALYTICS_PLUGIN_SRC})
target_include_directories(detection_analytics_plugin PRIVATE ${DETECTION_ANALYTICS_PLUGIN_SRC})
target_link_libraries(detection_analytics_plugin PRIVATE nx_kit nx_sdk)

target_compile_definitions(detection_analytics_plugin
    PRIVATE NX_PLUGIN_API=${API_EXPORT_MACRO}
)



# Define DahuaSDK lib, dynamic.

set(EXTERNAL_SDK_DIR ${CMAKE_CURRENT_LIST_DIR}/lib)
file(GLOB_RECURSE EXTERNAL_SDK_SRC CONFIGURE_DEPENDS ${EXTERNAL_SDK_DIR}/*)
add_library(external_sdk SHARED IMPORTED)
if(WIN32)
    set_target_properties(external_sdk PROPERTIES
    IMPORTED_IMPLIB         ${EXTERNAL_SDK_DIR}/dhnetsdk.lib
    INTERFACE_INCLUDE_DIRECTORIES  ${EXTERNAL_SDK_DIR}/include)
else()
    include_directories(${EXTERNAL_SDK_DIR}/include)
    find_library(external_sdk libdhnetsdk HINTS ${EXTERNAL_SDK_DIR})
    message("Please set external_sdk variable to location of libdhnetsdk")
    set_target_properties(external_sdk PROPERTIES
    IMPORTED_LOCATION     /opt/lib/libdhnetsdk.so)
endif()
target_include_directories(detection_analytics_plugin PRIVATE ${EXTERNAL_SDK_DIR})
target_link_libraries(detection_analytics_plugin PRIVATE external_sdk)

#define boost
find_package(Boost 1.78.0 REQUIRED)

target_include_directories(detection_analytics_plugin PRIVATE ${Boost_INCLUDE_DIRS})

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS}) 
    target_link_libraries(detection_analytics_plugin PRIVATE Boost::boost)
endif()




