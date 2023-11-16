/*
 * Head tracker configuration panels
 * Handles common activities concerning configuration windows
 * Copyright (c) 2021 Supperware Ltd.
 */

#pragma once

class Properties;

namespace ConfigPanel
{
    enum class LabelStyle { SectionHeading, Description, Data, SubData };

    class BasePanel : public juce::Component, public juce::MultiTimer,
private juce::Button::Listener, private juce::ComboBox::Listener, private juce::Label::Listener,
        private Midi::TrackerDriver::Listener
    {
    public:
        static constexpr int LabelWidth { 256 }; // window width is derived from LabelWidth

        BasePanel(Midi::TrackerDriver& trackerDriver, Properties* properties, juce::String titleText) :
            td(trackerDriver),
            props(properties),
            title(titleText),
            doRepaint(false)
        {
            setOpaque(false);
            td.addListener(this);


        }

        // ---------------------------------------------------------------------

        ~BasePanel()
        {
            for (auto tb : toggleButtons) { tb->setLookAndFeel(nullptr); }
        }

        // ---------------------------------------------------------------------

        virtual void click(juce::Button*, const bool /*isTextButton*/, const int /*index*/, const bool /*isChecked*/) {}
        virtual void comboBox(const int /*index*/, const int /*option*/) {}
        virtual void setUdpPort(int /*newPort*/) {}
        virtual void setOscAddress (juce::String /*newAddreess*/) {}


        // ---------------------------------------------------------------------
        
        void buttonClicked(juce::Button* button) override
        {
            juce::TextButton* textB = dynamic_cast<juce::TextButton*>(button);
            if (textB)
            {
                click(button, true, textButtons.indexOf(textB), false);
            }
            else
            {
                juce::ToggleButton* toggleB = dynamic_cast<juce::ToggleButton*>(button);
                if (toggleB)
                {
                    click(button, false, toggleButtons.indexOf(toggleB), toggleB->getToggleState());
                }
            }
        }

        // ---------------------------------------------------------------------

        void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override
        {
            int cbIndex = comboBoxes.indexOf(comboBoxThatHasChanged);
            comboBox(cbIndex, comboBoxThatHasChanged->getSelectedItemIndex());
        }

        // ---------------------------------------------------------------------

        void labelTextChanged(juce::Label* label) override
        {
            juce::String id = label->getComponentID();
            if (id == "port")
                setUdpPort(label->getText().getIntValue());
            else if (id == "address")
                setOscAddress(label->getText());
        }

        /** Unilaterally enable or disable all controls */
        void setEnabled(const bool shouldBeEnabled)
        {
            for (juce::TextButton* tb : textButtons)
            {
                tb->setEnabled(shouldBeEnabled);
            }
            for (juce::ToggleButton* tb : toggleButtons)
            {
                tb->setEnabled(shouldBeEnabled);
            }
            for (juce::ComboBox* cb : comboBoxes)
            {
                cb->setEnabled(shouldBeEnabled);
            }
        }

        // ---------------------------------------------------------------------

        void paint(juce::Graphics& g) override
        {
            if (!title.isEmpty())
            {
                // draw title
                g.setColour(juce::Colour(0xff0c0c0c));
                g.fillRect(0, 2, LabelWidth, 21);
                g.setColour(TitleLabel);
                g.setFont(juce::Font(16.0, juce::Font::plain));
                g.drawText(title, 8, 4, 200, 200, juce::Justification::topLeft);
            }
        }

        // ------------------------------------------------------------------------



protected:
        Midi::TrackerDriver& td;

        Properties* props;
        juce::String title;
        const juce::Colour TitleLabel = juce::Colour(0xff28789c);
        const juce::Colour SectionLabel = juce::Colour(0xff529aca);

        juce::OwnedArray<juce::Label> labels;
        juce::OwnedArray<juce::TextButton> textButtons;
        juce::OwnedArray<juce::ToggleButton> toggleButtons;
        juce::OwnedArray<juce::ComboBox> comboBoxes;

        static constexpr int Indent { 18 };

        // ------------------------------------------------------------------------

        int addLabel(juce::Point<int>& topLeft, const juce::String text, const LabelStyle style, bool isEditable=false, bool nextNewLine=true, bool indent=false, juce::String id="")
        {
            int styleIndex;
            switch (style)
            {
            case LabelStyle::SectionHeading: styleIndex = 0; break;
            case LabelStyle::Description: styleIndex = 1; break;
            case LabelStyle::Data: styleIndex = 2; break;
            default: styleIndex = 3;
            }

            const float LabelFontSize[4] = { 15.5f, 14.5f, 16.0f, 16.0f };
            const int LabelFontStyle[4] = { juce::Font::FontStyleFlags::bold, 0, 0, 0 };
            const juce::Colour LabelColour[4] = { SectionLabel, juce::Colour(0xff808186), juce::Colour(0xffd8d8d8), juce::Colour(0xffa4a5a8)};
            const int LineHeight[4] = { 28, 24, 24, 24 };

            int index = labels.size();

            juce::Label* lb = labels.add(new juce::Label(text, text));

            if (!indent)
                lb->setTopLeftPosition(topLeft);
            else
                lb->setTopLeftPosition(topLeft.translated(Indent, 0));

            lb->setFont(juce::Font(LabelFontSize[styleIndex], LabelFontStyle[styleIndex]));
            lb->setColour(juce::Label::ColourIds::textColourId, LabelColour[styleIndex]);
            lb->setComponentID(id);
            lb->addListener(this);
            if (isEditable)
            {
                lb->setEditable(true);
                lb->setSize(LabelWidth/2, LineHeight[styleIndex]);
            }
            else
                lb->setSize(LabelWidth, LineHeight[styleIndex]);

            if (nextNewLine)
                topLeft.y += LineHeight[styleIndex] + 3;

            addAndMakeVisible(lb);

            return index;
        }

        // ------------------------------------------------------------------------

        int addTextButton(juce::Point<int>& topLeft, const juce::String text, const int width)
        {
            constexpr int ButtonHeight { 24 };
            int index = textButtons.size();
            juce::TextButton* tb = textButtons.add(new juce::TextButton(text));
            tb->setTopLeftPosition(topLeft.translated(Indent + 10, 2));
            tb->setSize(width, ButtonHeight);
            tb->addListener(this);
            topLeft.y += ButtonHeight + 8;
            addAndMakeVisible(tb);
            return index;
        }

        // ------------------------------------------------------------------------

        int addToggle(juce::Point<int>& topLeft, const juce::String text, const int radioGroupID = -1)
        {
            constexpr int ToggleHeight { 22 };
            int index = toggleButtons.size();
            juce::ToggleButton* tb = toggleButtons.add(new juce::ToggleButton(text));
            if (radioGroupID >= 0)
            {
                tb->setRadioGroupId(radioGroupID, juce::dontSendNotification);
                tb->setLookAndFeel(&lookAndFeelRadio);
            }
            tb->setTopLeftPosition(topLeft.translated(Indent, 0));
            tb->setSize(LabelWidth - 16, ToggleHeight);
            tb->addListener(this);
            topLeft.y += ToggleHeight + 4;
            addAndMakeVisible(tb);
            return index;
        }

        // ------------------------------------------------------------------------

        int addComboBox(juce::Point<int>& topLeft, const juce::StringArray& text)
        {
            constexpr int ComboBoxHeight { 24 };
            int index = comboBoxes.size();
            juce::ComboBox* cb = comboBoxes.add(new juce::ComboBox());
            cb->setEditableText(false);
            cb->addItemList(text, 1);
            cb->setTopLeftPosition(topLeft.translated(Indent - 4, 0));
            cb->setSize(LabelWidth - 2 * Indent - topLeft.x, ComboBoxHeight);
            cb->addListener(this);
            topLeft.y += ComboBoxHeight + 4;
            addAndMakeVisible(cb);
            return index;
        }

        // ------------------------------------------------------------------------

        void timerCallback(int timerID) override
        {
            if (timerID == 0)
            {
                repaint();
                stopTimer(timerID);
            }
        }

        // ------------------------------------------------------------------------

        int yOrigin()
        {
            return (title.isEmpty()) ? 2 : 22;
        }

        // ------------------------------------------------------------------------

        void repaintAsync()
        {
            if (!doRepaint)
            {
                doRepaint = true;
                startTimer(0, 16); // 60Hz repaint
            }
        }

        // ------------------------------------------------------------------------

    private:
        bool doRepaint;
        LookAndFeelRadio lookAndFeelRadio;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BasePanel)
    };
};
