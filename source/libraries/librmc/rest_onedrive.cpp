
#include "stdafx.h"

#include <nx\rmc\rest.h>
#include <nx\rmc\repo.h>

#include <nx\common\rapidjson.h>
#include <nx\common\time.h>


using namespace NX;
using namespace NX::rapidjson;
using namespace NX::REST::http;

static const std::string boundary("7e1c41151168");
static const std::string boundaryBegin("--" + boundary);
static const std::string boundaryEnding("\r\n--" + boundary + "--");

static const std::wstring oneDriveApiUrl    {L"https://api.onedrive.com"};

namespace {

    class RequestGetFileInfo : public REST::http::StringBodyRequest
    {
    public:
        RequestGetFileInfo(const std::wstring &accessToken, const std::wstring& fileOdId)
            : StringBodyRequest(REST::http::methods::GET,
                std::wstring(L"/v1.0/drive/items/" + fileOdId),
                REST::http::HttpHeaders({
                    std::pair<std::wstring, std::wstring>(L"Authorization", L"Bearer " + accessToken)
                }),
                std::vector<LPCWSTR>({ REST::http::mime_types::application_json.c_str(), NULL }),
                std::string())
        {
        }
        virtual ~RequestGetFileInfo() {}
    };

    class RequestListOrSearchFiles : public REST::http::StringBodyRequest
    {
    public:
        RequestListOrSearchFiles(const std::wstring &accessToken, const std::wstring& folderOdId, const std::wstring& keywords, _In_opt_ bool* cancelFlag)
            : StringBodyRequest(REST::http::methods::GET,
                std::wstring(L"/v1.0/drive/" + (folderOdId == L"/" ? L"root" : L"items/" + folderOdId) + (keywords.empty() ? L"/children" : L"/oneDrive.search(q='" + keywords + L"')?filter=contains(tolower(name),'" + NX::duplower<wchar_t>(keywords) + L"')")),
                REST::http::HttpHeaders({
                    std::pair<std::wstring, std::wstring>(L"Authorization", L"Bearer " + accessToken)
                }),
                std::vector<LPCWSTR>({ REST::http::mime_types::application_json.c_str(), NULL }),
                std::string(), cancelFlag)
        {
        }
        virtual ~RequestListOrSearchFiles() {}
    };

    class RequestCreateFolder : public REST::http::StringBodyRequest
    {
    public:
        RequestCreateFolder(const std::wstring &accessToken, const std::wstring& parentFolderOdId, const std::wstring& folderName)
            : StringBodyRequest(REST::http::methods::POST,
                std::wstring(L"/v1.0/drive/" + (parentFolderOdId == L"/" ? L"root" : L"items/" + parentFolderOdId) + L"/children"),
                REST::http::HttpHeaders({
                    std::pair<std::wstring, std::wstring>(L"Authorization", L"Bearer " + accessToken),
                    std::pair<std::wstring, std::wstring>(REST::http::header_names::content_type, REST::http::mime_types::application_json)
                }),
                std::vector<LPCWSTR>({ REST::http::mime_types::application_json.c_str(), NULL }),
                BuildBody(folderName))
        {
        }
        virtual ~RequestCreateFolder() {}

    private:
        std::string BuildBody(const std::wstring& folderName)
        {
            std::string s;
            std::shared_ptr<JsonObject> root(NX::rapidjson::JsonValue::CreateObject());
            if (root == nullptr)
                return s;
            JsonString* ps = JsonValue::CreateString(folderName);
            if (ps == nullptr)
                return s;
            root->set(L"name", ps);
            JsonObject* po = JsonValue::CreateObject();
            if (po == nullptr)
                return s;
            root->set(L"folder", po);

            JsonStringWriter<char> writer;
            s = writer.Write(root.get());
            root.reset();
            return s;
        }
    };

