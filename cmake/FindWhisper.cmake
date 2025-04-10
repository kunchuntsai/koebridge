# FindWhisper.cmake
#
# Finds the Whisper library
#
# This will define the following variables:
#   Whisper_FOUND        - True if the system has Whisper
#   Whisper_INCLUDE_DIRS - Whisper include directory
#   Whisper_LIBRARIES    - Whisper library path

# Try to find package config first
find_package(Whisper QUIET CONFIG)

if(Whisper_FOUND)
    return()
endif()

# Look for the header file
find_path(Whisper_INCLUDE_DIR
    NAMES whisper.h
    PATHS
        ${WHISPER_ROOT}
        $ENV{WHISPER_ROOT}
        ${CMAKE_SOURCE_DIR}/third_party/whisper
        ${CMAKE_SOURCE_DIR}/third_party/whisper.cpp
        /usr
        /usr/local
        /opt/homebrew
        /opt/homebrew/Cellar/whisper-cpp/1.7.5/libinternal
    PATH_SUFFIXES
        include
        include/whisper
)

# Look for the library
find_library(Whisper_LIBRARY
    NAMES whisper
    PATHS
        ${WHISPER_ROOT}
        $ENV{WHISPER_ROOT}
        ${CMAKE_SOURCE_DIR}/third_party/whisper.cpp
        /usr
        /usr/local
        /opt/homebrew
        /opt/homebrew/Cellar/whisper-cpp/1.7.5/libinternal
    PATH_SUFFIXES
        lib
        lib64
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Whisper
    REQUIRED_VARS
        Whisper_LIBRARY
        Whisper_INCLUDE_DIR
)

if(Whisper_FOUND)
    set(Whisper_LIBRARIES ${Whisper_LIBRARY})
    set(Whisper_INCLUDE_DIRS ${Whisper_INCLUDE_DIR})
endif()

mark_as_advanced(
    Whisper_INCLUDE_DIR
    Whisper_LIBRARY
) 