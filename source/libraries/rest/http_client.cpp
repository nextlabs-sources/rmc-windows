
#include "stdafx.h"

#include <nx\rest\http_const.h>
#include <nx\rest\http_client.h>
#include <nx\rest\uri.h>

#include <winhttp.h>
#include <VersionHelpers.h>

using namespace NX;
using namespace NX::REST::http;


#ifdef _DEBUG
static const int default_timeout_seconds = 20; // 20 seconds
#else
static const int default_timeout_seconds = 5;  // 5 seconds
#endif


namespace NX {
    namespace REST {

        namespace http {

            class RequestContext
            {
            public:
                virtual ~RequestContext();

                static RequestContext* Create(Connection* c, Request* request, Response* response);

                inline const Result& GetResult() const { return _result; }

            protected:
                RequestContext(Request* request, Response* response, PVOID h);

                void OnRequestError(const WINHTTP_ASYNC_RESULT* result);
                void OnDataAvailable(unsigned long data_size);
                void OnHeadersAvailable();
                void OnReadComplete(unsigned long read_size);
                void OnSendComplete();
                void OnWriteComplete(unsigned long written_size);

                void CompleteRequest();

            protected:
                ULONG QueryResponseHeaderLength(ULONG info_level);
                USHORT QueryResponseStatusCode();
                std::wstring QueryResponseReasonPhrase();
                HttpHeaders QueryResponseHeaders();

            private:
                std::vector<char>  _buf;
                static const DWORD BUFSIZE = 4096;

            private:
				PVOID		 _h;
                HANDLE       _ce;   // Completion Event
                Request*     _request;
                Response*    _response;
                Result       _result;
                ULONG        _bytes_read;
                friend class Client;
                friend class Connection;
            };
        }
    }
}


NX::REST::http::Client::Client() : _h(nullptr), _ref(0), _ssl(false), _tos(default_timeout_seconds)
{
    ::InitializeCriticalSection(&_lock);
}

NX::REST::http::Client::Client(const std::wstring& agent, bool ssl_enabled, unsigned long timeout_seconds)
    : _agent(agent), _h(nullptr), _ref(0), _ssl(ssl_enabled), _tos(timeout_seconds ? timeout_seconds : default_timeout_seconds)
{
    ::InitializeCriticalSection(&_lock);
}

NX::REST::http::Client::~Client()
{
    Close();
    ::DeleteCriticalSection(&_lock);
}

Result NX::REST::http::Client::Open()
{
    Result res = RESULT(0);

    ::EnterCriticalSection(&_lock);
    if (_h) {
        res = RESULT(0);
    }
    else {
        res = InterOpen();
    }
    ::LeaveCriticalSection(&_lock);

    return res;
}

Result NX::REST::http::Client::InterOpen()
{
    Result  res = RESULT(0);
    bool    callback_is_set = false;

    assert(NULL == _h);

    do {

        static const DWORD dwAccessType = IsWindows8Point1OrGreater()  ? WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY : WINHTTP_ACCESS_TYPE_DEFAULT_PROXY;
    
        _h = ::WinHttpOpen(_agent.empty() ? NULL : _agent.c_str(), dwAccessType, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, WINHTTP_FLAG_ASYNC);
        if (nullptr == _h) {
            res = RESULT(GetLastError());
            break;
        }

        // Set timeouts.
        const unsigned int milliseconds = _tos * 1000;
        if (!WinHttpSetTimeouts(_h, milliseconds, milliseconds, milliseconds, milliseconds)) {
            res = RESULT(GetLastError());
            break;
        }

        if (Secured()) {
            const unsigned long protos = WINHTTP_FLAG_SECURE_PROTOCOL_TLS1_2;
            if (!::WinHttpSetOption(_h, WINHTTP_OPTION_SECURE_PROTOCOLS, (LPVOID)&protos, sizeof(protos))) {
                res = RESULT(GetLastError());
                break;
            }
        }

        // Register asynchronous callback.
        if (WINHTTP_INVALID_STATUS_CALLBACK == WinHttpSetStatusCallback(_h, HttpCompletionCallback, WINHTTP_CALLBACK_FLAG_ALL_COMPLETIONS | WINHTTP_CALLBACK_FLAG_HANDLES, 0)) {
            res = RESULT(GetLastError());
            break;
        }
        callback_is_set = true;

    } while (FALSE);

    if (!res) {

        if (callback_is_set) {
            assert(_h);
            WinHttpSetStatusCallback(_h, nullptr, WINHTTP_CALLBACK_FLAG_ALL_NOTIFICATIONS, NULL);
        }

        WinHttpCloseHandle(_h);
        _h = nullptr;
    }

    return res;
}

