// StealthLib
#include "Injector.h"
#include "Seh.h"
#include "StringWrap.h"
#include "ProcFindMethod.h"
#include "getopt.h"

// Windows API
#include <Windows.h>
#include <tchar.h>
#include <TlHelp32.h>

// C++ Standard Library
#include <iostream>
#include <string>
#include <algorithm>

// Entry point
int main()
{
	try
	{
		// Needed to proxy SEH exceptions to C++ exceptions
		SehGuard Guard;

		// StealthLib version number
		const std::tstring VerNum(_T("20090421b"));

		// StealthLib module name
		// Different names for x86/x64
#ifdef _WIN64
		const std::tstring ModuleName(_T("Stealth64.dll"));
#else
		const std::tstring ModuleName(_T("Stealth86.dll"));
#endif // _WIN64

		// Version and copyright output
#ifdef _WIN64
		std::tcout << _T("StealthLib x64 [Version ") << VerNum << _T("]") << std::endl;
#else
		std::tcout << _T("StealthLib x86 [Version ") << VerNum << _T("]") << std::endl;
#endif
		std::tcout << _T("Copyright (c) 2009 Cypher. All rights reserved.") << std::endl << std::endl;

		// Prompt for input
		std::tcout << _T("Please choose a method to choose the target process:") << std::endl;
		std::tcout << _T("1. Process Name") << std::endl;
		std::tcout << _T("2. Window Name") << std::endl;
		std::tcout << _T("3. Process ID") << std::endl;
		std::tcout << _T("Choice: ") << std::endl;

		// Get method
		FindProcMethod InjectType;
		std::tcin >> InjectType;
		if (!std::tcin.good())
			throw new std::out_of_range("Invalid input for search method.");

		// Variable to store process ID
		DWORD ProcID;
		// Fully qualified module path
		std::tstring ModulePath(Injector::Get()->GetPath(ModuleName));

		// Switch over method
		switch (InjectType)
		{
			// Find and inject via process name
		case FindProcName:
			{
				// Prompt for input
				std::tcout << _T("Please enter the process name: ") << std::endl;

				// Get entered process name and convert to lowercase
				std::tstring ProcessName;
				std::tcin.sync();
				std::getline(std::tcin,ProcessName);
				std::transform(ProcessName.begin(),ProcessName.end(),ProcessName.begin(),tolower);

				// Attempt injection via process name
				ProcID = Injector::Get()->GetProcessIdByName(ProcessName);

				break;
			}
			// Find and inject via window name
		case FindProcWindow:
			{
				// Prompt for input
				std::tcout << _T("Please enter the window name: ") << std::endl;

				// Get entered window name
				std::tstring WindowName;
				std::tcin.sync();
				std::getline(std::tcin,WindowName);

				// Attempt injection via window name
				ProcID = Injector::Get()->GetProcessIdByWindow(WindowName);
			
				break;
			}
			// Find and inject via process id
		case FindProcID:
			{
				// Prompt for input
				std::tcout << _T("Please enter the process identifier: ") << std::endl;

				// Get entered process id
				std::tcin.sync();
				std::tcin >> ProcID;

				// Check if entered PID was valid
				if (!std::tcin.good())
					throw std::out_of_range("Invalid input for process id.");

				break;
			}
			// Unknown method
		default:
			throw new std::out_of_range("Invalid value for process finding method.");
		}

		// Get privileges required to perform the injection
		Injector::Get()->GetSeDebugPrivilege();
		// Inject module
		Injector::Get()->InjectLib(ProcID,ModulePath);
		// If we get to this point then no exceptions have been thrown so we
		// assume success.
		std::tcout << "Successfully injected module!" << std::endl;
		// Stop automatic ejection
		std::tcin.clear();
		std::tcin.sync();
		std::tcin.get();
		// Eject module
		Injector::Get()->EjectLib(ProcID,ModulePath);
		// If we get to this point then no exceptions have been thrown so we
		// assume success.
		std::tcout << "Successfully ejected module!" << std::endl;

	}
	// Catch STL-based exceptions.
	catch (const std::exception& e)
	{
		std::string TempError(e.what());
		std::tstring Error(TempError.begin(),TempError.end());
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

	// Stop window from automatically closing
	std::tcin.clear();
	std::tcin.sync();
	std::tcin.get();

	// Return success
	return 0;
}
