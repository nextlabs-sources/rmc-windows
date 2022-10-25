
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

static const std::wstring googleDriveApiUrl {L"https://www.googleapis.com"};

static std::wstring EscapeQuotes(const std::wstring& str)
{
    std::wstring escStr;
    for (auto& c : str) {
        if (c == L'\'') {
            escStr += L"\\'";
        } else {
            escStr += c;
        }
    }
    return escStr;
}

namespace {

    class RequestGetFileMimeType : public REST::http::StringBodyRequest
    {
    public:
        RequestGetFileMimeType(const std::wstring &accessToken, const std::wstring& fileGdId, _In_opt_ bool* cancelFlag)
            : StringBodyRequest(REST::http::methods::GET,
                std::wstring(L"/drive/v2/files/") + fileGdId,
                REST::http::HttpHeaders({
                    std::pair<std::wstring, std::wstring>(L"Authorization", L"Bearer " + accessToken)
                }),
                std::vector<LPCWSTR>({ REST::http::mime_types::application_json.c_str(), NULL }),
                std::string(), cancelFlag)
        {
        }
        virtual ~RequestGetFileMimeType() {}
    };

    class RequestListFiles : public REST::http::StringBodyRequest
    {
    public:
        RequestListFiles(const std::wstring &accessToken, const std::wstring& pageToken, const std::wstring& keywords, _In_opt_ bool* cancelFlag)
            : StringBodyRequest(REST::http::methods::GET,
                std::wstring(L"/drive/v2/files?maxResults=1000&q=trashed = false")
                    + (keywords.empty() ? std::wstring() : L" and (mimeType = 'application/vnd.google-apps.folder' or title contains '" + EscapeQuotes(keywords) + L"')")
                    + (pageToken.empty() ? std::wstring() : L"&pageToken=" + pageToken),
                REST::http::HttpHeaders({
                    std::pair<std::wstring, std::wstring>(L"Authorization", L"Bearer " + accessToken)
                }),
                std::vector<LPCWSTR>({ REST::http::mime_types::application_json.c_str(), REST::http::mime_types::text.c_str(), NULL }),
                std::string(), cancelFlag)
        {
        }
        virtual ~RequestListFiles() {}
    };

    class RequestCreateFolder : public REST::http::StringBodyRequest
    {
    public:
        RequestCreateFolder(const std::wstring &accessToken, const std::wstring& parentFolderGdId, const std::wstring& folderName)
            : StringBodyRequest(REST::http::methods::POST,
                std::wstring(L"/drive/v2/files/"),
                REST::http::HttpHeaders({
                    std::pair<std::wstring, std::wstring>(L"Authorization", L"Bearer " + accessToken),
                    std::pair<std::wstring, std::wstring>(REST::http::header_names::content_type, REST::http::mime_types::application_json)
                }),
                std::vector<LPCWSTR>({ REST::http::mime_types::application_json.c_str(), REST::http::mime_types::text.c_str(), NULL }),
                BuildBody(parentFolderGdId, folderName))
        {
        }
        virtual ~RequestCreateFolder() {}

    private:
        std::string BuildBody(const std::wstring& parentFolderGdId, const std::wstring& folderName)
        {
            std::string s;
            std::shared_ptr<JsonObject> root(NX::rapidjson::JsonValue::CreateObject());
            if (root == nullptr)
                return s;
            JsonString* ps = JsonValue::CreateString(folderName);
            if (ps == nullptr)
                return s;
            root->set(L"title", ps);
            ps = JsonValue::CreateString(L"application/vnd.google-apps.folder");
            if (ps == nullptr)
                return s;
            root->set(L"mimeType", ps);

            if (!parentFolderGdId.empty()) {
                JsonArray* pa = JsonValue::CreateArray();
                if (pa == nullptr)
                    return s;
                JsonObject* parent = JsonValue::CreateObject();
                if (parent == nullptr)
                    return s;
                ps = JsonValue::CreateString(parentFolderGdId);
                if (ps == nullptr)
                    return s;

                parent->set(L"id", ps);
                pa->push_back(parent);
                root->set(L"parents", pa);
            }

            JsonStringWriter<char> writer;
            s = writer.Write(root.get());
            root.reset();
            return s;
        }
    };

