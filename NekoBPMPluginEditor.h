#pragma once

#include <JuceHeader.h>
#include "NekoBPMPluginProcessor.h" 

// juce::AudioProcessorValueTreeState::Listener を継承しない
class NekoBPMAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                           public juce::Timer 
{
public:
    NekoBPMAudioProcessorEditor(NekoBPMAudioProcessor&);
    ~NekoBPMAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    
    void timerCallback() override; // juce::Timer のコールバック

private:
    void loadNekoFrames(); // GIFフレームを読み込むメソッド
    
    std::vector<juce::Image> nekoFrames; // 読み込んだフレームを格納
    int currentFrameIndex = 0;           // 現在表示しているフレームのインデックス
    
    // 158枚のフレームが120BPMで4秒かけて再生されると仮定したベースFPS (158f / 4sec = 39.5 FPS)
    const float originalNekoFPS = 27.95f; 

    NekoBPMAudioProcessor& processor; // プロセッサ参照

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NekoBPMAudioProcessorEditor)
};