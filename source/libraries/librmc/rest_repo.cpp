
#include "stdafx.h"

#include <exception>

#include <nx\rmc\rest.h>
#include <nx\rmc\repo.h>

#include <nx\common\rapidjson.h>
#include <nx\common\stringex.h>
#include <nx\crypt\md5.h>



using namespace NX;
using namespace NX::rapidjson;
using namespace NX::REST::http;


namespace {

	class RequestGetAuthUrl : public REST::http::StringBodyRequest
	{
	public:
		RequestGetAuthUrl(const RmRestCommonParams& param,
			const std::wstring& repoName,
			const std::wstring& repoType,
			const std::wstring& siteUrl)
			: StringBodyRequest(REST::http::methods::POST,
				std::wstring(L"/rms/rs/repository/authURL"),
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
				BuildBody(repoName, repoType, siteUrl))
		{
		}
		virtual ~RequestGetAuthUrl() {}

	private:
		std::string BuildBody(const std::wstring& repoName,
			const std::wstring& repoType,
			const std::wstring& siteUrl)
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

				parameters->set(L"type", JsonValue::CreateString(repoType));
				parameters->set(L"name", JsonValue::CreateString(repoName));
				if (!siteUrl.empty()) {
					parameters->set(L"siteURL", JsonValue::CreateString(siteUrl));
				}

			} while (FALSE);

			JsonStringWriter<char> writer;
			s = writer.Write(root.get());
			root.reset();
			return s;
		}
	};

    class RequestGetAllRepos : public REST::http::StringBodyRequest
    {
    public:
        RequestGetAllRepos(const RmRestCommonParams& param)
            : StringBodyRequest(REST::http::methods::GET,
                std::wstring(L"/rms/rs/repository"),
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
        virtual ~RequestGetAllRepos() {}
    };

	class RequestAddNewRepo : public REST::http::StringBodyRequest
	{
	public:
		RequestAddNewRepo(const RmRestCommonParams& param,
			const std::wstring& repoName,
			const std::wstring& repoType,
			bool isRepoShared,
			const std::wstring& accountName,
			const std::wstring& accountId,
			const std::wstring& accountToken)
			: StringBodyRequest(REST::http::methods::POST,
				std::wstring(L"/rms/rs/repository"),
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
				BuildBody(param.GetDeviceId(), param.GetDeviceType(), repoName, repoType, isRepoShared, accountName, accountId, accountToken))
		{
		}
		virtual ~RequestAddNewRepo() {}

	private:
		std::string BuildBody(const std::wstring& deviceId,
			const std::wstring& deviceType,
			const std::wstring& repoName,
			const std::wstring& repoType,
			bool isRepoShared,
			const std::wstring& accountName,
			const std::wstring& accountId,
			const std::wstring& accountToken)
		{
			std::string s;

			std::shared_ptr<JsonObject> root(JsonValue::CreateObject());
			if (root == NULL)
				return s;

			do {

				JsonObject* parameters = JsonValue::CreateObject();
				if (NULL == parameters)
					break;

				parameters->set(L"deviceId", JsonValue::CreateString(deviceId));
				parameters->set(L"deviceType", JsonValue::CreateString(deviceType));

				JsonObject* repository = JsonValue::CreateObject();
				if (NULL == repository)
					break;
				parameters->set(L"repository", repository);
				repository->set(L"name", JsonValue::CreateString(repoName));
				repository->set(L"type", JsonValue::CreateString(repoType));
				repository->set(L"isShared", JsonValue::CreateBool(isRepoShared));
				repository->set(L"accountName", JsonValue::CreateString(accountName));
				repository->set(L"accountId", JsonValue::CreateString(accountId));
				repository->set(L"token", JsonValue::CreateString(accountToken));
				repository->set(L"preference", JsonValue::CreateString(L""));
				repository->set(L"creationTime", JsonValue::CreateNumber(0));

			} while (FALSE);

			JsonStringWriter<char> writer;
			s = writer.Write(root.get());
			root.reset();
			return s;
		}
	};

	class RequestUpdateRepo : public REST::http::StringBodyRequest
	{
	public:
		RequestUpdateRepo(const RmRestCommonParams& param,
			const std::wstring& repoId,
			const std::wstring& repoName,
			const std::wstring& repoToken)
			: StringBodyRequest(REST::http::methods::PUT,
				std::wstring(L"/rms/rs/repository"),
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
				BuildBody(param.GetDeviceId(), param.GetDeviceType(), repoId, repoName, repoToken))
		{
		}
		virtual ~RequestUpdateRepo() {}

	private:
		std::string BuildBody( const std::wstring& deviceId,
			const std::wstring& deviceType,
			const std::wstring& repoId,
			const std::wstring& repoName,
			const std::wstring& repoToken)
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

				parameters->set(L"deviceId", JsonValue::CreateString(deviceId));
				parameters->set(L"deviceType", JsonValue::CreateString(deviceType));
				parameters->set(L"repoId", JsonValue::CreateString(repoId));
				parameters->set(L"token", JsonValue::CreateString(repoToken));
				parameters->set(L"name", JsonValue::CreateString(repoName));

			} while (FALSE);

			JsonStringWriter<char> writer;
			s = writer.Write(root.get());
			root.reset();
			return s;
		}
	};

	class RequestRemoveRepo : public REST::http::StringBodyRequest
	{
	public:
		RequestRemoveRepo(const RmRestCommonParams& param, const std::wstring& repoId)
			: StringBodyRequest(REST::http::methods::DEL,
				std::wstring(L"/rms/rs/repository"),
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
				BuildBody(param.GetDeviceId(), param.GetDeviceType(), repoId))
		{
		}
		virtual ~RequestRemoveRepo() {}

	private:
		std::string BuildBody(const std::wstring& deviceId, const std::wstring& deviceType, const std::wstring& repoId)
		{
			std::wstring s(L"{\"parameters\":{\"deviceId\":\"");
			s.append(deviceId);
			s.append(L"\",\"deviceType\":\"");
			s.append(deviceType);
			s.append(L"\",\"repoId\":\"");
			s.append(repoId);
			s.append(L"\"}}");
			return NX::conv::utf16toutf8(s);
		}
	};

	class RequestGetAccessToken : public REST::http::StringBodyRequest
	{
	public:
		RequestGetAccessToken(const RmRestCommonParams& param, const std::wstring& repoId)
			: StringBodyRequest(REST::http::methods::GET,
				std::wstring(L"/rms/rs/repository/" + repoId + L"/accessToken"),
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
		virtual ~RequestGetAccessToken() {}
	};

}