    class RequestDeleteFile : public REST::http::StringBodyRequest
    {
    public:
        RequestDeleteFile(const std::wstring &accessToken, const std::wstring& fileGdId)
            : StringBodyRequest(REST::http::methods::DEL,
                std::wstring(L"/drive/v2/files/") + fileGdId,
                REST::http::HttpHeaders({
                    std::pair<std::wstring, std::wstring>(L"Authorization", L"Bearer " + accessToken)
                }),
                std::vector<LPCWSTR>({ NULL }),
                std::string())
        {
        }
        virtual ~RequestDeleteFile() {}
    };

    class RequestDownloadFile : public REST::http::StringBodyRequest
    {
    public:
        RequestDownloadFile(const std::wstring &accessToken, const std::wstring& fileGdId, _In_opt_ bool* cancelFlag, const ULONG requestedLength)
            : StringBodyRequest(REST::http::methods::GET,
                std::wstring(L"/drive/v2/files/") + fileGdId + L"?alt=media",
                REST::http::HttpHeaders({
                    std::pair<std::wstring, std::wstring>(L"Authorization", L"Bearer " + accessToken)
                }),
                std::vector<LPCWSTR>({ REST::http::mime_types::application_json.c_str(), REST::http::mime_types::text.c_str(), NULL }),
                std::string(), cancelFlag, requestedLength)
        {
        }
        virtual ~RequestDownloadFile() {}
    };

    class RequestExportFile : public REST::http::StringBodyRequest
    {
    public:
        RequestExportFile(const std::wstring &accessToken, const std::wstring& fileGdId, const std::wstring& mimeType, _In_opt_ bool* cancelFlag, const ULONG requestedLength)
            : StringBodyRequest(REST::http::methods::GET,
                std::wstring(L"/drive/v2/files/") + fileGdId + L"/export?mimeType=" + mimeType,
                REST::http::HttpHeaders({
                    std::pair<std::wstring, std::wstring>(L"Authorization", L"Bearer " + accessToken)
                }),
                std::vector<LPCWSTR>({ REST::http::mime_types::application_json.c_str(), NULL }),
                std::string(), cancelFlag, requestedLength)
        {
        }
        virtual ~RequestExportFile() {}
    };

    class RequestUploadFile : public REST::http::Request
    {
    public:
        RequestUploadFile(const std::wstring &accessToken, const std::string& apiInput, const std::wstring& file, _In_opt_ bool* cancelFlag)
            : NX::REST::http::Request(REST::http::methods::POST,
                std::wstring(L"/upload/drive/v2/files?uploadType=multipart"),
                REST::http::HttpHeaders({
                    std::pair<std::wstring, std::wstring>(L"Authorization", L"Bearer " + accessToken),
                    std::pair<std::wstring, std::wstring>(NX::REST::http::header_names::content_type, std::wstring(L"multipart/related; boundary=" + std::wstring(boundary.begin(), boundary.end()))),
                }),
                std::vector<LPCWSTR>({ REST::http::mime_types::application_json.c_str(), REST::http::mime_types::text.c_str(), NULL }), cancelFlag),
            _buf(apiInput, file), _is(&_buf)
        {
            SetBodyLength((ULONG)_buf.GetTotalDataLength());
        }
        virtual ~RequestUploadFile() {}

		virtual  bool IsUpload() const { return false; }

        virtual ULONG GetRemainBodyLength()
        {
            return (ULONG)_buf.GetRemainDataLength();
        }
        virtual std::istream& GetBodyStream() { return _is; }

    protected:
        std::istream    _is;
        RmUploadBuffer  _buf;
    };

    std::string GoogleDriveBuildUploadApiParameters(const std::wstring& targetFolderGdId, const std::wstring& targetFileName, const FILETIME *lastModified)
    {
        std::string s;
        std::shared_ptr<JsonObject> root(NX::rapidjson::JsonValue::CreateObject());
        if (root == nullptr)
            return s;
        JsonString* ps = JsonValue::CreateString(targetFileName);
        if (ps == nullptr)
            return s;
        root->set(L"title", ps);

        if (lastModified) {
            ps = JsonValue::CreateString(NX::time::datetime(lastModified).serialize());
            if (ps == nullptr)
                return s;
            root->set(L"modifiedDate", ps);
        }

        if (!targetFolderGdId.empty()) {
            JsonArray* pa = JsonValue::CreateArray();
            if (pa == nullptr)
                return s;
            JsonObject* parent = JsonValue::CreateObject();
            if (parent == nullptr)
                return s;
            ps = JsonValue::CreateString(targetFolderGdId);
            if (ps == nullptr)
                return s;

            parent->set(L"id", ps);
            pa->push_back(parent);
            root->set(L"parents", pa);
        }

        JsonStringWriter<char> writer;
        s = writer.Write(root.get());
        root.reset();
        return s;
    }

}

