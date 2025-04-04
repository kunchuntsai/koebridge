cmake_minimum_required(VERSION 3.16)
project(koebridge VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable Qt Meta-Object Compiler
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt packages
find_package(Qt6 COMPONENTS Core Widgets REQUIRED)

# Include directories for all targets
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)  # Add src to include path

# Add source files
file(GLOB_RECURSE EXISTING_SOURCES "src/*.cc")
file(GLOB_RECURSE HEADERS "include/*.h" "src/*.h")

# Add new source directories from update
set(INFERENCE_SOURCES
    src/inference/engine.cc
)

set(MODELS_SOURCES
    src/models/ggml_model.cc
)

# Create executable - DEFINE THE TARGET FIRST before using target_* commands
add_executable(${PROJECT_NAME} 
    ${EXISTING_SOURCES}
    ${INFERENCE_SOURCES}
    ${MODELS_SOURCES}
    ${HEADERS}
)

# Now that the target is defined, we can use target_include_directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Widgets
)

# Tests configuration (if testing is enabled)
option(BUILD_TESTS "Build tests" OFF)
if(BUILD_TESTS)
    enable_testing()
    find_package(GTest REQUIRED)
    
    # Unit tests
    add_executable(unit_tests
        tests/unit/translation/model_manager_test.cc
        tests/unit/models/ggml_model_test.cc
        # Add other unit tests
    )
    
    target_link_libraries(unit_tests
        GTest::GTest
        GTest::Main
        ${PROJECT_NAME}  # Link directly to main project
    )
    
    # Integration tests
    add_executable(integration_tests
        tests/integration/end_to_end/translation_test.cc
        # Add other integration tests
    )
    
    target_link_libraries(integration_tests
        GTest::GTest
        GTest::Main
        ${PROJECT_NAME}
    )
    
    add_test(NAME UnitTests COMMAND unit_tests)
    add_test(NAME IntegrationTests COMMAND integration_tests)
endif()

# Install target
install(TARGETS ${PROJECT_NAME} DESTINATION bin)