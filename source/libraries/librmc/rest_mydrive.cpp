
#include "stdafx.h"

#include <nx\rmc\rest.h>
#include <nx\rmc\repo.h>

#include <nx\common\rapidjson.h>
#include <nx\common\stringex.h>
#include <nx\crypt\md5.h>


using namespace NX;
using namespace NX::rapidjson;
using namespace NX::REST::http;


std::vector<std::pair<std::wstring, std::wstring>> GetJsonProperties(const NX::rapidjson::JsonValue* node, bool ignore_empty_value);
std::pair<std::wstring, std::wstring> GetJsonProperty(const NX::rapidjson::JsonValue* node);


namespace {

    class RequestGetUsage : public REST::http::StringBodyRequest
    {
    public:
        RequestGetUsage(const RmRestCommonParams& param)
            : StringBodyRequest(REST::http::methods::POST,
                std::wstring(L"/rms/rs/myDrive/getUsage"),
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
                BuildBody(param))
        {
        }
        virtual ~RequestGetUsage() {}

    private:
        std::string BuildBody(const RmRestCommonParams& param)
        {
            std::wstring s = L"{\"parameters\":{\"userId\":";
            s += param.GetUserId();
            s += L",\"ticket\":\"";
            s += param.GetTicket();
            s += L"\"}}";
            return NX::conv::utf16toutf8(s);
        }
    };

    class RequestListFile : public REST::http::StringBodyRequest
    {
    public:
        RequestListFile(const RmRestCommonParams& param, const std::wstring& path, _In_opt_ bool* cancelFlag)
            : StringBodyRequest(REST::http::methods::POST,
                std::wstring(L"/rms/rs/myDrive/list"),
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
                BuildBody(path), cancelFlag)
        {
        }
        virtual ~RequestListFile() {}

    private:
        std::string BuildBody(const std::wstring& path)
        {
            std::wstring s = L"{\"parameters\":{\"pathId\":\"";
            s += path;
            s += L"\"}}";
            return NX::conv::utf16toutf8(s);
        }
    };

    class RequestSearchFile : public REST::http::StringBodyRequest
    {
    public:
        RequestSearchFile(const RmRestCommonParams& param, const std::wstring& keywords, _In_opt_ bool* cancelFlag)
            : StringBodyRequest(REST::http::methods::POST,
                std::wstring(L"/rms/rs/myDrive/search"),
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
                BuildBody(keywords), cancelFlag)
        {
        }
        virtual ~RequestSearchFile() {}

    private:
        std::string BuildBody(const std::wstring& keywords)
        {
            std::wstring s = L"{\"parameters\":{\"query\":\"";
            s += keywords;
            s += L"\"}}";
            return NX::conv::utf16toutf8(s);
        }
    };

    class RequestCreateFolder : public REST::http::StringBodyRequest
    {
    public:
        RequestCreateFolder(const RmRestCommonParams& param, const std::wstring& parentDir, const std::wstring& folderName)
            : StringBodyRequest(REST::http::methods::POST,
                std::wstring(L"/rms/rs/myDrive/createFolder"),
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
                BuildBody(parentDir, folderName))
        {
        }
        virtual ~RequestCreateFolder() {}

    private:
        std::string BuildBody(const std::wstring& parentDir, const std::wstring& folderName)
        {
			std::string s;

			std::shared_ptr<JsonObject> root(JsonValue::CreateObject());
			if (root == nullptr)
				return s;

			JsonObject* params = JsonValue::CreateObject();
			if (params == nullptr)
				return s;
			root->set(L"parameters", params);

			JsonString* pv = JsonValue::CreateString(parentDir);
			if (pv == nullptr)
				return s;
			params->set(L"parentPathId", pv);
			pv = JsonValue::CreateString(folderName);
			if (pv == nullptr)
				return s;
			params->set(L"name", pv);

			JsonStringWriter<char> writer;
			s = writer.Write(root.get());

			return s;
        }
    };

    class RequestDeleteFile : public REST::http::StringBodyRequest
    {
    public:
        RequestDeleteFile(const RmRestCommonParams& param, const std::wstring& path)
            : StringBodyRequest(REST::http::methods::POST,
                std::wstring(L"/rms/rs/myDrive/delete"),
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
                BuildBody(path))
        {
        }
        virtual ~RequestDeleteFile() {}

