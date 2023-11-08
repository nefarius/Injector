#pragma once

#include <string>
#include <locale>
#include <algorithm>
#include <iterator>


template <typename S_type>
S_type toLower(const S_type& in)
{
	S_type out;
	std::transform(in.begin(), in.end(), std::back_inserter(out), [](typename S_type::value_type ch)
	{
		return std::tolower(ch, std::locale());
	});
	return out;
}

inline std::wstring utf8_to_wstr(const std::string& utf8)
{
	const auto wideCharCount = MultiByteToWideChar(CP_UTF8, 0, utf8.data(), static_cast<int>(utf8.size()), nullptr, 0);
	std::wstring wstr;
	wstr.resize(wideCharCount);

	// While the docs say the return value includes trailing null, this is only the case if the
	// input has a trailing null, and if we include it in the size parameter.
	//
	// utf8.size() excludes the trailing null, so the wstring does too
	MultiByteToWideChar(CP_UTF8, 0, utf8.data(), static_cast<int>(utf8.size()), (LPWSTR)wstr.data(), wideCharCount);
	return wstr;
}
