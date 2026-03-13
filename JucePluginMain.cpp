#include "JuceHeader.h"
#include "NekoBPMPluginProcessor.h"

//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new NekoBPMAudioProcessor();
}