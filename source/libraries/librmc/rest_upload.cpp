
#include "stdafx.h"

#include <nx\rmc\rest.h>
#include <nx\rmc\user.h>

#include <nx\common\rapidjson.h>
#include <nx\common\stringex.h>
#include <nx\crypt\md5.h>



using namespace NX;
using namespace NX::rapidjson;
using namespace NX::REST::http;


static const std::string boundary("7e1c41151168");
static const std::string boundaryBegin("--" + boundary);
static const std::string boundaryEnding("\r\n--" + boundary + "--");

RmUploadBuffer::RmUploadBuffer(const std::string& apiInput, const std::wstring& file)
	: std::streambuf(), _h(INVALID_HANDLE_VALUE), _current(0), _feof(false),
	_header(InitHeader(apiInput, file)), _end(InitSize(file))
{
}

RmUploadBuffer::~RmUploadBuffer()
{
	clear();
}

void RmUploadBuffer::clear()
{
	if (INVALID_HANDLE_VALUE != _h) {
		CloseHandle(_h);
		_h = INVALID_HANDLE_VALUE;
	}
}

std::string RmUploadBuffer::InitHeader(const std::string& apiInput, const std::wstring& file)
{
	static const std::string api_input_disposition(boundaryBegin + "\r\nContent-Disposition: form-data; name=\"API-input\"\r\nContent-Type: application/json\r\n\r\n");
	static const std::string file_content_disposition_begin("\r\n" + boundaryBegin + "\r\nContent-Disposition: form-data; name=\"file\"; filename=\"");
	static const std::string file_content_disposition_end("\"\r\nContent-Type: application/octet-stream\r\n\r\n");

	auto pos = file.rfind(L'\\');
	const std::wstring fileName((std::wstring::npos == pos) ? file : file.substr(pos + 1));

	std:: string header(api_input_disposition);
	header.append(apiInput);
	header.append(file_content_disposition_begin);
	header.append(NX::conv::utf16toutf8(fileName));
	header.append(file_content_disposition_end);

	return header;
}

size_t RmUploadBuffer::InitSize(const std::wstring& file)
{
	size_t endPos = _header.length();

	_h = ::CreateFileW(file.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE != _h) {
		const DWORD fileSize = GetFileSize(_h, NULL);
		if(INVALID_FILE_SIZE != fileSize)
			endPos += fileSize;
	}
	endPos += boundaryEnding.length();
	return endPos;
}

std::streambuf::int_type RmUploadBuffer::underflow()
{
	if (_current >= _end) {
		_feof = true;
		return traits_type::eof();
	}

	if (_current < _header.length())
		return _header[_current];

	char c = 0;
	DWORD bytesRead = 0;
	if (!ReadFile(_h, &c, 1, &bytesRead, NULL)) {
		_feof = true;
		return traits_type::eof();
	}
	if (0 == bytesRead) {
		_feof = true;
		return traits_type::eof();
	}
	SetFilePointer(_h, -1, NULL, FILE_CURRENT);
	return c;
}

std::streambuf::int_type RmUploadBuffer::uflow()
{
	if (_current >= _end) {
		_feof = true;
		return traits_type::eof();
	}

	if (_current < _header.length()) {
		return _header[_current++];
	}
	else if (_current >= _header.length() && _current < (_end - boundaryEnding.length())) {
		char c = 0;
		DWORD bytesRead = 0;
		if (!ReadFile(_h, &c, 1, &bytesRead, NULL)) {
			_feof = true;
			return traits_type::eof();
		}
		if (0 == bytesRead) {
			_feof = true;
			return traits_type::eof();
		}
		if (c == 0) {
			int a = 1;
		}
		_current++;
		return c;
	}
	else {
		assert(_current >= (_end - boundaryEnding.length()));
		char c = boundaryEnding[boundaryEnding.length() - (_end - _current)];
		_current++;
		return c;
	}
}

std::streambuf::int_type RmUploadBuffer::pbackfail(std::streambuf::int_type ch)
{
	if (_current == 0 || (ch != traits_type::eof() && ch != 0xCC))
		return traits_type::eof();


	if (_current <= _header.length())
		return _header[--_current];

	char c = 0;
	DWORD bytesRead = 0;
	SetFilePointer(_h, -1, NULL, FILE_CURRENT);
	--_current;
	if (!ReadFile(_h, &c, 1, &bytesRead, NULL))
		return traits_type::eof();
	if (0 == bytesRead)
		return traits_type::eof();
	SetFilePointer(_h, -1, NULL, FILE_CURRENT);
	return c;
}

std::streamsize RmUploadBuffer::showmanyc()
{
	return (_end - _current);
}

std::streamsize RmUploadBuffer::xsgetn(char* _Ptr, std::streamsize _Count)
{	// get _Count characters from stream
	int_type _Meta;
	std::streamsize _Size, _Copied;

	for (_Copied = 0; 0 < _Count; ) {
		if (0 < (_Size = _Gnavail()))
		{	// copy from read buffer
			if (_Count < _Size)
				_Size = _Count;
			traits_type::copy(_Ptr, gptr(), (size_t)_Size);
			_Ptr += _Size;
			_Copied += _Size;
			_Count -= _Size;
			gbump((int)_Size);
		}
		else if (std::char_traits<char>::eq_int_type(traits_type::eof(), _Meta = uflow()) && _feof)
		{
			break;	// end of file, quit
		}
		else
		{	// get a single character
			*_Ptr++ = std::char_traits<char>::to_char_type(_Meta);
			++_Copied;
			--_Count;
		}
	}

	return (_Copied);
}