Result NX::REST::http::Client::Close()
{
    assert(0 == _ref);
    ::EnterCriticalSection(&_lock);
    if (_h) {
        WinHttpCloseHandle(_h);
        _h = nullptr;
    }
    ::LeaveCriticalSection(&_lock);
    return RESULT(0);
}

Connection* NX::REST::http::Client::CreateConnection(const std::wstring& url, Result* res)
{
    URL_COMPONENTS components = { 0 };

    memset(&components, 0, sizeof(components));
    components.dwStructSize = sizeof(URL_COMPONENTS);
    components.dwHostNameLength = (DWORD)-1;

    if (!WinHttpCrackUrl(url.c_str(), (DWORD)url.length(), 0, &components)) {
        if (res)
            *res = RESULT(GetLastError());
        return nullptr;
    }

    if (0 == components.dwHostNameLength && NULL == components.lpszHostName) {
        if (res)
            *res = RESULT(ERROR_INVALID_PARAMETER);
        return nullptr;
    }

    const std::wstring serverHost(components.lpszHostName, components.lpszHostName + components.dwHostNameLength);
    return CreateConnection(serverHost, components.nPort, res);
}

Connection* NX::REST::http::Client::CreateConnection(const std::wstring& server, USHORT port, Result* res)
{
    Connection* p = nullptr;
    Result r = RESULT(0);

    p = new Connection(this, server, port);
    if (p) {
        r = p->Connect();
        if (!r) {
            delete p;
            p = nullptr;
        }
    }
    else {
        r = RESULT(ERROR_NOT_ENOUGH_MEMORY);
    }
    
    if (res) {
        *res = r;
    }
    return p;
}

