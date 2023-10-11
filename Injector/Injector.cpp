// Windows Includes
#include <Windows.h>
#include <TlHelp32.h>
#include <malloc.h>
#include <Tchar.h>

// C++ Standard Library
#include <vector>

// StealthLib
#include "Injector.h"
#include "EnsureCleanup.h"
#include "StringUtil.h"
#include "UniUtil.h"

// Static data
Injector* Injector::m_pSingleton = nullptr;

// Get injector singleton
Injector* Injector::Get()
{
	if (!m_pSingleton)
		m_pSingleton = new Injector();
	return m_pSingleton;
}

// Injector constructor
Injector::Injector()
{ }

bool Injector::icompare_pred(const wchar_t a, const wchar_t b)
{
	return std::tolower(a) == std::tolower(b);
}

bool Injector::icompare(const std::wstring& a, const std::wstring& b) const
{
	if (a.length() == b.length())
	{
		return std::equal(b.begin(), b.end(),
		                  a.begin(), icompare_pred);
	}
	return false;
}

// Check if a module is injected via process ID, and return the base address
BYTE* Injector::GetModuleBaseAddress(DWORD ProcID, const std::wstring& Path) {
	// Grab a new snapshot of the process
	EnsureCloseHandle Snapshot(CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, ProcID));
	if (Snapshot == INVALID_HANDLE_VALUE)
		throw std::runtime_error("Could not get module snapshot for remote process.");;

	// Get the HMODULE of the desired library
	MODULEENTRY32W ModEntry = { sizeof(ModEntry) };
	bool Found = false;
	BOOL bMoreMods = Module32FirstW(Snapshot, &ModEntry);
	for (; bMoreMods; bMoreMods = Module32NextW(Snapshot, &ModEntry)) 
	{
		std::wstring ModuleName(ModEntry.szModule);
		std::wstring ExePath(ModEntry.szExePath);
		Found = (icompare(ModuleName, Path) || icompare(ExePath, Path));
		if (Found)
			return ModEntry.modBaseAddr;
	}
	return nullptr;
}

// MBCS version of GetModuleBaseAddress
BYTE* Injector::GetModuleBaseAddress(DWORD ProcID, const std::string& Path)
{
	// Convert path to unicode
	std::wstring UnicodePath(Path.begin(),Path.end());

	// Call the Unicode version of the function to actually do the work.
	return GetModuleBaseAddress(ProcID, UnicodePath);
}

// Injects a module (fully qualified path) via process id
void Injector::InjectLib(DWORD ProcID, const std::wstring& Path)
{
	// Get a handle for the target process.
	EnsureCloseHandle Process(OpenProcess(
		PROCESS_QUERY_INFORMATION |   // Required by Alpha
		PROCESS_CREATE_THREAD     |   // For CreateRemoteThread
		PROCESS_VM_OPERATION      |   // For VirtualAllocEx/VirtualFreeEx
		PROCESS_VM_WRITE,             // For WriteProcessMemory
		FALSE, ProcID));
	if (!Process) 
		throw std::runtime_error("Could not get handle to process.");

	// Calculate the number of bytes needed for the DLL's pathname
	size_t Size  = (Path.length() + 1) * sizeof(wchar_t);

	// Allocate space in the remote process for the pathname
	EnsureReleaseRegionEx LibFileRemote(VirtualAllocEx(Process, NULL, Size, MEM_COMMIT, PAGE_READWRITE),
		Process);
	if (!LibFileRemote)
		throw std::runtime_error("Could not allocate memory in remote process.");

	// Copy the DLL's pathname to the remote process' address space
	if (!WriteProcessMemory(Process, LibFileRemote, 
		Path.c_str(), Size, NULL))
		throw std::runtime_error("Could not write to memory in remote process.");;

	// Get the real address of LoadLibraryW in Kernel32.dll
	HMODULE hKernel32 = GetModuleHandle(TEXT("Kernel32"));
	if (!hKernel32)
		throw std::runtime_error("Could not get handle to Kernel32.");
	PTHREAD_START_ROUTINE pfnThreadRtn = reinterpret_cast<PTHREAD_START_ROUTINE>
		(GetProcAddress(hKernel32, "LoadLibraryW"));
	if (!pfnThreadRtn)
		throw std::runtime_error("Could not get pointer to LoadLibraryW.");

	// Create a remote thread that calls LoadLibraryW(DLLPathname)
	EnsureCloseHandle Thread(CreateRemoteThread(Process, NULL, 0, pfnThreadRtn, 
		LibFileRemote, 0, NULL));
	if (!Thread)
		throw std::runtime_error("Could not create thread in remote process.");

	// Wait for the remote thread to terminate
	WaitForSingleObject(Thread, INFINITE);

	// it's possible that we get a thread exit code of 0 with a non-zero HMODULE,
	// as the thread exit code is a DWORD, which is smaller than an HMODULE - so,
	// check the process list.
	if (!GetModuleBaseAddress(ProcID, Path))
		throw std::runtime_error("Call to LoadLibraryW in remote process failed.");
}

