#================= Project Setup ==========================

# CMake
cmake_minimum_required(VERSION 3.23.0...3.25.0)

# Project
# NOTE: DON'T USE TRAILING ZEROS IN VERSIONS
project(Stex
    VERSION 0.3.1
    LANGUAGES CXX
    DESCRIPTION "A tool to pack/unpack Klei TEX format atlases"
)

# Get helper scripts
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/FetchOBCMake.cmake)
fetch_ob_cmake("a23b02ed06e6d1a2c91f5d81d74c127157289094")

# Initialize project according to standard rules
include(OB/Project)
ob_standard_project_setup()

# Additional Project Variables
set(PROJECT_FORMAL_NAME "Stexatlaser")

# Configuration options
# Handled by fetched libs, but set this here formally since they aren't part of the main project
option(BUILD_SHARED_LIBS "Build Stex with shared libraries" OFF)

# C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build augmentation
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
if(CMAKE_SYSTEM_NAME STREQUAL Windows)
    enable_language("RC")
endif()

#================= Build =========================

# Import Qt
set(STEX_QT_COMPONENTS
    Core
    Gui
)

# Find Qt package
add_compile_definitions(QT_DISABLE_DEPRECATED_BEFORE=0x060000)
find_package(Qt6 REQUIRED COMPONENTS ${STEX_QT_COMPONENTS})

# Fetch Qx (build and import from source)
set(STEX_QX_COMPONENTS
    Core
    Io
    Xml
)

# Fetch Qx
include(OB/FetchQx)
ob_fetch_qx(
    REF "5b184c1ccea191ae6a885ea9a240a0d15b77b08c"
    COMPONENTS
        ${STEX_QX_COMPONENTS}
)

# Fetch libsquish
include(OB/FetchLibSquish)
ob_fetch_modern_libsquish("655dab336bd017ec1a23917a850b8384ce30cb7d")

# Process Targets
set(APP_TARGET_NAME ${PROJECT_NAMESPACE_LC}_${PROJECT_NAMESPACE_LC})
set(APP_ALIAS_NAME ${PROJECT_NAMESPACE})
add_subdirectory(app)

#--------------------Package Config-----------------------

ob_standard_project_package_config(
    COMPATIBILITY "SameMinorVersion"
    CONFIG STANDARD
        TARGET_CONFIGS
            TARGET "${PROJECT_NAMESPACE}::${APP_ALIAS_NAME}" COMPONENT "${APP_ALIAS_NAME}" DEFAULT
)

#================= Install ==========================

ob_standard_project_misc_install()

#====================== CPack ==============================

include(OB/Packaging)
ob_standard_project_package(VENDOR "oblivioncth")