    private:
        std::string BuildBody(const std::wstring& path)
        {
            std::wstring s = L"{\"parameters\":{\"pathId\":\"";
            s += path;
            s += L"\"}}";
            return NX::conv::utf16toutf8(s);
        }
    };

    class RequestDownloadFile : public REST::http::StringBodyRequest
    {
    public:
        RequestDownloadFile(const RmRestCommonParams& param, const RmRepoFile& source, _In_opt_ bool* cancelFlag, const ULONG requestedLength = 0)
            : StringBodyRequest(REST::http::methods::POST,
                std::wstring(L"/rms/rs/myDrive/download"),
                REST::http::HttpHeaders({
					std::pair<std::wstring, std::wstring>(L"clientId", param.GetClientId()),
					std::pair<std::wstring, std::wstring>(L"platformId", param.GetPlatformId()),
					std::pair<std::wstring, std::wstring>(L"deviceId", param.GetDeviceId()),
					std::pair<std::wstring, std::wstring>(L"tenant", param.GetTenant()),
					std::pair<std::wstring, std::wstring>(L"userId", param.GetUserId()),
					std::pair<std::wstring, std::wstring>(L"ticket", param.GetTicket()),
					std::pair<std::wstring, std::wstring>(REST::http::header_names::content_type, REST::http::mime_types::application_json)
                }),
                std::vector<LPCWSTR>({ REST::http::mime_types::application_octetstream.c_str(), NULL }),
                BuildBody(source), cancelFlag, requestedLength)
        {
        }
        virtual ~RequestDownloadFile() {}

    private:
        std::string BuildBody(const RmRepoFile& source)
        {
            std::shared_ptr<NX::rapidjson::JsonValue> root(NX::rapidjson::JsonValue::CreateObject());
            NX::rapidjson::JsonValue* params = NX::rapidjson::JsonValue::CreateObject();
            params->AsObject()->set(L"pathId", NX::rapidjson::JsonValue::CreateString(source.GetId()));
            root->AsObject()->set(L"parameters", params);
            NX::rapidjson::JsonStringWriter<char> writer;
            std::string s = writer.Write(root.get());
            root.reset();
            return s;
        }
    };

	class RequestCreatePublicShareUrl : public REST::http::StringBodyRequest
	{
	public:
		RequestCreatePublicShareUrl(const RmRestCommonParams& param, const std::wstring& path)
			: StringBodyRequest(REST::http::methods::POST,
				std::wstring(L"/rms/rs/myDrive/getPublicUrl"),
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
				BuildBody(path))
		{
		}
		virtual ~RequestCreatePublicShareUrl() {}

	private:
		std::string BuildBody(const std::wstring& path)
		{
			std::shared_ptr<NX::rapidjson::JsonValue> root(NX::rapidjson::JsonValue::CreateObject());
			NX::rapidjson::JsonValue* params = NX::rapidjson::JsonValue::CreateObject();
			params->AsObject()->set(L"pathId", NX::rapidjson::JsonValue::CreateString(path));
			root->AsObject()->set(L"parameters", params);
			NX::rapidjson::JsonStringWriter<char> writer;
			std::string s = writer.Write(root.get());
			root.reset();
			return s;
		}
	};


	class RequestCopy : public REST::http::StringBodyRequest
	{
	public:
		RequestCopy(const RmRestCommonParams& param, const std::wstring& source, const std::wstring& target)
			: StringBodyRequest(REST::http::methods::POST,
				std::wstring(L"/rms/rs/myDrive/copy"),
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
				BuildBody(source, target))
		{
		}
		virtual ~RequestCopy() {}

	private:
		std::string BuildBody(const std::wstring& source, const std::wstring& target)
		{
			std::shared_ptr<NX::rapidjson::JsonValue> root(NX::rapidjson::JsonValue::CreateObject());
			NX::rapidjson::JsonValue* params = NX::rapidjson::JsonValue::CreateObject();
			root->AsObject()->set(L"parameters", params);
			params->AsObject()->set(L"srcPathId", NX::rapidjson::JsonValue::CreateString(source));
			params->AsObject()->set(L"destPathId", NX::rapidjson::JsonValue::CreateString(target));
			NX::rapidjson::JsonStringWriter<char> writer;
			std::string s = writer.Write(root.get());
			root.reset();
			return s;
		}
	};

