//#include <string>
#include "Application.h"
#include <dynamic_libs/os_functions.h>
#include <dynamic_libs/fs_functions.h>
#include <dynamic_libs/gx2_functions.h>
#include <dynamic_libs/sys_functions.h>
#include <dynamic_libs/vpad_functions.h>
#include <dynamic_libs/padscore_functions.h>
#include <dynamic_libs/socket_functions.h>
#include <dynamic_libs/ax_functions.h>
#include <fs/FSUtils.h>
#include <fs/sd_fat_devoptab.h>
#include <system/memory.h>
#include <utils/logger.h>
#include <utils/utils.h>
#include "common/common.h"

/* Entry point */
extern "C" int Menu_Main(void)
{
    //!*******************************************************************
    //!                   Initialize function pointers                   *
    //!*******************************************************************
    //! do OS (for acquire) and sockets first so we got logging
    InitOSFunctionPointers();
    InitSocketFunctionPointers();

    log_init();
    // For some reason this is required. Otherwise the HBL would softlock on loading application when the logging is disabled
    // in the Makefile
    socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    DEBUG_FUNCTION_LINE("Starting launcher\n");

    InitFSFunctionPointers();
    InitGX2FunctionPointers();
    InitSysFunctionPointers();
    InitVPadFunctionPointers();
    InitPadScoreFunctionPointers();
    InitAXFunctionPointers();

    DEBUG_FUNCTION_LINE("Function exports loaded\n");

    //!*******************************************************************
    //!                    Initialize heap memory                        *
    //!*******************************************************************
    DEBUG_FUNCTION_LINE("Initialize memory management\n");
    memoryInitialize();

    //!*******************************************************************
    //!                        Initialize FS                             *
    //!*******************************************************************
    DEBUG_FUNCTION_LINE("Mount SD partition\n");
    mount_sd_fat("sd");

    //!*******************************************************************
    //!                    Enter main application                        *
    //!*******************************************************************
    DEBUG_FUNCTION_LINE("Start main application\n");
    int returnCode = Application::instance()->exec();

    DEBUG_FUNCTION_LINE("Main application stopped\n");

    Application::destroyInstance();

    DEBUG_FUNCTION_LINE("Unmount SD\n");
    unmount_sd_fat("sd");
    DEBUG_FUNCTION_LINE("Release memory\n");
    memoryRelease();
    //log_deinit();

    return returnCode;
}

