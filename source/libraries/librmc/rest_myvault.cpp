
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
	
	class RequestDeleteFile : public REST::http::StringBodyRequest
	{
	public:
		RequestDeleteFile(const RmRestCommonParams& param, const std::wstring& duid, const std::wstring& filePath)
			: StringBodyRequest(REST::http::methods::POST,
				std::wstring(L"/rms/rs/myVault/" + duid + L"/delete"),
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
				BuildBody(filePath))
		{
		}
		virtual ~RequestDeleteFile() {}

	protected:
		std::string BuildBody(const std::wstring& filePath)
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
				parameters->set(L"pathId", JsonValue::CreateString(filePath));

				JsonStringWriter<char> writer;
				s = writer.Write(root.get());

			} while (FALSE);

			return s;
		}
	};
	
	static std::wstring BuildQueryUrl(
		const RmRestCommonParams& param,
		__int64 pageId,
		__int64 pageSize,
		const std::wstring& orderBy,
		const std::wstring& searchKeyWords,
		const std::wstring& searchFilter)
	{
		//   /rms/rs/myVault?page=1&size=10&orderBy=creationTime&filter=activeTransaction&q.fileName=document
		std::wstring s(L"/rms/rs/myVault?page=");
		s += NX::i64tos<wchar_t>(pageId);
		s += L"&size=";
		s += NX::i64tos<wchar_t>(pageSize);
		s += L"&orderBy=";
		s += (orderBy.empty() ? L"creationTime" : orderBy);
		s += L"&filter=";
		s += (searchFilter.empty() ? L"activeTransaction" : searchFilter);
		if (!searchKeyWords.empty()) {
			s += L"&q.fileName=";
			s += searchKeyWords;
		}
		return s;
	}

	class RequestQueryFiles : public REST::http::StringBodyRequest
	{
	public:
		RequestQueryFiles(const RmRestCommonParams& param,
			__int64 pageId,
			__int64 pageSize,
			const std::wstring& orderBy,
			const std::wstring& searchKeyWords,
			const std::wstring& searchFilter,
			_In_opt_ bool* cancelFlag)
			: StringBodyRequest(REST::http::methods::GET,
				BuildQueryUrl(param, pageId, pageSize, orderBy, searchKeyWords, searchFilter),
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
				std::string(), cancelFlag)
		{
		}
		virtual ~RequestQueryFiles() {}
	};

	class RequestGetFileMeta : public REST::http::StringBodyRequest
	{
	public:
		RequestGetFileMeta(const RmRestCommonParams& param, const std::wstring& duid, const std::wstring& filePath)
			: StringBodyRequest(REST::http::methods::POST,
				std::wstring(L"/rms/rs/myVault/" + duid + L"/metadata"),
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
				BuildBody(filePath))
		{
		}
		virtual ~RequestGetFileMeta() {}

	protected:
		std::string BuildBody(const std::wstring& filePath)
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
				parameters->set(L"pathId", JsonValue::CreateString(filePath));

				JsonStringWriter<char> writer;
				s = writer.Write(root.get());

			} while (FALSE);

			return s;
		}
	};
	
	class RequestDownloadFile : public REST::http::StringBodyRequest
	{
	public:
		RequestDownloadFile(const RmRestCommonParams& param, const std::wstring& source, bool forViewer, _In_opt_ bool* cancelFlag, const ULONG requestedLength)
			: StringBodyRequest(REST::http::methods::POST,
				std::wstring(L"/rms/rs/myVault/download"),
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
				BuildBody(source, forViewer), cancelFlag, requestedLength)
		{
		}
		virtual ~RequestDownloadFile() {}

	private:
		std::string BuildBody(const std::wstring& source, bool forViewer)
		{
			std::string s;

			std::shared_ptr<JsonValue> root(JsonValue::CreateObject());
			if (root == NULL)
				return s;

			JsonObject* params = JsonValue::CreateObject();
			if (params == NULL)
				return s;
			root->AsObject()->set(L"parameters", params);
			params->set(L"pathId", JsonValue::CreateString(source));
			params->set(L"forViewer", JsonValue::CreateBool(forViewer));

			NX::rapidjson::JsonStringWriter<char> writer;
			s = writer.Write(root.get());

			root.reset();
			return s;
		}
	};

	std::string MyVaultBuildUploadApiParameters(const std::wstring& pathId, const std::wstring& displayPath, const std::wstring& repoId, const std::wstring& repoName, const std::wstring& repoType)
	{
		std::shared_ptr<NX::rapidjson::JsonValue> root(NX::rapidjson::JsonValue::CreateObject());
		NX::rapidjson::JsonValue* params = NX::rapidjson::JsonValue::CreateObject();
		params->AsObject()->set(L"srcPathId", NX::rapidjson::JsonValue::CreateString(pathId));
		params->AsObject()->set(L"srcPathDisplay", NX::rapidjson::JsonValue::CreateString(displayPath));
		params->AsObject()->set(L"srcRepoId", NX::rapidjson::JsonValue::CreateString(repoId));
		params->AsObject()->set(L"srcRepoName", NX::rapidjson::JsonValue::CreateString(repoName));
		params->AsObject()->set(L"srcRepoType", NX::rapidjson::JsonValue::CreateString(repoType));
		root->AsObject()->set(L"parameters", params);
		NX::rapidjson::JsonStringWriter<char> writer;
		std::string s = writer.Write(root.get());
		root.reset();
		return s;
	}

}


