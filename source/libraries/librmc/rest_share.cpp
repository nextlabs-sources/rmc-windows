
#include "stdafx.h"

#include <nx\rmc\rest.h>
#include <nx\nxl\nxlfile.h>

#include <nx\common\rapidjson.h>
#include <nx\common\stringex.h>
#include <nx\crypt\md5.h>


using namespace NX;
using namespace NX::rapidjson;
using namespace NX::REST::http;


namespace {

	class RequestShareRepoFile : public REST::http::StringBodyRequest
	{
	public:
		RequestShareRepoFile(const RmRestCommonParams& param,
			const std::wstring& membership,
			const std::vector<std::wstring>& recipients,
			const ULONGLONG permission,
			const __int64 expireTime,
			const std::wstring& repoId,
			const std::wstring& fileName,
			const std::wstring& filePath,
			const std::wstring& filePathId,
			const NXL::FileMeta& fileMetadata,
			const NXL::FileTags& fileTags)
			: StringBodyRequest(REST::http::methods::POST,
				std::wstring(L"/rms/rs/share/repository"),
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
				BuildBody(membership, recipients, permission, expireTime, repoId, fileName, filePath, filePathId, fileMetadata, fileTags))
		{
		}
		virtual ~RequestShareRepoFile() {}

	private:
		std::string BuildBody(
			const std::wstring& membership,
			const std::vector<std::wstring>& recipients,
			const ULONGLONG permission,
			const __int64 expireTime,
			const std::wstring& repoId,
			const std::wstring& fileName,
			const std::wstring& filePath,
			const std::wstring& filePathId,
			const NXL::FileMeta& fileMetadata,
			const NXL::FileTags& fileTags)
		{
			std::string s;
			std::shared_ptr<JsonObject> root(JsonValue::CreateObject());

			do {

				JsonObject* parameters = JsonValue::CreateObject();
				if (NULL == parameters)
					break;
				root->set(L"parameters", parameters);
				parameters->set(L"asAttachment", JsonValue::CreateString(L"false"));

				JsonObject* sharedDocument = JsonValue::CreateObject();
				if (NULL == sharedDocument)
					break;
				parameters->set(L"sharedDocument", sharedDocument);				
				sharedDocument->set(L"membershipId", JsonValue::CreateString(membership));
				sharedDocument->set(L"permissions", JsonValue::CreateNumber(permission));
				sharedDocument->set(L"metadata", JsonValue::CreateString(fileMetadata.ToString()));
				sharedDocument->set(L"expireTime", JsonValue::CreateNumber(expireTime));
				sharedDocument->set(L"fileName", JsonValue::CreateString(fileName));
				sharedDocument->set(L"repositoryId", JsonValue::CreateString(repoId));
				sharedDocument->set(L"filePathId", JsonValue::CreateString(filePathId));
				sharedDocument->set(L"filePath", JsonValue::CreateString(filePath));

				PJSONVALUE tags = BuildTags(fileTags);
				if (tags) {
					sharedDocument->set(L"tags", tags);
				}

				PJSONVALUE recipientsArray = BuildRecipients(recipients);
				if (recipientsArray) {
					sharedDocument->set(L"recipients", recipientsArray);
				}

			} while (FALSE);

			NX::rapidjson::JsonStringWriter<char> writer;
			s = writer.Write(root.get());
			root.reset();
			return s;
		}

		PJSONVALUE BuildTags(const NXL::FileTags& fileTags)
		{
			JsonObject* tags = JsonValue::CreateObject();
			if (tags == NULL)
				return NULL;

			for (int i = 0; i < (int)fileTags.GetTags().size(); i++) {
				JsonArray* tagValueArray = JsonValue::CreateArray();
				if (NULL == tagValueArray)
					continue;

				tags->set(fileTags.GetTags()[i].first, tagValueArray);
				const NXL::FileTags::FileTagValues& values = fileTags.GetTags()[i].second;
				std::for_each(values.begin(), values.end(), [&](const std::wstring& item) {
					tagValueArray->push_back(JsonValue::CreateString(item));
				});
			}
			return tags;
		}

