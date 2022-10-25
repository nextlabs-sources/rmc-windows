

#ifndef __NX_REST_HTTP_CLIENT_H__
#define __NX_REST_HTTP_CLIENT_H__

#include <nx\common\result.h>
#include <nx\rest\http_const.h>

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <sstream>
#include <fstream>



namespace NX {
    namespace REST {

        namespace http {

            class Connection;
            class Request;
            class Response;
            
            class Client
            {
            public:
                Client();
                Client(const std::wstring& agent, bool ssl_enabled, unsigned long timout_seconds = 0);
                virtual ~Client();

                Result Open();
                Result Close();

                Connection* CreateConnection(const std::wstring& url, Result* res = nullptr);
                Connection* CreateConnection(const std::wstring& server, USHORT port, Result* res = nullptr);
                
                inline bool Opened() const { return (_h != nullptr); }
                inline bool Secured() const { return _ssl; }
                inline unsigned long TimeoutSeconds() const { return _tos; }
                inline PVOID GetSessionHandle() const { return _h; }

            protected:
                // No copy is allowed
                Client(const Client& rhs) {}

                Result InterOpen();

                // Callback used with WinHTTP to listen for async completions.
                static void CALLBACK HttpCompletionCallback(
                    HANDLE request_handle,
                    ULONG_PTR context,
                    unsigned long status_code,
                    void* status_info,
                    unsigned long status_info_length);

            private:
                std::wstring     _agent;
				PVOID            _h;
                bool             _ssl;
                unsigned long    _tos;
                ULONG            _ref;
                CRITICAL_SECTION _lock;
            };

            class Connection
            {
            public:
                virtual ~Connection();

                Result Connect();
                Result Disconnect();
                Result SendRequest(Request& request, Response& response, DWORD wait_ms = INFINITE);

                inline bool Connected() const { return (nullptr != _h); }
                inline PVOID GetConnectHandle() const { return _h; }
                inline const std::wstring& GetServer() const { return _server; }
                inline USHORT GetPort() const { return _port; }
                inline Client* GetClient() const { return _client; }

            protected:
                // No copy is allowed
                Connection(const Connection& rhs) {}

            protected:
                Connection();
                Connection(Client* client, const std::wstring& server, USHORT port);

            private:
				PVOID			_h;
                std::wstring    _server;
				USHORT			_port;
                Client*         _client;

                friend class Client;
            };

            typedef std::vector<std::pair<std::wstring, std::wstring>> HttpHeaders;
            typedef std::vector<LPCWSTR> HttpAcceptTypes;

            class Request
            {
            public:
                Request() : _body_length(0), _requested_length(0), _canceled(NULL) {}
                Request(const std::wstring& method, const std::wstring& path, const HttpHeaders& headers, const HttpAcceptTypes& accept_types, bool* canceled = NULL, const ULONG requested_length = 0)
                    : _method(method), _path(path), _headers(headers), _accept_types(accept_types), _body_length(0), _requested_length(requested_length), _canceled(canceled)
                {
                }
                virtual ~Request() {}

                virtual ULONG GetRemainBodyLength() = 0;
                virtual std::istream& GetBodyStream() = 0;

				virtual  bool IsUpload() const { return false; }

				inline bool Canceled() const { return (_canceled ? (*_canceled) : false); }
                inline const std::wstring& GetMethod() const { return _method; }
                inline const std::wstring& GetPath() const { return _path; }
                inline const HttpHeaders& GetHeaders() const { return _headers; }
                inline const HttpAcceptTypes& GetAcceptTypes() const { return _accept_types; }
                inline ULONG GetBodyLength() const { return _body_length; }
                inline ULONG GetRequestedLength() const { return _requested_length; }

            protected:
                // Copy is not allowed
                Request(const Request& rhs) {}
                inline void SetBodyLength(ULONG len) { _body_length = len; }

            private:
                std::wstring    _method;
                std::wstring    _path;
                std::vector<LPCWSTR> _accept_types;
                HttpHeaders     _headers;
                DWORD           _body_length;
				DWORD			_requested_length;
				bool*			_canceled;
            };

