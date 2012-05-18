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
int _tmain(int argc, TCHAR** argv)
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

		static struct option long_options[] =
		{
			{ _T("process-name"),	ARG_REQ,	0, FindProcName },
			{ _T("window-name"),	ARG_REQ,	0, FindProcWindow },
			{ _T("process-id"),		ARG_REQ,	0, FindProcID },
			{ _T("module-name"),	ARG_REQ,	0, 0 },
			{ _T("inject"),			ARG_NONE,	0, 4 },
			{ _T("eject"),			ARG_NONE,	0, 5 }
		};

		// Variable to store process ID
		DWORD ProcID = 0;
		// Fully qualified module path
		std::tstring ModulePath;

		int option_index = 0, c;

		do 
		{
			c = getopt_long(argc, argv, _T(""), long_options, &option_index);

			// Switch over method
			switch (c)
			{
			case 0:
				ModulePath = Injector::Get()->GetPath(optarg);
				break;
				// Find and inject via process name
			case FindProcName:
				{
					// Get entered process name and convert to lowercase
					std::tstring ProcessName(optarg);
					std::transform(ProcessName.begin(),ProcessName.end(),ProcessName.begin(),tolower);

					std::tcout << _T("Process name given: ") << ProcessName << std::endl;

					// Attempt injection via process name
					ProcID = Injector::Get()->GetProcessIdByName(ProcessName);
					std::tcout << _T("Process identifier: ") << ProcID << std::endl;

					break;
				}
				// Find and inject via window name
			case FindProcWindow:
				{
					// Get entered window name
					std::tstring WindowName(optarg);

					std::tcout << _T("Window name given: ") << WindowName << std::endl;

					// Attempt injection via window name
					ProcID = Injector::Get()->GetProcessIdByWindow(WindowName);

					break;
				}
				// Find and inject via process id
			case FindProcID:
				{
					std::tstring ProcessID(optarg);

					std::tcout << _T("Process identifier given: ") << ProcessID << std::endl;

					// Check if entered PID was valid
					ProcID = _tstoi(ProcessID.c_str());

					break;
				}
			}
		} while (c != -1);

		// Get privileges required to perform the injection
		Injector::Get()->GetSeDebugPrivilege();

		optind = 1;

		do 
		{
			c = getopt_long(argc, argv, _T(""), long_options, &option_index);

			switch(c)
			{
			case 4:
				// Inject module
				Injector::Get()->InjectLib(ProcID,ModulePath);
				// If we get to this point then no exceptions have been thrown so we
				// assume success.
				std::tcout << "Successfully injected module!" << std::endl;
				break;
			case 5:
				// Eject module
				Injector::Get()->EjectLib(ProcID,ModulePath);
				// If we get to this point then no exceptions have been thrown so we
				// assume success.
				std::tcout << "Successfully ejected module!" << std::endl;
				break;
			}
		} while (c != -1);
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

	// Return success
	return ERROR_SUCCESS;
}