		PJSONVALUE BuildRecipients(const std::vector<std::wstring>& recipients)
		{
			JsonArray* recipientsArray = JsonValue::CreateArray();
			if (recipientsArray == NULL)
				return NULL;

			std::for_each(recipients.begin(), recipients.end(), [&](const std::wstring& item) {
				PJSONOBJECT recipient = JsonValue::CreateObject();
				if (NULL == recipient)
					return;
				recipient->set(L"email", JsonValue::CreateString(item));
				recipientsArray->push_back(recipient);
			});
			return recipientsArray;
		}
	};


	class RequestShareLocalFile : public RmUploadRequest
	{
	public:
		RequestShareLocalFile(const RmRestCommonParams& commonParams,
			const std::wstring& file,
			const std::wstring& membership,
			const std::vector<std::wstring>& recipients,
			const ULONGLONG permission,
			const __int64 expireTime,
			const std::wstring& fileName,
			const NXL::FileMeta& fileMetadata,
			const NXL::FileTags& fileTags,
			_In_opt_ bool* cancelFlag)
			: RmUploadRequest(commonParams,
				std::wstring(L"/rms/rs/share/local"),
				BuildBody(membership, recipients, permission, expireTime, fileName, fileMetadata, fileTags),
				file, cancelFlag)
		{
		}
		virtual ~RequestShareLocalFile() {}

	private:
		std::string BuildBody(
			const std::wstring& membership,
			const std::vector<std::wstring>& recipients,
			const ULONGLONG permission,
			const __int64 expireTime,
			const std::wstring& fileName,
			const NXL::FileMeta& fileMetadata,
			const NXL::FileTags& fileTags)
		{
			std::string s;
			std::shared_ptr<JsonObject> root(JsonValue::CreateObject());

			do {

				JsonObject* parameters = JsonValue::CreateObject();
				if (NULL == parameters)
					break;
				root->set(L"parameters", parameters);
				parameters->set(L"asAttachment", JsonValue::CreateString(L"false"));

				JsonObject* sharedDocument = JsonValue::CreateObject();
				if (NULL == sharedDocument)
					break;
				parameters->set(L"sharedDocument", sharedDocument);
				sharedDocument->set(L"membershipId", JsonValue::CreateString(membership));
				sharedDocument->set(L"permissions", JsonValue::CreateNumber(permission));
				sharedDocument->set(L"metadata", JsonValue::CreateString(fileMetadata.ToString()));
				sharedDocument->set(L"expireTime", JsonValue::CreateNumber(expireTime));
				sharedDocument->set(L"fileName", JsonValue::CreateString(fileName));
				sharedDocument->set(L"filePathId", JsonValue::CreateString(fileName));
				sharedDocument->set(L"filePath", JsonValue::CreateString(fileName));

				PJSONVALUE tags = BuildTags(fileTags);
				if (tags) {
					sharedDocument->set(L"tags", tags);
				}

				PJSONVALUE recipientsArray = BuildRecipients(recipients);
				if (recipientsArray) {
					sharedDocument->set(L"recipients", recipientsArray);
				}

			} while (FALSE);

			NX::rapidjson::JsonStringWriter<char> writer;
			s = writer.Write(root.get());
			root.reset();

			return s;
		}

		PJSONVALUE BuildTags(const NXL::FileTags& fileTags)
		{
			JsonObject* tags = JsonValue::CreateObject();
			if (tags == NULL)
				return NULL;

			for (int i = 0; i < (int)fileTags.GetTags().size(); i++) {
				JsonArray* tagValueArray = JsonValue::CreateArray();
				if (NULL == tagValueArray)
					continue;

				tags->set(fileTags.GetTags()[i].first, tagValueArray);
				const NXL::FileTags::FileTagValues& values = fileTags.GetTags()[i].second;
				std::for_each(values.begin(), values.end(), [&](const std::wstring& item) {
					tagValueArray->push_back(JsonValue::CreateString(item));
				});
			}
			return tags;
		}

