#pragma once

#include <stdbool.h>

struct IConsole
{
    void* filler[0x22];
    //! Execute a string in the console.
    //! \param command Console command e.g. "map testy" - no leading slash
    //! \param bSilentMode true=suppresses log in error case and logging the command to the console
    //! \param bDeferExecution true=the command is stored in special fifo that allows delayed execution by using wait_seconds and wait_frames commands
    //! \par Example
    //! \include CrySystem/Examples/ConsoleCommand.cpp
    void (*ExecuteString)(void* _this, const char* command, const bool bSilentMode, const bool bDeferExecution);
};
