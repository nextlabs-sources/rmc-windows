

#ifndef __LIBRMC_RMSERVER_H__
#define __LIBRMC_RMSERVER_H__

#include "dirs.h"
#include "tenant.h"
#include "user.h"
#include "rest.h"

#include <nx\common\result.h>

#include <string>

namespace NX {

    class RmServer
    {
    public:
        RmServer();
        RmServer(const std::wstring& tenant, const std::wstring& url);
        RmServer(const RmServer& rhs);
        ~RmServer();

        RmServer& operator = (const RmServer& rhs);

        inline bool Empty() const { return (_host.empty() || _tenant.empty()); }
        inline const std::wstring& GetUrl() const { return _url; }
        inline const std::wstring& GetHost() const { return _host; }
        inline USHORT GetPort() const { return _port; }
        inline const std::wstring& GetTenantName() const { return _tenant; }
        inline void Clear() { _host.clear(); _tenant.clear(); }

    private:
        std::wstring    _tenant;
        std::wstring    _scheme;
        std::wstring    _host;
        USHORT          _port;
        std::wstring    _url;
    };
}


#endif