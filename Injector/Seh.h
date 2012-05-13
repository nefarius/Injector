#pragma once

// Function-local SEH guard. Proxies SEH to C++ EH
// Catch via SehException
class SehGuard
{
public:
	SehGuard();
	~SehGuard();

private:
	void* m_prev;
};

// SEH proxy exception.
// Catch this to catch structured exceptions as C++ exceptions.
// Must have SehGuard object created for it to work (function-local).
class SehException
{
public:
	SehException(int Code, struct _EXCEPTION_POINTERS* pException);

	unsigned int GetCode() const;
	struct _EXCEPTION_POINTERS* GetExceptionPointers() const;

private:
	unsigned int m_Code;
	struct _EXCEPTION_POINTERS* m_pException;
};

// SEH to C++ EH proxy function
extern void SehTranslatorFunction(unsigned int, struct _EXCEPTION_POINTERS*);
// Generic unhandled exception filter
extern long __stdcall MyGenericUnhandledExceptionFilter(struct _EXCEPTION_POINTERS* ExceptionInfo);