		PJSONVALUE BuildRecipients(const std::vector<std::wstring>& recipients)
		{
			JsonArray* recipientsArray = JsonValue::CreateArray();
			if (recipientsArray == NULL)
				return NULL;

			std::for_each(recipients.begin(), recipients.end(), [&](const std::wstring& item) {
				PJSONOBJECT recipient = JsonValue::CreateObject();
				if (NULL == recipient)
					return;
				recipient->set(L"email", JsonValue::CreateString(item));
				recipientsArray->push_back(recipient);
			});
			return recipientsArray;
		}
	};

	class RequestUpdateRecipients : public REST::http::StringBodyRequest
	{
	public:
		RequestUpdateRecipients( const RmRestCommonParams& param,
			const std::wstring& duid,
			const std::vector<std::wstring>& addRecipients,
			const std::vector<std::wstring>& removeRecipients)
			: StringBodyRequest(REST::http::methods::POST,
				std::wstring(L"/rms/rs/share/" + duid + L"/update"),
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
				BuildBody(addRecipients, removeRecipients))
		{
		}
		virtual ~RequestUpdateRecipients() {}

	private:
		std::string BuildBody(const std::vector<std::wstring>& addRecipients, const std::vector<std::wstring>& removeRecipients)
		{
			std::string s("{\"parameters\":{\"newRecipients\":[");

			for (int i = 0; i < (int)addRecipients.size(); i++) {
				if (0 != i) s.append(",");
				s.append("{\"email\":\"");
				s.append(NX::conv::utf16toutf8(addRecipients[i]));
				s.append("\"}");
			}
			s.append("],\"removedRecipients\":[");

			for (int i = 0; i < (int)removeRecipients.size(); i++) {
				if (0 != i) s.append(",");
				s.append("{\"email\":\"");
				s.append(NX::conv::utf16toutf8(removeRecipients[i]));
				s.append("\"}");
			}
			s.append("]}}");

			return s;
		}
	};

	class RequestRevokeDocument : public REST::http::StringBodyRequest
	{
	public:
		RequestRevokeDocument(const RmRestCommonParams& param, const std::wstring& duid)
			: StringBodyRequest(REST::http::methods::DEL,
				std::wstring(L"/rms/rs/share/" + duid + L"/revoke"),
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
		virtual ~RequestRevokeDocument() {}
	};


