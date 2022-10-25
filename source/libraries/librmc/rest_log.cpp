
#include "stdafx.h"

#include <nx\rmc\rest.h>
#include <nx\rmc\activity.h>

#include <nx\common\rapidjson.h>
#include <nx\common\stringex.h>
#include <nx\crypt\md5.h>


using namespace NX;
using namespace NX::rapidjson;
using namespace NX::REST::http;


namespace {

	class RequestLogActivity : public REST::http::StringBodyRequest
	{
	public:
		RequestLogActivity(const RmRestCommonParams& param, const std::string& logdata)
			: StringBodyRequest(REST::http::methods::PUT,
				std::wstring(L"/rms/rs/log/v2/activity"),
				REST::http::HttpHeaders({
						std::pair<std::wstring, std::wstring>(L"clientId", param.GetClientId()),
						std::pair<std::wstring, std::wstring>(L"platformId", param.GetPlatformId()),
						std::pair<std::wstring, std::wstring>(L"deviceId", param.GetDeviceId()),
						std::pair<std::wstring, std::wstring>(L"tenant", param.GetTenant()),
						std::pair<std::wstring, std::wstring>(L"userId", param.GetUserId()),
						std::pair<std::wstring, std::wstring>(L"ticket", param.GetTicket()),
						std::pair<std::wstring, std::wstring>(REST::http::header_names::content_type, L"text/csv")
					}),
				std::vector<LPCWSTR>({ REST::http::mime_types::application_json.c_str(), REST::http::mime_types::text_csv.c_str(), NULL }),
				logdata)
		{
		}
		virtual ~RequestLogActivity() {}
	};

	class RequestFetchActivity : public REST::http::StringBodyRequest
	{
	public:
		// URL:
		// "/rms/rs/log/v2/activity/{DUID}?start=N1&count=N2&searchField=field1&searchText=filter&orderBy=field2&orderByReverse=false"
		RequestFetchActivity(const RmRestCommonParams& param,
			const std::wstring& duid,
			__int64 startPos,
			__int64 count,
			const std::wstring& searchField,
			const std::wstring& searchText,
			const std::wstring& orderByField,
			bool orderByReverse)
			: StringBodyRequest(REST::http::methods::GET,
				BuildUrl(duid, startPos, count, searchField, searchText, orderByField, orderByReverse),
				REST::http::HttpHeaders({
						std::pair<std::wstring, std::wstring>(L"clientId", param.GetClientId()),
						std::pair<std::wstring, std::wstring>(L"platformId", param.GetPlatformId()),
						std::pair<std::wstring, std::wstring>(L"deviceId", param.GetDeviceId()),
						std::pair<std::wstring, std::wstring>(L"tenant", param.GetTenant()),
						std::pair<std::wstring, std::wstring>(L"userId", param.GetUserId()),
						std::pair<std::wstring, std::wstring>(L"ticket", param.GetTicket()),
						std::pair<std::wstring, std::wstring>(REST::http::header_names::content_type, L"text/csv")
					}),
				std::vector<LPCWSTR>({ REST::http::mime_types::application_json.c_str(), REST::http::mime_types::text_csv.c_str(), NULL }),
				std::string())
		{
		}
		virtual ~RequestFetchActivity() {}

	private:
		std::wstring BuildUrl(
			const std::wstring& duid,
			__int64 startPos,
			__int64 count,
			const std::wstring& searchField,
			const std::wstring& searchText,
			const std::wstring& orderByField,
			bool orderByReverse)
		{
			std::wstring url(L"/rms/rs/log/v2/activity/");
			url += duid;
			url += L"?start=";
			url += NX::i64tos<wchar_t>(startPos);
			url += L"&count=";
			url += NX::i64tos<wchar_t>(count);
			if (!searchField.empty() && !searchText.empty()) {
				url += L"&searchField=";
				url += searchField;
				url += L"&searchText=";
				url += searchText;
			}
			if (!orderByField.empty()) {
				url += L"&orderBy=";
				url += orderByField;
			}
			if(!orderByReverse)
				url += L"&orderByReverse=false";

			return url;
		}
	};

}

Result RmRestClient::LogActivities(const std::wstring& rms_url,
    const std::vector<unsigned char>& logdata,
    RmRestStatus& status)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(rms_url, res));
    if (p) {
        res = LogActivities(p.get(), logdata, status);
    }
    return res;
}

Result RmRestClient::LogActivities(NX::REST::http::Connection* conn,
    const std::vector<unsigned char>& logdata,
    RmRestStatus& status)
{
	RequestLogActivity request(_commonParams, std::string(logdata.begin(), logdata.end()));
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
		}
		catch (...) {
			// The JSON data is NOT correct
			res = RESULT2(ERROR_INVALID_DATA, "JSON response is not correct");
		}

	} while (FALSE);


	return RESULT(0);
}

Result RmRestClient::FetchActivities(const std::wstring& rms_url,
	const std::wstring& duid,
	__int64 startPos,
	__int64 count,
	const std::wstring& searchField,
	const std::wstring& searchText,
	const std::wstring& orderByField,
	bool orderByReverse,
	RmRestStatus& status,
	std::wstring& fileName,
	std::wstring& fileDuid,
	std::vector<RmFileActivityRecord>& records)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(rms_url, res));
    if (p) {
        res = FetchActivities(p.get(), duid, startPos, count, searchField, searchText, orderByField, orderByReverse, status, fileName, fileDuid, records);
    }
    return res;
}

Result RmRestClient::FetchActivities(NX::REST::http::Connection* conn,
	const std::wstring& duid,
	__int64 startPos,
	__int64 count,
	const std::wstring& searchField,
	const std::wstring& searchText,
	const std::wstring& orderByField,
	bool orderByReverse,
	RmRestStatus& status,
	std::wstring& fileName,
	std::wstring& fileDuid,
	std::vector<RmFileActivityRecord>& records)
{
	RequestFetchActivity request(_commonParams, duid, startPos, count, searchField, searchText, orderByField, orderByReverse);
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
				if (NULL == results)
					throw ERROR_INVALID_DATA;
				PCJSONVALUE pData = results->AsObject()->at(L"data");
				if (NULL == pData)
					throw ERROR_INVALID_DATA;
				PCJSONVALUE pFileName = pData->AsObject()->at(L"name");
				if (NULL == pFileName)
					throw ERROR_INVALID_DATA;
				PCJSONVALUE pDuid = pData->AsObject()->at(L"duid");
				if (NULL == pDuid)
					throw ERROR_INVALID_DATA;
				PCJSONVALUE pRecords = pData->AsObject()->at(L"logRecords");
				if (NULL == pRecords)
					throw ERROR_INVALID_DATA;
				if(!pFileName->IsString() || !pDuid->IsString() || !pRecords->IsArray())
					throw ERROR_INVALID_DATA;

				fileName = pFileName->AsString()->GetString();
				fileDuid = pDuid->AsString()->GetString();
				const size_t count = pRecords->AsArray()->size();
				for (size_t i = 0; i < count; i++) {
					PCJSONVALUE record = pRecords->AsArray()->at(i);
					if (record && record->IsObject()) {
						const RmFileActivityRecord& item = RmFileActivityRecord::CreateFromJsonObject(record->AsObject());
						if (!item.Empty())
							records.push_back(item);
					}
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
