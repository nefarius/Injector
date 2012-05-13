// StealthLib
#include "Seh.h"
#include "StringWrap.h"

// Windows API
#include <Windows.h>
#include <Dbghelp.h>
#include <eh.h>
#include <tchar.h>

// C++ Standard Library
#include <string>
#include <vector>

// Proxies SEH to C++ EH
void SehTranslatorFunction(unsigned int Code, struct _EXCEPTION_POINTERS* pException)
{
	throw SehException(Code,pException);
}

// Constructor
SehGuard::SehGuard()
{
	m_prev = _set_se_translator(SehTranslatorFunction);
}

// Destructor
SehGuard::~SehGuard()
{
	_set_se_translator(reinterpret_cast<_se_translator_function>(m_prev));
}

// Proxy exception constructor
SehException::SehException( int Code, struct _EXCEPTION_POINTERS* pException ) 
: m_Code(Code), m_pException(pException)
{ }

// Get exception code
unsigned int SehException::GetCode() const
{
	return m_Code;
}

// Get exception data pointer
struct _EXCEPTION_POINTERS* SehException::GetExceptionPointers() const
{
	return m_pException;
}

// Generic unhandled exception filter
LONG WINAPI MyGenericUnhandledExceptionFilter(struct _EXCEPTION_POINTERS* ExceptionInfo)
{
	// Get the current time
	SYSTEMTIME sTime;
	GetLocalTime( &sTime );

	// Pull out the date
	std::vector<TCHAR> Date(10);
	GetDateFormat(LOCALE_USER_DEFAULT, 0, &sTime, TEXT("yyyyMMdd"), &Date[0], 
		10);

	// Pull out the time
	std::vector<TCHAR> Time(10);
	GetTimeFormat(LOCALE_USER_DEFAULT, TIME_FORCE24HOURFORMAT, &sTime, 
		TEXT("hhmmss"), &Time[0], 10);

	// Create a filename for the crash dump out of the current
	// date and time.
	std::tstring Path(TEXT("Crash-"));
	Path.append(&Date[0]).append(&Time[0]).append(TEXT(".txt"));

	// Create file to dump output
	HANDLE hFile = CreateFile(Path.c_str(), GENERIC_WRITE, 0, NULL,
		CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);

	// Create minidump
	MINIDUMP_EXCEPTION_INFORMATION aMiniDumpInfo;
	aMiniDumpInfo.ThreadId = GetCurrentThreadId();
	aMiniDumpInfo.ExceptionPointers = ExceptionInfo;
	aMiniDumpInfo.ClientPointers = TRUE;

	// Write minidump
	MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile,
		(MINIDUMP_TYPE) (MiniDumpWithFullMemory|MiniDumpWithHandleData),
		&aMiniDumpInfo, NULL, NULL);

	// Close file handle
	CloseHandle(hFile);

	// Execute handler
	return EXCEPTION_EXECUTE_HANDLER;
}
