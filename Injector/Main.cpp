// Injector
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

		// Injector version number
		const std::tstring VerNum(_T("20120519c"));

		// Version and copyright output
#ifdef _WIN64
		std::tcout << _T("Injector x64 [Version ") << VerNum << _T("]") << std::endl;
#else
		std::tcout << _T("Injector x86 [Version ") << VerNum << _T("]") << std::endl;
#endif
		std::tcout << _T("Copyright (c) 2009 Cypher. All rights reserved.") << std::endl << std::endl;

		enum ExtraOptions
		{
			OptModuleName = 4,
			OptInject,
			OptEject
		};

		static struct option long_options[] =
		{
			{ _T("process-name"),	ARG_REQ,	0, FindProcName },
			{ _T("window-name"),	ARG_REQ,	0, FindProcWindow },
			{ _T("process-id"),		ARG_REQ,	0, FindProcID },
			{ _T("module-name"),	ARG_REQ,	0, OptModuleName },
			{ _T("inject"),			ARG_NONE,	0, OptInject },
			{ _T("eject"),			ARG_NONE,	0, OptEject }
		};

		// Variable to store process ID
		DWORD ProcID = 0;
		// Fully qualified module path
		std::tstring ModulePath;

		int option_index = 0, c, action = 0;

		do 
		{
			// Parse long arguments passed
			c = getopt_long(argc, argv, _T(""), long_options, &option_index);

			// Switch over method
			switch (c)
			{
			case OptModuleName:
				// ModulePath = Injector::Get()->GetPath(optarg);
				ModulePath = optarg;
				break;
				// Find and inject via process name
			case FindProcName:
				{
					// Get entered process name and convert to lowercase
					std::tstring ProcessName(optarg);
					std::transform(ProcessName.begin(),ProcessName.end(),ProcessName.begin(),tolower);

					// Attempt injection via process name
					ProcID = Injector::Get()->GetProcessIdByName(ProcessName);

					break;
				}
				// Find and inject via window name
			case FindProcWindow:
				{
					// Get entered window name
					std::tstring WindowName(optarg);

					// Attempt injection via window name
					ProcID = Injector::Get()->GetProcessIdByWindow(WindowName);

					break;
				}
				// Find and inject via process id
			case FindProcID:
				{
					std::tstring ProcessID(optarg);

					// Check if entered PID was valid
					ProcID = _tstoi(ProcessID.c_str());

					break;
				}
			case OptInject:
				{
					action = OptInject;
					break;
				}
			case OptEject:
				{
					action = OptEject;
					break;
				}
				// Ignore end of argument list
			case -1:
				{
					break;
				}
			default:
				{
					// Print usage of command line arguments
					std::tcout << _T("It looks like you didn't pass some valid parameters. Pease use:") 
						<< std::endl << std::endl;
					std::tcout << _T("--process-name notepad.exe") << std::endl;
					std::tcout << _T("\tIdentifies the target process by it's module name.") 
						<< std::endl << std::endl;
					std::tcout << _T("--window-name \"Unnamed - Editor\"") << std::endl;
					std::tcout << _T("\tIdentifies the target process by it's main windows name.") 
						<< std::endl << std::endl;
					std::tcout << _T("--process-id 7968") << std::endl;
					std::tcout << _T("\tIdentifies the target process by it's PID.") 
						<< std::endl << std::endl;
					std::tcout << _T("--module-name C:\\temp\\mylib.dll") << std::endl;
					std::tcout << _T("\tSets the absolute path of the DLL to be in-/ejected.") 
						<< std::endl << std::endl;
					std::tcout << _T("--inject or --eject") << std::endl;
					std::tcout << _T("\tSpecifies the action to perform (inject or eject the DLL).") 
						<< std::endl << std::endl;

					break;
				}
			}
		} while (c != -1);

		// Get privileges required to perform the injection
		Injector::Get()->GetSeDebugPrivilege();
		
		switch(action)
		{
		case OptInject:
			{
				// Inject module
				Injector::Get()->InjectLib(ProcID,ModulePath);
				// If we get to this point then no exceptions have been thrown so we
				// assume success.
				std::tcout << "Successfully injected module!" << std::endl;
				break;
			}
		case OptEject:
			{
				// Eject module
				Injector::Get()->EjectLib(ProcID,ModulePath);
				// If we get to this point then no exceptions have been thrown so we
				// assume success.
				std::tcout << "Successfully ejected module!" << std::endl;
				break;
			}
		default:
			{
				std::tcout << "No action specified!" << std::endl;
				break;
			}
		}
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
