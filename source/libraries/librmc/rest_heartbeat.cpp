
#include "stdafx.h"

#include <nx\rmc\rest.h>
#include <nx\rmc\heartbeat.h>

#include <nx\common\rapidjson.h>
#include <nx\common\stringex.h>
#include <nx\crypt\md5.h>


using namespace NX;
using namespace NX::rapidjson;
using namespace NX::REST::http;


namespace {

    class RequestHeartbeat : public REST::http::StringBodyRequest
    {
    public:
        RequestHeartbeat(const RmRestCommonParams& param, const std::vector<std::pair<std::wstring, std::wstring>>& objects)
            : StringBodyRequest(REST::http::methods::GET,
                std::wstring(L"/rms/rs/heartbeat?tenant=" + param.GetTenant()),
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
				BuildBody(param.GetUserId(), param.GetTicket(), objects))
        {
        }
        virtual ~RequestHeartbeat() {}

	private:
		std::string BuildBody(const std::wstring& userId, const std::wstring& ticket, const std::vector<std::pair<std::wstring, std::wstring>>& objects)
		{
			std::wstring s(L"{\"parameters\":{\"ticket\":\"");
			s += ticket;
			s += L"\",\"objects\":[";
			for (int i = 0; i < (int)objects.size(); i++) {
				const std::pair<std::wstring, std::wstring>& item = objects[i];
				if (0 != i) s += L",";
				s += L"{\"name\":\"";
				s += item.first;
				s += L"\",\"serialNumber\":\"";
				s += item.second;
				s += L"\"}";
			}
			s += L"],\"platformId\":0,\"userId\":\"";
			s += userId;
			s += L"\"}}";
			return NX::conv::utf16toutf8(s);
		}
    };

}

Result RmRestClient::HeartBeat(const std::wstring& rms_url,
	const std::vector<std::pair<std::wstring, std::wstring>>& objects,
	RmRestStatus& status,
	std::vector<RmHeartbeatObject>& returnedObjects)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(rms_url, res));
    if (p) {
        res = HeartBeat(p.get(), objects, status, returnedObjects);
    }
    return res;
}

Result RmRestClient::HeartBeat(NX::REST::http::Connection* conn,
	const std::vector<std::pair<std::wstring, std::wstring>>& objects,
	RmRestStatus& status,
	std::vector<RmHeartbeatObject>& returnedObjects)
{
	RequestHeartbeat request(_commonParams, objects);
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
				PCJSONVALUE results = root->AsObject()->at(L"results");
				if (results && results->IsObject()) {
					std::for_each(results->AsObject()->cbegin(), results->AsObject()->cend(), [&](const JsonObject::value_type& item) {
						if (!item.first.empty() && item.second && item.second->IsObject()) {
							std::wstring sSerial;
							std::wstring sContent;
							PCJSONVALUE pv = item.second->AsObject()->at(L"serialNumber");
							if (pv && pv->IsString())
								sSerial = pv->AsString()->GetString();
							pv = item.second->AsObject()->at(L"content");
							if (pv && pv->IsString())
								sContent = pv->AsString()->GetString();
							if (!sSerial.empty()) {
								returnedObjects.push_back(RmHeartbeatObject(item.first, sSerial, sContent));
							}
						}
					});
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