void CALLBACK NX::REST::http::Client::HttpCompletionCallback(
    HANDLE request_handle,
    ULONG_PTR context,
    unsigned long status_code,
    void* status_info,
    unsigned long status_info_length)
{
    UNREFERENCED_PARAMETER(status_info_length);

    if (status_code == WINHTTP_CALLBACK_STATUS_HANDLE_CLOSING)
        return;

    RequestContext* rc = reinterpret_cast<RequestContext*>(context);
    if (rc == nullptr) {
        return;
    }

    try {

        switch (status_code)
        {
        case WINHTTP_CALLBACK_STATUS_CLOSING_CONNECTION:
            break;

        case WINHTTP_CALLBACK_STATUS_CONNECTED_TO_SERVER:
            break;

        case WINHTTP_CALLBACK_STATUS_CONNECTING_TO_SERVER:
            break;

        case WINHTTP_CALLBACK_STATUS_CONNECTION_CLOSED:
            break;

        case WINHTTP_CALLBACK_STATUS_DATA_AVAILABLE:
			if (!rc->_request->IsUpload() && rc->_request->Canceled()) {
				throw RESULT2(ERROR_CANCELLED, "OnDataAvailable: User canceled");
			}
            assert(status_info_length == sizeof(unsigned long));
            rc->OnDataAvailable(status_info_length);
            break;

        case WINHTTP_CALLBACK_STATUS_HANDLE_CREATED:
            break;

        case WINHTTP_CALLBACK_STATUS_HANDLE_CLOSING:
            break;

        case WINHTTP_CALLBACK_STATUS_HEADERS_AVAILABLE:
			if (!rc->_request->IsUpload() && rc->_request->Canceled()) {
				throw RESULT2(ERROR_CANCELLED, "OnHeadersAvailable: User canceled");
			}
            rc->OnHeadersAvailable();
            break;

        case WINHTTP_CALLBACK_STATUS_INTERMEDIATE_RESPONSE:
            break;

        case WINHTTP_CALLBACK_STATUS_NAME_RESOLVED:
            break;

        case WINHTTP_CALLBACK_STATUS_READ_COMPLETE:
			if (!rc->_request->IsUpload() && rc->_request->Canceled()) {
				throw RESULT2(ERROR_CANCELLED, "OnReadComplete: User canceled");
			}
            rc->OnReadComplete(status_info_length);
            break;

        case WINHTTP_CALLBACK_STATUS_RECEIVING_RESPONSE:
            break;

        case WINHTTP_CALLBACK_STATUS_REDIRECT:
            break;

        case WINHTTP_CALLBACK_STATUS_REQUEST_ERROR:
            rc->OnRequestError(reinterpret_cast<const WINHTTP_ASYNC_RESULT*>(status_info));
            break;

        case WINHTTP_CALLBACK_STATUS_REQUEST_SENT:
            break;

        case WINHTTP_CALLBACK_STATUS_RESOLVING_NAME:
            break;

        case WINHTTP_CALLBACK_STATUS_RESPONSE_RECEIVED:
            break;

        case WINHTTP_CALLBACK_STATUS_SECURE_FAILURE:
            break;

        case WINHTTP_CALLBACK_STATUS_SENDING_REQUEST:
            break;

        case WINHTTP_CALLBACK_STATUS_SENDREQUEST_COMPLETE:
			if (!rc->_request->IsUpload() && rc->_request->Canceled()) {
				throw RESULT2(ERROR_CANCELLED, "OnSendComplete: User canceled");
			}
            rc->OnSendComplete();
            break;

        case WINHTTP_CALLBACK_STATUS_WRITE_COMPLETE:
            assert(status_info_length == sizeof(unsigned long));
			if (rc->_request->Canceled()) {
				throw RESULT2(ERROR_CANCELLED, "OnWriteComplete: User canceled");
			}
            rc->OnWriteComplete(*((unsigned long*)status_info));
            break;

        case WINHTTP_CALLBACK_STATUS_GETPROXYFORURL_COMPLETE:
            break;

        case WINHTTP_CALLBACK_STATUS_CLOSE_COMPLETE:
            break;

        case WINHTTP_CALLBACK_STATUS_SHUTDOWN_COMPLETE:
            break;

        default:
            break;
        }
    }
    catch (const NX::Result& e) {
        rc->_result = e;
        rc->CompleteRequest();
    }
    catch (const std::exception& e) {
        DWORD dwErr = GetLastError();
        if (0 == dwErr) dwErr = -1;
        rc->_result = RESULT2(dwErr, e.what() ? e.what() : "Unknown error");
        rc->CompleteRequest();
    }
    catch (...) {
        // Unknown exception
        throw;
    }
}


NX::REST::http::Connection::Connection() : _client(nullptr), _h(nullptr), _port(0)
{
}

NX::REST::http::Connection::Connection(Client* client, const std::wstring& server, USHORT port) : _client(client), _h(nullptr), _server(server), _port(port)
{
}

NX::REST::http::Connection::~Connection()
{
    Disconnect();
}

Result NX::REST::http::Connection::Connect()
{
    if (!_client) {
        return RESULT(ERROR_INVALID_HANDLE);
    }

    assert(nullptr == _h);
    _h = WinHttpConnect(_client->GetSessionHandle(), _server.c_str(), (INTERNET_PORT)_port, 0);
    if (_h == nullptr) {
        return RESULT(GetLastError());
    }

    return RESULT(0);
}

Result NX::REST::http::Connection::Disconnect()
{
    if (_h) {
        WinHttpCloseHandle(_h);
        _h = nullptr;
    }
    return RESULT(0);
}

class RequestError
{
public:
    RequestError() : _error(0) {}
    RequestError(DWORD error, const std::wstring& msg) : _error(error), _msg(msg) {}
    RequestError(const RequestError& rhs) : _error(rhs._error), _msg(rhs._msg) {}
    ~RequestError() {}

    RequestError& operator = (const RequestError& rhs)
    {
        if (this != &rhs) {
            _error = rhs._error;
            _msg = rhs._msg;
        }
        return *this;
    }


    inline DWORD GetError() const { return _error; }
    inline const std::wstring& GetMsg() const { return _msg; }

protected:
    DWORD _MoveError() { DWORD tmp = _error; _error = 0; return tmp; }

private:
    DWORD _error;
    std::wstring _msg;
};

