#include "NekoBPMPluginProcessor.h"
#include "NekoBPMPluginEditor.h"
#include <cmath> 

//==============================================================================
NekoBPMAudioProcessor::NekoBPMAudioProcessor()
    #if JucePlugin_Enable_ARA
    : AudioProcessor (BusesProperties().withAnyAudio (true)),
    #else
    : AudioProcessor (BusesProperties().withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                                      .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                      ),
    #endif
    parameters (*this, nullptr, "NekoBPMParameters", createParameterLayout())
{
}

NekoBPMAudioProcessor::~NekoBPMAudioProcessor() {}

//==============================================================================
const juce::String NekoBPMAudioProcessor::getName() const { return JucePlugin_Name; }
bool NekoBPMAudioProcessor::acceptsMidi() const { return false; }
bool NekoBPMAudioProcessor::producesMidi() const { return false; }
bool NekoBPMAudioProcessor::isMidiEffect() const { return false; }
double NekoBPMAudioProcessor::getTailLengthSeconds() const { return 0.0; }

//==============================================================================
juce::AudioProcessorEditor* NekoBPMAudioProcessor::createEditor()
{
    return new NekoBPMAudioProcessorEditor (*this); 
}

bool NekoBPMAudioProcessor::hasEditor() const { return true; }

//==============================================================================
bool NekoBPMAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    #if JucePlugin_Build_Standalone
    return true;
    #else
    return layouts.getMainInputChannelSet()  == juce::AudioChannelSet::stereo()
        && layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
    #endif
}

//==============================================================================
void NekoBPMAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused (sampleRate, samplesPerBlock);
}

void NekoBPMAudioProcessor::releaseResources() {}

//==============================================================================
void NekoBPMAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // 🐈 DAWのテンポ情報を取得
    if (auto* playHead = getPlayHead())
    {
        // 1. PositionInfoを取得
        auto positionInfo = playHead->getPosition();
        
        // hasValue() で値の有無をチェック
        if (positionInfo.hasValue())
        {
            // 2. BPMを取得 (juce::Optional<double>)
            auto bpmValue = positionInfo->getBpm();
            
            // hasValue() でBPMの値の有無をチェック
            if (bpmValue.hasValue())
            {
                // operator* を使用して値を取得
                currentDAWBPM = (float)*bpmValue;
            }

            // 再生状態の取得
            isPlaying = positionInfo->getIsPlaying();
        }
    }
    
    // NekoBPMはオーディオプロセッサとしては入力をそのまま出力
}

//==============================================================================
int NekoBPMAudioProcessor::getNumPrograms() { return 1; }
int NekoBPMAudioProcessor::getCurrentProgram() { return 0; }
void NekoBPMAudioProcessor::setCurrentProgram (int index) { juce::ignoreUnused (index); }
const juce::String NekoBPMAudioProcessor::getProgramName (int index) { juce::ignoreUnused (index); return {}; }
void NekoBPMAudioProcessor::changeProgramName (int index, const juce::String& newName) { juce::ignoreUnused (index, newName); }

//==============================================================================
void NekoBPMAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // パラメータがないため、空のまま
    juce::MemoryOutputStream stream(destData, true);
    parameters.state.writeToStream(stream);
}

void NekoBPMAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    auto tree = juce::ValueTree::readFromData(data, sizeInBytes);
    if (tree.isValid())
        parameters.state = tree;
}

//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout NekoBPMAudioProcessor::createParameterLayout()
{
    return {};
}