// MBCS version of InjectLib
void Injector::InjectLib(DWORD ProcID, const std::string& Path)
{
	// Convert path to unicode
	std::wstring UnicodePath(Path.begin(),Path.end());

	// Call the Unicode version of the function to actually do the work.
	InjectLib(ProcID, UnicodePath);
}

// Ejects a module (fully qualified path) via process id
void Injector::EjectLib(DWORD ProcID, const std::wstring& Path)
{
	const auto BaseAddress = GetModuleBaseAddress(ProcID, Path);
	if (!BaseAddress)
		throw std::runtime_error("Could not find module in remote process.");;

	// Get a handle for the target process.
	EnsureCloseHandle Process(OpenProcess(
		PROCESS_QUERY_INFORMATION |   
		PROCESS_CREATE_THREAD     | 
		PROCESS_VM_OPERATION,  // For CreateRemoteThread
		FALSE, ProcID));
	if (!Process) 
		throw std::runtime_error("Could not get handle to process.");

	// Get the real address of LoadLibraryW in Kernel32.dll
	HMODULE hKernel32 = GetModuleHandle(TEXT("Kernel32"));
	if (hKernel32 == NULL) 
		throw std::runtime_error("Could not get handle to Kernel32.");
	PTHREAD_START_ROUTINE pfnThreadRtn = (PTHREAD_START_ROUTINE)
		GetProcAddress(hKernel32, "FreeLibrary");
	if (pfnThreadRtn == NULL) 
		throw std::runtime_error("Could not get pointer to FreeLibrary.");

	// Create a remote thread that calls FreeLibrary()
	EnsureCloseHandle Thread(CreateRemoteThread(Process, NULL, 0, 
		pfnThreadRtn, BaseAddress, 0, NULL));
	if (!Thread) 
		throw std::runtime_error("Could not create thread in remote process.");

	// Wait for the remote thread to terminate
	WaitForSingleObject(Thread, INFINITE);

	// Get thread exit code
	DWORD ExitCode;
	if (!GetExitCodeThread(Thread,&ExitCode))
		throw std::runtime_error("Could not get thread exit code.");

	// Check LoadLibrary succeeded and returned a module base
	if(!ExitCode)
		throw std::runtime_error("Call to FreeLibrary in remote process failed.");
}

// MBCS version of EjectLib
void Injector::EjectLib(DWORD ProcID, const std::string& Path)
{
	// Convert path to unicode
	std::wstring UnicodePath(Path.begin(),Path.end());

	// Call the Unicode version of the function to actually do the work.
	EjectLib(ProcID, UnicodePath);
}