Result NX::REST::http::Connection::SendRequest(Request& request, Response& response, DWORD wait_ms)
{
    Result res = RESULT(0);

    // send asynchronously
    RequestContext* context = RequestContext::Create(this, &request, &response);
    if (nullptr == context) {
        return RESULT(ERROR_NOT_ENOUGH_MEMORY);
    }

    if (!WinHttpSendRequest(context->_h,
                            WINHTTP_NO_ADDITIONAL_HEADERS,
                            0,
                            nullptr,
                            0,
                            request.GetBodyLength(),
                            (DWORD_PTR)context)) {
		delete context;
		context = nullptr;
        return RESULT(GetLastError());
    }

    DWORD dwWaitResult = ::WaitForSingleObject(context->_ce, wait_ms);
    if (dwWaitResult == WAIT_TIMEOUT) {
        res = RESULT(ERROR_TIMEOUT);
    }
    else if (dwWaitResult == WAIT_OBJECT_0) {
        res = context->GetResult();
    }
    else {
        res = RESULT(GetLastError());
    }

    delete context;
    context = nullptr;
    return res;
}


RequestContext* NX::REST::http::RequestContext::Create(Connection* c, Request* request, Response* response)
{
    RequestContext* p = nullptr;
    HINTERNET       rh = nullptr;
    bool            succeed = false;

    rh = WinHttpOpenRequest(c->GetConnectHandle(),
                            request->GetMethod().c_str(),
                            request->GetPath().c_str(),
                            nullptr,
                            WINHTTP_NO_REFERER,
                            (LPCWSTR*)request->GetAcceptTypes().data(),
                            WINHTTP_FLAG_ESCAPE_DISABLE | (c->GetClient()->Secured() ? WINHTTP_FLAG_SECURE : 0));
    if (nullptr == rh)
        return nullptr;

    try {

        static const unsigned long data = SECURITY_FLAG_IGNORE_UNKNOWN_CA |
                                          SECURITY_FLAG_IGNORE_CERT_DATE_INVALID |
                                          SECURITY_FLAG_IGNORE_CERT_CN_INVALID |
                                          SECURITY_FLAG_IGNORE_CERT_WRONG_USAGE;
        if (!WinHttpSetOption(rh, WINHTTP_OPTION_SECURITY_FLAGS, (LPVOID)&data, sizeof(data))) {
            throw RESULT(GetLastError());
        }

        std::for_each(request->GetHeaders().cbegin(), request->GetHeaders().cend(), [&](const std::pair<std::wstring, std::wstring>& item) {
            std::wstring header(item.first);
            header += L": ";
            header += item.second;
            if (!WinHttpAddRequestHeaders(rh, header.c_str(), static_cast<DWORD>(header.length()), WINHTTP_ADDREQ_FLAG_ADD | WINHTTP_ADDREQ_FLAG_REPLACE)) {
                throw RESULT(GetLastError());
            }
        });

        p = new RequestContext(request, response, rh);
        if (nullptr == p) {
            throw RESULT(ERROR_NOT_ENOUGH_MEMORY);
        }

        rh = nullptr; // Transfer ownership
        succeed = true;
    }
    catch (const Result& r) {
        UNREFERENCED_PARAMETER(r);
        succeed = false;
    }

    if (!succeed) {
        if (rh) {
            WinHttpCloseHandle(rh);
            rh = nullptr;
        }
        if (nullptr != p) {
            delete p;
            p = nullptr;
        }
    }

    return p;
}

NX::REST::http::RequestContext::RequestContext(Request* request, Response* response, PVOID h)
    : _request(request), _response(response), _h(h), _ce(::CreateEventW(nullptr, TRUE, FALSE, nullptr)), _bytes_read(0)
{
}

NX::REST::http::RequestContext::~RequestContext()
{
    _bytes_read = 0;
	_buf.clear();
    _request = nullptr;
    _response = nullptr;
    if (nullptr != _h) {
        WinHttpCloseHandle(_h);
        _h = nullptr;
    }
    if (nullptr != _ce) {
        CloseHandle(_ce);
        _ce = nullptr;
    }
}

void NX::REST::http::RequestContext::CompleteRequest()
{
    if (nullptr != _h) {
        WinHttpCloseHandle(_h);
        _h = nullptr;
    }
    if (nullptr != _ce) {
        SetEvent(_ce);
    }
}