void RmRestClient::GoogleDriveListFilesProcessParents(const std::unordered_map<std::wstring, GdRepoFile>& gdIdFileMap,
    const std::wstring& descendantGdId,
    const GdRepoFile& descendantGdRepoFile,
    const std::wstring& childGdId,
    const std::wstring& childToDescendentId,
    const std::wstring& childToDescendentPath,
    std::vector<std::pair<RmRepoFile, std::wstring>>& filesAndGdIds)
{
    const GdRepoFile& childGdRepoFile = gdIdFileMap.at(childGdId);

    for (const auto& parent : childGdRepoFile.parents) {
        if (parent.isRoot) {
            RmRepoFile repoFile;
            repoFile._id = L'/' + childToDescendentId;
            repoFile._path = L'/'+ childToDescendentPath;
            repoFile._name = descendantGdRepoFile.name;
            repoFile._folder = descendantGdRepoFile.folder;
            repoFile._size = descendantGdRepoFile.size;
            repoFile._time = descendantGdRepoFile.lastModifiedTime;
            filesAndGdIds.push_back(std::pair<RmRepoFile, std::wstring>(repoFile, descendantGdId));
        } else {
            const std::wstring parentToDescendentId = parent.gdId + L'/' + childToDescendentId;
            const std::wstring parentToDescendentPath = gdIdFileMap.at(parent.gdId).name + L'/' + childToDescendentPath;
            GoogleDriveListFilesProcessParents(gdIdFileMap, descendantGdId, descendantGdRepoFile, parent.gdId, parentToDescendentId, parentToDescendentPath, filesAndGdIds);
        }
    }
}

Result RmRestClient::GoogleDriveGetFileMimeType(
    _In_opt_ bool* cancelFlag,
    const std::wstring& accessToken,
    const std::wstring& fileGdId,
    RmRestStatus& status,
    std::wstring& mimeType)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(googleDriveApiUrl, res));
    if (p) {
        res = GoogleDriveGetFileMimeType(p.get(), cancelFlag, accessToken, fileGdId, status, mimeType);
    }
    return res;
}

Result RmRestClient::GoogleDriveGetFileMimeType(NX::REST::http::Connection* conn,
    _In_opt_ bool* cancelFlag,
    const std::wstring& accessToken,
    const std::wstring& fileGdId,
    RmRestStatus& status,
    std::wstring& mimeType)
{
    RequestGetFileMimeType request(accessToken, fileGdId, cancelFlag);
    StringResponse response;

    Result res = conn->SendRequest(request, response);
    if (!res) {
        return res;
    }

    status._status = response.GetStatus();
    status._phrase = response.GetPhrase();
    const HttpHeaders& headers = response.GetHeaders();
    const std::string& jsondata = response.GetBody();

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
            const rapidjson::JsonValue* type = root->AsObject()->at(L"mimeType");
            if (type && type->IsString())
                mimeType = type->AsString()->GetString();
            else
                res = RESULT(ERROR_INVALID_DATA);
        }
        catch (...) {
            // The JSON data is NOT correct
            res = RESULT2(ERROR_INVALID_DATA, "JSON response is not correct");
        }

    } while (FALSE);


    return RESULT(0);
}

Result RmRestClient::GoogleDriveListFiles(_In_opt_ bool* cancelFlag, const std::wstring& accessToken,
                                          const std::wstring& keywords,
                                          RmRestStatus& status,
                                          std::vector<std::pair<RmRepoFile, std::wstring>>& filesAndGdIds)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(googleDriveApiUrl, res));
    if (p) {
        res = GoogleDriveListFiles(p.get(), cancelFlag, accessToken, keywords, status, filesAndGdIds);
    }
    return res;
}