    class RequestDelete : public REST::http::StringBodyRequest
    {
    public:
        RequestDelete(const std::wstring &accessToken, const std::wstring& itemOdId)
            : StringBodyRequest(REST::http::methods::DEL,
                std::wstring(L"/v1.0/drive/items/" + itemOdId),
                REST::http::HttpHeaders({
                    std::pair<std::wstring, std::wstring>(L"Authorization", L"Bearer " + accessToken)
                }),
                std::vector<LPCWSTR>({ REST::http::mime_types::application_json.c_str(), NULL }),
                std::string())
        {
        }
        virtual ~RequestDelete() {}
    };

    class RequestDownloadFile : public REST::http::StringBodyRequest
    {
    public:
        RequestDownloadFile(const std::wstring &accessToken, const std::wstring& fileOdId, _In_opt_ bool* cancelFlag, const ULONG requestedLength)
            : StringBodyRequest(REST::http::methods::GET,
                std::wstring(L"/v1.0/drive/items/" + fileOdId + L"/content"),
                REST::http::HttpHeaders({
                    std::pair<std::wstring, std::wstring>(L"Authorization", L"Bearer " + accessToken)
                }),
                std::vector<LPCWSTR>({ REST::http::mime_types::text.c_str(), NULL }),
                std::string(),
				cancelFlag, requestedLength)
        {
        }
        virtual ~RequestDownloadFile() {}
    };

#if 0///multipart not working yet.
    class RequestUploadFile : public REST::http::Request
    {
    public:
        RequestUploadFile(const std::wstring& accessToken, const std::wstring& path, const std::wstring& repoParentPath)
            : NX::REST::http::Request(REST::http::methods::POST,
                std::wstring(L"/v1.0/drive/root" + (repoParentPath == L"/" ? L"" : L':' + repoParentPath + L':') + L"/children"),
                REST::http::HttpHeaders({
                    std::pair<std::wstring, std::wstring>(L"Authorization", L"Bearer " + accessToken),
                    std::pair<std::wstring, std::wstring>(NX::REST::http::header_names::content_type, std::wstring(L"multipart/related; boundary=" + std::wstring(boundary.begin(), boundary.end())))
                }),
                std::vector<LPCWSTR>({ REST::http::mime_types::application_json.c_str(), NULL })),
            _buf(path), _is(&_buf)
        {
            SetBodyLength((ULONG)_buf.GetTotalDataLength());
        }
        virtual ~RequestUploadFile() {}

        virtual ULONG GetRemainBodyLength()
        {
            return (ULONG)_buf.GetRemainDataLength();
        }
        virtual std::istream& GetBodyStream() { return _is; }

    protected:
        std::istream    _is;
        RmUploadBuffer  _buf;
    };
#endif

    class RequestUploadFile : public REST::http::Request
    {
    public:
        RequestUploadFile(_In_opt_ bool* cancelFlag, const std::wstring& accessToken, const std::wstring& parentFolderOdId, const std::wstring& repoFileName, const std::wstring& path)
            : NX::REST::http::Request(REST::http::methods::PUT,
                std::wstring(L"/v1.0/drive/" + (parentFolderOdId == L"/" ? L"root:/" + repoFileName : L"items/" + parentFolderOdId + L":/" + repoFileName) + L":/content"),
                REST::http::HttpHeaders({
                    std::pair<std::wstring, std::wstring>(NX::REST::http::header_names::content_type, REST::http::mime_types::application_octetstream),
                    std::pair<std::wstring, std::wstring>(L"Authorization", L"Bearer " + accessToken)
                }),
                std::vector<LPCWSTR>({ REST::http::mime_types::application_json.c_str(), NULL }), cancelFlag),
            _buf(path), _is(&_buf)
        {
            SetBodyLength((ULONG)_buf.GetTotalDataLength());
        }
        virtual ~RequestUploadFile() {}

        virtual ULONG GetRemainBodyLength()
        {
            return (ULONG)_buf.GetRemainDataLength();
        }
        virtual std::istream& GetBodyStream() { return _is; }

		virtual  bool IsUpload() const { return false; }

    protected:
        std::istream            _is;
        RmSimpleUploadBuffer    _buf;
    };