	class RequestSharedWithMe : public REST::http::StringBodyRequest
	{
	public:
		RequestSharedWithMe(const RmRestCommonParams& param,
			_In_opt_ bool* cancelFlag,
			__int64 pageId,
			__int64 pageSize,
			const std::wstring& orderBy,
			const std::wstring& searchKeyWords)
			: StringBodyRequest(REST::http::methods::GET,
				std::wstring(BuildUrl(pageId, pageSize, orderBy, searchKeyWords)),
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
		virtual ~RequestSharedWithMe() {}

	private:
		std::wstring BuildUrl(
			__int64 pageId,
			__int64 pageSize,
			const std::wstring& orderBy,
			const std::wstring& keyWords)
		{
			std::wstring s(L"/rms/rs/sharedWithMe/list?page=");
			s.append(NX::i64tos<wchar_t>(pageId));
			s.append(L"&size=");
			s.append(NX::i64tos<wchar_t>(pageSize));
			if (!orderBy.empty()) {
				s.append(L"&orderBy=");
				s.append(orderBy);
			}
			if (!keyWords.empty()) {
				s.append(L"&q=name&searchString=");
				s.append(keyWords);
			}

			return s;
		}
	};

}


RmFileShareResult::RmFileShareResult()
{
}

RmFileShareResult::RmFileShareResult(const RmFileShareResult& rhs)
	: _name(rhs._name), _duid(rhs._duid), _path(rhs._path), _transactionId(rhs._transactionId)
{
}

RmFileShareResult::~RmFileShareResult()
{
}

RmFileShareResult& RmFileShareResult::operator = (const RmFileShareResult& rhs)
{
	if (this != &rhs) {
		_name = rhs._name;
		_duid = rhs._duid;
		_path = rhs._path;
		_transactionId = rhs._transactionId;
	}
	return *this;
}

void RmFileShareResult::Clear()
{
	_name.clear();
	_duid.clear();
	_path.clear();
	_transactionId.clear();
}



RmSharedWithMeFile::RmSharedWithMeFile() : _sharedDate(0), _size(0)
{
}

RmSharedWithMeFile::RmSharedWithMeFile(const RmSharedWithMeFile& rhs) :
		_duid(rhs._duid),
		_name(rhs._name),
		_sharedBy(rhs._sharedBy),
		_sharedLink(rhs._sharedLink),
		_sharedDate(rhs._sharedDate),
		_size(rhs._size)
{
}

RmSharedWithMeFile::~RmSharedWithMeFile()
{
}

RmSharedWithMeFile& RmSharedWithMeFile::operator = (const RmSharedWithMeFile& rhs)
{
	if (this != &rhs) {
		_duid = rhs._duid;
		_name = rhs._name;
		_sharedBy = rhs._sharedBy;
		_sharedLink = rhs._sharedLink;
		_sharedDate = rhs._sharedDate;
		_size = rhs._size;
	}
	return *this;
}

void RmSharedWithMeFile::Clear()
{
	_duid.clear();
	_name.clear();
	_sharedBy.clear();
	_sharedLink.clear();
	_sharedDate = 0;
	_size = 0;
}



Result RmRestClient::ShareRepositoryFile(const std::wstring& rms_url,
    const std::wstring& membership,
    const std::vector<std::wstring>& recipients,
    const ULONGLONG permission,
    const __int64 expireTime,
    const std::wstring& repoId,
    const std::wstring& fileName,
    const std::wstring& filePath,
    const std::wstring& filePathId,
	const NXL::FileMeta& fileMetadata,
	const NXL::FileTags& fileTags,
    RmRestStatus& status,
	RmFileShareResult& sr)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(rms_url, res));
    if (p) {
        res = ShareRepositoryFile(p.get(), membership, recipients, permission, expireTime, repoId, fileName, filePath, filePathId, fileMetadata, fileTags, status, sr);
    }
    return res;
}

Result RmRestClient::ShareRepositoryFile(NX::REST::http::Connection* conn,
    const std::wstring& membership,
    const std::vector<std::wstring>& recipients,
    const ULONGLONG permission,
    const __int64 expireTime,
    const std::wstring& repoId,
    const std::wstring& fileName,
    const std::wstring& filePath,
    const std::wstring& filePathId,
	const NXL::FileMeta& fileMetadata,
	const NXL::FileTags& fileTags,
    RmRestStatus& status,
	RmFileShareResult& sr)
{
	RequestShareRepoFile request(_commonParams, membership, recipients, permission, expireTime, repoId, fileName, filePath, filePathId, fileMetadata, fileTags);
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
					PCJSONVALUE pv = results->AsObject()->at(L"fileName");
					if (IsValidJsonString(pv))
						sr._name = pv->AsString()->GetString();
					pv = results->AsObject()->at(L"duid");
					if (IsValidJsonString(pv))
						sr._duid = pv->AsString()->GetString();
					pv = results->AsObject()->at(L"filePathId");
					if (IsValidJsonString(pv))
						sr._path = pv->AsString()->GetString();
					pv = results->AsObject()->at(L"transactionId");
					if (IsValidJsonString(pv))
						sr._transactionId = pv->AsString()->GetString();
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

Result RmRestClient::ShareLocalFile(const std::wstring& rms_url,
    const std::wstring& membership,
    const std::vector<std::wstring>& recipients,
    const ULONGLONG permission,
    const __int64 expireTime,
    const std::wstring& filePath,
	const std::wstring& displayPath,
	const NXL::FileMeta& fileMetadata,
	const NXL::FileTags& fileTags,
    RmRestStatus& status,
	RmFileShareResult& sr,
	_In_opt_ bool* cancelFlag)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(rms_url, res));
    if (p) {
        res = ShareLocalFile(p.get(), membership, recipients, permission, expireTime, filePath, displayPath, fileMetadata, fileTags, status, sr, cancelFlag);
    }
    return res;
}