	class RequestMove : public REST::http::StringBodyRequest
	{
	public:
		RequestMove(const RmRestCommonParams& param, const std::wstring& source, const std::wstring& target)
			: StringBodyRequest(REST::http::methods::POST,
				std::wstring(L"/rms/rs/myDrive/move"),
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
				BuildBody(source, target))
		{
		}
		virtual ~RequestMove() {}

	private:
		std::string BuildBody(const std::wstring& source, const std::wstring& target)
		{
			std::shared_ptr<NX::rapidjson::JsonValue> root(NX::rapidjson::JsonValue::CreateObject());
			NX::rapidjson::JsonValue* params = NX::rapidjson::JsonValue::CreateObject();
			params->AsObject()->set(L"srcPathId", NX::rapidjson::JsonValue::CreateString(source));
			params->AsObject()->set(L"destPathId", NX::rapidjson::JsonValue::CreateString(target));
			root->AsObject()->set(L"parameters", params);
			NX::rapidjson::JsonStringWriter<char> writer;
			std::string s = writer.Write(root.get());
			root.reset();
			return s;
		}
	};

	class RequestGetFolderMetadata : public REST::http::StringBodyRequest
	{
	public:
		RequestGetFolderMetadata(const RmRestCommonParams& param, const std::wstring& folderId, __int64 lastModifiedTime)
			: StringBodyRequest(REST::http::methods::POST,
				std::wstring(L"/rms/rs/myDrive/folderMetadata"),
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
				BuildBody(folderId, lastModifiedTime))
		{
		}
		virtual ~RequestGetFolderMetadata() {}

	private:
		std::string BuildBody(const std::wstring& folderId, __int64 lastModifiedTime)
		{
			std::shared_ptr<NX::rapidjson::JsonValue> root(NX::rapidjson::JsonValue::CreateObject());
			NX::rapidjson::JsonValue* params = NX::rapidjson::JsonValue::CreateObject();
			root->AsObject()->set(L"parameters", params);
			params->AsObject()->set(L"pathId", NX::rapidjson::JsonValue::CreateString(folderId));
			params->AsObject()->set(L"lastModifiedTime", NX::rapidjson::JsonValue::CreateNumber(lastModifiedTime));
			NX::rapidjson::JsonStringWriter<char> writer;
			std::string s = writer.Write(root.get());
			root.reset();
			return s;
		}
	};

	std::string MyDriveBuildUploadApiParameters(const std::wstring& path)
	{
		std::wstring parentDir;
		std::wstring fileName;
		auto pos = path.rfind(L'/');
		if (pos == std::wstring::npos) {
			fileName = path;
			parentDir = L"/";
		}
		else {
			fileName = path.substr(pos + 1);
			parentDir = path.substr(0, pos + 1);
			if (parentDir.empty())
				parentDir = L"/";
		}

		std::string s;
		std::shared_ptr<JsonObject> root(NX::rapidjson::JsonValue::CreateObject());
		if (root == nullptr)
			return s;
		JsonObject* params = NX::rapidjson::JsonValue::CreateObject();
		if (params == nullptr)
			return s;
		root->set(L"parameters", params);

		JsonString* pv = JsonValue::CreateString(parentDir);
		if (pv == nullptr)
			return s;
		params->set(L"parentPathId", pv);

		pv = JsonValue::CreateString(fileName);
		if (pv == nullptr)
			return s;
		params->set(L"name", pv);

		JsonStringWriter<char> writer;
		s = writer.Write(root.get());
		root.reset();
		return s;
	}
}

Result RmRestClient::MyDriveGetUsage(const std::wstring& rms_url,
    RmRestStatus& status,
	RmMyDriveUsage& usage)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(rms_url, res));
    if (p) {
        res = MyDriveGetUsage(p.get(), status, usage);
    }
    return res;
}

