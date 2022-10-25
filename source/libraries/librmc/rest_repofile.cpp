
#include "stdafx.h"

#include <nx\rmc\rest.h>
#include <nx\rmc\repo.h>

#include <nx\common\rapidjson.h>
#include <nx\common\stringex.h>
#include <nx\crypt\md5.h>


using namespace NX;
using namespace NX::rapidjson;
using namespace NX::REST::http;


namespace {

	class RequestGetAllFavorites : public REST::http::StringBodyRequest
	{
	public:
		RequestGetAllFavorites(const RmRestCommonParams& param)
			: StringBodyRequest(REST::http::methods::GET,
				std::wstring(L"/rms/rs/favorite"),
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
				std::string())
		{
		}
		virtual ~RequestGetAllFavorites() {}
	};

	static std::string BuildMarkUnmarkParameters(const std::wstring& parentFolderId, const std::vector<RmRepoMarkedFile>& files)
	{
		std::string s;

		std::shared_ptr<JsonObject> root(JsonValue::CreateObject());
		if (root == NULL)
			return s;

		do {
			JsonObject* parameters = JsonValue::CreateObject();
			if (NULL == parameters)
				break;
			root->set(L"parameters", parameters);

			JsonArray* pFiles = JsonValue::CreateArray();
			if (NULL == pFiles)
				break;
			parameters->set(L"files", pFiles);

			std::for_each(files.begin(), files.end(), [&](const RmRepoMarkedFile& file) {
				if (file.Empty())
					return;
				JsonObject* pFile = JsonValue::CreateObject();
				if (NULL == pFile)
					return;
				pFiles->push_back(pFile);
				pFile->set(L"pathId", JsonValue::CreateString(file.GetFileId()));
				pFile->set(L"pathDisplay", JsonValue::CreateString(file.GetFilePath()));
				if(!parentFolderId.empty())
					pFile->set(L"parentFileId", JsonValue::CreateString(parentFolderId));
			});

			JsonStringWriter<char> writer;
			s = writer.Write(root.get());

		} while (FALSE);

		return s;
	}

	class RequestMarkFavoriteFiles : public REST::http::StringBodyRequest
	{
	public:
		RequestMarkFavoriteFiles(const RmRestCommonParams& param, const std::wstring& repoId, const std::wstring& parentFolderId, const std::vector<RmRepoMarkedFile>& files)
			: StringBodyRequest(REST::http::methods::POST,
				std::wstring(L"/rms/rs/favorite/" + repoId),
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
				BuildMarkUnmarkParameters(parentFolderId, files))
		{
		}
		virtual ~RequestMarkFavoriteFiles() {}
	};

	class RequestUnmarkFavoriteFiles : public REST::http::StringBodyRequest
	{
	public:
		RequestUnmarkFavoriteFiles(const RmRestCommonParams& param, const std::wstring& repoId, const std::vector<RmRepoMarkedFile>& files)
			: StringBodyRequest(REST::http::methods::DEL,
				std::wstring(L"/rms/rs/favorite/" + repoId),
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
				BuildMarkUnmarkParameters(std::wstring(), files))
		{
		}
		virtual ~RequestUnmarkFavoriteFiles() {}
	};

	RmRepoMarkedFile ReadRepoMarkedFile(PCJSONVALUE root)
	{
		RmRepoMarkedFile file;

		do {

			if (!IsValidJsonObject(root))
				break;

			std::wstring fileId;
			std::wstring filePath;
			std::wstring parentFileId;

			PCJSONVALUE pv = root->AsObject()->at(L"pathId");
			if (pv && pv->IsString())
				fileId = pv->AsString()->GetString();
			if (fileId.empty())
				break;
			
			pv = root->AsObject()->at(L"pathDisplay");
			if (pv && pv->IsString())
				filePath = pv->AsString()->GetString();
			if (filePath.empty())
				break;
			
			pv = root->AsObject()->at(L"parentFileId");
			if (IsValidJsonString(pv))
				parentFileId = pv->AsString()->GetString();

			file = RmRepoMarkedFile(fileId, filePath, parentFileId);
		} while (FALSE);

		return file;
	}

}

Result RmRestClient::RepoGetAllFavoriteFiles(const std::wstring& rms_url,
    RmRestStatus& status,
	std::vector<std::pair<std::wstring, std::vector<RmRepoMarkedFile>>>& favoriteFiles)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(rms_url, res));
    if (p) {
        res = RepoGetAllFavoriteFiles(p.get(),
			status,
			favoriteFiles);
    }
    return res;
}

Result RmRestClient::RepoGetAllFavoriteFiles(NX::REST::http::Connection* conn,
    RmRestStatus& status,
	std::vector<std::pair<std::wstring, std::vector<RmRepoMarkedFile>>>& favoriteFiles)
{
	RequestGetAllFavorites request(_commonParams);
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

		rapidjson::JsonValue* root = doc.GetRoot();
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
					PCJSONVALUE repos = results->AsObject()->at(L"repos");
					if (repos && repos->IsArray()) {

						std::for_each(repos->AsArray()->cbegin(), repos->AsArray()->cend(), [&](const JsonArray::value_type& repo) {

							std::wstring repoId;
							std::vector<RmRepoMarkedFile> files;

							PCJSONVALUE pv = repo->AsObject()->at(L"repoId");
							if (!IsValidJsonString(pv))
								return;

							repoId = pv->AsString()->GetString();
							if (repoId.empty())
								return;

							pv = repo->AsObject()->at(L"markedFavoriteFiles");
							if (!IsValidJsonArray(pv))
								return;

							std::for_each(pv->AsArray()->cbegin(), pv->AsArray()->cend(), [&](const JsonArray::value_type& item) {

								if (!IsValidJsonObject(item))
									return;

								const RmRepoMarkedFile& file = ReadRepoMarkedFile(item);
								if (!file.Empty())
									files.push_back(file);

							});

							if (!files.empty())
								favoriteFiles.push_back(std::pair<std::wstring, std::vector<RmRepoMarkedFile>>(repoId, files));
						});
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

Result RmRestClient::RepoMarkFavoriteFiles(const std::wstring& rms_url,
    const std::wstring& repoId,
	const std::wstring& parentFolderId,
    const std::vector<RmRepoMarkedFile>& files,
    RmRestStatus& status)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(rms_url, res));
    if (p) {
        res = RepoMarkFavoriteFiles(p.get(), repoId, parentFolderId, files, status);
    }
    return res;
}

Result RmRestClient::RepoMarkFavoriteFiles(NX::REST::http::Connection* conn,
    const std::wstring& repoId,
    const std::wstring& parentFolderId,
    const std::vector<RmRepoMarkedFile>& files,
    RmRestStatus& status)
{
	RequestMarkFavoriteFiles   request(_commonParams, repoId, parentFolderId, files);
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

		rapidjson::JsonValue* root = doc.GetRoot();
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

Result RmRestClient::RepoUnmarkFavoriteFiles(const std::wstring& rms_url,
    const std::wstring& repoId,
    const std::vector<RmRepoMarkedFile>& files,
    RmRestStatus& status)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(rms_url, res));
    if (p) {
        res = RepoUnmarkFavoriteFiles(p.get(), repoId, files, status);
    }
    return res;
}

Result RmRestClient::RepoUnmarkFavoriteFiles(NX::REST::http::Connection* conn,
    const std::wstring& repoId,
    const std::vector<RmRepoMarkedFile>& files,
    RmRestStatus& status)
{
	RequestUnmarkFavoriteFiles   request(_commonParams, repoId, files);
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

		rapidjson::JsonValue* root = doc.GetRoot();
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