Result RmRestClient::ShareLocalFile(NX::REST::http::Connection* conn,
    const std::wstring& membership,
    const std::vector<std::wstring>& recipients,
    const ULONGLONG permission,
    const __int64 expireTime,
    const std::wstring& filePath,
	const std::wstring& displayPath,
	const NXL::FileMeta& fileMetadata,
	const NXL::FileTags& fileTags,
    RmRestStatus& status,
	RmFileShareResult& sr,
	_In_opt_ bool* cancelFlag)
{
	RequestShareLocalFile request(_commonParams, filePath, membership, recipients, permission, expireTime, displayPath.empty() ? filePath : displayPath, fileMetadata, fileTags, cancelFlag);
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
					PCJSONVALUE pv = results->AsObject()->at(L"fileName");
					if (IsValidJsonString(pv))
						sr._name = pv->AsString()->GetString();
					pv = results->AsObject()->at(L"duid");
					if (IsValidJsonString(pv))
						sr._duid = pv->AsString()->GetString();
					pv = results->AsObject()->at(L"filePathId");
					if (IsValidJsonString(pv))
						sr._path = pv->AsString()->GetString();
					pv = results->AsObject()->at(L"transactionId");
					if (IsValidJsonString(pv))
						sr._transactionId = pv->AsString()->GetString();
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

Result RmRestClient::ShareChangeRecipients(const std::wstring& rms_url,
    const std::wstring& duid,
    const std::vector<std::wstring>& addRecipients,
    const std::vector<std::wstring>& removeRecipients,
    RmRestStatus& status,
    std::vector<std::wstring>& addedRecipients,
    std::vector<std::wstring>& removedRecipients)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(rms_url, res));
    if (p) {
        res = ShareChangeRecipients(p.get(), duid, addRecipients, removeRecipients, status, addedRecipients, removedRecipients);
    }
    return res;
}

