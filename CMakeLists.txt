cmake_minimum_required(VERSION 3.24)
project(ReferenceLab VERSION 2.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)
FetchContent_Declare(JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.8
    GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(JUCE)

set(FOLEYS_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(FOLEYS_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(FOLEYS_RUN_PLUGINVAL OFF CACHE BOOL "" FORCE)
FetchContent_Declare(FoleysGuiMagic
    GIT_REPOSITORY https://github.com/ffAudio/foleys_gui_magic.git
    GIT_TAG 1a4ffc359471487ad653dae4d289fedde59901e3)
FetchContent_MakeAvailable(FoleysGuiMagic)

# Foley still calls a JUCE 8 deprecated font helper internally. Keep the
# compatibility warning local to the third-party amalgamation instead of
# disabling C4996 for ReferenceLab sources.
if (MSVC)
    set_source_files_properties(
        "${foleysguimagic_SOURCE_DIR}/modules/foleys_gui_magic/foleys_gui_magic.cpp"
        PROPERTIES COMPILE_OPTIONS "/wd4996")
endif()

juce_add_plugin(ReferenceLab
    COMPANY_NAME "ReferenceLab"
    BUNDLE_ID "it.referencelab.plugin"
    PLUGIN_MANUFACTURER_CODE RLab
    # Preserve the v1 plug-in identity so DAW sessions upgrade in place.
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
    JUCE_REPORT_APP_USAGE=0
    FOLEYS_SHOW_GUI_EDITOR_PALLETTE=0
    FOLEYS_ENABLE_OPEN_GL_CONTEXT=0
    FOLEYS_ENABLE_BINARY_DATA=0)

target_link_libraries(ReferenceLab PRIVATE
    juce::juce_audio_utils
    juce::juce_audio_processors
    juce::juce_dsp
    juce::juce_gui_extra
    foleys::foleys_gui_magic
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
        Source/Library/ReferenceManager.cpp
        Source/Library/LocalReferenceProvider.cpp
        Source/Library/JsonCatalogProvider.cpp
        Source/Library/HttpReferenceProvider.cpp
        Source/Audio/ComparisonProcessor.cpp
        Source/Audio/ReferencePlayer.cpp
        Source/Audio/AnalysisEngine.cpp
        Source/Audio/SpectrumAnalyzer.cpp
        Source/Audio/LoudnessMatcher.cpp)
    target_sources(ReferenceLabTests PRIVATE Source/Audio/TransportController.cpp Source/Audio/CacheManager.cpp)
    target_include_directories(ReferenceLabTests PRIVATE Source)
    target_compile_definitions(ReferenceLabTests PRIVATE JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0)
    target_link_libraries(ReferenceLabTests PRIVATE
        juce::juce_audio_formats juce::juce_dsp juce::juce_gui_basics)
    add_test(NAME ReferenceLabTests COMMAND ReferenceLabTests)
endif()
