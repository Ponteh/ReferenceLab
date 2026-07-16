cmake_minimum_required(VERSION 3.24)
project(ReferenceLab VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)
FetchContent_Declare(JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.8
    GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(JUCE)

juce_add_plugin(ReferenceLab
    COMPANY_NAME "ReferenceLab"
    BUNDLE_ID "it.referencelab.plugin"
    PLUGIN_MANUFACTURER_CODE RLab
    PLUGIN_CODE Rfl1
    PRODUCT_NAME "ReferenceLab"
    FORMATS VST3 Standalone
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD FALSE)

file(GLOB_RECURSE REFERENCE_LAB_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.h")
target_sources(ReferenceLab PRIVATE ${REFERENCE_LAB_SOURCES})

target_compile_definitions(ReferenceLab PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_REPORT_APP_USAGE=0)

target_link_libraries(ReferenceLab PRIVATE
    juce::juce_audio_utils
    juce::juce_audio_processors
    juce::juce_dsp
    juce::juce_gui_extra
    PUBLIC
    juce::juce_recommended_config_flags
    juce::juce_recommended_warning_flags)

option(REFERENCELAB_BUILD_TESTS "Build ReferenceLab tests" ON)
if (REFERENCELAB_BUILD_TESTS)
    enable_testing()
    juce_add_console_app(ReferenceLabTests PRODUCT_NAME "ReferenceLab Tests")
    target_sources(ReferenceLabTests PRIVATE
        Tests/ReferenceLabTests.cpp
        Source/Library/ReferenceModel.cpp
        Source/Library/MetadataRepository.cpp
        Source/Audio/ComparisonProcessor.cpp
        Source/Audio/ReferencePlayer.cpp)
    target_include_directories(ReferenceLabTests PRIVATE Source)
    target_compile_definitions(ReferenceLabTests PRIVATE JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0)
    target_link_libraries(ReferenceLabTests PRIVATE
        juce::juce_audio_formats juce::juce_dsp juce::juce_gui_basics)
    add_test(NAME ReferenceLabTests COMMAND ReferenceLabTests)
endif()
