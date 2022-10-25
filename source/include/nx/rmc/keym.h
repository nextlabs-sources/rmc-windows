

#ifndef __LIBRMC_KEY_MANAGER_H__
#define __LIBRMC_KEY_MANAGER_H__

#include "dirs.h"
#include "tenant.h"
#include "user.h"
#include "rest.h"

#include <nx\common\result.h>
#include <nx\crypt\basic.h>
#include <nx\crypt\cert.h>

#include <string>

namespace NX {

    class RmClientId
    {
    public:
        RmClientId();
        RmClientId(const RmClientId& rhs);
        ~RmClientId();

        inline bool Empty() const { return _id.empty(); }
        inline const std::wstring& GetClientId() const { return _id; }
        inline const std::vector<UCHAR>& GetClientKey() const { return _key; }

        Result Create(bool machineStore = false);
        Result Load(bool machineStore = false);
        RmClientId& operator = (const RmClientId& rhs);

    private:
        Result CreateCertificate(crypt::CertContext* pcc, bool machineStore = false);
        Result FindCertificate(crypt::CertContext* pcc, bool machineStore = false);
        Result GenerateIdAndKey(const std::vector<UCHAR>& key1, const std::vector<UCHAR>& key2);

    private:
        std::wstring  _id;
        std::vector<UCHAR> _key;
    };

	extern const UCHAR DH_P_2048[256];
	extern const UCHAR DH_G_2048[256];
	extern const UCHAR DH_P_1024[128];
	extern const UCHAR DH_G_1024[128];

	Result GetDHParameterY(const std::wstring& cert, std::vector<UCHAR>& y);
}



#endif