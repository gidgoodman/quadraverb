cmake_minimum_required(VERSION 3.22)
project(QuadraVerbPlus VERSION 0.1.0)

include(FetchContent)
FetchContent_Declare(JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.4)
FetchContent_MakeAvailable(JUCE)

juce_add_plugin(QuadraVerbPlus
    COMPANY_NAME "QuadraVerb"
    PLUGIN_MANUFACTURER_CODE Ysnm
    PLUGIN_CODE Qvp1
    FORMATS VST3 Standalone
    PRODUCT_NAME "QuadraVerb Plus"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE)

target_sources(QuadraVerbPlus PRIVATE
    Source/PluginProcessor.cpp Source/PluginEditor.cpp
    Source/QuadraVerbSysEx.cpp Source/SysExPanel.cpp)

target_compile_definitions(QuadraVerbPlus PUBLIC
    JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0 JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(QuadraVerbPlus
    PRIVATE juce::juce_audio_utils juce::juce_dsp
    PUBLIC juce::juce_recommended_config_flags)
