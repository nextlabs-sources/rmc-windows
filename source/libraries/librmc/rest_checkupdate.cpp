
#include "stdafx.h"

#include <nx\rmc\rest.h>
#include <nx\rmc\upgrade.h>

#include <nx\common\rapidjson.h>
#include <nx\common\stringex.h>
#include <nx\crypt\md5.h>
#include <nx\winutil\cpu.h>


using namespace NX;
using namespace NX::REST::http;


namespace {

    class RequestCheckUpdate : public REST::http::StringBodyRequest
    {
    public:
        RequestCheckUpdate(const RmRestCommonParams& param, const std::string& processorArch, const std::string& currentVersion)
            : StringBodyRequest(REST::http::methods::POST,
                std::wstring(L"/rms/rs/upgrade?tenant=" + param.GetTenant()),
				REST::http::HttpHeaders({
					std::pair<std::wstring, std::wstring>(L"clientId", param.GetClientId()),
					std::pair<std::wstring, std::wstring>(L"platformId", param.GetPlatformId()),
					std::pair<std::wstring, std::wstring>(L"deviceId", param.GetDeviceId()),
					std::pair<std::wstring, std::wstring>(L"tenant", param.GetTenant()),
					std::pair<std::wstring, std::wstring>(L"userId", param.GetUserId()),
					std::pair<std::wstring, std::wstring>(L"ticket", param.GetTicket()),
					std::pair<std::wstring, std::wstring>(REST::http::header_names::content_type, REST::http::mime_types::application_json)
				}),
                std::vector<LPCWSTR>({ REST::http::mime_types::application_json.c_str(), REST::http::mime_types::text.c_str(), NULL }),
                std::string("{\"parameters\":{\"platformId\":0,\"processorArch\":\"" + processorArch + "\",\"currentVersion\":\"" + currentVersion + "\"}}"))
        {
        }
        virtual ~RequestCheckUpdate() {}
    };

}

Result RmRestClient::CheckSoftwareUpdate(const std::wstring& rms_url,
    const std::wstring& tenant,
	const std::wstring& currentVersion,
    RmRestStatus& status,
	RmUpgradeInfo& info)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(rms_url, res));
    if (p) {
        res = CheckSoftwareUpdate(p.get(), tenant, currentVersion, status, info);
    }
    return res;
}

Result RmRestClient::CheckSoftwareUpdate(NX::REST::http::Connection* conn,
    const std::wstring& tenant,
	const std::wstring& currentVersion,
    RmRestStatus& status,
	RmUpgradeInfo& info)
{
#ifndef _WIN64
	const char* cpuArch = "x86";
#else
	const char* cpuArch = "x64";
#endif
	RequestCheckUpdate request(_commonParams, cpuArch, std::string(currentVersion.begin(), currentVersion.end()));
	StringResponse  response;

	Result res = conn->SendRequest(request, response);
	if (!res) {
		return res;
	}

	status._status = response.GetStatus();
	status._phrase = response.GetPhrase();
	const HttpHeaders& headers = response.GetHeaders();
	const std::string& jsondata = response.GetBody();

	res = RESULT(0);

	do {

		if (status.GetStatus() != status_codes::OK.id) {
			break;
		}

		if (jsondata.empty()) {
			res = RESULT(ERROR_INVALID_DATA);
			break;
		}

		NX::rapidjson::JsonDocument doc;
		int err_code = 0;
		size_t err_pos = 0;
		if (!doc.LoadJsonString(jsondata, &err_code, &err_pos)) {
			res = RESULT(ERROR_INVALID_DATA);
			break;
		}

		const rapidjson::JsonValue* root = doc.GetRoot();
		if (!(NULL != root && root->IsObject())) {
			res = RESULT(ERROR_INVALID_DATA);
			break;
		}

		try {
			status._status = (USHORT)root->AsObject()->at(L"statusCode")->AsNumber()->ToInt();
			status._phrase = root->AsObject()->at(L"message")->AsString()->GetString();
			if (status.GetStatus() == status_codes::OK.id) {
				const rapidjson::JsonValue* results = root->AsObject()->at(L"results");
				if (NULL != results && results->IsObject()) {
					const rapidjson::JsonValue* pVer = results->AsObject()->at(L"newVersion");
					const rapidjson::JsonValue* pDownloadURL = results->AsObject()->at(L"downloadURL");
					const rapidjson::JsonValue* pChecksum = results->AsObject()->at(L"sha1Checksum");
					if (pVer && pVer->IsString() && pDownloadURL && pDownloadURL->IsString() && pChecksum && pChecksum->IsString())
						info = RmUpgradeInfo(pVer->AsString()->GetString(), pDownloadURL->AsString()->GetString(), pChecksum->AsString()->GetString());
				}
			}
		}
		catch (...) {
			// The JSON data is NOT correct
			res = RESULT2(ERROR_INVALID_DATA, "JSON response is not correct");
		}

	} while (FALSE);


	return RESULT(0);
}