            class Response
            {
            public:
                Response() : _status(0) {}
                virtual ~Response() {}

                virtual ULONG GetBodyLength() = 0;
                virtual std::ostream& GetBodyStream() = 0;

                inline unsigned short GetStatus() const { return _status; }
                inline const std::wstring& GetPhrase() const { return _phrase; }
                inline const HttpHeaders& GetHeaders() const { return _headers; }

                inline void SetStatus(unsigned short status) { _status = status; }
                inline void SetPhrase(const std::wstring& phrase) { _phrase = phrase; }
                inline void SetHeaders(const HttpHeaders& headers) { _headers = headers; }

            protected:
                // Copy is not allowed
                Response(const Response& rhs) {}

            private:
                unsigned short  _status;
                std::wstring    _phrase;
                HttpHeaders     _headers;
            };

            class StringBodyRequest : public Request
            {
            public:
                StringBodyRequest() : Request() {}
                StringBodyRequest(const std::wstring& method, const std::wstring& path, const HttpHeaders& headers, const HttpAcceptTypes& accept_types, const std::string& body, bool* canceled = NULL, const ULONG requested_length = 0)
                    : Request(method, path, headers, accept_types, canceled, requested_length), _iss(body)
                {
                    SetBodyLength((ULONG)body.length());
                }
                virtual ~StringBodyRequest() {}

                virtual ULONG GetRemainBodyLength()
                {
                    return (GetBodyLength() - (ULONG)_iss.tellg());
                }

                virtual std::istream& GetBodyStream()
                {
                    return _iss;
                }

            private:
                std::istringstream _iss;
            };

            class FileBodyRequest : public Request
            {
            public:
                FileBodyRequest() : Request() {}
                FileBodyRequest(const std::wstring& method, const std::wstring& path, const HttpHeaders& headers, const HttpAcceptTypes& accept_types, const std::string& file, bool* canceled = NULL, const ULONG requested_length = 0)
                    : Request(method, path, headers, accept_types, canceled, requested_length), _ifs(file, std::ifstream::in|std::ifstream::binary)
                {
                    if (_ifs.is_open()) {
                        _ifs.seekg(0, std::ios::end);
                        SetBodyLength((ULONG)_ifs.tellg());
                    }
                }
                FileBodyRequest(const std::wstring& method, const std::wstring& path, const HttpHeaders& headers, const HttpAcceptTypes& accept_types, const std::wstring& file)
                    : Request(method, path, headers, accept_types), _ifs(file, std::ifstream::in|std::ifstream::binary)
                {
                    if (_ifs.is_open()) {
                        _ifs.seekg(0, std::ios::end);
                        SetBodyLength((ULONG)_ifs.tellg());
                    }
                }
                virtual ~FileBodyRequest() {}

                virtual ULONG GetRemainBodyLength()
                {
                    return (GetBodyLength() - (ULONG)_ifs.tellg());
                }

                virtual std::istream& GetBodyStream()
                {
                    return _ifs;
                }

            private:
                std::ifstream _ifs;
            };

            class StringResponse : public Response
            {
            public:
                StringResponse() : Response() {}
                virtual ~StringResponse() {}

                virtual ULONG GetBodyLength()
                {
                    return (ULONG)_oss.tellp();
                }

                virtual std::ostream& GetBodyStream()
                {
                    return _oss;
                }

                std::string GetBody() const { return _oss.str(); }

            private:
                std::ostringstream _oss;
            };

            class FileResponse : public Response
            {
            public:
                FileResponse(const std::string& file);
                FileResponse(const std::wstring& file);
                virtual ~FileResponse() {}

                inline bool is_open() { return _ofs.is_open(); }

                virtual std::ostream& GetBodyStream()
                {
                    return _ofs;
                }

                void Finish() { _ofs.close(); }

			protected:
                FileResponse() : Response(){}

            private:
                virtual ULONG GetBodyLength()
                {
                    return 0;
                }

            private:
                std::ofstream _ofs;
            };

        }
    }
}

#endif