    std::string OneDriveBuildUploadApiParameters(const std::wstring& name, const FILETIME *lastModified)
    {
        std::string s;
        std::shared_ptr<JsonObject> root(NX::rapidjson::JsonValue::CreateObject());
        if (root == nullptr)
            return s;
        JsonString* pv = JsonValue::CreateString(name);
        if (pv == nullptr)
            return s;
        root->set(L"name", pv);

        if (lastModified) {
            pv = JsonValue::CreateString(NX::time::datetime(lastModified).serialize());
            if (pv == nullptr)
                return s;
            root->set(L"lastModifiedDateTime", pv);
        }

        JsonStringWriter<char> writer;
        s = writer.Write(root.get());
        root.reset();
        return s;
    }

}

void RmRestClient::OneDriveMetadataToRepoFile(NX::rapidjson::PCJSONOBJECT fileObject,
    const std::wstring parentFolderId,
    RmRepoFile& repoFile)
{
    const rapidjson::JsonValue* name = fileObject->AsObject()->at(L"name");
    if (name && name->IsString())
        repoFile._name = name->AsString()->GetString();
    const rapidjson::JsonValue* id = fileObject->AsObject()->at(L"id");
    if (id && id->IsString())
        repoFile._id = (parentFolderId == L"/" ? L"" : parentFolderId) + L'/' + id->AsString()->GetString();
    const rapidjson::JsonValue* parent = fileObject->AsObject()->at(L"parentReference");
    if (parent && parent->IsObject()) {
        const rapidjson::JsonValue* parentPath = parent->AsObject()->at(L"path");
        if (parentPath && parentPath->IsString()) {
            std::wstring parentPathStr = parentPath->AsString()->GetString().substr(std::wstring(L"/drive/root:").length());
			parentPathStr = NX::conv::to_wstring(NX::conv::UrlDecode(NX::conv::to_string(parentPathStr)));
            repoFile._path = parentPathStr + L'/' + repoFile._name;
        }
    }
    const rapidjson::JsonValue* folder = fileObject->AsObject()->at(L"folder");
    if (folder && folder->IsObject())
        repoFile._folder = true;
    const rapidjson::JsonValue* fsInfo = fileObject->AsObject()->at(L"fileSystemInfo");
    if (fsInfo && fsInfo->IsObject()) {
        const rapidjson::JsonValue* lastModified = fsInfo->AsObject()->at(L"lastModifiedDateTime");
        if (lastModified && lastModified->IsString())
            repoFile._time = NX::time::datetime(lastModified->AsString()->GetString()).to_java_time();
    }
    const rapidjson::JsonValue* fileSize = fileObject->AsObject()->at(L"size");
    if (fileSize && fileSize->IsNumber())
        repoFile._size = fileSize->AsNumber()->ToInt64();
}

Result RmRestClient::OneDriveGetFileInfo(const std::wstring& accessToken,
    const std::wstring& fileId,
    RmRestStatus& status,
    RmRepoFile& file)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(oneDriveApiUrl, res));
    if (p) {
        res = OneDriveGetFileInfo(p.get(), accessToken, fileId, status, file);
    }
    return res;
}

Result RmRestClient::OneDriveGetFileInfo(NX::REST::http::Connection* conn,
    const std::wstring& accessToken,
    const std::wstring& fileId,
    RmRestStatus& status,
    RmRepoFile& file)
{
    std::wstring parentFolderId;
    const std::wstring fileOdId = NX::remove_tail<wchar_t, L'/'>(fileId, parentFolderId);
    if (parentFolderId.empty()) {
        parentFolderId = L"/";
    }

    RequestGetFileInfo request(accessToken, fileOdId);
    StringResponse response;

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
            OneDriveMetadataToRepoFile(root->AsObject(), parentFolderId, file);
        }
        catch (...) {
            // The JSON data is NOT correct
            res = RESULT2(ERROR_INVALID_DATA, "JSON response is not correct");
        }

    } while (FALSE);


    return RESULT(0);
}