RmMyVaultUploadResult::RmMyVaultUploadResult() : _lastModifiedTime(0), _size(0)
{
}

RmMyVaultUploadResult::RmMyVaultUploadResult(const RmMyVaultUploadResult& rhs)
	: _name(rhs._name),
	_duid(rhs._duid),
	_path(rhs._path),
	_displayPath(rhs._displayPath),
	_lastModifiedTime(rhs._lastModifiedTime),
	_size(rhs._size)
{
}

RmMyVaultUploadResult::~RmMyVaultUploadResult()
{
}

RmMyVaultUploadResult& RmMyVaultUploadResult::operator = (const RmMyVaultUploadResult& rhs)
{
	if (this != &rhs) {
		_name = rhs._name;
		_duid = rhs._duid;
		_path = rhs._path;
		_displayPath = rhs._displayPath;
		_lastModifiedTime = rhs._lastModifiedTime;
		_size = rhs._size;
	}
	return *this;
}

void RmMyVaultUploadResult::Clear()
{
	_name.clear();
	_duid.clear();
	_path.clear();
	_displayPath.clear();
	_lastModifiedTime = 0;
	_size = 0;
}



Result RmRestClient::MyVaultDeleteFile(const std::wstring& rms_url,
	const std::wstring& duid,
    const std::wstring& filePath,
    RmRestStatus& status)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(rms_url, res));
    if (p) {
        res = MyVaultDeleteFile(p.get(), duid, filePath, status);
    }
    return res;
}

Result RmRestClient::MyVaultDeleteFile(NX::REST::http::Connection* conn,
    const std::wstring& duid,
    const std::wstring& filePath,
    RmRestStatus& status)
{
	RequestDeleteFile   request(_commonParams, duid, filePath);
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

Result RmRestClient::MyVaultListFiles(const std::wstring& rms_url, _In_opt_ bool* cancelFlag,
    __int64 pageId,
    __int64 pageSize,
    const std::wstring& orderBy,
    const std::wstring& searchKeyWords,
    const std::wstring& searchFilter,
    RmRestStatus& status,
	_Out_opt_ __int64* total,
	std::vector<RmMyVaultFile>& files)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(rms_url, res));
    if (p) {
        res = MyVaultListFiles(p.get(), cancelFlag, pageId, pageSize, orderBy, searchKeyWords, searchFilter, status, total, files);
    }
    return res;
}

