# FindSentencePiece.cmake
# Finds the SentencePiece library
#
# This will define the following variables
#
#   SentencePiece_FOUND        - True if the system has SentencePiece
#   SentencePiece_INCLUDE_DIRS - The include directory for SentencePiece
#   SentencePiece_LIBRARIES    - The libraries needed to use SentencePiece
#   SentencePiece_VERSION      - The version of SentencePiece found

# Try pkg-config first
find_package(PkgConfig QUIET)
pkg_check_modules(PC_SENTENCEPIECE QUIET sentencepiece)

# Find include directory
find_path(SENTENCEPIECE_INCLUDE_DIR
    NAMES sentencepiece_processor.h
    PATHS
        ${PC_SENTENCEPIECE_INCLUDE_DIRS}
        /opt/homebrew/include
        /usr/local/include
    PATH_SUFFIXES sentencepiece
)

# Find library
find_library(SENTENCEPIECE_LIBRARY
    NAMES sentencepiece
    PATHS
        ${PC_SENTENCEPIECE_LIBRARY_DIRS}
        /opt/homebrew/lib
        /usr/local/lib
)

# Get version from pkg-config
if(PC_SENTENCEPIECE_FOUND)
    set(SENTENCEPIECE_VERSION ${PC_SENTENCEPIECE_VERSION})
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SentencePiece
    REQUIRED_VARS
        SENTENCEPIECE_LIBRARY
        SENTENCEPIECE_INCLUDE_DIR
    VERSION_VAR SENTENCEPIECE_VERSION
)

if(SentencePiece_FOUND)
    set(SentencePiece_LIBRARIES ${SENTENCEPIECE_LIBRARY})
    set(SentencePiece_INCLUDE_DIRS ${SENTENCEPIECE_INCLUDE_DIR})
endif()

mark_as_advanced(SENTENCEPIECE_INCLUDE_DIR SENTENCEPIECE_LIBRARY) 