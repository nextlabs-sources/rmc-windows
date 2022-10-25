
#include "stdafx.h"

#include <nx\rmc\rest.h>
#include <nx\rmc\repo.h>

#include <nx\common\rapidjson.h>
#include <nx\common\time.h>


using namespace NX;
using namespace NX::rapidjson;
using namespace NX::REST::http;

static const std::wstring dropboxApiUrl     {L"https://api.dropboxapi.com"};
static const std::wstring dropboxContentUrl {L"https://content.dropboxapi.com"};

static std::wstring EscapeUnicodeCodePoint(const std::wstring& str)
{
    std::wstring esc;
    std::for_each(str.begin(), str.end(), [&](const wchar_t c) {
        if (c > 31 && c < 127) {
            esc.append(&c, 1);
        }
        else {
            wchar_t wz[16] = { 0 };
            swprintf_s(wz, L"\\u%04X", c);
            esc.append(wz);
        }
    });
    return esc;
}

namespace {

    class RequestListFiles : public REST::http::StringBodyRequest
    {
    public:
        RequestListFiles(const std::wstring &accessToken, const std::wstring& path, _In_opt_ bool* cancelFlag)
            : StringBodyRequest(REST::http::methods::POST,
                std::wstring(L"/2/files/list_folder"),
                REST::http::HttpHeaders({
                    std::pair<std::wstring, std::wstring>(L"Authorization", L"Bearer " + accessToken),
                    std::pair<std::wstring, std::wstring>(REST::http::header_names::content_type, REST::http::mime_types::application_json)
                }),
                std::vector<LPCWSTR>({ REST::http::mime_types::application_json.c_str(), REST::http::mime_types::text.c_str(), NULL }),
                BuildBody(path), cancelFlag)
        {
        }
        virtual ~RequestListFiles() {}

    private:
        std::string BuildBody(const std::wstring& path)
        {
            std::wstring s = L"{\"path\":\"";
            if (path.empty() || path == L"/") {
                // Use empty string for root folder
            } else {
                // Use leading slash for non-root folder
                if (path[0] != L'/') {
                    s += L"/";
                }
                s += path;
            }
            s += L"\"}";
            return NX::conv::utf16toutf8(s);
        }
    };

    class RequestSearchFiles : public REST::http::StringBodyRequest
    {
    public:
        RequestSearchFiles(const std::wstring &accessToken, const std::wstring& path, const std::wstring& keywords, _In_opt_ bool* cancelFlag)
            : StringBodyRequest(REST::http::methods::POST,
                std::wstring(L"/2/files/search"),
                REST::http::HttpHeaders({
                    std::pair<std::wstring, std::wstring>(L"Authorization", L"Bearer " + accessToken),
                    std::pair<std::wstring, std::wstring>(REST::http::header_names::content_type, REST::http::mime_types::application_json)
                }),
                std::vector<LPCWSTR>({ REST::http::mime_types::application_json.c_str(), REST::http::mime_types::text.c_str(), NULL }),
                BuildBody(path, keywords), cancelFlag)
        {
        }
        virtual ~RequestSearchFiles() {}

    private:
        std::string BuildBody(const std::wstring& path, const std::wstring& keywords)
        {
            std::wstring s = L"{\"path\":\"";
            if (path.empty() || path == L"/") {
                // Use empty string for root folder
            } else {
                // Use leading slash for non-root folder
                if (path[0] != L'/') {
                    s += L"/";
                }
                s += path;
            }
            s += L"\",";
            s += L"\"query\":\"" + keywords + L"\"";
            s += L"}";
            return NX::conv::utf16toutf8(s);
        }
    };

