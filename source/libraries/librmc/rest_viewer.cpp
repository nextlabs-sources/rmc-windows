
#include "stdafx.h"

#include <nx\rmc\rest.h>
#include <nx\rmc\user.h>

#include <nx\common\rapidjson.h>
#include <nx\common\stringex.h>
#include <nx\crypt\md5.h>



using namespace NX;
using namespace NX::rapidjson;
using namespace NX::REST::http;


namespace {

	class RequestRemoteViewerUrl : public RmUploadRequest
	{
	public:
		const static unsigned int OperationViewFileInfo     = 0x00000001;
		const static unsigned int OperationPrint            = 0x00000002;
		const static unsigned int OperationProtect          = 0x00000004;
		const static unsigned int OperationShare            = 0x00000008;
		const static unsigned int OperationDownload         = 0x00000010;

		RequestRemoteViewerUrl(const RmRestCommonParams& commonParams,
			const std::wstring& filePath,
			const std::wstring& fileName, _In_opt_ bool* cancelFlag)
			: RmUploadRequest(commonParams,
				std::wstring(L"/rms/rs/remoteView/local"),
				BuildApiInput(commonParams.GetUserName(), commonParams.GetTenant(), commonParams.GetTenant(), fileName),
				filePath, cancelFlag)
		{
		}
		virtual ~RequestRemoteViewerUrl() {}
	protected:
		std::string BuildApiInput(const std::wstring& userName, const std::wstring& tenantId, const std::wstring& tenantName, const std::wstring& fileName)
		{
			std::string s;

			do {
				std::shared_ptr<JsonObject> root(JsonValue::CreateObject());
				if (root == NULL)
					break;

				JsonObject* params = JsonValue::CreateObject();
				if (NULL == params)
					break;
				root->set(L"parameters", params);


				JsonValue* pv = JsonValue::CreateString(userName);
				if (NULL == pv) break;
				params->set(L"userName", pv);

				pv = JsonValue::CreateString(tenantId);
				if (NULL == pv) break;
				params->set(L"tenantId", pv);

				pv = JsonValue::CreateString(tenantName);
				if (NULL == pv) break;
				params->set(L"tenantName", pv);

				pv = JsonValue::CreateString(fileName);
				if (NULL == pv) break;
				params->set(L"fileName", pv);

				pv = JsonValue::CreateNumber(OperationPrint);
				if (NULL == pv) break;
				params->set(L"operations", pv);

				JsonStringWriter<char> writer;
				s = writer.Write(root.get());

			} while (FALSE);

			return s;
		}
	};

	class RequestRemoteViewerUrlForRepoFile : public StringBodyRequest
	{
	public:
		const static unsigned int OperationViewFileInfo = 0x00000001;
		const static unsigned int OperationPrint = 0x00000002;
		const static unsigned int OperationProtect = 0x00000004;
		const static unsigned int OperationShare = 0x00000008;
		const static unsigned int OperationDownload = 0x00000010;

		RequestRemoteViewerUrlForRepoFile(const RmRestCommonParams& param,
			_In_opt_ bool* cancelFlag,
			const std::wstring& repoId,
			const std::wstring& repoName,
			const std::wstring& repoType,
			const std::wstring& pathId,
			const std::wstring& pathDisplay,
			const std::wstring& email,
			__int64 lastModifiedDate
			)
			: StringBodyRequest(REST::http::methods::POST,
				std::wstring(L"/rms/rs/remoteView/repository"),
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
				std::string(BuildBody(param, repoId, repoName, repoType, pathId, pathDisplay, email, lastModifiedDate)),
				cancelFlag)
		{
		}
		virtual ~RequestRemoteViewerUrlForRepoFile() {}
	protected:
		std::string BuildBody(const RmRestCommonParams& param,
			const std::wstring& repoId,
			const std::wstring& repoName,
			const std::wstring& repoType,
			const std::wstring& pathId,
			const std::wstring& pathDisplay,
			const std::wstring& email,
			__int64 lastModifiedDate
		)
		{
			std::string s;

			do {
				std::shared_ptr<JsonObject> root(JsonValue::CreateObject());
				if (root == NULL)
					break;

				JsonObject* params = JsonValue::CreateObject();
				if (NULL == params)
					break;
				root->set(L"parameters", params);


				JsonValue* pv = JsonValue::CreateString(repoId);
				if (NULL == pv) break;
				params->set(L"repoId", pv);

				pv = JsonValue::CreateString(repoName);
				if (NULL == pv) break;
				params->set(L"repoName", pv);

				pv = JsonValue::CreateString((repoType==L"MYDRIVE" ? L"S3" : repoType));
				if (NULL == pv) break;
				params->set(L"repoType", pv);

				pv = JsonValue::CreateString(pathId);
				if (NULL == pv) break;
				params->set(L"pathId", pv);

				pv = JsonValue::CreateString(pathDisplay);
				if (NULL == pv) break;
				params->set(L"pathDisplay", pv);

				pv = JsonValue::CreateString(email);
				if (NULL == pv) break;
				params->set(L"email", pv);

				pv = JsonValue::CreateString(param.GetTenant());
				if (NULL == pv) break;
				params->set(L"tenantName", pv);

				pv = JsonValue::CreateNumber(-480);
				if (NULL == pv) break;
				params->set(L"offset", pv);

				pv = JsonValue::CreateNumber(lastModifiedDate);
				if (NULL == pv) break;
				params->set(L"lastModifiedDate", pv);

				pv = JsonValue::CreateNumber(OperationPrint);
				if (NULL == pv) break;
				params->set(L"operations", pv);

				JsonStringWriter<char> writer;
				s = writer.Write(root.get());

			} while (FALSE);

			return s;
		}
	};


