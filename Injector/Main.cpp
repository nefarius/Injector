// Injector
#include "Injector.h"
#include "Seh.h"
#include "StringWrap.h"
#include "argh.h"
#include "StringUtil.h"

// Windows API
#include <Windows.h>
#include <tchar.h>
#include <TlHelp32.h>
#include <Shlwapi.h>

// C++ Standard Library
#include <iostream>
#include <string>
#include <locale>


// Entry point
int main(int, char* argv[])
{
    try
    {
        // Needed to proxy SEH exceptions to C++ exceptions
        SehGuard Guard;

        // Injector version number
        const std::tstring VerNum(_T("20180821"));

        // Version and copyright output
#ifdef _WIN64
        std::tcout << _T("Injector x64 [Version ") << VerNum << _T("]") << std::endl;
#else
        std::tcout << _T("Injector x86 [Version ") << VerNum << _T("]") << std::endl;
#endif
        std::tcout << _T("Copyright (c) 2009 Cypher, 2012-2018 Nefarius. All rights reserved.") << std::endl << std::endl;

        argh::parser cmdl;

        cmdl.add_params({
            "n", "process-name",
            "w", "window-name",
            "p", "process-id",
            "m", "module-name"
            });

        cmdl.parse(argv);

        // Display help
        if (cmdl[{ "-h", "--help" }])
        {
            std::cout << "usage: Injector [options]" << std::endl << std::endl;
            std::cout << "  specify at least one of the following methods:" << std::endl;
            std::cout << "    -n, --process-name        Identify target process by process name" << std::endl;
            std::cout << "    -w, --window-name         Identify target process by window title" << std::endl;
            std::cout << "    -p, --process-id          Identify target process by numeric ID" << std::endl << std::endl;
            std::cout << "  specify DLL name or absolute path:" << std::endl;
            std::cout << "    -m, --module-name         DLL name or absolute path" << std::endl << std::endl;
            std::cout << "  specify at least one of the following actions:" << std::endl;
            std::cout << "    -i, --inject              Inject/load referenced module" << std::endl;
            std::cout << "    -e, --eject               Eject/unload referenced module" << std::endl;
            std::cout << std::endl;

            return ERROR_SUCCESS;
        }

        // Check if at least one action is specified
        if (!cmdl[{ "-i", "--inject", "-e", "--eject" }])
        {
            std::tcout << "No action specified!" << std::endl;
            return ERROR_INVALID_PARAMETER;
        }

        // Check if user wants more than we can handle ;)
        if (cmdl[{ "-i", "--inject" }] && cmdl[{ "-e", "--eject" }])
        {
            std::tcout << "Only one action at a time allowed!" << std::endl;
            return ERROR_INVALID_PARAMETER;
        }

        // Check if there's at least one process identification method specified
        if (!(cmdl({ "-n", "--process-name" }))
            && !(cmdl({ "-w", "--window-name" }))
            && !(cmdl({ "-p", "--process-id" })))
        {
            std::tcout << "No process identifier specified!" << std::endl;
            return ERROR_INVALID_PARAMETER;
        }

        // Check for the module we should work with
        if (!(cmdl({ "-m", "--module-name" })))
        {
            std::tcout << "No module name or path specified!" << std::endl;
            return ERROR_INVALID_PARAMETER;
        }

        // Variable to store process ID
        DWORD ProcID = 0;
        // Fully qualified module path
        std::tstring ModulePath;

        // Temporary place for argument
        std::string optArg;

        // Get provided module name
        if ((cmdl({ "-m", "--module-name" }) >> optArg))
        {
            if (PathIsRelativeA(optArg.c_str()))
            {
                ModulePath = Injector::Get()->GetPath(utf8_to_wstr(optArg));
            }
            else
            {
                ModulePath = utf8_to_wstr(optArg);
            }
        }

        // Find and inject via process name
        if ((cmdl({ "-n", "--process-name" }) >> optArg))
        {
            // Attempt injection via process name
            ProcID = Injector::Get()->GetProcessIdByName(utf8_to_wstr(toLower(optArg)));
        }

        // Find and inject via window name
        if ((cmdl({ "-w", "--window-name" }) >> optArg))
        {
            // Attempt injection via window name
            ProcID = Injector::Get()->GetProcessIdByWindow(utf8_to_wstr(optArg));
        }

        // Find and inject via process id
        if ((cmdl({ "-p", "--process-id" }) >> optArg))
        {
            // Convert PID
            ProcID = _tstoi(utf8_to_wstr(optArg).c_str());

            if (ProcID == 0)
            {
                throw std::exception("Invalid PID entered!");
            }
        }

        // Get privileges required to perform the injection
        Injector::Get()->GetSeDebugPrivilege();

        // Inject action
        if (cmdl[{ "-i", "--inject" }])
        {
            // Inject module
            Injector::Get()->InjectLib(ProcID, ModulePath);
            // If we get to this point then no exceptions have been thrown so we
            // assume success.
            std::tcout << "Successfully injected module!" << std::endl;
        }

        // Eject action
        if (cmdl[{ "-e", "--eject" }])
        {
            // Eject module
            Injector::Get()->EjectLib(ProcID, ModulePath);
            // If we get to this point then no exceptions have been thrown so we
            // assume success.
            std::tcout << "Successfully ejected module!" << std::endl;
        }
    }
    // Catch STL-based exceptions.
    catch (const std::exception& e)
    {
        std::string TempError(e.what());
        std::tstring Error(TempError.begin(), TempError.end());
        std::tcout << "General Error:" << std::endl
            << Error << std::endl;
    }
    // Catch custom SEH-proxy exceptions.
    // Currently only supports outputting error code.
    // TODO: Convert to string and dump more verbose output.
    catch (const SehException& e)
    {
        std::tcout << "SEH Error:" << std::endl
            << e.GetCode() << std::endl;
    }
    // Catch any other unknown exceptions.
    // TODO: Find a better way to handle this. Should never happen anyway, but
    // you never know.
    // Note: Could use SetUnhandledExceptionFilter but would potentially be 
    // messy.
    catch (...)
    {
        std::tcout << "Unknown error!" << std::endl;
    }

    // Return success
    return ERROR_SUCCESS;
}