    class RequestCreateFolder : public REST::http::StringBodyRequest
    {
    public:
        RequestCreateFolder(const std::wstring &accessToken, const std::wstring& parentDir, const std::wstring& folderName)
            : StringBodyRequest(REST::http::methods::POST,
                std::wstring(L"/2/files/create_folder"),
                REST::http::HttpHeaders({
                    std::pair<std::wstring, std::wstring>(L"Authorization", L"Bearer " + accessToken),
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
            std::wstring s = L"{\"path\":\"" + parentDir;
            if (parentDir != L"/") {
                s += L"/";
            }
            s += folderName;
            s += L"\"}";
            return NX::conv::utf16toutf8(s);
        }
    };

    class RequestDeleteFile : public REST::http::StringBodyRequest
    {
    public:
        RequestDeleteFile(const std::wstring &accessToken, const std::wstring& path)
            : StringBodyRequest(REST::http::methods::POST,
                std::wstring(L"/2/files/delete"),
                REST::http::HttpHeaders({
                    std::pair<std::wstring, std::wstring>(L"Authorization", L"Bearer " + accessToken),
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
            std::wstring s = L"{\"path\":\"";

            // Make sure that there is leading slash
            if (path[0] != L'/') {
                s += L"/";
            }
            s += path;
            s += L"\"}";
            return NX::conv::utf16toutf8(s);
        }
    };

    class RequestDownloadFile : public REST::http::StringBodyRequest
    {
    public:
        RequestDownloadFile(const std::wstring &accessToken, const std::wstring& path, _In_opt_ bool* cancelFlag, const ULONG requestedLength)
            : StringBodyRequest(REST::http::methods::POST,
                std::wstring(L"/2/files/download"),
                REST::http::HttpHeaders({
                    std::pair<std::wstring, std::wstring>(L"Authorization", L"Bearer " + accessToken),
                    std::pair<std::wstring, std::wstring>(L"Dropbox-API-Arg", L"{\"path\":\"" + EscapeUnicodeCodePoint(path) + L"\"}")
                }),
                std::vector<LPCWSTR>({ REST::http::mime_types::application_json.c_str(), REST::http::mime_types::text.c_str(), NULL }),
                std::string(), cancelFlag, requestedLength)
        {
        }
        virtual ~RequestDownloadFile() {}
    };

    class RequestUploadFile : public REST::http::Request
    {
    public:
        RequestUploadFile(const std::wstring& accessToken, const std::string& apiInput, const std::wstring& path)
            : NX::REST::http::Request(REST::http::methods::POST,
                std::wstring(L"/2/files/upload"),
                REST::http::HttpHeaders({
                    std::pair<std::wstring, std::wstring>(NX::REST::http::header_names::content_type, REST::http::mime_types::application_octetstream),
                    std::pair<std::wstring, std::wstring>(L"Authorization", L"Bearer " + accessToken),
                    std::pair<std::wstring, std::wstring>(L"Dropbox-API-Arg", std::wstring(apiInput.begin(), apiInput.end())),
                }),
                std::vector<LPCWSTR>({ REST::http::mime_types::application_octetstream.c_str(), NULL })),
            _buf(path), _is(&_buf)
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
        std::istream            _is;
        RmSimpleUploadBuffer    _buf;
    };

    std::string DropboxBuildUploadApiParameters(const std::wstring& path, const FILETIME *lastModified)
    {
        std::string s;
        std::shared_ptr<JsonObject> root(NX::rapidjson::JsonValue::CreateObject());
        if (root == nullptr)
            return s;
        JsonString* pv = JsonValue::CreateString(path);
        if (pv == nullptr)
            return s;
        root->set(L"path", pv);

        if (lastModified) {
            pv = JsonValue::CreateString(NX::time::datetime(lastModified).serialize());
            if (pv == nullptr)
                return s;
            root->set(L"client_modified", pv);
        }

        JsonStringWriter<char> writer;
        s = writer.Write(root.get());
        root.reset();
        return s;
    }

}

void RmRestClient::DropboxFileMetadataToRepoFile(NX::rapidjson::PCJSONOBJECT fileObject,
    RmRepoFile& repoFile)
{
    const rapidjson::JsonValue* displayPath = fileObject->at(L"path_display");
    if (displayPath && displayPath->IsString())
        repoFile._path = displayPath->AsString()->GetString();
    if (repoFile.GetPath().empty())
        return;
    repoFile._id = repoFile._path;
    const rapidjson::JsonValue* name = fileObject->at(L"name");
    if (name && name->IsString())
        repoFile._name = name->AsString()->GetString();
    if (repoFile.GetName().empty()) {
        auto pos = repoFile._id.find_last_of(L'/');
        repoFile._name = (pos == std::wstring::npos) ? repoFile._id : repoFile._id.substr(pos + 1);
    }
    const rapidjson::JsonValue* tag = fileObject->at(L".tag");
    if (tag && tag->IsString())
        repoFile._folder = (tag->AsString()->GetString() == L"folder");
    const rapidjson::JsonValue* lastModified = fileObject->at(L"client_modified");
    if (lastModified && lastModified->IsString())
        repoFile._time = NX::time::datetime(lastModified->AsString()->GetString()).to_java_time();
    else
        repoFile._time = -1;
    const rapidjson::JsonValue* fileSize = fileObject->at(L"size");
    if (fileSize && fileSize->IsNumber())
        repoFile._size = fileSize->AsNumber()->ToInt64();
    else
        repoFile._size = -1;
}

Result RmRestClient::DropboxListFiles(_In_opt_ bool* cancelFlag, const std::wstring& accessToken,
    const std::wstring& path,
    RmRestStatus& status,
    std::vector<RmRepoFile>& files)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(dropboxApiUrl, res));
    if (p) {
        res = DropboxListFiles(p.get(), cancelFlag, accessToken, path, status, files);
    }
    return res;
}

Result RmRestClient::DropboxListFiles(NX::REST::http::Connection* conn, _In_opt_ bool* cancelFlag,
    const std::wstring& accessToken,
    const std::wstring& path,
    RmRestStatus& status,
    std::vector<RmRepoFile>& files)
{
    RequestListFiles request(accessToken, path, cancelFlag);
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
            const rapidjson::JsonValue* entries = root->AsObject()->at(L"entries");
            if (entries && entries->IsArray()) {
                const int count = (int)entries->AsArray()->size();
                for (int i = 0; i < count; i++) {
                    RmRepoFile repoFile;
                    const rapidjson::JsonValue* fileObject = entries->AsArray()->at(i);
                    assert(NULL != fileObject);
                    DropboxFileMetadataToRepoFile(fileObject->AsObject(), repoFile);
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

Result RmRestClient::DropboxSearchFiles(_In_opt_ bool* cancelFlag, const std::wstring& accessToken,
    const std::wstring& path,
    const std::wstring& keywords,
    RmRestStatus& status,
    std::vector<RmRepoFile>& files)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(dropboxApiUrl, res));
    if (p) {
        res = DropboxSearchFiles(p.get(), cancelFlag, accessToken, path, keywords, status, files);
    }
    return res;
}

Result RmRestClient::DropboxSearchFiles(NX::REST::http::Connection* conn, _In_opt_ bool* cancelFlag,
    const std::wstring& accessToken,
    const std::wstring& path,
    const std::wstring& keywords,
    RmRestStatus& status,
    std::vector<RmRepoFile>& files)
{
    RequestSearchFiles request(accessToken, path, keywords, cancelFlag);
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
            const rapidjson::JsonValue* matches = root->AsObject()->at(L"matches");
            if (matches && matches->IsArray()) {
                const int count = (int)matches->AsArray()->size();
                for (int i = 0; i < count; i++) {
                    RmRepoFile repoFile;
                    const rapidjson::JsonValue* fileObject = matches->AsArray()->at(i)->AsObject()->at(L"metadata");
                    assert(NULL != fileObject);
                    DropboxFileMetadataToRepoFile(fileObject->AsObject(), repoFile);
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

Result RmRestClient::DropboxCreateFolder(const std::wstring& accessToken,
    const std::wstring& parentDir,
    const std::wstring& folderName,
    RmRestStatus& status,
    RmRepoFile& folder)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(dropboxApiUrl, res));
    if (p) {
        res = DropboxCreateFolder(p.get(), accessToken, parentDir, folderName, status, folder);
    }
    return res;
}

Result RmRestClient::DropboxCreateFolder(NX::REST::http::Connection* conn,
    const std::wstring& accessToken,
    const std::wstring& parentDir,
    const std::wstring& folderName,
    RmRestStatus& status,
    RmRepoFile& folder)
{
    RequestCreateFolder request(accessToken, parentDir, folderName);
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
            DropboxFileMetadataToRepoFile(root->AsObject(), folder);
            folder._folder = true;
        }
        catch (...) {
            // The JSON data is NOT correct
            res = RESULT2(ERROR_INVALID_DATA, "JSON response is not correct");
        }

    } while (FALSE);


    return RESULT(0);
}

Result RmRestClient::DropboxDeleteFile(const std::wstring& accessToken,
    const std::wstring& repoPath,
    RmRestStatus& status)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(dropboxApiUrl, res));
    if (p) {
        res = DropboxDeleteFile(p.get(), accessToken, repoPath, status);
    }
    return res;
}

