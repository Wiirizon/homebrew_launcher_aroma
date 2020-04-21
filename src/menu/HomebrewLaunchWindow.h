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
#ifndef _HOMEBREW_LAUNCHER_WINDOW_H_
#define _HOMEBREW_LAUNCHER_WINDOW_H_

#include "gui/Gui.h"
#include "gui/GuiFrame.h"
#include "gui/ControllerBase.h"

class HomebrewLaunchWindow : public GuiFrame, public sigslot::has_slots<>
{
public:
    HomebrewLaunchWindow(const std::string & launchPath, GuiImageData * iconImgData);
    virtual ~HomebrewLaunchWindow();

    sigslot::signal1<GuiElement *> backButtonClicked;
    sigslot::signal1<const GuiController*> selected;
private:
    void OnBackButtonClick(GuiButton *button, const GuiController *controller, GuiTrigger *trigger)
    {
        // If there is no pointer dont click
        if (trigger == &wpadTouchTrigger && !((ControllerBase*) controller)->showPointer)
        {
            return;
        }

        backButtonClicked(this);
    }

    void OnLoadButtonClick(GuiButton *button, const GuiController *controller, GuiTrigger *trigger);

    void OnFileLoadFinish(GuiElement *element, const std::string & filepath, int result);
    void OnOpenEffectFinish(GuiElement *element);
    void OnCloseEffectFinish(GuiElement *element);

    void OnDpadClick(GuiButton *button, const GuiController *controller, GuiTrigger *trigger);
    void OnButtonClick(GuiButton *button, const GuiController *controller, GuiTrigger *trigger);

    int findSelectedButton();

    GuiSound *buttonClickSound;
    GuiImageData * backgroundImgData;
    GuiImage backgroundImg;

    GuiImageData *buttonImgData;
    GuiImageData *buttonSelectedImgData;
    GuiImage iconImage;

    GuiText titleText;
    GuiText versionText;
    GuiText versionValueText;
    GuiText authorText;
    GuiText authorValueText;
    GuiText descriptionText;

    GuiText loadBtnLabel;
    GuiImage loadImg;
    GuiImage loadSelectedImg;
    GuiButton loadBtn;

    GuiText backBtnLabel;
    GuiImage backImg;
    GuiImage backSelectedImg;
    GuiButton backBtn;

    GuiTrigger touchTrigger;
    GuiTrigger wpadTouchTrigger;
    GuiTrigger buttonRightTrigger;
    GuiTrigger buttonLeftTrigger;
    GuiTrigger buttonATrigger;
    GuiTrigger buttonBTrigger;
    GuiButton dpadButtons;
    GuiButton buttons;

    const std::string homebrewLaunchPath;
};

#endif //_HOMEBREW_LAUNCHER_WINDOW_H_