ULONG NX::REST::http::RequestContext::QueryResponseHeaderLength(ULONG info_level)
{
    ULONG length = 0;
    if (!WinHttpQueryHeaders(_h,
        info_level,
        WINHTTP_HEADER_NAME_BY_INDEX,
        WINHTTP_NO_OUTPUT_BUFFER,
        &length,
        WINHTTP_NO_HEADER_INDEX)) {
        if (ERROR_INSUFFICIENT_BUFFER != GetLastError()) {
            throw RESULT2(GetLastError(), "WinHttpQueryHeaders failed");
        }
    }
    return length;
}

USHORT NX::REST::http::RequestContext::QueryResponseStatusCode()
{
    unsigned long length = QueryResponseHeaderLength(WINHTTP_QUERY_STATUS_CODE);
    std::vector<wchar_t> buffer;
    buffer.resize(length + 1, 0);
    if (!WinHttpQueryHeaders(_h, WINHTTP_QUERY_STATUS_CODE, WINHTTP_HEADER_NAME_BY_INDEX, buffer.data(), &length, WINHTTP_NO_HEADER_INDEX)) {
        throw RESULT2(GetLastError(), "WinHttpQueryHeaders failed");
    }
    return (USHORT)_wtoi(buffer.data());
}

std::wstring NX::REST::http::RequestContext::QueryResponseReasonPhrase()
{
    std::wstring phrase;
    unsigned long length = 0;

    length = QueryResponseHeaderLength(WINHTTP_QUERY_STATUS_TEXT);
    phrase.resize(length);
    if (!WinHttpQueryHeaders(_h, WINHTTP_QUERY_STATUS_TEXT, WINHTTP_HEADER_NAME_BY_INDEX, &phrase[0], &length, WINHTTP_NO_HEADER_INDEX)) {
        throw RESULT2(GetLastError(), "WinHttpQueryHeaders failed");
    }
    // WinHTTP reports back the wrong length, trim any null characters.
    return phrase;
}

HttpHeaders NX::REST::http::RequestContext::QueryResponseHeaders()
{
    HttpHeaders header_list;

    unsigned short response_code = 0;

    // First need to query to see what the headers size is.
    unsigned long header_buffer_length = QueryResponseHeaderLength(WINHTTP_QUERY_RAW_HEADERS_CRLF);

    // Now allocate buffer for headers and query for them.
    std::vector<wchar_t> header_raw_buffer;
    header_raw_buffer.resize((header_buffer_length + 1) / 2 + 1, 0);
    if (!WinHttpQueryHeaders(_h,
        WINHTTP_QUERY_RAW_HEADERS_CRLF,
        WINHTTP_HEADER_NAME_BY_INDEX,
        header_raw_buffer.data(),
        &header_buffer_length,
        WINHTTP_NO_HEADER_INDEX)) {
        throw RESULT2(GetLastError(), "WinHttpQueryHeaders failed");
    }

    if (header_buffer_length == 0) {
        return header_list;
    }

    const wchar_t* p = header_raw_buffer.data();
    while (p != nullptr && *p != L'\r' && *p != L'\n') {

        std::wstring line;
        const wchar_t* pe = wcschr(p, L'\n');


        if (pe == nullptr) {
            line = p;
            p = nullptr;
        }
        else {
            line = std::wstring(p, (L'\r' == *(pe - 1)) ? (pe - 1) : pe);
            p = pe + 1;
        }

        const size_t pos = line.find(L":");
        if (pos == std::wstring::npos) {
            continue;
        }

        std::wstring key = line.substr(0, pos);
        std::wstring value = line.substr(pos + 1);
        header_list.push_back(std::pair<std::wstring, std::wstring>(key, value));
    }

    return header_list;
}

void NX::REST::http::RequestContext::OnRequestError(const WINHTTP_ASYNC_RESULT* result)
{
    switch (result->dwResult)
    {
    case API_RECEIVE_RESPONSE:
        throw RESULT2((int)result->dwError, "WinHttpReceiveResponse failed");
    case API_QUERY_DATA_AVAILABLE:
        throw RESULT2((int)result->dwError, "WinHttpQueryDataAvaliable failed");
    case API_READ_DATA:
        throw RESULT2((int)result->dwError, "WinHttpReadData failed");
    case API_WRITE_DATA:
        throw RESULT2((int)result->dwError, "WinHttpWriteData failed");
    case API_SEND_REQUEST:
        throw RESULT2((int)result->dwError, "WinHttpSendRequest failed");
    default:
        throw RESULT2((int)result->dwError, "Unknown WinHTTP Function failed");
    }
}

