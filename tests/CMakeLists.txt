# Set test options
option(BUILD_TESTS "Build test executables" ON)

if(BUILD_TESTS)
    # Include directories
    include_directories(
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/include
    )

    # Add test source files
    file(GLOB TEST_SOURCES
        "unit/audio/*.cc"
        "unit/translation/*.cc"
        "unit/llm/*.cc"
        "unit/stt/*.cc"
    )

    # Add source files needed for tests
    file(GLOB TEST_DEPS_SOURCES
        "../src/audio/*.cc"
        "../src/translation/*.cc"
        "../src/models/*.cc"
        "../src/utils/*.cc"
        "../src/inference/*.cc"
        "../src/llm/*.cc"
        "../src/stt/*.cc"
    )

    # Create test executables
    foreach(TEST_SOURCE ${TEST_SOURCES})
        get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
        add_executable(${TEST_NAME} ${TEST_SOURCE} ${TEST_DEPS_SOURCES})

        # Set Qt MOC properties for test targets
        set_target_properties(${TEST_NAME} PROPERTIES AUTOMOC ON)

        # Link test executables
        target_link_libraries(${TEST_NAME} PRIVATE
            Qt6::Core
            Qt6::Widgets
            Qt6::Concurrent
            ${PortAudio_LIBRARIES}
            ${SentencePiece_LIBRARIES}
            GTest::gtest
            GTest::gtest_main
            ggml
            ggml-base
            whisper
        )

        # Include directories for each test
        target_include_directories(${TEST_NAME} PRIVATE
            ${CMAKE_SOURCE_DIR}/src
            ${CMAKE_SOURCE_DIR}/include
            ${PortAudio_INCLUDE_DIRS}
            ${SentencePiece_INCLUDE_DIRS}
            ${CMAKE_SOURCE_DIR}/third_party/whisper.cpp/include
            ${CMAKE_SOURCE_DIR}/third_party/ggml/include
        )

        # Add test to CTest
        add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
    endforeach()
endif()