Result RmRestClient::ShareChangeRecipients(NX::REST::http::Connection* conn,
    const std::wstring& duid,
    const std::vector<std::wstring>& addRecipients,
    const std::vector<std::wstring>& removeRecipients,
    RmRestStatus& status,
    std::vector<std::wstring>& addedRecipients,
    std::vector<std::wstring>& removedRecipients)
{
	RequestUpdateRecipients request(_commonParams, duid, addRecipients, removeRecipients);
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
					// Get recipients added into document
					PCJSONVALUE pv = results->AsObject()->at(L"newRecipients");
					if (pv && pv->IsArray()) {
						const size_t count = pv->AsArray()->size();
						for (size_t i = 0; i < count; i++) {
							PCJSONVALUE item = pv->AsArray()->at(i);
							if (item && item->IsString()) {
								const std::wstring& recipient = item->AsString()->GetString();
								if (!recipient.empty())
									addedRecipients.push_back(recipient);
							}
						}
					}

					// Get recipients removed from document
					pv = results->AsObject()->at(L"removedRecipients");
					if (pv && pv->IsArray()) {
						const size_t count = pv->AsArray()->size();
						for (size_t i = 0; i < count; i++) {
							PCJSONVALUE item = pv->AsArray()->at(i);
							if (item && item->IsString()) {
								const std::wstring& recipient = item->AsString()->GetString();
								if (!recipient.empty())
									removedRecipients.push_back(recipient);
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

Result RmRestClient::ShareRevoke(const std::wstring& rms_url,
    const std::wstring& duid,
    RmRestStatus& status)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(rms_url, res));
    if (p) {
        res = ShareRevoke(p.get(), duid, status);
    }
    return res;
}

Result RmRestClient::ShareRevoke(NX::REST::http::Connection* conn,
    const std::wstring& duid,
    RmRestStatus& status)
{
	RequestRevokeDocument request(_commonParams, duid);
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

Result RmRestClient::ListSharedWithMeFiles(const std::wstring& rms_url,
	_In_opt_ bool* cancelFlag,
	__int64 pageId,
	__int64 pageSize,
	const std::wstring& orderBy,
	const std::wstring& keyWords,
	_Out_opt_ __int64* totalRecords,
	RmRestStatus& status,
	std::vector<RmSharedWithMeFile>& files)
{
	Result res = RESULT(0);
	std::shared_ptr<Connection> p(Connect(rms_url, res));
	if (p) {
		res = ListSharedWithMeFiles(p.get(), cancelFlag, pageId, pageSize, orderBy, keyWords, totalRecords, status, files);
	}
	return res;
}

Result RmRestClient::ListSharedWithMeFiles(NX::REST::http::Connection* conn,
	_In_opt_ bool* cancelFlag,
	__int64 pageId,
	__int64 pageSize,
	const std::wstring& orderBy,
	const std::wstring& keyWords,
	_Out_opt_ __int64* totalRecords,
	RmRestStatus& status,
	std::vector<RmSharedWithMeFile>& files)
{
	RequestSharedWithMe request(_commonParams, cancelFlag, pageId, pageSize, orderBy, keyWords);
	StringResponse  response;

	if (NULL != totalRecords) {
		*totalRecords = 0;
	}

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
				
					PCJSONVALUE details = results->AsObject()->at(L"detail");
					if (IsValidJsonObject(details)) {

						// Get total files
						if (NULL != totalRecords) {
							PCJSONVALUE pTotalRecords = details->AsObject()->at(L"totalFiles");
							if (IsValidJsonNumber(pTotalRecords))
								*totalRecords = pTotalRecords->AsNumber()->ToInt64();
						}

						PCJSONVALUE pFiles = details->AsObject()->at(L"files");
						if (IsValidJsonArray(pFiles)) {

							const size_t count = pFiles->AsArray()->size();
							for (size_t i = 0; i < count; i++) {
								PCJSONVALUE item = pFiles->AsArray()->at(i);
								if (IsValidJsonObject(item)) {
									RmSharedWithMeFile file;
									PCJSONVALUE pv = item->AsObject()->at(L"duid");
									if (!IsValidJsonString(pv))
										continue;
									file._duid = pv->AsString()->GetString();
									pv = item->AsObject()->at(L"name");
									if (!IsValidJsonString(pv))
										continue;
									file._name = pv->AsString()->GetString();
									pv = item->AsObject()->at(L"sharedBy");
									if (!IsValidJsonString(pv))
										continue;
									file._sharedBy = pv->AsString()->GetString();
									pv = item->AsObject()->at(L"sharedLink");
									if (!IsValidJsonString(pv))
										continue;
									file._sharedLink = pv->AsString()->GetString();
									pv = item->AsObject()->at(L"sharedDate");
									if (!IsValidJsonNumber(pv))
										continue;
									file._sharedDate = pv->AsNumber()->ToInt64();
									pv = item->AsObject()->at(L"size");
									if (!IsValidJsonNumber(pv))
										continue;
									file._size = pv->AsNumber()->ToInt64();
									files.push_back(file);
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