	class RequestRemoteViewerUrlForProjectFile : public StringBodyRequest
	{
	public:
		const static unsigned int OperationViewFileInfo = 0x00000001;
		const static unsigned int OperationPrint = 0x00000002;
		const static unsigned int OperationProtect = 0x00000004;
		const static unsigned int OperationShare = 0x00000008;
		const static unsigned int OperationDownload = 0x00000010;

		RequestRemoteViewerUrlForProjectFile(const RmRestCommonParams& param,
											 _In_opt_ bool* cancelFlag,
											 const std::wstring& projectId,
											 const std::wstring& pathId,
											 const std::wstring& pathDisplay,
											 const std::wstring& email,
											 __int64 lastModifiedDate
										     )
			: StringBodyRequest(REST::http::methods::POST,
				std::wstring(L"/rms/rs/remoteView/project"),
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
				std::string(BuildBody(param, projectId, pathId, pathDisplay, email, lastModifiedDate)),
				cancelFlag)
		{
		}
		virtual ~RequestRemoteViewerUrlForProjectFile() {}
	protected:
		std::string BuildBody(const RmRestCommonParams& param,
			const std::wstring& projectId,
			const std::wstring& pathId,
			const std::wstring& pathDisplay,
			const std::wstring& email,
			__int64 lastModifiedDate
		)
		{
			std::string s;

			do {
				std::shared_ptr<JsonObject> root(JsonValue::CreateObject());
				if (root == NULL)
					break;

				JsonObject* params = JsonValue::CreateObject();
				if (NULL == params)
					break;
				root->set(L"parameters", params);


				JsonValue* pv = JsonValue::CreateNumber(std::stoll(projectId));
				if (NULL == pv) break;
				params->set(L"projectId", pv);

				pv = JsonValue::CreateString(pathId);
				if (NULL == pv) break;
				params->set(L"pathId", pv);

				pv = JsonValue::CreateString(pathDisplay);
				if (NULL == pv) break;
				params->set(L"pathDisplay", pv);

				pv = JsonValue::CreateString(email);
				if (NULL == pv) break;
				params->set(L"email", pv);

				pv = JsonValue::CreateString(param.GetTenant());
				if (NULL == pv) break;
				params->set(L"tenantName", pv);

				pv = JsonValue::CreateNumber(-480);
				if (NULL == pv) break;
				params->set(L"offset", pv);

				pv = JsonValue::CreateNumber(lastModifiedDate);
				if (NULL == pv) break;
				params->set(L"lastModifiedDate", pv);

				pv = JsonValue::CreateNumber(OperationPrint);
				if (NULL == pv) break;
				params->set(L"operations", pv);

				JsonStringWriter<char> writer;
				s = writer.Write(root.get());

			} while (FALSE);

			return s;
		}
	};

}

Result RmRestClient::RemoteViewerNew(const std::wstring& rms_url,
	const std::wstring& filePath,
	const std::wstring& fileName,
	RmRestStatus& status,
	std::vector<std::wstring>& cookies,
	std::wstring& url,
	_In_opt_ bool* cancelFlag)
{
	Result res = RESULT(0);
	std::shared_ptr<Connection> p(Connect(rms_url, res));
	if (p) {
		res = RemoteViewerNew(p.get(), filePath, fileName, status, cookies, url, cancelFlag);
	}
	return res;
}

