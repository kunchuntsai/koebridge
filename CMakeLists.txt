cmake_minimum_required(VERSION 3.16)
project(koebridge VERSION 0.1.0 LANGUAGES CXX)

# Add custom module path
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
endif()

# Build tests option
option(BUILD_TESTS "Build test executables" ON)

# GGML options
option(GGML_METAL "Enable GGML Metal backend" OFF)
option(GGML_USE_METAL "Use Metal for GGML operations" OFF)
option(GGML_CPU "Enable CPU backend" ON)
option(GGML_BLAS "Use BLAS" ON)
option(GGML_OPENMP "Use OpenMP" ON)
option(GGML_NATIVE "Optimize for current system" ON)

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
    src/translation/model_manager.cc
    src/translation/translation_service.cc
    src/translation/translator.cc
    src/translation/worker.cc
    src/models/ggml_model.cc
    src/utils/config.cc
    src/utils/logger.cc
    src/inference/engine.cc
)

# Add header files
set(HEADERS
    src/audio/audio_capture.h
    src/translation/model_manager.h
    src/translation/translation_service.h
    src/translation/translator.h
    src/translation/worker.h
    src/models/ggml_model.h
    src/translation/data_structures.h
    src/translation/interfaces/i_translation_service.h
    src/utils/config.h
    src/utils/logger.h
    src/inference/engine.h
)

# Create main executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Set Qt MOC properties
set_target_properties(${PROJECT_NAME} PROPERTIES
    AUTOMOC ON
    AUTORCC ON
    AUTOUIC ON
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Concurrent
    ${PortAudio_LIBRARIES}
    ${SENTENCEPIECE_LIBRARIES}
    GTest::gtest
    Threads::Threads
    OpenGL::GL
    ggml
    ggml-base
)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${PortAudio_INCLUDE_DIRS}
    ${SENTENCEPIECE_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/ggml/include
)

# Add tests if enabled
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Installation
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)