Result RmRestClient::MyDriveGetUsage(NX::REST::http::Connection* conn,
    RmRestStatus& status,
	RmMyDriveUsage& usage)
{
    RequestGetUsage request(_commonParams);
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

		PCJSONVALUE root = doc.GetRoot();
        if (!(NULL != root && root->IsObject())) {
            res = RESULT(ERROR_INVALID_DATA);
            break;
        }

        try {
            status._status = (USHORT)root->AsObject()->at(L"statusCode")->AsNumber()->ToInt();
            status._phrase = root->AsObject()->at(L"message")->AsString()->GetString();
            if (status.GetStatus() == status_codes::OK.id) {
                PCJSONVALUE results = root->AsObject()->at(L"results");
				if (IsValidJsonObject(results)) {
					PCJSONVALUE pv = results->AsObject()->at(L"quota");
					if (IsValidJsonNumber(pv))
						usage._quota = pv->AsNumber()->ToInt64();
					pv = results->AsObject()->at(L"usage");
					if (IsValidJsonNumber(pv))
						usage._used = pv->AsNumber()->ToInt64();
					pv = results->AsObject()->at(L"myVaultUsage");
					if (IsValidJsonNumber(pv))
						usage._usedByMyVault = pv->AsNumber()->ToInt64();
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

Result RmRestClient::MyDriveListFiles(const std::wstring& rms_url, _In_opt_ bool* cancelFlag,
    const std::wstring& folderId,
    RmRestStatus& status,
    std::vector<RmRepoFile>& files)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(rms_url, res));
    if (p) {
        res = MyDriveListFiles(p.get(), cancelFlag, folderId, status, files);
    }
    return res;
}

Result RmRestClient::MyDriveListFiles(NX::REST::http::Connection* conn, _In_opt_ bool* cancelFlag,
    const std::wstring& folderId,
    RmRestStatus& status,
    std::vector<RmRepoFile>& files)
{
    RequestListFile request(_commonParams, folderId, cancelFlag);
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
                if (results && results->IsObject()) {
                    rapidjson::JsonValue* entries = results->AsObject()->at(L"entries");
                    if (entries && entries->IsArray()) {
                        const int count = (int)entries->AsArray()->size();
                        for (int i = 0; i < count; i++) {
                            RmRepoFile repoFile;
                            const rapidjson::JsonValue* fileObject = entries->AsArray()->at(i);
                            assert(NULL != fileObject);
                            const rapidjson::JsonValue* path = fileObject->AsObject()->at(L"pathId");
                            if (path && path->IsString())
                                repoFile._id = path->AsString()->GetString();
                            if (repoFile.GetId().empty())
                                continue;
                            const rapidjson::JsonValue* displayPath = fileObject->AsObject()->at(L"pathDisplay");
                            if (displayPath && displayPath->IsString())
                                repoFile._path = displayPath->AsString()->GetString();
                            if (repoFile.GetPath().empty())
                                repoFile._path = repoFile._id;
                            const rapidjson::JsonValue* name = fileObject->AsObject()->at(L"name");
                            if (name && name->IsString())
                                repoFile._name = name->AsString()->GetString();
                            if (repoFile.GetName().empty()) {
                                auto pos = repoFile._id.find_last_of(L'/');
                                repoFile._name = (pos == std::wstring::npos) ? repoFile._id : repoFile._id.substr(pos + 1);
                            }
                            const rapidjson::JsonValue* folder = fileObject->AsObject()->at(L"folder");
                            if (folder && folder->IsBool())
                                repoFile._folder = folder->AsBool()->GetBool();
                            const rapidjson::JsonValue* lastModified = fileObject->AsObject()->at(L"lastModified");
                            if (lastModified && lastModified->IsNumber())
                                repoFile._time = lastModified->AsNumber()->ToInt64();
                            const rapidjson::JsonValue* fileSize = fileObject->AsObject()->at(L"size");
                            if (fileSize && fileSize->IsNumber())
                                repoFile._size = fileSize->AsNumber()->ToInt64();

                            files.push_back(repoFile);
                        }
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

Result RmRestClient::MyDriveSearchFiles(const std::wstring& rms_url, _In_opt_ bool* cancelFlag,
	const std::wstring& parentDir,
    const std::wstring& keywords,
	bool recursively,
    RmRestStatus& status,
    std::vector<RmRepoFile>& files)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(rms_url, res));
    if (p) {
        res = MyDriveSearchFiles(p.get(), cancelFlag, parentDir, keywords, recursively, status, files);
    }
    return res;
}

Result RmRestClient::MyDriveSearchFiles(NX::REST::http::Connection* conn, _In_opt_ bool* cancelFlag,
	const std::wstring& parentDir,
    const std::wstring& keywords,
	bool recursively,
    RmRestStatus& status,
    std::vector<RmRepoFile>& files)
{
    RequestSearchFile request(_commonParams, keywords, cancelFlag);
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
		
		const std::wstring parentDirPathId(parentDir.empty() ? L"" : (NX::iend_with<wchar_t>(parentDir, L"/") ? parentDir : std::wstring(parentDir + L"/")));

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
                if (results && results->IsObject()) {
                    rapidjson::JsonValue* entries = results->AsObject()->at(L"matches");
                    if (entries && entries->IsArray()) {
                        const int count = (int)entries->AsArray()->size();
                        for (int i = 0; i < count; i++) {
                            RmRepoFile repoFile;
                            const rapidjson::JsonValue* fileObject = entries->AsArray()->at(i);
                            assert(NULL != fileObject);
                            const rapidjson::JsonValue* path = fileObject->AsObject()->at(L"pathId");
                            if (path && path->IsString())
                                repoFile._id = path->AsString()->GetString();
                            if (repoFile.GetId().empty())
                                continue;

                            const rapidjson::JsonValue* displayPath = fileObject->AsObject()->at(L"pathDisplay");
                            if (displayPath && displayPath->IsString())
                                repoFile._path = displayPath->AsString()->GetString();
                            if (repoFile.GetPath().empty())
                                repoFile._path = repoFile._id;
                            const rapidjson::JsonValue* name = fileObject->AsObject()->at(L"name");
                            if (name && name->IsString())
                                repoFile._name = name->AsString()->GetString();
                            if (repoFile.GetName().empty()) {
                                auto pos = repoFile._id.find_last_of(L'/');
                                repoFile._name = (pos == std::wstring::npos) ? repoFile._id : repoFile._id.substr(pos + 1);
                            }
                            const rapidjson::JsonValue* folder = fileObject->AsObject()->at(L"folder");
                            if (folder && folder->IsBool())
                                repoFile._folder = folder->AsBool()->GetBool();
                            const rapidjson::JsonValue* lastModified = fileObject->AsObject()->at(L"lastModified");
                            if (lastModified && lastModified->IsNumber())
                                repoFile._time = lastModified->AsNumber()->ToInt64();
                            const rapidjson::JsonValue* fileSize = fileObject->AsObject()->at(L"size");
                            if (fileSize && fileSize->IsNumber())
                                repoFile._size = fileSize->AsNumber()->ToInt64();

                            const rapidjson::JsonValue* properties = fileObject->AsObject()->at(L"properties");
                            if (properties && properties->IsArray()) {
                                repoFile._props = GetJsonProperties(properties, true);
                            }

							if (parentDirPathId.empty()) {
								// No filter, get all files
								files.push_back(repoFile);
							}
							else {
								if (NX::ibegin_with<wchar_t>(repoFile.GetId(), parentDirPathId) && 0 != _wcsicmp(repoFile.GetId().c_str(), parentDirPathId.c_str())) {
									if (recursively) {
										// Include files under sub-dir
										files.push_back(repoFile);
									}
									else {
										// Exclude files under sub-dir
										if (std::wstring::npos == repoFile.GetId().find(L'/', parentDirPathId.length())) {
											files.push_back(repoFile);
										}
									}
								}
							}
                        }
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

Result RmRestClient::MyDriveCreateFolder(const std::wstring& rms_url,
	const std::wstring& parentDir,
	const std::wstring& folderName,
	RmRestStatus& status,
	RmRepoFile& folder)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(rms_url, res));
    if (p) {
        res = MyDriveCreateFolder(p.get(), parentDir, folderName, status, folder);
    }
    return res;
}

Result RmRestClient::MyDriveCreateFolder(NX::REST::http::Connection* conn,
    const std::wstring& parentDir,
    const std::wstring& folderName,
    RmRestStatus& status,
    RmRepoFile& folder)
{
    RequestCreateFolder request(_commonParams, parentDir, folderName);
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
				if (IsValidJsonObject(results)) {
					PCJSONVALUE pv = results->AsObject()->at(L"pathId");
					if (IsValidJsonString(pv))
						folder._id = pv->AsString()->GetString();
					pv = results->AsObject()->at(L"pathDisplay");
					if (IsValidJsonString(pv))
						folder._path = pv->AsString()->GetString();
					pv = results->AsObject()->at(L"name");
					if (IsValidJsonString(pv))
						folder._name = pv->AsString()->GetString();
					pv = results->AsObject()->at(L"size");
					if (IsValidJsonNumber(pv))
						folder._size = pv->AsNumber()->ToInt64();
					pv = results->AsObject()->at(L"lastModified");
					if (IsValidJsonNumber(pv))
						folder._time = pv->AsNumber()->ToInt64();
					pv = results->AsObject()->at(L"folder");
					if (IsValidJsonBool(pv)) {
						folder._folder = pv->AsBool()->GetBool();
						assert(folder._folder);
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

Result RmRestClient::MyDriveDownloadFile(const std::wstring& rms_url,
	_In_opt_ bool* cancelFlag,
    const RmRepoFile& source,
    const std::wstring& target_path,
	const ULONG requestedLength)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(rms_url, res));
    if (p) {
        res = MyDriveDownloadFile(p.get(), cancelFlag, source, target_path, requestedLength);
    }
    return res;
}

Result RmRestClient::MyDriveDownloadFile(NX::REST::http::Connection* conn,
	_In_opt_ bool* cancelFlag,
    const RmRepoFile& source,
    const std::wstring& target_path,
	const ULONG requestedLength)
{
    RequestDownloadFile request(_commonParams, source, cancelFlag, requestedLength);
    FileResponse  response(target_path);

    if (!response.is_open()) {
        return RESULT2(GetLastError(), "fail to open target file");
    }

    Result res = conn->SendRequest(request, response);
    response.Finish();
    if (!res) {
        return res;
    }
	if (response.GetStatus() < 200 || response.GetStatus() > 299) {
		return RESULT2((int)response.GetStatus(), NX::conv::utf16toutf8(response.GetPhrase()));
	}

    return RESULT(0);
}

Result RmRestClient::MyDriveUploadFile(const std::wstring& rms_url,
	_In_opt_ bool* cancelFlag,
    const std::wstring& source_path,
    const std::wstring& target_path,
	RmRestStatus& status,
	RmRepoFile& newFile,
	_Out_opt_ RmMyDriveUsage* newUsage)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(rms_url, res));
    if (p) {
        res = MyDriveUploadFile(p.get(), cancelFlag, source_path, target_path, status, newFile, newUsage);
    }
    return res;
}

Result RmRestClient::MyDriveUploadFile(NX::REST::http::Connection* conn,
	_In_opt_ bool* cancelFlag,
    const std::wstring& source_path,
    const std::wstring& target_path,
	RmRestStatus& status,
	RmRepoFile& newFile,
	_Out_opt_ RmMyDriveUsage* newUsage)
{
	if (INVALID_FILE_ATTRIBUTES == GetFileAttributesW(source_path.c_str()))
		return RESULT(ERROR_NOT_FOUND);

	RmUploadRequest request(_commonParams, L"/rms/rs/myDrive/uploadFile", MyDriveBuildUploadApiParameters(target_path), source_path, cancelFlag);
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

		PCJSONVALUE root = doc.GetRoot();
		if (!IsValidJsonObject(root)) {
			res = RESULT(ERROR_INVALID_DATA);
			break;
		}

		try {

			status._status = (USHORT)root->AsObject()->at(L"statusCode")->AsNumber()->ToInt();
			status._phrase = root->AsObject()->at(L"message")->AsString()->GetString();
			if (status.GetStatus() == status_codes::OK.id) {
				PCJSONVALUE results = root->AsObject()->at(L"results");
				if (IsValidJsonObject(results)) {

					PCJSONVALUE entry = results->AsObject()->at(L"entry");
					if (IsValidJsonObject(entry)) {

						PCJSONVALUE pv = NULL;

						pv = entry->AsObject()->at(L"pathId");
						if(IsValidJsonString(pv))
							newFile._id = pv->AsString()->GetString();

						pv = entry->AsObject()->at(L"pathDisplay");
						if(IsValidJsonString(pv))
							newFile._path = pv->AsString()->GetString();

						pv = entry->AsObject()->at(L"name");
						if(IsValidJsonString(pv))
							newFile._name = pv->AsString()->GetString();

						pv = entry->AsObject()->at(L"size");
						if(IsValidJsonNumber(pv))
							newFile._size = pv->AsNumber()->ToInt64();

						pv = entry->AsObject()->at(L"lastModified");
						if(IsValidJsonNumber(pv))
							newFile._time = pv->AsNumber()->ToInt64();

						pv = entry->AsObject()->at(L"folder");
						if(IsValidJsonBool(pv))
							newFile._folder = pv->AsBool()->GetBool();

						if (newUsage) {
							pv = results->AsObject()->at(L"quota");
							newUsage->_quota = IsValidJsonNumber(pv) ? pv->AsNumber()->ToInt64() : 0;
							pv = results->AsObject()->at(L"usage");
							newUsage->_used = IsValidJsonNumber(pv) ? pv->AsNumber()->ToInt64() : 0;
							pv = results->AsObject()->at(L"myVaultUsage");
							newUsage->_usedByMyVault = IsValidJsonNumber(pv) ? pv->AsNumber()->ToInt64() : 0;
						}
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

Result RmRestClient::MyDriveDeleteFile(const std::wstring& rms_url,
    const std::wstring& path,
    RmRestStatus& status,
    __int64& usage)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(rms_url, res));
    if (p) {
        res = MyDriveDeleteFile(p.get(), path, status, usage);
    }
    return res;
}

Result RmRestClient::MyDriveDeleteFile(NX::REST::http::Connection* conn,
    const std::wstring& path,
    RmRestStatus& status,
    __int64& usage)
{
    RequestDeleteFile request(_commonParams, path);
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
                if (results && results->IsObject()) {
                    rapidjson::JsonValue* newUsage = results->AsObject()->at(L"usage");
                    if (newUsage && newUsage->IsNumber()) {
                        usage = newUsage->AsNumber()->ToInt64();
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

Result RmRestClient::MyDriveCreatePublicShareUrl(const std::wstring& rms_url,
    const std::wstring& path,
    RmRestStatus& status,
    std::wstring& public_url)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(rms_url, res));
    if (p) {
        res = MyDriveCreatePublicShareUrl(p.get(), path, status, public_url);
    }
    return res;
}

Result RmRestClient::MyDriveCreatePublicShareUrl(NX::REST::http::Connection* conn,
    const std::wstring& path,
    RmRestStatus& status,
    std::wstring& public_url)
{
	RequestCreatePublicShareUrl request(_commonParams, path);
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
				if (results && results->IsObject()) {
					rapidjson::JsonValue* pUrl = results->AsObject()->at(L"url");
					if (pUrl && pUrl->IsString()) {
						public_url = pUrl->AsString()->GetString();
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

Result RmRestClient::MyDriveCopyFile(const std::wstring& rms_url,
    const std::wstring& source_path,
    const std::wstring& target_path,
    RmRestStatus& status,
    __int64* usage)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(rms_url, res));
    if (p) {
        res = MyDriveCopyFile(p.get(), source_path, target_path, status, usage);
    }
    return res;
}

Result RmRestClient::MyDriveCopyFile(NX::REST::http::Connection* conn,
    const std::wstring& source_path,
    const std::wstring& target_path,
    RmRestStatus& status,
    __int64* usage)
{
	RequestCopy request(_commonParams, source_path, target_path);
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
				const rapidjson::JsonValue* results = root->AsObject()->at(L"results");
				if (results && results->IsObject() && usage) {
					const rapidjson::JsonValue* newUsage = results->AsObject()->at(L"usage");
					if (newUsage && newUsage->IsNumber()) {
						*usage = newUsage->AsNumber()->ToInt64();
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

Result RmRestClient::MyDriveMoveFile(const std::wstring& rms_url,
    const std::wstring& source_path,
    const std::wstring& target_path,
    RmRestStatus& status)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(rms_url, res));
    if (p) {
        res = MyDriveMoveFile(p.get(), source_path, target_path, status);
    }
    return res;
}

Result RmRestClient::MyDriveMoveFile(NX::REST::http::Connection* conn,
    const std::wstring& source_path,
    const std::wstring& target_path,
    RmRestStatus& status)
{
	RequestMove request(_commonParams, source_path, target_path);
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

Result RmRestClient::MyDriveGetFolderMetadata(const std::wstring& rms_url, _In_opt_ bool* cancelFlag,
	const std::wstring& folderId,
	__int64 lastModifiedTime,
	RmRestStatus& status,
	std::vector<RmRepoFile>& files)
{
	Result res = RESULT(0);
	std::shared_ptr<Connection> p(Connect(rms_url, res));
	if (p) {
		res = MyDriveGetFolderMetadata(p.get(), cancelFlag, folderId, lastModifiedTime, status, files);
	}
	return res;
}

Result RmRestClient::MyDriveGetFolderMetadata(NX::REST::http::Connection* conn, _In_opt_ bool* cancelFlag,
	const std::wstring& folderId,
	__int64 lastModifiedTime,
	RmRestStatus& status,
	std::vector<RmRepoFile>& files)
{
	RequestGetFolderMetadata request(_commonParams, folderId, lastModifiedTime);
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
					
					PCJSONVALUE entries = results->AsObject()->at(L"entries");
					if (IsValidJsonArray(entries)) {

						const int count = (int)entries->AsArray()->size();
						for (int i = 0; i < count; i++) {

							PCJSONVALUE item = entries->AsArray()->at(i);
							if (!IsValidJsonObject(item))
								continue;

							RmRepoFile file;

							PCJSONVALUE pv = item->AsObject()->at(L"pathId");
							if (!IsValidJsonString(pv))
								continue;
							file._id = pv->AsString()->GetString();
							
							pv = item->AsObject()->at(L"pathDisplay");
							if (!IsValidJsonString(pv))
								continue;
							file._path = pv->AsString()->GetString();
							
							pv = item->AsObject()->at(L"name");
							if (!IsValidJsonString(pv))
								continue;
							file._name = pv->AsString()->GetString();
							
							pv = item->AsObject()->at(L"size");
							if (IsValidJsonNumber(pv))
								file._size = pv->AsNumber()->ToInt64();
							
							pv = item->AsObject()->at(L"lastModified");
							if (IsValidJsonNumber(pv))
								file._time = pv->AsNumber()->ToInt64();
							
							pv = item->AsObject()->at(L"folder");
							if (IsValidJsonBool(pv))
								file._folder = pv->AsBool()->GetBool();

							files.push_back(file);
						}

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

std::vector<std::pair<std::wstring, std::wstring>> GetJsonProperties(const NX::rapidjson::JsonValue* node, bool ignore_empty_value)
{
    std::vector<std::pair<std::wstring, std::wstring>>  props;

    if (node && node->IsArray()) {
        for (size_t i = 0; i < node->AsArray()->size(); ++i) {
            const std::pair<std::wstring, std::wstring>& prop = GetJsonProperty(node->AsArray()->at(i));
            if (prop.first.empty())
                continue;
            if (ignore_empty_value && prop.second.empty())
                continue;
            props.push_back(prop);
        }
    }

    return props;
}

std::pair<std::wstring, std::wstring> GetJsonProperty(const NX::rapidjson::JsonValue* node)
{
    if (node && node->IsObject()) {
        std::wstring sName;
        std::wstring sValue;
        const rapidjson::JsonValue* name = node->AsObject()->at(L"name");
        if (name && name->IsString()) {
            sName = name->AsString()->GetString();
        }
        if (!sName.empty()) {
            const rapidjson::JsonValue* value = node->AsObject()->at(L"value");
            if (value && value->IsString()) {
                sValue = value->AsString()->GetString();
            }
            return std::pair<std::wstring, std::wstring>(sName, sValue);
        }
    }

    return std::pair<std::wstring, std::wstring>();
}