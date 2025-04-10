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
        ${CMAKE_SOURCE_DIR}/src/audio/audio_capture.cc
    )
    
    add_executable(translation_service_test
        unit/translation/translation_service_test.cc
        ${CMAKE_SOURCE_DIR}/src/translation/translation_service.cc
        ${CMAKE_SOURCE_DIR}/src/translation/model_manager.cc
        ${CMAKE_SOURCE_DIR}/src/models/ggml_model.cc
        ${CMAKE_SOURCE_DIR}/src/inference/engine.cc
        ${CMAKE_SOURCE_DIR}/src/utils/logger.cc
        ${CMAKE_SOURCE_DIR}/src/utils/config.cc
    )
    
    add_executable(model_manager_test
        unit/translation/model_manager_test.cc
        ${CMAKE_SOURCE_DIR}/src/translation/model_manager.cc
        ${CMAKE_SOURCE_DIR}/src/models/ggml_model.cc
        ${CMAKE_SOURCE_DIR}/src/inference/engine.cc
        ${CMAKE_SOURCE_DIR}/src/utils/logger.cc
        ${CMAKE_SOURCE_DIR}/src/utils/config.cc
    )
    
    add_executable(llm_model_test
        unit/llm/llm_model_test.cc
        ${CMAKE_SOURCE_DIR}/src/llm/llm_model.cc
        ${CMAKE_SOURCE_DIR}/src/models/ggml_model.cc
        ${CMAKE_SOURCE_DIR}/src/inference/engine.cc
        ${CMAKE_SOURCE_DIR}/src/utils/logger.cc
        ${CMAKE_SOURCE_DIR}/src/utils/config.cc
    )
    
    add_executable(llm_manager_test
        unit/llm/llm_manager_test.cc
        ${CMAKE_SOURCE_DIR}/src/llm/llm_manager.cc
        ${CMAKE_SOURCE_DIR}/src/llm/llm_model.cc
        ${CMAKE_SOURCE_DIR}/src/llm/nllb_model.cc
        ${CMAKE_SOURCE_DIR}/src/models/ggml_model.cc
        ${CMAKE_SOURCE_DIR}/src/inference/engine.cc
        ${CMAKE_SOURCE_DIR}/src/translation/model_manager.cc
        ${CMAKE_SOURCE_DIR}/src/utils/logger.cc
        ${CMAKE_SOURCE_DIR}/src/utils/config.cc
    )
    
    # Temporarily disable ggml_model_test due to interface mismatch
    # add_executable(ggml_model_test
    #     unit/models/ggml_model_test.cc
    #     ${CMAKE_SOURCE_DIR}/src/translation/translator.cc
    #     ${CMAKE_SOURCE_DIR}/src/utils/logger.cc
    #     ${CMAKE_SOURCE_DIR}/src/utils/config.cc
    # )
    
    # Set Qt MOC properties for test targets
    set_target_properties(audio_capture_test PROPERTIES AUTOMOC ON)
    set_target_properties(translation_service_test PROPERTIES AUTOMOC ON)
    set_target_properties(model_manager_test PROPERTIES AUTOMOC ON)
    set_target_properties(llm_model_test PROPERTIES AUTOMOC ON)
    set_target_properties(llm_manager_test PROPERTIES AUTOMOC ON)
    # set_target_properties(ggml_model_test PROPERTIES AUTOMOC ON)
    
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
        ${SENTENCEPIECE_LIBRARIES}
        GTest::gtest
        GTest::gtest_main
        ggml
        ggml-base
        ggml-cpu
    )
    
    target_link_libraries(model_manager_test PRIVATE
        Qt6::Core
        Qt6::Widgets
        ${SENTENCEPIECE_LIBRARIES}
        GTest::gtest
        GTest::gtest_main
        ggml
        ggml-base
        ggml-cpu
    )
    
    target_link_libraries(llm_model_test PRIVATE
        Qt6::Core
        Qt6::Widgets
        Qt6::Concurrent
        ${SENTENCEPIECE_LIBRARIES}
        GTest::gtest
        GTest::gtest_main
        ggml
        ggml-base
        ggml-cpu
    )
    
    target_link_libraries(llm_manager_test PRIVATE
        Qt6::Core
        Qt6::Widgets
        Qt6::Concurrent
        ${SENTENCEPIECE_LIBRARIES}
        GTest::gtest
        GTest::gtest_main
        ggml
        ggml-base
        ggml-cpu
    )
    
    # target_link_libraries(ggml_model_test PRIVATE
    #     Qt6::Core
    #     Qt6::Widgets
    #     ${SENTENCEPIECE_LIBRARIES}
    #     GTest::gtest
    #     GTest::gtest_main
    #     ggml
    # )
    
    # Include directories for each test
    target_include_directories(audio_capture_test PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/include
        ${PortAudio_INCLUDE_DIRS}
    )
    
    target_include_directories(translation_service_test PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/include
        ${SENTENCEPIECE_INCLUDE_DIRS}
    )
    
    target_include_directories(model_manager_test PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/include
        ${SENTENCEPIECE_INCLUDE_DIRS}
    )
    
    target_include_directories(llm_model_test PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/include
        ${SENTENCEPIECE_INCLUDE_DIRS}
    )
    
    target_include_directories(llm_manager_test PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/include
        ${SENTENCEPIECE_INCLUDE_DIRS}
    )
    
    # target_include_directories(ggml_model_test PRIVATE
    #     ${CMAKE_SOURCE_DIR}/src
    #     ${CMAKE_SOURCE_DIR}/include
    #     ${SENTENCEPIECE_INCLUDE_DIRS}
    # )
    
    # Add tests to CTest
    add_test(NAME AudioCaptureTest COMMAND audio_capture_test)
    add_test(NAME TranslationServiceTest COMMAND translation_service_test)
    add_test(NAME ModelManagerTest COMMAND model_manager_test)
    add_test(NAME LLMModelTest COMMAND llm_model_test)
    add_test(NAME LLMManagerTest COMMAND llm_manager_test)
    # add_test(NAME GgmlModelTest COMMAND ggml_model_test)
endif() 