//
//
//

RmSimpleUploadBuffer::RmSimpleUploadBuffer(const std::wstring& file)
	: std::streambuf(), _h(INVALID_HANDLE_VALUE), _current(0), _feof(false),
	_end(InitSize(file))
{
}

RmSimpleUploadBuffer::~RmSimpleUploadBuffer()
{
	clear();
}

void RmSimpleUploadBuffer::clear()
{
	if (INVALID_HANDLE_VALUE != _h) {
		CloseHandle(_h);
		_h = INVALID_HANDLE_VALUE;
	}
}

size_t RmSimpleUploadBuffer::InitSize(const std::wstring& file)
{
	size_t endPos = 0;

	_h = ::CreateFileW(file.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE != _h) {
		const DWORD fileSize = GetFileSize(_h, NULL);
		if(INVALID_FILE_SIZE != fileSize)
			endPos += fileSize;
	}
	return endPos;
}

std::streambuf::int_type RmSimpleUploadBuffer::underflow()
{
	if (_current >= _end) {
		_feof = true;
		return traits_type::eof();
	}

	char c = 0;
	DWORD bytesRead = 0;
	if (!ReadFile(_h, &c, 1, &bytesRead, NULL)) {
		_feof = true;
		return traits_type::eof();
	}
	if (0 == bytesRead) {
		_feof = true;
		return traits_type::eof();
	}
	SetFilePointer(_h, -1, NULL, FILE_CURRENT);
	return c;
}

std::streambuf::int_type RmSimpleUploadBuffer::uflow()
{
	if (_current >= _end) {
		_feof = true;
		return traits_type::eof();
	}

	char c = 0;
	DWORD bytesRead = 0;
	if (!ReadFile(_h, &c, 1, &bytesRead, NULL)) {
		_feof = true;
		return traits_type::eof();
	}
	if (0 == bytesRead) {
		_feof = true;
		return traits_type::eof();
	}
	if (c == 0) {
		int a = 1;
	}
	_current++;
	return c;
}

std::streambuf::int_type RmSimpleUploadBuffer::pbackfail(std::streambuf::int_type ch)
{
	if (_current == 0 || (ch != traits_type::eof() && ch != 0xCC))
		return traits_type::eof();

	char c = 0;
	DWORD bytesRead = 0;
	SetFilePointer(_h, -1, NULL, FILE_CURRENT);
	--_current;
	if (!ReadFile(_h, &c, 1, &bytesRead, NULL))
		return traits_type::eof();
	if (0 == bytesRead)
		return traits_type::eof();
	SetFilePointer(_h, -1, NULL, FILE_CURRENT);
	return c;
}

std::streamsize RmSimpleUploadBuffer::showmanyc()
{
	return (_end - _current);
}

std::streamsize RmSimpleUploadBuffer::xsgetn(char* _Ptr, std::streamsize _Count)
{	// get _Count characters from stream
	int_type _Meta;
	std::streamsize _Size, _Copied;

	for (_Copied = 0; 0 < _Count; ) {
		if (0 < (_Size = _Gnavail()))
		{	// copy from read buffer
			if (_Count < _Size)
				_Size = _Count;
			traits_type::copy(_Ptr, gptr(), (size_t)_Size);
			_Ptr += _Size;
			_Copied += _Size;
			_Count -= _Size;
			gbump((int)_Size);
		}
		else if (std::char_traits<char>::eq_int_type(traits_type::eof(), _Meta = uflow()) && _feof)
		{
			break;	// end of file, quit
		}
		else
		{	// get a single character
			*_Ptr++ = std::char_traits<char>::to_char_type(_Meta);
			++_Copied;
			--_Count;
		}
	}

	return (_Copied);
}


//
//
//

RmUploadRequest::RmUploadRequest(const RmRestCommonParams& commonParams, const std::wstring& url, const std::string& apiInput, const std::wstring& file, bool* canceled)
	: NX::REST::http::Request(REST::http::methods::POST,
		url,
		REST::http::HttpHeaders({
			std::pair<std::wstring, std::wstring>(NX::REST::http::header_names::content_type, std::wstring(L"multipart/form-data; boundary=\"" + std::wstring(boundary.begin(), boundary.end()) + L"\"")),
			std::pair<std::wstring, std::wstring>(L"clientId", commonParams.GetClientId()),
			std::pair<std::wstring, std::wstring>(L"platformId", commonParams.GetPlatformId()),
			std::pair<std::wstring, std::wstring>(L"deviceId", commonParams.GetDeviceId()),
			std::pair<std::wstring, std::wstring>(L"tenant", commonParams.GetTenant()),
			std::pair<std::wstring, std::wstring>(L"userId", commonParams.GetUserId()),
			std::pair<std::wstring, std::wstring>(L"ticket", commonParams.GetTicket()),
		}),
		std::vector<LPCWSTR>({ REST::http::mime_types::application_json.c_str(), REST::http::mime_types::text.c_str(), NULL }), canceled),
	_buf(apiInput, file), _is(&_buf)
{
	SetBodyLength((ULONG)_buf.GetTotalDataLength());
}

RmUploadRequest::~RmUploadRequest()
{
}