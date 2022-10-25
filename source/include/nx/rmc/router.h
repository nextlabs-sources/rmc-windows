

#ifndef __LIBRMC_ROUTER_H__
#define __LIBRMC_ROUTER_H__

#include "dirs.h"
#include "tenant.h"
#include "user.h"
#include "rest.h"

#include <nx\common\result.h>

#include <string>

namespace NX {

    class RmRouter
    {
    public:
        RmRouter();
        RmRouter(const std::wstring& name, const std::wstring& hostUrl);
        RmRouter(const RmRouter& rhs);
        ~RmRouter();

        RmRouter& operator = (const RmRouter& rhs);

        inline bool Empty() const { return (_host.empty() || _name.empty()); }
        inline const std::wstring& GetHost() const { return _host; }
        inline const std::wstring& GetHostUrl() const { return _hostUrl; }
        inline const std::wstring& GetName() const { return _name; }
        inline void Clear() { _host.clear(); _name.clear(); }

		std::vector<std::wstring> GetTenantHistory() const;
		Result SaveTenantHistory(const std::vector<std::wstring>& tenants) const;
		Result SaveToRegistry() const;
		bool ExistInRegistry() const;

		static RmRouter LoadFromRegistry(const std::wstring& name);
		static std::vector<RmRouter> FindAvailableRouters();

	protected:
		std::wstring NormalizeHostUrl(const std::wstring& url);

    private:
        std::wstring    _host;
        std::wstring    _hostUrl;
        std::wstring    _name;
    };

    const RmRouter& GetDefaultRouter();
}


#endif