// Gives the current process the SeDebugPrivilege so we can get the
// required process handle.
// Note: Requires administrator rights
void Injector::GetSeDebugPrivilege()
{
	// Open current process token with adjust rights
	HANDLE TempToken;
	BOOL RetVal = OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES
		| TOKEN_QUERY, &TempToken);
	if (!RetVal) 
		throw std::runtime_error("Could not open process token.");
	EnsureCloseHandle Token(TempToken);

	// Get the LUID for SE_DEBUG_NAME 
	LUID Luid = { NULL }; // Locally unique identifier
	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &Luid)) 
		throw std::runtime_error("Could not look up privilege value for SeDebugName.");
	if (Luid.LowPart == NULL && Luid.HighPart == NULL) 
		throw std::runtime_error("Could not get LUID for SeDebugName.");

	// Process privileges
	TOKEN_PRIVILEGES Privileges = { NULL };
	// Set the privileges we need
	Privileges.PrivilegeCount = 1;
	Privileges.Privileges[0].Luid = Luid;
	Privileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	// Apply the adjusted privileges
	if (!AdjustTokenPrivileges(Token, FALSE, &Privileges,
		sizeof (Privileges), NULL, NULL)) 
		throw std::runtime_error("Could not adjust token privileges.");
}

// Get fully qualified path from module name. Assumes base directory is the
// directory of the currently executing binary.
std::tstring Injector::GetPath( const std::tstring& ModuleName )
{
	// Get handle to self
	HMODULE Self = GetModuleHandle(NULL);

	// Get path to loader
	std::vector<TCHAR> LoaderPath(MAX_PATH);
	if (!GetModuleFileName(Self,&LoaderPath[0],static_cast<DWORD>(LoaderPath.size())) || 
		GetLastError() == ERROR_INSUFFICIENT_BUFFER)
		throw std::runtime_error("Could not get path to loader.");

	// Convert path to loader to path to module
	std::tstring ModulePath(&LoaderPath[0]);
	ModulePath = ModulePath.substr(0, ModulePath.rfind( _T("\\") ) + 1);
	ModulePath.append(ModuleName);

	// Check path/file is valid
	if (GetFileAttributes(ModulePath.c_str()) == INVALID_FILE_ATTRIBUTES)
	{
#ifdef _UNICODE
		std::string NarrowModulePath(ConvertWideToANSI(ModulePath));
#else
		std::string NarrowModulePath(ModulePath.begin(), ModulePath.end());
#endif
		throw std::runtime_error("Could not find module. Path: '" + NarrowModulePath + "'.");
	}

	// Return module path
	return ModulePath;
}

// Get process ID via name (must pass name as lowercase)
DWORD Injector::GetProcessIdByName(const std::tstring& Name)
{
	// Grab a new snapshot of the process
	EnsureCloseHandle Snap(CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0));
	if (Snap == INVALID_HANDLE_VALUE)
		throw std::runtime_error("Could not get process snapshot.");

	// Search for process
	PROCESSENTRY32 ProcEntry = { sizeof(ProcEntry) };
	bool Found = false;
	BOOL MoreMods = Process32First(Snap, &ProcEntry);
	for (; MoreMods; MoreMods = Process32Next(Snap, &ProcEntry)) 
	{
		std::tstring CurrentProcess(ProcEntry.szExeFile);
        CurrentProcess = toLower(CurrentProcess);
		Found = (CurrentProcess == Name);
		if (Found) break;
	}

	// Check process was found
	if (!Found)
		throw std::runtime_error("Could not find process.");

	// Return PID
	return ProcEntry.th32ProcessID;
}

// Get process id from window name
DWORD Injector::GetProcessIdByWindow(const std::tstring& Name)
{
	// Find window
	HWND MyWnd = FindWindow(NULL,Name.c_str());
	if (!MyWnd)
		throw std::runtime_error("Could not find window.");

	// Get process ID from window
	DWORD ProcID;
	GetWindowThreadProcessId(MyWnd,&ProcID);
	if (!ProcID)
		throw std::runtime_error("Could not get process id from window.");

	// Return process id
	return ProcID;
}