void NX::REST::http::RequestContext::OnDataAvailable(unsigned long data_size)
{
    // No more data.
    if (0 == data_size) {
        CompleteRequest();
        return;
    }

    if (_buf.size() != BUFSIZE) {
        _buf.resize(BUFSIZE, 0);
    }

    if (!WinHttpReadData(_h, _buf.data(), BUFSIZE, nullptr)) {
        throw RESULT2(GetLastError(), "OnDataAvailable: WinHttpReadData failed");
    }
}

void NX::REST::http::RequestContext::OnHeadersAvailable()
{
    // Step 3: The response headers are ready
    _response->SetStatus(QueryResponseStatusCode());
    _response->SetPhrase(QueryResponseReasonPhrase());
    _response->SetHeaders(QueryResponseHeaders());

    // If the method is "HEAD", then we finish all the work
    if (0 == _wcsicmp(_request->GetMethod().c_str(), NX::REST::http::methods::HEAD.c_str())) {
        // done
        CompleteRequest();
        return;
    }

    // Otherwise, we need to get all the data
    if (!WinHttpQueryDataAvailable(_h, nullptr)) {
        throw RESULT2(GetLastError(), "OnHeadersAvailable: WinHttpQueryDataAvailable failed");
    }
}

void NX::REST::http::RequestContext::OnReadComplete(unsigned long read_size)
{
    if (0 == read_size) {
        // All the data has been read
        CompleteRequest();
        return;
    }

    // We do read some data, write it to stream
	if (0 != _request->GetRequestedLength() && (_bytes_read + read_size) > _request->GetRequestedLength()) {
		// Well, reach the max requested data length
		read_size = _request->GetRequestedLength() - _bytes_read;
	}
    _bytes_read += read_size;
    _response->GetBodyStream().write(_buf.data(), read_size);

	if (0 != _request->GetRequestedLength() && _bytes_read >= _request->GetRequestedLength()) {
		// Well, reach the max requested data length
		// Done!
		CompleteRequest();
		return;
	}

    // And then try to read again
    if (!WinHttpReadData(_h, _buf.data(), BUFSIZE, nullptr)) {
        throw RESULT2(GetLastError(), "OnReadComplete: WinHttpReadData failed");
    }
}

void NX::REST::http::RequestContext::OnSendComplete()
{
    // Step 1: the request has been sent, we need to write extra data (body)
    if (_request->GetBodyLength() == 0) {
        // No body? try to get response directly
        if (!WinHttpReceiveResponse(_h, nullptr)) {
            throw RESULT(GetLastError());
        }
    }
    else {
        // Write body

        // Prepare read/write buffer
        if (_buf.size() != BUFSIZE)
            _buf.resize(BUFSIZE, 0);

        // Write data
        DWORD dwBytesToWrite = min(_request->GetRemainBodyLength(), BUFSIZE);
        DWORD dwBytesWritten = 0;
        _request->GetBodyStream().read(_buf.data(), dwBytesToWrite);
        if (!WinHttpWriteData(_h, _buf.data(), dwBytesToWrite, &dwBytesWritten)) {
            throw RESULT(GetLastError());
        }
    }
}

void NX::REST::http::RequestContext::OnWriteComplete(unsigned long written_size)
{
    // Step 2: continue write all the data

    const ULONG remain_size = _request->GetRemainBodyLength();
    // Previous data has been written, do it again if there are more
    if (0 == remain_size) {
        // all the data has been sent
        if (!WinHttpReceiveResponse(_h, nullptr)) {
            throw RESULT(GetLastError());
        }
    }
    else {
        DWORD dwBytesToWrite = min(remain_size, BUFSIZE);
        DWORD dwBytesWritten = 0;
        _request->GetBodyStream().read(_buf.data(), dwBytesToWrite);
        if (!WinHttpWriteData(_h, _buf.data(), dwBytesToWrite, &dwBytesWritten)) {
            throw RESULT(GetLastError());
        }
    }
}


//
//
//

FileResponse::FileResponse(const std::string& file) : Response()
{
    _ofs.open(file, std::ofstream::out | std::ofstream::binary | std::ofstream::trunc);
}

FileResponse::FileResponse(const std::wstring& file) : Response()
{
    _ofs.open(file, std::ofstream::out | std::ofstream::binary | std::ofstream::trunc);
}
