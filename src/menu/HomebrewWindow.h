/****************************************************************************
 * Copyright (C) 2015 Dimok
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ****************************************************************************/
#ifndef _HOMEBREW_WINDOW_H_
#define _HOMEBREW_WINDOW_H_

#include "gui/Gui.h"
#include "gui/GuiFrame.h"
#include "TcpReceiver.h"

class HomebrewWindow : public GuiFrame, public sigslot::has_slots<>
{
public:
    HomebrewWindow(int w, int h);
    virtual ~HomebrewWindow();

    void draw(CVideo *pVideo);

    void clearSelections()
    {
        for (unsigned int i = 0; i < homebrewButtons.size(); i++)
        {
            homebrewButtons[i].button->clearState(STATE_SELECTED);
        }
    }

    sigslot::signal1<const GuiController*> selected;

private:
    void OnOpenEffectFinish(GuiElement *element);
    void OnCloseEffectFinish(GuiElement *element);
    void OnLaunchBoxCloseClick(GuiElement *element);
    void OnHomebrewButtonClick(GuiButton *button, const GuiController *controller, GuiTrigger *trigger);
    void OnLeftArrowClick(GuiButton *button, const GuiController *controller, GuiTrigger *trigger);
    void OnRightArrowClick(GuiButton *button, const GuiController *controller, GuiTrigger *trigger);

    int searchSelectedButton();
    void OnUpDownClick(GuiButton *button, const GuiController *controller, GuiTrigger *trigger);

    void OnAClick(GuiButton *button, const GuiController *controller, GuiTrigger *trigger);

    void OnCloseTcpReceiverFinish(GuiElement *element);
    void OnTcpReceiveStart(GuiElement *element, u32 ip);
    void OnTcpReceiveFinish(GuiElement *element, u32 ip, int result);

    void LaunchBoxSelected(const GuiController* controller)
    {
        selected(controller);
    }

    GuiSound *buttonClickSound;
    GuiImageData * homebrewButtonImgData;

    GuiImageData* arrowRightImageData;
    GuiImageData* arrowLeftImageData;
    GuiImageData* homebrewButtonSelectedImageData;
    GuiImage arrowRightImage;
    GuiImage arrowLeftImage;
    GuiButton arrowRightButton;
    GuiButton arrowLeftButton;
    GuiButton updownButtons;
    GuiButton aButton;
    GuiText hblVersionText;
    GuiText miiMakerHintText;

    typedef struct
    {
        std::string execPath;
        GuiImage *image;
        GuiButton *button;
        GuiText *nameLabel;
        GuiText *descriptionLabel;
        GuiImageData *iconImgData;
        GuiImage *iconImg;
        GuiImage *selectImg;
    } homebrewButton;

    std::vector<homebrewButton> homebrewButtons;
    GuiTrigger touchTrigger;
    GuiTrigger wpadTouchTrigger;
    GuiTrigger buttonLTrigger;
    GuiTrigger buttonRTrigger;
    GuiTrigger buttonUpTrigger;
    GuiTrigger buttonDownTrigger;
    GuiTrigger buttonATrigger;
    int listOffset;
    int currentLeftPosition;
    int targetLeftPosition;

    TcpReceiver tcpReceiver;

    bool inputDisabled;
    FreeTypeGX * osFontSystem;
};

#endif //_HOMEBREW_WINDOW_H_
