# Set test options
option(BUILD_TESTS "Build test executables" ON)

if(BUILD_TESTS)
    # Include directories
    include_directories(
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/include
    )

    # Add test executables
    add_executable(audio_capture_test
        unit/audio/audio_capture_test.cc
        ../src/audio/audio_capture.cc
    )

    add_executable(translation_service_test
        unit/translation/translation_service_test.cc
        ../src/translation/translation_service.cc
        ../src/translation/model_manager.cc
        ../src/translation/translator.cc
        ../src/translation/worker.cc
        ../src/models/ggml_model.cc
        ../src/utils/config.cc
        ../src/utils/logger.cc
        ../src/inference/engine.cc
    )

    add_executable(model_manager_test
        unit/translation/model_manager_test.cc
        ../src/translation/model_manager.cc
        ../src/models/ggml_model.cc
        ../src/utils/config.cc
        ../src/utils/logger.cc
        ../src/inference/engine.cc
    )

    add_executable(llm_model_test
        unit/llm/llm_model_test.cc
        ../src/llm/llm_model.cc
        ../src/models/ggml_model.cc
        ../src/utils/config.cc
        ../src/utils/logger.cc
        ../src/inference/engine.cc
    )

    add_executable(nllb_model_test
        unit/llm/nllb_model_test.cc
        ../src/llm/nllb_model.cc
        ../src/llm/llm_model.cc
        ../src/models/ggml_model.cc
        ../src/utils/config.cc
        ../src/utils/logger.cc
        ../src/inference/engine.cc
    )

    add_executable(llm_manager_test
        unit/llm/llm_manager_test.cc
        ../src/llm/llm_manager.cc
        ../src/llm/llm_model.cc
        ../src/llm/nllb_model.cc
        ../src/models/ggml_model.cc
        ../src/utils/config.cc
        ../src/utils/logger.cc
        ../src/inference/engine.cc
        ../src/translation/model_manager.cc
    )

    add_executable(whisper_wrapper_test
        unit/stt/whisper_wrapper_test.cc
        ../src/stt/whisper_wrapper.cc
        ../src/utils/logger.cc
    )

    # Set Qt MOC properties for test targets
    set_target_properties(audio_capture_test PROPERTIES AUTOMOC ON)
    set_target_properties(translation_service_test PROPERTIES AUTOMOC ON)
    set_target_properties(model_manager_test PROPERTIES AUTOMOC ON)
    set_target_properties(llm_model_test PROPERTIES AUTOMOC ON)
    set_target_properties(nllb_model_test PROPERTIES AUTOMOC ON)
    set_target_properties(llm_manager_test PROPERTIES AUTOMOC ON)
    set_target_properties(whisper_wrapper_test PROPERTIES AUTOMOC ON)

    # Link test executables
    target_link_libraries(audio_capture_test PRIVATE
        Qt6::Core
        Qt6::Widgets
        ${PortAudio_LIBRARIES}
        GTest::gtest
        GTest::gtest_main
    )

    target_link_libraries(translation_service_test PRIVATE
        Qt6::Core
        Qt6::Widgets
        Qt6::Concurrent
        ${PortAudio_LIBRARIES}
        ${SentencePiece_LIBRARIES}
        GTest::gtest
        GTest::gtest_main
        ggml
        ggml-base
    )

    target_link_libraries(model_manager_test PRIVATE
        Qt6::Core
        Qt6::Widgets
        ${PortAudio_LIBRARIES}
        ${SentencePiece_LIBRARIES}
        GTest::gtest
        GTest::gtest_main
        ggml
        ggml-base
    )

    target_link_libraries(llm_model_test PRIVATE
        Qt6::Core
        Qt6::Widgets
        ${PortAudio_LIBRARIES}
        ${SentencePiece_LIBRARIES}
        GTest::gtest
        GTest::gtest_main
        ggml
        ggml-base
    )

    target_link_libraries(nllb_model_test PRIVATE
        Qt6::Core
        Qt6::Widgets
        ${PortAudio_LIBRARIES}
        ${SentencePiece_LIBRARIES}
        GTest::gtest
        GTest::gtest_main
        ggml
        ggml-base
    )

    target_link_libraries(llm_manager_test PRIVATE
        Qt6::Core
        Qt6::Widgets
        ${PortAudio_LIBRARIES}
        ${SentencePiece_LIBRARIES}
        GTest::gtest
        GTest::gtest_main
        ggml
        ggml-base
    )

    target_link_libraries(whisper_wrapper_test PRIVATE
        Qt6::Core
        whisper
        ggml
        ggml-base
        GTest::gtest
        GTest::gtest_main
    )

    # Include directories for each test
    target_include_directories(audio_capture_test PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/include
        ${PortAudio_INCLUDE_DIRS}
    )

    target_include_directories(translation_service_test PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/include
        ${SentencePiece_INCLUDE_DIRS}
    )

    target_include_directories(model_manager_test PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/include
        ${SentencePiece_INCLUDE_DIRS}
    )

    target_include_directories(llm_model_test PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/include
        ${SentencePiece_INCLUDE_DIRS}
    )

    target_include_directories(nllb_model_test PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/include
        ${SentencePiece_INCLUDE_DIRS}
    )

    target_include_directories(llm_manager_test PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/include
        ${SentencePiece_INCLUDE_DIRS}
    )

    target_include_directories(whisper_wrapper_test PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/third_party/whisper.cpp/include
        ${CMAKE_SOURCE_DIR}/third_party/ggml/include
    )

    # Add tests to CTest
    add_test(NAME audio_capture_test COMMAND audio_capture_test)
    add_test(NAME translation_service_test COMMAND translation_service_test)
    add_test(NAME model_manager_test COMMAND model_manager_test)
    add_test(NAME llm_model_test COMMAND llm_model_test)
    add_test(NAME nllb_model_test COMMAND nllb_model_test)
    add_test(NAME llm_manager_test COMMAND llm_manager_test)
    add_test(NAME whisper_wrapper_test COMMAND whisper_wrapper_test)
endif()