Result RmRestClient::GetRepoAuthUrl(const std::wstring& rms_url,
    const std::wstring& repoName,
    const std::wstring& repoType,
    const std::wstring& siteUrl,
    RmRestStatus& status,
    std::wstring& url)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(rms_url, res));
    if (p) {
        res = GetRepoAuthUrl(p.get(), repoName, repoType, siteUrl, status, url);
    }
    return res;
}

Result RmRestClient::GetRepoAuthUrl(NX::REST::http::Connection* conn,
    const std::wstring& repoName,
    const std::wstring& repoType,
    const std::wstring& siteUrl,
    RmRestStatus& status,
    std::wstring& url)
{
    RequestGetAuthUrl request(_commonParams, repoName, repoType, siteUrl);
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
            url = root->AsObject()->at(L"results")->AsObject()->at(L"authUrl")->AsString()->GetString();
        }
        catch (...) {
            // The JSON data is NOT correct
            res = RESULT2(ERROR_INVALID_DATA, "JSON response is not correct");
        }

    } while (FALSE);


    return RESULT(0);
}

Result RmRestClient::GetRepositories(const std::wstring& rms_url,
    RmRestStatus& status,
    std::vector<RmRepository>& repos,
    bool& isFullCopy)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(rms_url, res));
    if (p) {
        res = GetRepositories(p.get(), status, repos, isFullCopy);
    }
    return res;
}

Result RmRestClient::GetRepositories(NX::REST::http::Connection* conn,
    RmRestStatus& status,
    std::vector<RmRepository>& repos,
    bool& isFullCopy)
{
    RequestGetAllRepos request(_commonParams);
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

                rapidjson::JsonValue* results = root->AsObject()->at(L"results");
                if (NULL == results)
                    throw std::exception("bad response data");

                rapidjson::JsonValue* fullCopy = results->AsObject()->at(L"isFullCopy");
                if (fullCopy && fullCopy->IsBool()) {
                    isFullCopy = fullCopy->AsBool()->GetBool();
                }

                rapidjson::JsonValue* repoItems = results->AsObject()->at(L"repoItems");
                if (NULL == repoItems)
                    throw std::exception("bad response data");
                if (!repoItems->IsArray())
                    throw std::exception("bad response data");

                std::for_each(repoItems->AsArray()->cbegin(), repoItems->AsArray()->cend(), [&](const rapidjson::JsonValue* item) {

                    RmRepository repo;

                    if (NULL == item)
                        return;
                    if (!item->IsObject())
                        return;
                    
                    const rapidjson::JsonValue* prop = item->AsObject()->at(L"repoId");
                    if (NULL != prop && prop->IsString())
                        repo._id = prop->AsString()->GetString();
                    if (repo.GetId().empty())
                        return;

                    prop = item->AsObject()->at(L"name");
                    if (NULL != prop && prop->IsString())
                        repo._name = prop->AsString()->GetString();
                    if (repo.GetName().empty())
                        return;

                    prop = item->AsObject()->at(L"type");
                    if (NULL != prop && prop->IsString()) {
                        if(0 == _wcsicmp(repo._name.c_str(), L"MyDrive"))
                            repo._type = RmRepository::MYDRIVE;
                        else
                            repo._type = repo.StringToType(prop->AsString()->GetString());
                    }

                    prop = item->AsObject()->at(L"isShared");
                    if (NULL != prop && prop->IsBool())
                        repo._shared = prop->AsBool()->GetBool();

                    prop = item->AsObject()->at(L"accountName");
                    if (NULL != prop && prop->IsString())
                        repo._accountName = prop->AsString()->GetString();

                    prop = item->AsObject()->at(L"accountId");
                    if (NULL != prop && prop->IsString())
                        repo._accountId = prop->AsString()->GetString();

                    prop = item->AsObject()->at(L"token");
                    if (NULL != prop && prop->IsString())
                        repo._token = prop->AsString()->GetString();

                    prop = item->AsObject()->at(L"creationTime");
                    if (NULL != prop && prop->IsNumber())
                        repo._creationTime = prop->AsNumber()->ToInt64();

                    if (RmRepository::MYDRIVE == repo.GetType())
                        repos.insert(repos.begin(), repo);
                    else
                        repos.push_back(repo);
                });
            }
        }
        catch (...) {
            // The JSON data is NOT correct
            res = RESULT2(ERROR_INVALID_DATA, "JSON response is not correct");
        }

    } while (FALSE);


    return RESULT(0);
}