Result RmRestClient::GoogleDriveListFiles(NX::REST::http::Connection* conn, _In_opt_ bool* cancelFlag,
                                          const std::wstring& accessToken,
                                          const std::wstring& keywords,
                                          RmRestStatus& status,
                                          std::vector<std::pair<RmRepoFile, std::wstring>>& filesAndGdIds)
{
    std::wstring pageToken;
    Result res;
    std::unordered_map<std::wstring, GdRepoFile> gdIdFileMap;

    do {

        RequestListFiles request(accessToken, pageToken, keywords, cancelFlag);
        StringResponse response;

        res = conn->SendRequest(request, response);
        if (!res) {
            return res;
        }

        status._status = response.GetStatus();
        status._phrase = response.GetPhrase();
        const HttpHeaders& headers = response.GetHeaders();
        const std::string& jsondata = response.GetBody();

        res = RESULT(0);

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
            const rapidjson::JsonValue* items = root->AsObject()->at(L"items");
            if (items && items->IsArray()) {
                const int count = (int)items->AsArray()->size();

                for (int i = 0; i < count; i++) {
                    GdRepoFile gdRepoFile;
                    const rapidjson::JsonValue* fileObject = items->AsArray()->at(i);
                    assert(NULL != fileObject);
                    const rapidjson::JsonValue* id = fileObject->AsObject()->at(L"id");
                    if (!(id && id->IsString())) {
                        continue;
                    }
                    const std::wstring& gdIdStr = id->AsString()->GetString();

                    const rapidjson::JsonValue* name = fileObject->AsObject()->at(L"title");
                    if (name && name->IsString())
                        gdRepoFile.name = name->AsString()->GetString();

                    const rapidjson::JsonValue* tag = fileObject->AsObject()->at(L"mimeType");
                    if (tag && tag->IsString())
                        gdRepoFile.folder = (tag->AsString()->GetString() == L"application/vnd.google-apps.folder");
                    const rapidjson::JsonValue* lastModified = fileObject->AsObject()->at(L"modifiedDate");
                    if (lastModified && lastModified->IsString())
                        gdRepoFile.lastModifiedTime = NX::time::datetime(lastModified->AsString()->GetString()).to_java_time();
                    const rapidjson::JsonValue* fileSize = fileObject->AsObject()->at(L"fileSize");
                    if (fileSize && fileSize->IsString())
                        gdRepoFile.size = std::stoll(fileSize->AsString()->GetString());
                    else
                        gdRepoFile.size = -1;

                    const rapidjson::JsonValue* parents = fileObject->AsObject()->at(L"parents");
                    if (parents && parents->IsArray()) {
                        const size_t parentCount = parents->AsArray()->size();
                        for (unsigned int j = 0; j < parentCount; j++) {
                            const rapidjson::JsonValue* parent = parents->AsArray()->at(j);
                            const std::wstring parentGdIdStr = parent->AsObject()->at(L"id")->AsString()->GetString();
                            bool parentIsRoot = parent->AsObject()->at(L"isRoot")->AsBool()->GetBool();
                            gdRepoFile.parents.push_back(GdParentInfo{parentGdIdStr, parentIsRoot});
                        }
                    }

                    gdIdFileMap[gdIdStr] = gdRepoFile;
                }

                const rapidjson::JsonValue* nextPageToken = root->AsObject()->at(L"nextPageToken");
                if (nextPageToken && nextPageToken->IsString()) {
                    pageToken = nextPageToken->AsString()->GetString();
                } else {
                    pageToken = std::wstring();
                }
            }
        }
        catch (...) {
            // The JSON data is NOT correct
            res = RESULT2(ERROR_INVALID_DATA, "JSON response is not correct");
        }

    } while (!pageToken.empty());

    try {
        for (const auto& gdIdFile : gdIdFileMap) {
            // Construct full id(s) and path(s) of this file/folder
            GoogleDriveListFilesProcessParents(gdIdFileMap, gdIdFile.first, gdIdFile.second, gdIdFile.first, gdIdFile.first, gdIdFile.second.name, filesAndGdIds);
        }
    } catch (...) {
        // The JSON data is NOT correct
        res = RESULT2(ERROR_INVALID_DATA, "JSON response is not correct");
    }

    return RESULT(0);
}