Result RmRestClient::MyVaultListFiles(NX::REST::http::Connection* conn, _In_opt_ bool* cancelFlag,
    __int64 pageId,
    __int64 pageSize,
    const std::wstring& orderBy,
    const std::wstring& searchKeyWords,
    const std::wstring& searchFilter,
    RmRestStatus& status,
	_Out_opt_ __int64* total,
	std::vector<RmMyVaultFile>& files)
{
	RequestQueryFiles   request(_commonParams, pageId, pageSize, orderBy, searchKeyWords, searchFilter, cancelFlag);
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
					PCJSONVALUE detail = results->AsObject()->at(L"detail");
					if (detail && detail->IsObject()) {
						PCJSONVALUE pv = NULL;
						if (total) {
							*total = 0;
							pv = detail->AsObject()->at(L"totalFiles");
							if (pv && pv->IsNumber()) {
								*total = pv->AsNumber()->ToInt64();
							}
						}
						pv = detail->AsObject()->at(L"files");
						if (pv && pv->IsArray()) {
							const size_t count = pv->AsArray()->size();
							for (size_t i = 0; i < count; i++) {
								RmMyVaultFile file;
								file.FromJsonValue(pv->AsArray()->at(i));
								if (!file.Empty())
									files.push_back(file);
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

Result RmRestClient::MyVaultGetFileMetadata(const std::wstring& rms_url,
	const std::wstring& duid,
    const std::wstring& filePath,
    RmRestStatus& status,
	RmMyVaultFileMetadata& metadata)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(rms_url, res));
    if (p) {
        res = MyVaultGetFileMetadata(p.get(), duid, filePath, status, metadata);
    }
    return res;
}

Result RmRestClient::MyVaultGetFileMetadata(NX::REST::http::Connection* conn,
	const std::wstring& duid,
    const std::wstring& filePath,
    RmRestStatus& status,
	RmMyVaultFileMetadata& metadata)
{
	RequestGetFileMeta   request(_commonParams, duid, filePath);
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
					PCJSONVALUE detail = results->AsObject()->at(L"detail");
					if (detail && detail->IsObject()) {
						metadata.FromJsonValue(detail);
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

Result RmRestClient::MyVaultDownloadFile(const std::wstring& rms_url,
	_In_opt_ bool* cancelFlag,
	const ULONG requestedLength,
    const std::wstring& source_path,
    const std::wstring& target_path,
    __int64 start_pos,
    __int64 download_size,
    bool forViewerOnly,
    RmRestStatus& status)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(rms_url, res));
    if (p) {
        res = MyVaultDownloadFile(p.get(), cancelFlag, requestedLength, source_path, target_path, start_pos, download_size, forViewerOnly, status);
    }
    return res;
}

Result RmRestClient::MyVaultDownloadFile(NX::REST::http::Connection* conn,
	_In_opt_ bool* cancelFlag,
	const ULONG requestedLength,
    const std::wstring& source_path,
    const std::wstring& target_path,
    __int64 start_pos,
    __int64 download_size,
    bool forViewerOnly,
    RmRestStatus& status)
{
	RequestDownloadFile request(_commonParams, source_path, forViewerOnly, cancelFlag, requestedLength);
	FileResponse  response(target_path);

	if (!response.is_open()) {
		return RESULT2(GetLastError(), "fail to open target file");
	}

	Result res = conn->SendRequest(request, response);
	response.Finish();
	if (!res) {
		return res;
	}

	status._status = response.GetStatus();
	status._phrase = response.GetPhrase();
	return RESULT(0);
}

Result RmRestClient::MyVaultUploadFile(const std::wstring& rms_url,
	_In_opt_ bool* cancelFlag,
	const std::wstring& file,
	const std::wstring& repoFilePath,
	const std::wstring& repoFilePathDisplay,
	const std::wstring& repoId,
	const std::wstring& repoName,
	const std::wstring& repoType,
    RmRestStatus& status,
	RmMyVaultUploadResult& result)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(rms_url, res));
    if (p) {
        res = MyVaultUploadFile(p.get(), cancelFlag, file, repoFilePath, repoFilePathDisplay, repoId, repoName, repoType, status, result);
    }
    return res;
}

Result RmRestClient::MyVaultUploadFile(NX::REST::http::Connection* conn,
	_In_opt_ bool* cancelFlag,
	const std::wstring& file,
	const std::wstring& repoFilePath,
	const std::wstring& repoFilePathDisplay,
	const std::wstring& repoId,
	const std::wstring& repoName,
	const std::wstring& repoType,
    RmRestStatus& status,
	RmMyVaultUploadResult& result)
{
	RmUploadRequest request(_commonParams, L"/rms/rs/myVault/upload", MyVaultBuildUploadApiParameters(repoFilePath, repoFilePathDisplay, repoId, repoName, repoType), file, cancelFlag);
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

					rapidjson::JsonValue* pv = results->AsObject()->at(L"name");
					if (pv && pv->IsString()) {
						result._name = pv->AsString()->GetString();
					}

					pv = results->AsObject()->at(L"pathId");
					if (pv && pv->IsString()) {
						result._path = pv->AsString()->GetString();
					}

					pv = results->AsObject()->at(L"duid");
					if (pv && pv->IsString()) {
						result._duid = pv->AsString()->GetString();
					}

					pv = results->AsObject()->at(L"pathDisplay");
					if (pv && pv->IsString()) {
						result._displayPath = pv->AsString()->GetString();
					}

					pv = results->AsObject()->at(L"lastModified");
					if (pv && pv->IsNumber()) {
						result._lastModifiedTime = pv->AsNumber()->ToInt64();
					}

					pv = results->AsObject()->at(L"size");
					if (pv && pv->IsNumber()) {
						result._size = pv->AsNumber()->ToInt64();
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
