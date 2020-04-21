/**
 * Base for controllers
 *
 * Created by GaryOderNichts in April 2020.
 */

#ifndef CONTROLLER_BASE_H_
#define CONTROLLER_BASE_H_

#include <gui/GuiController.h>
#include "dynamic_libs/vpad_functions.h"

#define POINTER_TIMEOUT 250

class ControllerBase : public GuiController
{
public:
    
    //!Constructor
    ControllerBase(int channel) : GuiController(channel)
    {
        updates = 0;
        showPointer = false;
    }
    
    //!Destructor
    virtual ~ControllerBase() {}
    
    void invalidatePointer()
    {
        updates = POINTER_TIMEOUT;
    }

    virtual bool update(int width, int height) override
    {
        updates++;
        return true;
    }

    bool checkValidPointer()
    {
        if ((lastData.x == data.x && lastData.y == data.y))
        {
            if (updates >= POINTER_TIMEOUT)
            {
                // we did not move in time or our pointer is invalid
                showPointer = false;
                // do not overflow counter
                updates = POINTER_TIMEOUT + 1;

                return false;
            }
        }
        else
        {
            // we moved so show the pointer
            showPointer = true;

            // reset counter
            updates = 0;
        }

        return true;
    }

    bool showPointer;
    
protected:
    uint64_t updates;
};

#endif