Result RmRestClient::AddRepository(const std::wstring& rms_url,
    const std::wstring& repoName,
    const std::wstring& repoType,
    bool isRepoShared,
    const std::wstring& accountName,
    const std::wstring& accountId,
	const std::wstring& accountToken,
    RmRestStatus& status,
    std::wstring& repoId)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(rms_url, res));
    if (p) {
        res = AddRepository(p.get(), repoName, repoType, isRepoShared, accountName, accountId, accountToken, status, repoId);
    }
    return res;
}

Result RmRestClient::AddRepository(NX::REST::http::Connection* conn,
    const std::wstring& repoName,
    const std::wstring& repoType,
    bool isRepoShared,
    const std::wstring& accountName,
    const std::wstring& accountId,
	const std::wstring& accountToken,
    RmRestStatus& status,
    std::wstring& repoId)
{
	RequestAddNewRepo request(_commonParams, repoName, repoType, isRepoShared, accountName, accountId, accountToken);
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

				rapidjson::JsonValue* results = root->AsObject()->at(L"results");
				if (NULL == results)
					throw std::exception("bad response data");

				rapidjson::JsonValue* pRepoId = results->AsObject()->at(L"repoId");
				if (pRepoId && pRepoId->IsString()) {
					repoId = pRepoId->AsString()->GetString();
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

Result RmRestClient::UpdateRepository(const std::wstring& rms_url,
    const std::wstring& repoId,
    const std::wstring& repoName,
    const std::wstring& repoToken,
    RmRestStatus& status)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(rms_url, res));
    if (p) {
        res = UpdateRepository(p.get(), repoId, repoName, repoToken, status);
    }
    return res;
}

Result RmRestClient::UpdateRepository(NX::REST::http::Connection* conn,
    const std::wstring& repoId,
    const std::wstring& repoName,
    const std::wstring& repoToken,
    RmRestStatus& status)
{
	RequestUpdateRepo request(_commonParams, repoId, repoName, repoToken);
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

Result RmRestClient::RemoveRepository(const std::wstring& rms_url,
    const std::wstring& repoId,
    RmRestStatus& status)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(rms_url, res));
    if (p) {
        res = RemoveRepository(p.get(), repoId, status);
    }
    return res;
}

Result RmRestClient::RemoveRepository(NX::REST::http::Connection* conn,
    const std::wstring& repoId,
    RmRestStatus& status)
{
	RequestRemoveRepo request(_commonParams, repoId);
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

Result RmRestClient::GetAccessToken(const std::wstring& rms_url,
    const std::wstring& repoId,
    RmRestStatus& status,
    std::wstring& accessToken)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(rms_url, res));
    if (p) {
        res = GetAccessToken(p.get(), repoId, status, accessToken);
    }
    return res;
}

Result RmRestClient::GetAccessToken(NX::REST::http::Connection* conn,
    const std::wstring& repoId,
    RmRestStatus& status,
    std::wstring& accessToken)
{
	RequestGetAccessToken request(_commonParams, repoId);
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
                if (NULL == results)
                    throw std::exception("bad response data");

				const rapidjson::JsonValue* pAccessToken = results->AsObject()->at(L"accessToken");
				if (pAccessToken && pAccessToken->IsString()) {
					accessToken = pAccessToken->AsString()->GetString();
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