Result RmRestClient::OneDriveListOrSearchFiles(_In_opt_ bool* cancelFlag, const std::wstring& accessToken,
    const std::wstring& folderId,
    const std::wstring& keywords,
    RmRestStatus& status,
    std::vector<RmRepoFile>& files)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(oneDriveApiUrl, res));
    if (p) {
        res = OneDriveListOrSearchFiles(p.get(), cancelFlag, accessToken, folderId, keywords, status, files);
    }
    return res;
}

Result RmRestClient::OneDriveListOrSearchFiles(NX::REST::http::Connection* conn, _In_opt_ bool* cancelFlag,
    const std::wstring& accessToken,
    const std::wstring& folderId,
    const std::wstring& keywords,
    RmRestStatus& status,
    std::vector<RmRepoFile>& files)
{
    std::wstring folderOdId;

    if (folderId == L"/") {
        folderOdId = L"/";
    } else {
        std::wstring tmp;
        folderOdId = NX::remove_tail<wchar_t, L'/'>(folderId, tmp);
    }

    RequestListOrSearchFiles request(accessToken, folderOdId, keywords, cancelFlag);
    StringResponse response;

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
            const rapidjson::JsonValue* value = root->AsObject()->at(L"value");
            if (value && value->IsArray()) {
                const int count = (int)value->AsArray()->size();
                for (int i = 0; i < count; i++) {
                    RmRepoFile repoFile;
                    const rapidjson::JsonValue* fileObject = value->AsArray()->at(i);
                    assert(NULL != fileObject);
                    OneDriveMetadataToRepoFile(fileObject->AsObject(), folderId, repoFile);
                    files.push_back(repoFile);
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

Result RmRestClient::OneDriveCreateFolder(const std::wstring& accessToken,
    const std::wstring& parentFolderId,
    const std::wstring& folderName,
    RmRestStatus& status,
    RmRepoFile& folder)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(oneDriveApiUrl, res));
    if (p) {
        res = OneDriveCreateFolder(p.get(), accessToken, parentFolderId, folderName, status, folder);
    }
    return res;
}

Result RmRestClient::OneDriveCreateFolder(NX::REST::http::Connection* conn,
    const std::wstring& accessToken,
    const std::wstring& parentFolderId,
    const std::wstring& folderName,
    RmRestStatus& status,
    RmRepoFile& folder)
{
    std::wstring parentFolderOdId;

    if (parentFolderId == L"/") {
        parentFolderOdId = L"/";
    } else {
        std::wstring tmp;
        parentFolderOdId = NX::remove_tail<wchar_t, L'/'>(parentFolderId, tmp);
    }

    RequestCreateFolder request(accessToken, parentFolderOdId, folderName);
    StringResponse response;

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

        if (status.GetStatus() != status_codes::Created.id) {
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
            OneDriveMetadataToRepoFile(root->AsObject(), parentFolderId, folder);
        }
        catch (...) {
            // The JSON data is NOT correct
            res = RESULT2(ERROR_INVALID_DATA, "JSON response is not correct");
        }

    } while (FALSE);


    return RESULT(0);
}

Result RmRestClient::OneDriveDelete(const std::wstring& accessToken,
    const std::wstring& itemId,
    RmRestStatus& status)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(oneDriveApiUrl, res));
    if (p) {
        res = OneDriveDelete(p.get(), accessToken, itemId, status);
    }
    return res;
}

Result RmRestClient::OneDriveDelete(NX::REST::http::Connection* conn,
    const std::wstring& accessToken,
    const std::wstring& itemId,
    RmRestStatus& status)
{
    std::wstring tmp;
    const std::wstring itemOdId = NX::remove_tail<wchar_t, L'/'>(itemId, tmp);

    RequestDelete request(accessToken, itemOdId);
    StringResponse response;

    Result res = conn->SendRequest(request, response);
    if (!res) {
        return res;
    }

    status._status = response.GetStatus();
    status._phrase = response.GetPhrase();
    const HttpHeaders& headers = response.GetHeaders();
    const std::string& jsondata = response.GetBody();

    return RESULT(0);
}

