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
}

void MainComponent::trackerChanged(const HeadMatrix& headMatrix)
{
    // headMatrix.transform and headMatrix.transformTranspose can be used here
    // to rotate an object.
    float* mat = headMatrix.getMatrix();
    float yaw = std::atan2(mat[3],mat[0]) * 180/M_PI;
    float roll = std::atan2(-mat[6],std::sqrt(std::pow(mat[7], 2) + std::pow(mat[8], 2))) * 180/M_PI;
    float pitch = std::atan2(mat[7],mat[8]) * 180/M_PI;

/**
    float* mat = headMatrix.getMatrix();
    float yaw = (float)mat[1] * 180/M_PI;
    float pitch = (float)mat[2] * 180/M_PI;
    float roll = (float)mat[3] * 180/M_PI;
**/

    std::cout << "Yaw: " << yaw << "\n";
    std::cout << "Pitch: " << pitch << "\n";
    // std::cout << "Roll: " << roll << "\n";

    juce::OSCMessage message("/WONDER/tracker");
    message.addString("yaw-pitch-roll");
    message.addFloat32(yaw);
    message.addFloat32(pitch);
    // message.addFloat32(roll);
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

