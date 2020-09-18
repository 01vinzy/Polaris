#ifndef PLATFORM_H
#define PLATFORM_H

#include <windows.h>
#include <string>

#include "util.h"

namespace polaris
{
    class Platform
    {
    public:
        static std::string GetFontsDir()
        {
            char WinDir[MAX_PATH];
            if (!GetWindowsDirectoryA(WinDir, sizeof(WinDir)))
            {
                MessageBox(NULL, static_cast<LPCWSTR>(L"An unknown error has occured. Please relaunch Fortnite and try again!"), static_cast<LPCWSTR>(L"Error"), MB_ICONERROR);
                ExitProcess(EXIT_FAILURE);
            }

            return Util::GetConcatPath(WinDir, "Fonts");
        }
    };
}

#endif // PLATFORM_H