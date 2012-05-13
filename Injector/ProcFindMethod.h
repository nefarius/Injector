#pragma once

// StealthLib
#include "StringWrap.h"

// Windows API
#include <tchar.h>

// C++ Standard Library
#include <iostream>

// Enum for process searching methods
enum FindProcMethod
{
	FindProcName = 1,
	FindProcWindow,
	FindProcID,
};

// Input stream overload
// Needed to avoid casting
std::tistream& operator>> ( std::tistream& in, FindProcMethod& x )
{
	int val;
	if (in >> val)
	{
		switch (val)
		{
		case FindProcName:
		case FindProcWindow:
		case FindProcID:
			x = static_cast<FindProcMethod>(val);
			break;
		default:
			throw std::out_of_range("Invalid value for process finding method.");
		}
	}

	return in;
}

// Output stream overload
// Needed to avoid nasty preprocessor generated lists or explicit converter functions
std::tostream& operator<< ( std::tostream& out, const FindProcMethod& x )
{
	switch (x) 
	{
	case FindProcName:
		out << _T("FindProcName");
		break;
	case FindProcWindow: 
		out << _T("FindProcWindow");
		break;
	case FindProcID:
		out << _T("FindProcID");
		break;
	}

	return out;
}