Result RmRestClient::OneDriveDownloadFile(
	_In_opt_ bool* cancelFlag,
	const ULONG requestedLength,
	const std::wstring& accessToken,
    const std::wstring& fileId,
    const std::wstring& localFilePath,
    RmRestStatus& status)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(oneDriveApiUrl, res));
    if (p) {
        res = OneDriveDownloadFile(p.get(), cancelFlag, requestedLength, accessToken, fileId, localFilePath, status);
    }
    return res;
}

Result RmRestClient::OneDriveDownloadFile(NX::REST::http::Connection* conn,
	_In_opt_ bool* cancelFlag,
	const ULONG requestedLength,
    const std::wstring& accessToken,
    const std::wstring& fileId,
    const std::wstring& localFilePath,
    RmRestStatus& status)
{
    std::wstring tmp;
    const std::wstring fileOdId = NX::remove_tail<wchar_t, L'/'>(fileId, tmp);

    RequestDownloadFile request(accessToken, fileOdId, cancelFlag, requestedLength);
    FileResponse response(localFilePath);

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
    const HttpHeaders& headers = response.GetHeaders();

    if (status.GetStatus() != status_codes::OK.id) {
        return RESULT(0);
    }


    return RESULT(0);
}

Result RmRestClient::OneDriveUploadFile(
	_In_opt_ bool* cancelFlag,
	const std::wstring& accessToken,
    const std::wstring& file,
    const std::wstring& repoPath,
    RmRestStatus& status,
    RmRepoFile& newFile)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(oneDriveApiUrl, res));
    if (p) {
        res = OneDriveUploadFile(p.get(), cancelFlag, accessToken, file, repoPath, status, newFile);
    }
    return res;
}

Result RmRestClient::OneDriveUploadFile(NX::REST::http::Connection* conn,
	_In_opt_ bool* cancelFlag,
    const std::wstring& accessToken,
    const std::wstring& file,
    const std::wstring& repoPath,
    RmRestStatus& status,
    RmRepoFile& newFile)
{
    // Get the last-modification time of the file to be uploaded
    HANDLE hFile = ::CreateFile(file.c_str(), GENERIC_READ, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    FILETIME ft;
    bool hasFileTime = false;
    if (hFile != INVALID_HANDLE_VALUE) {
        if (::GetFileTime(hFile, NULL, NULL, &ft)) {
            hasFileTime = true;
        }
        ::CloseHandle(hFile);
    }

#if 0///multipart not working yet
    auto pos = repoPath.find_last_of(L'/');
    std::wstring& repoParentPath = repoPath.substr(0, pos);
    if (repoParentPath.empty()) {
        repoParentPath = L"/";
    }
    const std::wstring& repoFileName = repoPath.substr(pos + 1);

    RequestUploadFile request(accessToken, OneDriveBuildUploadApiParameters(repoFileName, hasFileTime ? &ft : nullptr), file, repoParentPath);
#endif

    std::wstring parentFolderId;
    std::wstring parentFolderOdId;
    const std::wstring& repoFileName = NX::remove_tail<wchar_t, L'/'>(repoPath, parentFolderId);
    if (parentFolderId.empty()) {
        parentFolderOdId = L"/";
    } else {
        std::wstring tmp;
        parentFolderOdId = NX::remove_tail<wchar_t, L'/'>(parentFolderId, tmp);
    }

    RequestUploadFile request(cancelFlag, accessToken, parentFolderOdId, repoFileName, file);
    StringResponse response;

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

        if (status.GetStatus() != status_codes::Created.id) {
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
            OneDriveMetadataToRepoFile(root->AsObject(), parentFolderId, newFile);
        }
        catch (...) {
            // The JSON data is NOT correct
            res = RESULT2(ERROR_INVALID_DATA, "JSON response is not correct");
        }

    } while (FALSE);


    return RESULT(0);
}
