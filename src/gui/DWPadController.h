/**
 * This class implements the DPAD mode for Wii Remote, Classic and Pro controllers, allowing you to use
 * the DPAD on your controller to move a virtual on screen pointer, rather than having to point at the
 * screen. This means that users who do not have a sensor bar or primarily use Classic/Pro controllers
 * can now use this program without trouble. The program will not be able to detect any DPAD button
 * presses in this mode, as it may interfere with the user who is navigating the pointer.
 *
 * Created by CreeperMario in July 2017 modified by GaryOderNichts in April 2020.
 * Modified by V10lator 2022
 */

#ifndef DWPAD_CONTROLLER_H_
#define DWPAD_CONTROLLER_H_

#include <gui/GuiController.h>
#include "dynamic_libs/padscore_functions.h"
#include "ControllerBase.h"

class DWPadController : public ControllerBase
{
public:
    
    //!Constructor
    DWPadController(int channel) : ControllerBase(channel)
    {
        memset(&kpadData, 0, sizeof(KPADData));
        
        data.validPointer = false;
    }
    
    //!Destructor
    virtual ~DWPadController() {}

    u32 remapWiiMoteButtons(u32 buttons)
    {
        u32 conv_buttons = 0;

        if(buttons & WPAD_BUTTON_LEFT)
            conv_buttons |= GuiTrigger::BUTTON_LEFT;

        if(buttons & WPAD_BUTTON_RIGHT)
            conv_buttons |= GuiTrigger::BUTTON_RIGHT;

        if(buttons & WPAD_BUTTON_DOWN)
            conv_buttons |= GuiTrigger::BUTTON_DOWN;

        if(buttons & WPAD_BUTTON_UP)
            conv_buttons |= GuiTrigger::BUTTON_UP;

        if(buttons & WPAD_BUTTON_PLUS)
            conv_buttons |= GuiTrigger::BUTTON_PLUS;

        if(buttons & WPAD_BUTTON_2)
            conv_buttons |= GuiTrigger::BUTTON_2;

        if(buttons & WPAD_BUTTON_1)
            conv_buttons |= GuiTrigger::BUTTON_1;

        if(buttons & WPAD_BUTTON_B)
            conv_buttons |= GuiTrigger::BUTTON_B;

        if(buttons & WPAD_BUTTON_A)
            conv_buttons |= GuiTrigger::BUTTON_A;

        if(buttons & WPAD_BUTTON_MINUS)
            conv_buttons |= GuiTrigger::BUTTON_MINUS;

        if(buttons & WPAD_BUTTON_Z)
            conv_buttons |= GuiTrigger::BUTTON_Z;

        if(buttons & WPAD_BUTTON_C)
            conv_buttons |= GuiTrigger::BUTTON_C;

        if(buttons & WPAD_BUTTON_HOME)
            conv_buttons |= GuiTrigger::BUTTON_HOME;

        return conv_buttons;
    }
    u32 remapClassicButtons(u32 buttons)
    {
        u32 conv_buttons = 0;

        if(buttons & WPAD_CLASSIC_BUTTON_LEFT)
            conv_buttons |= GuiTrigger::BUTTON_LEFT;

        if(buttons & WPAD_CLASSIC_BUTTON_RIGHT)
            conv_buttons |= GuiTrigger::BUTTON_RIGHT;

        if(buttons & WPAD_CLASSIC_BUTTON_DOWN)
            conv_buttons |= GuiTrigger::BUTTON_DOWN;

        if(buttons & WPAD_CLASSIC_BUTTON_UP)
            conv_buttons |= GuiTrigger::BUTTON_UP;

        if(buttons & WPAD_CLASSIC_BUTTON_PLUS)
            conv_buttons |= GuiTrigger::BUTTON_PLUS;

        if(buttons & WPAD_CLASSIC_BUTTON_X)
            conv_buttons |= GuiTrigger::BUTTON_X;

        if(buttons & WPAD_CLASSIC_BUTTON_Y)
            conv_buttons |= GuiTrigger::BUTTON_Y;

        if(buttons & WPAD_CLASSIC_BUTTON_B)
            conv_buttons |= GuiTrigger::BUTTON_B;

        if(buttons & WPAD_CLASSIC_BUTTON_A)
            conv_buttons |= GuiTrigger::BUTTON_A;

        if(buttons & WPAD_CLASSIC_BUTTON_MINUS)
            conv_buttons |= GuiTrigger::BUTTON_MINUS;

        if(buttons & WPAD_CLASSIC_BUTTON_HOME)
            conv_buttons |= GuiTrigger::BUTTON_HOME;

        if(buttons & WPAD_CLASSIC_BUTTON_ZR)
            conv_buttons |= GuiTrigger::BUTTON_ZR;

        if(buttons & WPAD_CLASSIC_BUTTON_ZL)
            conv_buttons |= GuiTrigger::BUTTON_ZL;

        if(buttons & WPAD_CLASSIC_BUTTON_R)
            conv_buttons |= GuiTrigger::BUTTON_R;

        if(buttons & WPAD_CLASSIC_BUTTON_L)
            conv_buttons |= GuiTrigger::BUTTON_L;

        return conv_buttons;
    }
    
    virtual bool update(int width, int height) override
    {
        ControllerBase::update(width, height);

        lastData = data;
        u32 controller_type;
        
        //! check if the controller is connected
        if(WPADProbe(chanIdx-1, &controller_type) != 0)
            return false;
        
        KPADRead(chanIdx-1, &kpadData, 1);
        
        data.validPointer = (kpadData.pos_valid == 1 || kpadData.pos_valid == 2) && (kpadData.pos_x >= -1.0f && kpadData.pos_x <= 1.0f) && (kpadData.pos_y >= -1.0f && kpadData.pos_y <= 1.0f);
        if(data.validPointer)
        {
            data.x = (width >> 1) * kpadData.pos_x;
            data.y = (height >> 1) * (-kpadData.pos_y);

            if(kpadData.angle_y > 0.0f)
                data.pointerAngle = (-kpadData.angle_x + 1.0f) * 0.5f * 180.0f;
            else
                data.pointerAngle = (kpadData.angle_x + 1.0f) * 0.5f * 180.0f - 180.0f;
        }
        else
        {
            data.validPointer = true;

            data.x += kpadData.nunchuck.stick_x * 20;
            data.y += kpadData.nunchuck.stick_y * 20;
        }

        data.buttons_r = remapWiiMoteButtons(kpadData.btns_r);
        data.buttons_h = remapWiiMoteButtons(kpadData.btns_h);
        data.buttons_d = remapWiiMoteButtons(kpadData.btns_d);

        if(kpadData.device_type > 1)
        {
            data.validPointer = true;

            data.x += kpadData.classic.lstick_x * 20;
            data.y += kpadData.classic.lstick_y * 20;

            data.buttons_r |= remapClassicButtons(kpadData.classic.btns_r);
            data.buttons_h |= remapClassicButtons(kpadData.classic.btns_h);
            data.buttons_d |= remapClassicButtons(kpadData.classic.btns_d);
        }

        if (data.x < -(width / 2)) data.x = -(width / 2);
        if (data.x > (width / 2)) data.x = (width / 2);
        if (data.y > (height / 2)) data.y = (height / 2);
        if (data.y < -(height / 2)) data.y = -(height / 2);
        
        if (!checkValidPointer())
            data.validPointer = false;

        return true;
    }
    
private:
    KPADData kpadData;
};

#endif
