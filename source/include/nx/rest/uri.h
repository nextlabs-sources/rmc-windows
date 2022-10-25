

#ifndef __NX_REST_URI_H__
#define __NX_REST_URI_H__

#include <nx\common\string.h>
#include <string>

namespace NX {
    namespace REST {


        //
        //                      hierarchical part
        //          ©°©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©Ø©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©´
        //                      authority               path
        //          ©°©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©Ø©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©´©°©¤©¤©¤©Ø©¤©¤©¤©¤©´
        //    abc://username:password@example.com:123/path/data?key=value#fragid1
        //    ©¸©Ð©¼   ©¸©¤©¤©¤©¤©¤©¤©¤©Ð©¤©¤©¤©¤©¤©¤©¤©¼ ©¸©¤©¤©¤©¤©Ð©¤©¤©¤©¤©¼ ©¸©Ð©¼           ©¸©¤©¤©¤©Ð©¤©¤©¤©¼ ©¸©¤©¤©Ð©¤©¤©¼
        //  scheme  user information     host     port            query   fragment
        //

        class Uri
        {
        public:
            Uri() : _port(0) {}

            Uri(const Uri& rhs)
                : _scheme(rhs._scheme),
                _user(rhs._user),
                _password(rhs._password),
                _host(rhs._host),
                _port(rhs._port),
                _path(rhs._path),
                _query(rhs._query),
                _fragment(rhs._fragment)
            {
            }

            Uri(const std::wstring& s) : _port(0) { parse(s); }
            virtual ~Uri() {}

            inline bool empty() const noexcept { return _scheme.empty(); }
            inline const std::wstring& scheme() const noexcept { return _scheme; }
            inline const std::wstring& user() const noexcept { return _user; }
            inline const std::wstring& password() const noexcept { return _password; }
            inline const std::wstring& host() const noexcept { return _host; }
            inline unsigned short port() const noexcept { return _port; }
            inline const std::wstring& path() const noexcept { return _path; }
            inline const std::wstring& query() const noexcept { return _query; }
            inline const std::wstring& fragment() const noexcept { return _fragment; }
            inline bool is_port_default() const noexcept { return (0 == _port); }

            void clear() noexcept
            {
                _scheme.clear();
                _user.clear();
                _password.clear();
                _host.clear();
                _port = 0;
                _path.clear();
                _query.clear();
                _fragment.clear();
            }

            Uri& operator = (const Uri& rhs) noexcept
            {
                if (this != &rhs) {
                    _scheme = rhs._scheme;
                    _user = rhs._user;
                    _password = rhs._password;
                    _host = rhs._host;
                    _port = rhs._port;
                    _path = rhs._path;
                    _query = rhs._query;
                    _fragment = rhs._fragment;
                }
                return *this;
            }

            std::wstring serialize() const
            {
                if (empty()) {
                    return std::wstring();
                }

                std::wstring full_uri(scheme());
                full_uri.append(L"://");
                if (!user().empty()) {
                    full_uri.append(user());
                    if (!password().empty()) {
                        full_uri.append(L":");
                        full_uri.append(password());
                    }
                    full_uri.append(L"@");
                }
                full_uri.append(host());
                if (0 != port()) {
                    full_uri.append(L":");
                    full_uri.append(std::to_wstring((int)port()));
                }

                if (!path().empty()) {
                    full_uri.append(path());
                    if (!query().empty()) {
                        full_uri.append(L"?");
                        full_uri.append(query());
                        if (!fragment().empty()) {
                            full_uri.append(L"#");
                            full_uri.append(fragment());
                        }
                    }
                }

                return full_uri;
            }

            unsigned short resolve_default_port()
            {
                if (0 == _port) {
                    if (0 == _wcsicmp(_scheme.c_str(), L"http"))
                        return 80;
                    else if (0 == _wcsicmp(_scheme.c_str(), L"https"))
                        return 443;
                    else if (0 == _wcsicmp(_scheme.c_str(), L"ftp"))
                        return 21;
                    else if (0 == _wcsicmp(_scheme.c_str(), L"smtp"))
                        return 23;
                    else
                        return 0;
                }
                else {
                    return _port;
                }
            }

        protected:
            void parse(const std::wstring& s)
            {
                std::wstring remain_uri;

                if (s.empty()) {
                    return;
                }

                //
                //  scheme
                //
                auto pos = s.find(L"://");
                if (pos == std::wstring::npos) {
                    SetLastError(ERROR_INVALID_DATA);
                    clear();
                    throw std::exception("no scheme");
                }
                _scheme = s.substr(0, pos);
                remain_uri = s.substr(pos + 3);

                //
                //  authority
                //
                std::wstring authority;
                pos = remain_uri.find(L"/");
                if (pos == std::wstring::npos) {
                    authority = remain_uri;
                    remain_uri.clear();
                }
                else {
                    authority = remain_uri.substr(0, pos);
                    remain_uri = remain_uri.substr(pos);
                }
                pos = authority.find(L"@");
                if (pos != std::wstring::npos) {
                    // with user info
                    std::wstring user_info;
                    std::wstring host_info;
                    user_info = authority.substr(0, pos);
                    host_info = authority.substr(pos + 1);
                    pos = user_info.find(L":");
                    if (pos != std::wstring::npos) {
                        _user = user_info.substr(0, pos);
                        _password = user_info.substr(pos + 1);
                    }
                    else {
                        _user = user_info;
                    }
                    pos = host_info.find(L":");
                    if (pos != std::wstring::npos) {
                        _host = host_info.substr(0, pos);
                        _port = _wtoi(host_info.substr(pos + 1).c_str());
                    }
                    else {
                        _host = host_info;
                    }
                }
                else {
                    // without user info
                    pos = authority.find(L":");
                    if (pos != std::wstring::npos) {
                        _host = authority.substr(0, pos);
                        _port = _wtoi(authority.substr(pos + 1).c_str());
                    }
                    else {
                        _host = authority;
                    }
                }

                //
                //  path, query and fragment
                //
                pos = remain_uri.find(L"?");
                if (pos == std::wstring::npos) {
                    _path = remain_uri;
                }
                else {
                    _path = remain_uri.substr(0, pos);
                    std::wstring query_and_fragment = remain_uri.substr(pos + 1);
                    pos = query_and_fragment.find(L"#");
                    if (pos == std::wstring::npos) {
                        _query = query_and_fragment;
                    }
                    else {
                        _query = query_and_fragment.substr(0, pos);
                        _fragment = query_and_fragment.substr(pos + 1);
                    }
                }
            }

        protected:
            inline unsigned short _MovePort() { unsigned short tmp = _port; _port = 0; return tmp; }

        private:
            std::wstring    _scheme;
            std::wstring    _user;
            std::wstring    _password;
            std::wstring    _host;
            unsigned short  _port;
            std::wstring    _path;
            std::wstring    _query;
            std::wstring    _fragment;
        };



    }
}

#endif