# FindPortAudio.cmake
# Finds the PortAudio library
#
# This will define the following variables
#
#   PortAudio_FOUND        - True if the system has PortAudio
#   PortAudio_INCLUDE_DIRS - The include directory for PortAudio
#   PortAudio_LIBRARIES    - The libraries needed to use PortAudio
#   PortAudio_VERSION      - The version of PortAudio found

# Try pkg-config first
find_package(PkgConfig QUIET)
pkg_check_modules(PC_PORTAUDIO QUIET portaudio-2.0)

# Find include directory
find_path(PORTAUDIO_INCLUDE_DIR
    NAMES portaudio.h
    PATHS
        ${PC_PORTAUDIO_INCLUDE_DIRS}
        /opt/homebrew/include
        /usr/local/include
    PATH_SUFFIXES portaudio
)

# Find library
find_library(PORTAUDIO_LIBRARY
    NAMES portaudio
    PATHS
        ${PC_PORTAUDIO_LIBRARY_DIRS}
        /opt/homebrew/lib
        /usr/local/lib
)

# Get version from pkg-config
if(PC_PORTAUDIO_FOUND)
    set(PORTAUDIO_VERSION ${PC_PORTAUDIO_VERSION})
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(PortAudio
    REQUIRED_VARS
        PORTAUDIO_LIBRARY
        PORTAUDIO_INCLUDE_DIR
    VERSION_VAR PORTAUDIO_VERSION
)

if(PortAudio_FOUND)
    set(PortAudio_LIBRARIES ${PORTAUDIO_LIBRARY})
    set(PortAudio_INCLUDE_DIRS ${PORTAUDIO_INCLUDE_DIR})
endif()

mark_as_advanced(PORTAUDIO_INCLUDE_DIR PORTAUDIO_LIBRARY) 