Result RmRestClient::DropboxDeleteFile(NX::REST::http::Connection* conn,
    const std::wstring& accessToken,
    const std::wstring& repoPath,
    RmRestStatus& status)
{
    RequestDeleteFile request(accessToken, repoPath);
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

Result RmRestClient::DropboxDownloadFile(_In_opt_ bool* cancelFlag, const ULONG requestedLength,
	const std::wstring& accessToken,
    const std::wstring& source,
    const std::wstring& target,
    RmRestStatus& status)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(dropboxContentUrl, res));
    if (p) {
        res = DropboxDownloadFile(p.get(), cancelFlag, requestedLength, accessToken, source, target, status);
    }
    return res;
}

Result RmRestClient::DropboxDownloadFile(NX::REST::http::Connection* conn,
	_In_opt_ bool* cancelFlag,
	const ULONG requestedLength,
    const std::wstring& accessToken,
    const std::wstring& source,
    const std::wstring& target,
    RmRestStatus& status)
{
    RequestDownloadFile request(accessToken, source, cancelFlag, requestedLength);
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

    // Set the last-modification time of the downloaded file
    for (auto p:headers) {
        if (p.first == L"dropbox-api-result") {
            const std::wstring& jsondata = p.second;
            if (jsondata.empty()) {
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
                const rapidjson::JsonValue* lastModified = root->AsObject()->at(L"client_modified");
                if (lastModified && lastModified->IsString()) {
                    ULARGE_INTEGER li;
                    li.QuadPart = NX::time::datetime(lastModified->AsString()->GetString());
                    const FILETIME ft = {li.LowPart, li.HighPart};

                    HANDLE hFile = ::CreateFile(target.c_str(), FILE_WRITE_ATTRIBUTES, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
                    if (hFile != INVALID_HANDLE_VALUE) {
                        ::SetFileTime(hFile, NULL, NULL, &ft);
                        ::CloseHandle(hFile);
                    }
                }
            }
            catch (...) {
                // The JSON data is NOT correct
                res = RESULT2(ERROR_INVALID_DATA, "JSON response is not correct");
            }

            break;
        }
    }

    return RESULT(0);
}

Result RmRestClient::DropboxUploadFile(_In_opt_ bool* cancelFlag, const std::wstring& accessToken,
    const std::wstring& file,
    const std::wstring& repoPath,
    RmRestStatus& status,
    RmRepoFile& repoFile)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(dropboxContentUrl, res));
    if (p) {
        res = DropboxUploadFile(p.get(), cancelFlag, accessToken, file, repoPath, status, repoFile);
    }
    return res;
}

Result RmRestClient::DropboxUploadFile(NX::REST::http::Connection* conn,
	_In_opt_ bool* cancelFlag,
    const std::wstring& accessToken,
    const std::wstring& file,
    const std::wstring& repoPath,
    RmRestStatus& status,
    RmRepoFile& repoFile)
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

    RequestUploadFile request(accessToken, DropboxBuildUploadApiParameters(repoPath, hasFileTime ? &ft : nullptr), file);
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
            DropboxFileMetadataToRepoFile(root->AsObject(), repoFile);
        }
        catch (...) {
            // The JSON data is NOT correct
            res = RESULT2(ERROR_INVALID_DATA, "JSON response is not correct");
        }

    } while (FALSE);


    return RESULT(0);
}
