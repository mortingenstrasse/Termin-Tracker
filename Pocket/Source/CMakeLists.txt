cmake_minimum_required(VERSION 3.16)
project(TerminPocket LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED IMPORTED_TARGET sdl2)
pkg_check_modules(CJSON QUIET IMPORTED_TARGET libcjson)
if(NOT CJSON_FOUND)
  # The PortMaster builder restricts pkg-config's search paths to its sysroot.
  # Ubuntu's libcjson-dev installs its headers and library in system paths.
  find_path(CJSON_INCLUDE_DIR NAMES cJSON.h PATH_SUFFIXES cjson REQUIRED)
  find_library(CJSON_LIBRARY NAMES cjson REQUIRED)
  add_library(PocketCJSON UNKNOWN IMPORTED)
  set_target_properties(PocketCJSON PROPERTIES
    IMPORTED_LOCATION "${CJSON_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${CJSON_INCLUDE_DIR}")
  set(CJSON_TARGET PocketCJSON)
else()
  set(CJSON_TARGET PkgConfig::CJSON)
endif()
add_executable(terminpocket Pocket.cpp EngineV2.cpp CaptureDSP.cpp)
target_compile_definitions(terminpocket PRIVATE TM_TRACKS=8)
target_compile_options(terminpocket PRIVATE -O3)
target_link_libraries(terminpocket PRIVATE PkgConfig::SDL2 ${CJSON_TARGET})