Result RmRestClient::GoogleDriveCreateFolder(const std::wstring& accessToken,
                                             const std::pair<RmRepoFile, std::wstring>& parentFolderFileAndGdId,
                                             const std::wstring& folderName,
                                             RmRestStatus& status,
                                             RmRepoFile& folder)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(googleDriveApiUrl, res));
    if (p) {
        res = GoogleDriveCreateFolder(p.get(), accessToken, parentFolderFileAndGdId, folderName, status, folder);
    }
    return res;
}

Result RmRestClient::GoogleDriveCreateFolder(NX::REST::http::Connection* conn,
                                             const std::wstring& accessToken,
                                             const std::pair<RmRepoFile, std::wstring>& parentFolderFileAndGdId,
                                             const std::wstring& folderName,
                                             RmRestStatus& status,
                                             RmRepoFile& folder)
{
    const std::wstring parentFolderGdId = &parentFolderFileAndGdId ? parentFolderFileAndGdId.second : std::wstring();
    RequestCreateFolder request(accessToken, parentFolderGdId, folderName);
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

        PCJSONVALUE root = doc.GetRoot();
        if (!IsValidJsonObject(root)) {
            res = RESULT(ERROR_INVALID_DATA);
            break;
        }

        try {
            const rapidjson::JsonValue* name = root->AsObject()->at(L"title");
            if (name && name->IsString())
                folder._name = name->AsString()->GetString();
            const rapidjson::JsonValue* id = root->AsObject()->at(L"id");
            if (id && id->IsString())
                folder._id = (&parentFolderFileAndGdId ? parentFolderFileAndGdId.first.GetId() : std::wstring()) + L'/' + id->AsString()->GetString();
            folder._path = (&parentFolderFileAndGdId ? parentFolderFileAndGdId.first.GetPath() : std::wstring()) + L'/' + folder._name;
            folder._folder = true;
        }
        catch (...) {
            // The JSON data is NOT correct
            res = RESULT2(ERROR_INVALID_DATA, "JSON response is not correct");
        }

    } while (FALSE);


    return RESULT(0);
}

Result RmRestClient::GoogleDriveDeleteFile(const std::wstring& accessToken,
    const std::wstring& fileGdId,
    RmRestStatus& status)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(googleDriveApiUrl, res));
    if (p) {
        res = GoogleDriveDeleteFile(p.get(), accessToken, fileGdId, status);
    }
    return res;
}

Result RmRestClient::GoogleDriveDeleteFile(NX::REST::http::Connection* conn,
    const std::wstring& accessToken,
    const std::wstring& fileGdId,
    RmRestStatus& status)
{
    RequestDeleteFile request(accessToken, fileGdId);
    StringResponse response;

    Result res = conn->SendRequest(request, response);
    if (!res) {
        return res;
    }

    status._status = response.GetStatus();
    status._phrase = response.GetPhrase();
    const HttpHeaders& headers = response.GetHeaders();

    return RESULT(0);
}

Result RmRestClient::GoogleDriveDownloadFile(
	_In_opt_ bool* cancelFlag,
	const ULONG requestedLength,
	const std::wstring& accessToken,
    const std::wstring& sourceFileGdId,
    const std::wstring& target,
    RmRestStatus& status)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(googleDriveApiUrl, res));
    if (p) {
        res = GoogleDriveDownloadFile(p.get(), cancelFlag, requestedLength, accessToken, sourceFileGdId, target, status);
    }
    return res;
}

Result RmRestClient::GoogleDriveDownloadFile(NX::REST::http::Connection* conn,
	_In_opt_ bool* cancelFlag,
	const ULONG requestedLength,
    const std::wstring& accessToken,
    const std::wstring& sourceFileGdId,
    const std::wstring& target,
    RmRestStatus& status)
{
    RequestDownloadFile request(accessToken, sourceFileGdId, cancelFlag, requestedLength);
    FileResponse response(target);

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

Result RmRestClient::GoogleDriveExportFile(
    _In_opt_ bool* cancelFlag,
    const ULONG requestedLength,
    const std::wstring& accessToken,
    const std::wstring& sourceFileGdId,
    const std::wstring& target,
    const std::wstring& targetMimeType,
    RmRestStatus& status)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(googleDriveApiUrl, res));
    if (p) {
        res = GoogleDriveExportFile(p.get(), cancelFlag, requestedLength, accessToken, sourceFileGdId, target, targetMimeType, status);
    }
    return res;
}

