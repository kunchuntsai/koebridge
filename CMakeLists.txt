cmake_minimum_required(VERSION 3.15)
project(koebridge VERSION 1.0.0 LANGUAGES CXX)

# Add custom module path
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
endif()

# Build tests option
option(BUILD_TESTS "Build test executables" ON)

# GGML options
option(GGML_METAL "Enable GGML Metal backend" OFF)
option(GGML_USE_METAL "Use Metal for GGML operations" OFF)

# Find required packages
find_package(Threads REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GTest REQUIRED)
find_package(PortAudio REQUIRED)
find_package(SentencePiece REQUIRED)
find_package(Qt6 COMPONENTS Core Widgets Concurrent REQUIRED)

# Add GGML as a subdirectory
add_subdirectory(third_party/ggml)

# Add source files
set(SOURCES
    src/main.cc
    src/audio/audio_capture.cc
    src/translation/translation_service.cc
    src/translation/model_manager.cc
    src/translation/translator.cc
    src/translation/worker.cc
    src/models/ggml_model.cc
    src/inference/engine.cc
    src/llm/llm_model.cc
    src/llm/llm_manager.cc
    src/llm/nllb_model.cc
    src/utils/config.cc
    src/utils/logger.cc
)

# Add header files
set(HEADERS
    src/audio/audio_capture.h
    src/translation/translation_service.h
    src/translation/model_manager.h
    src/translation/translator.h
    src/translation/worker.h
    src/translation/data_structures.h
    src/models/ggml_model.h
    src/inference/engine.h
    src/llm/llm_model.h
    src/llm/llm_manager.h
    src/llm/nllb_model.h
    src/utils/config.h
    src/utils/logger.h
)

# Create main executable
add_executable(koebridge ${SOURCES} ${HEADERS})

# Set Qt MOC properties
set_target_properties(koebridge PROPERTIES
    AUTOMOC ON
    AUTORCC ON
    AUTOUIC ON
)

# Link libraries
target_link_libraries(koebridge PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Concurrent
    ${PortAudio_LIBRARIES}
    ${SENTENCEPIECE_LIBRARIES}
    GTest::gtest
    Threads::Threads
    OpenGL::GL
    ggml
)

# Include directories
target_include_directories(koebridge PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${PortAudio_INCLUDE_DIRS}
    ${SENTENCEPIECE_INCLUDE_DIRS}
)

# Add tests if enabled
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Installation
install(TARGETS koebridge
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)