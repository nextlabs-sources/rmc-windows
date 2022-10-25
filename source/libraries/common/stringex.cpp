
#include "stdafx.h"

#include <Wincrypt.h>

#include <nx\common\stringex.h>

using namespace NX::conv;

std::string NX::conv::utf16toutf8(const std::wstring& ws)
{
    std::string s;
    if (!ws.empty()) {
        const int reserved_size = (int)(ws.length() * 3 + 1);
        if (0 == WideCharToMultiByte(CP_UTF8, 0, ws.c_str(), (int)ws.length(), NX::string_buffer(s, reserved_size), (int)reserved_size, nullptr, nullptr)) {
            s.clear();
        }
    }
    return s;
}

std::wstring NX::conv::utf8toutf16(const std::string& s)
{
    std::wstring ws;
    if (!s.empty()) {
        const int reserved_size = (int)s.length();
        if (0 == MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.length(), NX::wstring_buffer(ws, reserved_size), (int)reserved_size)) {
            ws.clear();
        }
    }
    return ws;
}

std::wstring NX::conv::ansitoutf16(const std::string& s)
{
    std::wstring ws;
    if (!s.empty()) {
        const int reserved_size = (int)s.length();
        if (0 == MultiByteToWideChar(CP_ACP, 0, s.c_str(), (int)s.length(), NX::wstring_buffer(ws, reserved_size), (int)reserved_size)) {
            ws.clear();
        }
    }
    return ws;
}

std::wstring NX::conv::to_wstring(const std::wstring& s)
{
    return s;
}

std::wstring NX::conv::to_wstring(const std::string& s)
{
    return utf8toutf16(s);
}

std::string NX::conv::to_string(const std::wstring& s)
{
    return utf16toutf8(s);
}

std::string NX::conv::to_string(const std::string& s)
{
    return s;
}


std::vector<unsigned char> NX::conv::Base64Decode(const std::string& base64)
{
	std::vector<unsigned char> buf;
	unsigned long out_size = 0;
	CryptStringToBinaryA(base64.data(), (unsigned long)base64.length(), CRYPT_STRING_ANY, NULL, &out_size, NULL, NULL);
	buf.resize(out_size, 0);
	if (!CryptStringToBinaryA(base64.data(), (unsigned long)base64.length(), CRYPT_STRING_ANY, buf.data(), &out_size, NULL, NULL)) {
		buf.clear();
	}
	return buf;
}

std::vector<unsigned char> NX::conv::Base64Decode(const std::wstring& base64)
{
	std::string s = NX::conv::utf16toutf8(base64);
	return Base64Decode(s);
}

std::string NX::conv::Base64Encode(const std::vector<unsigned char>& data)
{
	return Base64Encode(data.data(), (unsigned long)data.size());
}

std::string NX::conv::Base64Encode(const unsigned char* data, unsigned long size)
{
	unsigned long out_size = 0;
	CryptBinaryToStringA(data, size, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, NULL, &out_size);
	std::vector<char> buf;
	buf.resize(out_size + 1, 0);
	if (!CryptBinaryToStringA(data, size, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, buf.data(), &out_size)) {
		buf.clear();
		return std::string();
	}
	return std::string(buf.data());
}

std::string NX::conv::UrlEncode(const std::string& url, bool is_x_www_form)
{
	std::string s;
	std::for_each(url.begin(), url.end(), [&](const char& c) {
		if (isalnum(c) || c == '-' || c == '_' || c == '~' || (is_x_www_form && (c == '=' || c == '&' || c == '.'))) {
			s.push_back(c);
		}
		else {
			const std::string& hexs = utohs<char>((unsigned char)c);
			s.push_back('%');
			s.append(hexs);
		}
	});
	return s;
}

std::string NX::conv::UrlDecode(const std::string& url)
{
	std::string s;
	const char* p = url.c_str();

	while (*p) {
		if ('%' == *p) {
			++p;
			if (*p && *(p + 1)) {
				if (!ishex(*p) || !ishex(*(p + 1)))
					break;
				char c = ctoi<char>(*(p++));
				c <<= 4;
				c |= (char)ctoi<char>(*(p++));
				s.push_back(c);
			}
			else {
				break;
			}
		}
		else {
			s.push_back(*(p++));
		}
	}

	return s;
}

std::string NX::FormatString(const char* format, ...)
{
	va_list args;
	int     len = 0;
	std::string s;

	va_start(args, format);
	len = _vscprintf_l(format, 0, args) + 1;
	vsprintf_s(string_buffer(s, len), len, format, args);
	va_end(args);

	return s;
}

std::wstring NX::FormatString(const wchar_t* format, ...)
{
	va_list args;
	int     len = 0;
	std::wstring s;

	va_start(args, format);
	len = _vscwprintf_l(format, 0, args) + 1;
	vswprintf_s(wstring_buffer(s, len), len, format, args);
	va_end(args);

	return s;
}
