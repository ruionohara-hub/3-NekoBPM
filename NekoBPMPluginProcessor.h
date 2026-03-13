#pragma once

#include <JuceHeader.h>

class NekoBPMAudioProcessor  : public juce::AudioProcessor
{
public:
    NekoBPMAudioProcessor();
    ~NekoBPMAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    // パラメータ管理
    juce::AudioProcessorValueTreeState parameters;

    // 🐈 NekoBPM機能のための追加メンバー変数
    float currentDAWBPM = 120.0f; // 現在のDAWテンポを格納する変数
    // DAWの再生状態
    bool isPlaying = false;       

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout(); 

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NekoBPMAudioProcessor)
};