Result RmRestClient::RemoteViewerNew(NX::REST::http::Connection* conn,
	const std::wstring& filePath,
	const std::wstring& fileName,
	RmRestStatus& status,
	std::vector<std::wstring>& cookies,
	std::wstring& url,
	_In_opt_ bool* cancelFlag)
{
	RequestRemoteViewerUrl request(_commonParams, filePath, fileName, cancelFlag);
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
			if (status) {
				PCJSONVALUE results = root->AsObject()->at(L"results");
				if (IsValidJsonObject(results)) {
					PCJSONVALUE pv = results->AsObject()->at(L"cookies");
					if (IsValidJsonArray(pv)) {
						int count = (int)pv->AsArray()->size();
						for (int i = 0; i < count; i++) {
							PCJSONVALUE cookie = pv->AsArray()->at(i);
							if (IsValidJsonString(cookie)) {
								const std::wstring& sCookie = cookie->AsString()->GetString();
								if (!sCookie.empty())
									cookies.push_back(sCookie);
							}
						}
					}
					else if (IsValidJsonString(pv)) {
						const std::wstring& sCookie = pv->AsString()->GetString();
						if (!sCookie.empty())
							cookies.push_back(sCookie);
					}
					else {
					}
					pv = results->AsObject()->at(L"viewerURL");
					if (IsValidJsonString(pv)) {
						url = pv->AsString()->GetString();
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

Result RmRestClient::RemoteViewerForRepoFile(const std::wstring& rms_url,
	_In_opt_ bool* cancelFlag,
	const std::wstring& repoId,
	const std::wstring& repoName,
	const std::wstring& repoType,
	const std::wstring& pathId,
	const std::wstring& pathDisplay,
	const std::wstring& email,
	__int64 lastModifiedDate,
	RmRestStatus& status,
	std::vector<std::wstring>& cookies,
	std::wstring& url)
{
	Result res = RESULT(0);
	std::shared_ptr<Connection> p(Connect(rms_url, res));
	if (p) {
		res = RemoteViewerForRepoFile(p.get(), cancelFlag, repoId, repoName, repoType, pathId, pathDisplay, email, lastModifiedDate, status, cookies, url);
	}
	return res;
}

Result RmRestClient::RemoteViewerForRepoFile(NX::REST::http::Connection* conn,
	_In_opt_ bool* cancelFlag,
	const std::wstring& repoId,
	const std::wstring& repoName,
	const std::wstring& repoType,
	const std::wstring& pathId,
	const std::wstring& pathDisplay,
	const std::wstring& email,
	__int64 lastModifiedDate,
	RmRestStatus& status,
	std::vector<std::wstring>& cookies,
	std::wstring& url)
{
	RequestRemoteViewerUrlForRepoFile request(_commonParams, cancelFlag, repoId, repoName, repoType, pathId, pathDisplay, email, lastModifiedDate);
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
			if (status) {
				PCJSONVALUE results = root->AsObject()->at(L"results");
				if (IsValidJsonObject(results)) {
					PCJSONVALUE pv = results->AsObject()->at(L"cookies");
					if (IsValidJsonArray(pv)) {
						int count = (int)pv->AsArray()->size();
						for (int i = 0; i < count; i++) {
							PCJSONVALUE cookie = pv->AsArray()->at(i);
							if (IsValidJsonString(cookie)) {
								const std::wstring& sCookie = cookie->AsString()->GetString();
								if (!sCookie.empty())
									cookies.push_back(sCookie);
							}
						}
					}
					else if (IsValidJsonString(pv)) {
						const std::wstring& sCookie = pv->AsString()->GetString();
						if (!sCookie.empty())
							cookies.push_back(sCookie);
					}
					else {
					}
					pv = results->AsObject()->at(L"viewerURL");
					if (IsValidJsonString(pv)) {
						url = pv->AsString()->GetString();
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


Result RmRestClient::RemoteViewerForProjectFile(const std::wstring& rms_url,
	_In_opt_ bool* cancelFlag,
	const std::wstring& projectId,
	const std::wstring& pathId,
	const std::wstring& pathDisplay,
	const std::wstring& email,
	__int64 lastModifiedDate,
	RmRestStatus& status,
	std::vector<std::wstring>& cookies,
	std::wstring& url)
{
	Result res = RESULT(0);
	std::shared_ptr<Connection> p(Connect(rms_url, res));
	if (p) {
		res = RemoteViewerForProjectFile(p.get(), cancelFlag, projectId, pathId, pathDisplay, email, lastModifiedDate, status, cookies, url);
	}
	return res;
}

Result RmRestClient::RemoteViewerForProjectFile(NX::REST::http::Connection* conn,
	_In_opt_ bool* cancelFlag,
	const std::wstring& projectId,
	const std::wstring& pathId,
	const std::wstring& pathDisplay,
	const std::wstring& email,
	__int64 lastModifiedDate,
	RmRestStatus& status,
	std::vector<std::wstring>& cookies,
	std::wstring& url)
{
	RequestRemoteViewerUrlForProjectFile request(_commonParams, cancelFlag, projectId, pathId, pathDisplay, email, lastModifiedDate);
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
			if (status) {
				PCJSONVALUE results = root->AsObject()->at(L"results");
				if (IsValidJsonObject(results)) {
					PCJSONVALUE pv = results->AsObject()->at(L"cookies");
					if (IsValidJsonArray(pv)) {
						int count = (int)pv->AsArray()->size();
						for (int i = 0; i < count; i++) {
							PCJSONVALUE cookie = pv->AsArray()->at(i);
							if (IsValidJsonString(cookie)) {
								const std::wstring& sCookie = cookie->AsString()->GetString();
								if (!sCookie.empty())
									cookies.push_back(sCookie);
							}
						}
					}
					else if (IsValidJsonString(pv)) {
						const std::wstring& sCookie = pv->AsString()->GetString();
						if (!sCookie.empty())
							cookies.push_back(sCookie);
					}
					else {
					}
					pv = results->AsObject()->at(L"viewerURL");
					if (IsValidJsonString(pv)) {
						url = pv->AsString()->GetString();
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