Result RmRestClient::GoogleDriveExportFile(NX::REST::http::Connection* conn,
    _In_opt_ bool* cancelFlag,
    const ULONG requestedLength,
    const std::wstring& accessToken,
    const std::wstring& sourceFileGdId,
    const std::wstring& target,
    const std::wstring& targetMimeType,
    RmRestStatus& status)
{
    RequestExportFile request(accessToken, sourceFileGdId, targetMimeType, cancelFlag, requestedLength);
    FileResponse response(target);

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

Result RmRestClient::GoogleDriveUploadFile(
	_In_opt_ bool* cancelFlag,
	const std::wstring& accessToken,
    const std::wstring& sourceFile,
    const std::pair<RmRepoFile, std::wstring>& targetFolderFileAndGdId,
    const std::wstring& targetFileName,
    RmRestStatus& status,
    RmRepoFile& repoFile)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(googleDriveApiUrl, res));
    if (p) {
        res = GoogleDriveUploadFile(p.get(), cancelFlag, accessToken, sourceFile, targetFolderFileAndGdId, targetFileName, status, repoFile);
    }
    return res;
}

Result RmRestClient::GoogleDriveUploadFile(NX::REST::http::Connection* conn,
	_In_opt_ bool* cancelFlag,
    const std::wstring& accessToken,
    const std::wstring& sourceFile,
    const std::pair<RmRepoFile, std::wstring>& targetFolderFileAndGdId,
    const std::wstring& targetFileName,
    RmRestStatus& status,
    RmRepoFile& repoFile)
{
    // Get the last-modification time of the file to be uploaded
    HANDLE hFile = ::CreateFile(sourceFile.c_str(), GENERIC_READ, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    FILETIME ft;
    bool hasFileTime = false;
    if (hFile != INVALID_HANDLE_VALUE) {
        if (::GetFileTime(hFile, NULL, NULL, &ft)) {
            hasFileTime = true;
        }
        ::CloseHandle(hFile);
    }

    const std::wstring targetFolderGdId = &targetFolderFileAndGdId ? targetFolderFileAndGdId.second : std::wstring();
    RequestUploadFile request(accessToken, GoogleDriveBuildUploadApiParameters(targetFolderGdId, targetFileName, hasFileTime ? &ft : nullptr), sourceFile, cancelFlag);
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

        PCJSONVALUE root = doc.GetRoot();
        if (!IsValidJsonObject(root)) {
            res = RESULT(ERROR_INVALID_DATA);
            break;
        }

        try {
            const rapidjson::JsonValue* name = root->AsObject()->at(L"title");
            if (name && name->IsString())
                repoFile._name = name->AsString()->GetString();
            const rapidjson::JsonValue* id = root->AsObject()->at(L"id");
            if (id && id->IsString())
                repoFile._id = (&targetFolderFileAndGdId ? targetFolderFileAndGdId.first.GetId() : std::wstring()) + L'/' + id->AsString()->GetString();
            repoFile._path = (&targetFolderFileAndGdId ? targetFolderFileAndGdId.first.GetPath() : std::wstring()) + L'/' + repoFile._name;
            const rapidjson::JsonValue* tag = root->AsObject()->at(L"mimeType");
            if (tag && tag->IsString())
                repoFile._folder = (tag->AsString()->GetString() == L"application/vnd.google-apps.folder");
            const rapidjson::JsonValue* lastModified = root->AsObject()->at(L"modifiedDate");
            if (lastModified && lastModified->IsString())
                repoFile._time = NX::time::datetime(lastModified->AsString()->GetString()).to_java_time();
            const rapidjson::JsonValue* fileSize = root->AsObject()->at(L"fileSize");
            if (fileSize && fileSize->IsString())
                repoFile._size = std::stoll(fileSize->AsString()->GetString());
        }
        catch (...) {
            // The JSON data is NOT correct
            res = RESULT2(ERROR_INVALID_DATA, "JSON response is not correct");
        }

    } while (FALSE);


    return RESULT(0);
}
