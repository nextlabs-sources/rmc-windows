

#ifndef __LIBRMC_TENANT_H__
#define __LIBRMC_TENANT_H__

#include <string>

namespace NX {

	class RmSession;

    class RmTenant
    {
    public:
        RmTenant();
        RmTenant(const std::wstring& name) : _name(name) {};
        RmTenant(const RmTenant& rhs);
        ~RmTenant();
        
        RmTenant& operator = (const RmTenant& rhs);

        void Clear();

        inline bool Empty() const { return _name.empty(); }
        inline const std::wstring& GetName() const { return _name; }
        inline const std::wstring& GetId() const { return _id; }

        static RmTenant LoadFromRegistry(const std::wstring& routerName);

    private:
        std::wstring    _name;
        std::wstring    _id;

        friend class RmSession;
    };

    const RmTenant& GetDefaultTenant();

    class RmTenantPreference
    {
    public:
        RmTenantPreference();
        RmTenantPreference(const RmTenantPreference& rhs);
        ~RmTenantPreference();
        
        RmTenantPreference& operator = (const RmTenantPreference& rhs);

        void Clear();

        inline bool Empty() const { return (0 == _heartbeat_frequency); }
        inline __int64 GetHeartbeatFrequency() const { return _heartbeat_frequency; }

    private:
        __int64 _heartbeat_frequency;

        friend class RmRestClient;
    };
}


#endif