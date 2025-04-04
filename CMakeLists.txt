cmake_minimum_required(VERSION 3.16)
project(koebridge VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable Qt Meta-Object Compiler
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find required packages
find_package(Qt6 COMPONENTS Core Widgets REQUIRED)

# Find PortAudio using pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(PORTAUDIO REQUIRED portaudio-2.0)

# Find GTest using pkg-config
pkg_check_modules(GTEST REQUIRED gtest_main)

# Include directories for all targets
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)  # Add src to include path
include_directories(${PORTAUDIO_INCLUDE_DIRS})
include_directories(${GTEST_INCLUDE_DIRS})
link_directories(${PORTAUDIO_LIBRARY_DIRS})
link_directories(${GTEST_LIBRARY_DIRS})

# Add source files
file(GLOB_RECURSE EXISTING_SOURCES "src/*.cc")
file(GLOB_RECURSE HEADERS "include/*.h" "src/*.h")

# Add new source directories from update
set(AUDIO_SOURCES
    src/audio/audio_capture.cc
    src/audio/audio_processor.cc
)

set(INFERENCE_SOURCES
    src/inference/engine.cc
)

set(MODELS_SOURCES
    src/models/ggml_model.cc
)

# Create library target for core functionality
add_library(koebridge_lib STATIC
    ${EXISTING_SOURCES}
    ${AUDIO_SOURCES}
    ${INFERENCE_SOURCES}
    ${MODELS_SOURCES}
    ${HEADERS}
)

# Set include directories for the library
target_include_directories(koebridge_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces
)

# Link libraries for the core library
target_link_libraries(koebridge_lib PUBLIC
    Qt6::Core
    Qt6::Widgets
    ${PORTAUDIO_LIBRARIES}
)

# Create main executable
add_executable(${PROJECT_NAME} 
    src/main.cc
)

# Link main executable with the library
target_link_libraries(${PROJECT_NAME} PRIVATE
    koebridge_lib
)

# Tests configuration (if testing is enabled)
option(BUILD_TESTS "Build tests" OFF)
if(BUILD_TESTS)
    enable_testing()
    
    # Unit tests
    add_executable(unit_tests
        tests/unit/translation/model_manager_test.cc
        tests/unit/models/ggml_model_test.cc
        tests/unit/audio/audio_capture_test.cc
        # Add other unit tests
    )
    
    target_link_libraries(unit_tests
        ${GTEST_LIBRARIES}
        ${GTEST_MAIN_LIBRARIES}
        koebridge_lib
        ${PORTAUDIO_LIBRARIES}
    )
    
    # Integration tests
    add_executable(integration_tests
        tests/integration/end_to_end/translation_test.cc
        # Add other integration tests
    )
    
    target_link_libraries(integration_tests
        ${GTEST_LIBRARIES}
        ${GTEST_MAIN_LIBRARIES}
        koebridge_lib
        ${PORTAUDIO_LIBRARIES}
    )
    
    add_test(NAME UnitTests COMMAND unit_tests)
    add_test(NAME IntegrationTests COMMAND integration_tests)
endif()

# Install target
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
install(TARGETS koebridge_lib DESTINATION lib)