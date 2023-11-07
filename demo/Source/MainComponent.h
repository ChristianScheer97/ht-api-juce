#pragma once

#include <JuceHeader.h>

#include "HeadMatrix.h"
#include "Tracker.h"
#include "midi.h"
#include "configPanel.h"
#include "headPanel.h"

//==============================================================================

class MainComponent  : public juce::Component, HeadPanel::HeadPanel::Listener
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;
    void trackerChanged(const HeadMatrix& headMatrix) override;

    void setOscAddress(juce::String newAddress);
    void setUdpPort(int newPort);
    juce::String getOscAddress();
    int getUdpPort() const;

    static float midi2Angle(float msb, float lsb, bool degrees=true);
    static void showConnectionErrorMessage(const juce::String& message);

private:
    //==============================================================================
    HeadPanel::HeadPanel headPanel;
    juce::OSCSender oscSender;
    int udpPort = 59000;
    juce::String oscAddress = "127.0.0.1";

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
