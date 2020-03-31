#pragma once

// Macros to allow the writing of character-width independent STL code

#ifdef _UNICODE
#define tstring wstring
#define tfstream wfstream
#define tifstream wifstream
#define tofstream wofstream
#define tstringstream wstringstream
#define tcout wcout
#define tcin wcin
#define tcerr wcerr
#define tistream wistream
#define tostream wostream
#else
#define tstring string
#define tfstream fstream
#define tifstream ifstream
#define tofstream ofstream
#define tstringstream stringstream
#define tcout cout
#define tcin cin
#define tcerr cerr
#define tistream istream
#define tostream ostream
#endif // _UNICODE
