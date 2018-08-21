#pragma once

#include <string>
#include <locale>
#include <algorithm>
#include <iterator>
#include <codecvt>

template <typename S_type>
S_type toLower(const S_type& in)
{
    S_type out;
    std::transform(in.begin(), in.end(), std::back_inserter(out), [](S_type::value_type ch) {
        return std::tolower(ch, std::locale()); });
    return out;
}

inline std::wstring utf8_to_wstr(const std::string& utf8) {
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> wcu8;
    return wcu8.from_bytes(utf8);
}
