#include "MainComponent.h"

#include <utility>

//==============================================================================
MainComponent::MainComponent()
{
    juce::LookAndFeel& lf = getLookAndFeel();
    lf.setColour(juce::ResizableWindow::ColourIds::backgroundColourId, juce::Colour(0xff202020));
    headPanel.setListener(this);
    headPanel.setTopLeftPosition(8, 8);
    addAndMakeVisible(headPanel);
    setSize(headPanel.getWidth()+16, headPanel.getHeight()+16);

}

MainComponent::~MainComponent()
{
}

//==============================================================================
void MainComponent::paint(juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    // Note that the default JUCE look and feel colour is used here. The component works
    // best against dark backgrounds such as this: there's no 'light mode' at the moment.
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{
    // This is called when the MainComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
    std::cout << "test";
}

void MainComponent::trackerChanged(const HeadMatrix& headMatrix)
{
    // headMatrix.transform and headMatrix.transformTranspose can be used here
    // to rotate an object.
    float yaw;
    juce::OSCMessage message("/WONDER/tracker/move/pan");
    float* mat = headMatrix.getMatrix();
    yaw = (float)mat[1] * 180/M_PI;
    message.addFloat32(yaw);
    headPanel.sendOscMessage(&message);
}



float MainComponent::midi2Angle(float msb, float lsb, bool degrees) {
    float i = (128 * msb) + lsb;
    if (i >= 8192)
        i -= 16384;
    if (degrees)
        return (i * float(0.02797645484));
    else
        return (i * float(0.00048828125));
}

