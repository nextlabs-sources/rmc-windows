
#include "stdafx.h"
#include <unordered_set>

#include <nx\rmc\rest.h>
#include <nx\rmc\repo.h>

#include <nx\common\rapidjson.h>
#include <nx\common\string.h>


using namespace NX;
using namespace NX::rapidjson;
using namespace NX::REST::http;

namespace {

    // List of collections that we should handle.  This is used for us to
    // match the observed behavior of RMC for Web.  It's not clear how RMC for
    // Web decides which collections to handle or not handle.
    //
    // The entries are:
    // - realName:      real name of the collection
    // - displayName:   name to use when SharePoint/RMC displays the collection as a folder
    // - serverRelName: server-relative name of the collection in SharePoint URLs for the collection as a folder and for files/folders under the collection
    typedef struct {
        const std::wstring  realName;
        const std::wstring  displayName;
        const std::wstring  serverRelName;
    } collection_name_t;
    const std::vector<collection_name_t> collectionNameMap {
        {L"Documents",      L"Documents",       L"Shared Documents"},
        {L"FormTemplates",  L"Form Templates",  L"FormServerTemplates"},
        {L"Library",        L"library",         L"library"},
        {L"StyleLibrary",   L"Style Library",   L"Style Library"}
    };

    // List of content types that we should handle.  This is used for us to
    // match the observed behavior of RMC for Web.  It's not clear how RMC for
    // Web decides which content types to handle or not handle.
    const std::unordered_set<std::wstring> supportedContentTypes {
        L"Document",
        L"Folder",
        L"InfoPath Form Template"
    };

    class RequestListCollections : public REST::http::StringBodyRequest
    {
    public:
        RequestListCollections(const std::wstring &accessToken)
            : StringBodyRequest(REST::http::methods::GET,
                std::wstring(L"/_vti_bin/ListData.svc"),
                REST::http::HttpHeaders({
                    std::pair<std::wstring, std::wstring>(L"Authorization", L"Bearer " + accessToken)
                }),
                std::vector<LPCWSTR>({ REST::http::mime_types::application_json.c_str(), NULL }),
                std::string())
        {
        }
        virtual ~RequestListCollections() {}
    };

    class RequestListOrSearchFilesInCollection : public REST::http::StringBodyRequest
    {
    public:
        RequestListOrSearchFilesInCollection(const std::wstring &accessToken, const std::wstring& collection, const std::wstring& keywords, _In_opt_ bool* cancelFlag)
            : StringBodyRequest(REST::http::methods::GET,
                std::wstring(L"/_vti_bin/ListData.svc/" + collection + (keywords.empty() ? L"" : L"?$filter=substringof('" + keywords + L"', Name)")),
                REST::http::HttpHeaders({
                    std::pair<std::wstring, std::wstring>(L"Authorization", L"Bearer " + accessToken)
                }),
                std::vector<LPCWSTR>({ REST::http::mime_types::application_json.c_str(), NULL }),
                std::string(), cancelFlag)
        {
        }
        virtual ~RequestListOrSearchFilesInCollection() {}
    };

    class RequestCreateFolder : public REST::http::StringBodyRequest
    {
    public:
        RequestCreateFolder(const std::wstring &accessToken, const std::wstring& serverRelUrl)
            : StringBodyRequest(REST::http::methods::POST,
                std::wstring(L"_api/web/folders"),
                REST::http::HttpHeaders({
                    std::pair<std::wstring, std::wstring>(L"Authorization", L"Bearer " + accessToken),
                    std::pair<std::wstring, std::wstring>(REST::http::header_names::content_type, REST::http::mime_types::application_json)
                }),
                std::vector<LPCWSTR>({ REST::http::mime_types::application_json.c_str(), NULL }),
                BuildBody(serverRelUrl))
        {
        }
        virtual ~RequestCreateFolder() {}

    private:
        std::string BuildBody(const std::wstring& serverRelUrl)
        {
            std::string s;
            std::shared_ptr<JsonObject> root(NX::rapidjson::JsonValue::CreateObject());
            if (root == nullptr)
                return s;
            JsonString* ps = JsonValue::CreateString(serverRelUrl);
            if (ps == nullptr)
                return s;
            root->set(L"ServerRelativeUrl", ps);
            JsonStringWriter<char> writer;
            s = writer.Write(root.get());
            root.reset();
            return s;
        }
    };

    class RequestDeleteFile : public REST::http::StringBodyRequest
    {
    public:
        RequestDeleteFile(const std::wstring &accessToken, const std::wstring& collection, __int64 fileId)
            : StringBodyRequest(REST::http::methods::DEL,
                std::wstring(L"/_vti_bin/ListData.svc/" + collection + L'(' + NX::i64tos<wchar_t>(fileId) + L')'),
                REST::http::HttpHeaders({
                    std::pair<std::wstring, std::wstring>(L"Authorization", L"Bearer " + accessToken)
                }),
                std::vector<LPCWSTR>({ REST::http::mime_types::application_json.c_str(), NULL }),
                std::string())
        {
        }
        virtual ~RequestDeleteFile() {}
    };

    class RequestDownloadFile : public REST::http::StringBodyRequest
    {
    public:
        RequestDownloadFile(const std::wstring &accessToken, const std::wstring& fileSourceUrl, _In_opt_ bool* cancelFlag, const ULONG requestedLength)
            : StringBodyRequest(REST::http::methods::GET,
                std::wstring(L"/_layouts/15/download.aspx?SourceUrl=") + fileSourceUrl,
                REST::http::HttpHeaders({
                    std::pair<std::wstring, std::wstring>(L"Authorization", L"Bearer " + accessToken)
                }),
                std::vector<LPCWSTR>({ NULL }),
                std::string(), cancelFlag, requestedLength)
        {
        }
        virtual ~RequestDownloadFile() {}
    };

    class RequestGetFolderUniqId : public REST::http::StringBodyRequest
    {
    public:
        RequestGetFolderUniqId(const std::wstring &accessToken, const std::wstring& serverRelUrl)
            : StringBodyRequest(REST::http::methods::GET,
                std::wstring(L"_api/web/GetFolderByServerRelativeUrl('" + NX::conv::to_wstring(NX::conv::UrlEncode(NX::conv::to_string(serverRelUrl))) + L"')"),
                REST::http::HttpHeaders({
                    std::pair<std::wstring, std::wstring>(L"Authorization", L"Bearer " + accessToken)
                }),
                std::vector<LPCWSTR>({ REST::http::mime_types::application_json.c_str(), NULL }),
                std::string())
        {
        }
        virtual ~RequestGetFolderUniqId() {}
    };

    class RequestUploadFile : public REST::http::Request
    {
    public:
        RequestUploadFile(const std::wstring& accessToken, const std::wstring& repoParentFolderUniqId, const std::wstring& repoFileName, const std::wstring& path)
            : NX::REST::http::Request(REST::http::methods::POST,
                std::wstring(L"_api/web/GetFolderById('" + repoParentFolderUniqId + L"')/Files/Add(url='" + repoFileName + L"',overwrite=true)"),
                REST::http::HttpHeaders({
                    std::pair<std::wstring, std::wstring>(L"Authorization", L"Bearer " + accessToken)
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

		virtual  bool IsUpload() const { return false; }

    protected:
        std::istream            _is;
        RmSimpleUploadBuffer    _buf;
    };

}

Result RmRestClient::SharePointOnlineListCollections(const std::wstring& siteUrl,
    const std::wstring& accessToken,
    RmRestStatus& status,
    std::vector<RmRepoFile>& files,
    std::vector<std::wstring> *serverRelUrls)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(siteUrl, res));
    if (p) {
        res = SharePointOnlineListCollections(p.get(), accessToken, status, files, serverRelUrls);
    }
    return res;
}

Result RmRestClient::SharePointOnlineListCollections(NX::REST::http::Connection* conn,
    const std::wstring& accessToken,
    RmRestStatus& status,
    std::vector<RmRepoFile>& files,
    std::vector<std::wstring> *serverRelUrls)
{
    RequestListCollections request(accessToken);
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
            PCJSONARRAY entitySets = root->AsObject()->at(L"d")->AsObject()->at(L"EntitySets")->AsArray();
            const auto count = entitySets->size();
            for (size_t i = 0; i < count; i++) {
                const std::wstring& entitySet = entitySets->at(i)->AsString()->GetString();
                for (const auto& collectionName : collectionNameMap) {
                    if (collectionName.realName == entitySet) {
                        RmRepoFile repoFile;
                        repoFile._name = collectionName.displayName;
                        repoFile._id = L'/' + collectionName.realName;
                        repoFile._path = L'/' + collectionName.displayName;
                        repoFile._folder = true;
                        files.push_back(repoFile);
                        if (serverRelUrls) {
                            serverRelUrls->push_back(L'/' + collectionName.serverRelName);
                        }
                        break;
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

Result RmRestClient::SharePointOnlineListOrSearchFilesInCollection(const std::wstring& siteUrl, _In_opt_ bool* cancelFlag,
    const std::wstring& accessToken,
    const std::wstring& collection,
    const std::wstring& keywords,
    RmRestStatus& status,
    std::vector<RmRepoFile>& files,
    std::vector<__int64>& ids,
    std::vector<std::wstring>& sourceUrls,
    std::vector<std::wstring> *serverRelUrls)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(siteUrl, res));
    if (p) {
        res = SharePointOnlineListOrSearchFilesInCollection(p.get(), cancelFlag, accessToken, collection, keywords, status, files, ids, sourceUrls, serverRelUrls);
    }
    return res;
}

Result RmRestClient::SharePointOnlineListOrSearchFilesInCollection(NX::REST::http::Connection* conn, _In_opt_ bool* cancelFlag,
    const std::wstring& accessToken,
    const std::wstring& collection,
    const std::wstring& keywords,
    RmRestStatus& status,
    std::vector<RmRepoFile>& files,
    std::vector<__int64>& ids,
    std::vector<std::wstring>& sourceUrls,
    std::vector<std::wstring> *serverRelUrls)
{
    const std::wstring *collectionDisplayName = nullptr;
    for (const auto& collectionName : collectionNameMap) {
        if (collectionName.realName == collection) {
            collectionDisplayName = &collectionName.displayName;
        }
    }
    if (!collectionDisplayName) {
        return RESULT(ERROR_NOT_FOUND);
    }

    RequestListOrSearchFilesInCollection request(accessToken, collection, keywords, cancelFlag);
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
            PCJSONARRAY results = root->AsObject()->at(L"d")->AsObject()->at(L"results")->AsArray();
            const auto count = results->size();
            for (size_t i = 0; i < count; i++) {
                PCJSONOBJECT result = results->at(i)->AsObject();
                PCJSONVALUE contentType = result->at(L"ContentType");
                if (!contentType || !contentType->IsString()) {
                    continue;
                }

                const std::wstring& contentTypeStr = contentType->AsString()->GetString();
                if (supportedContentTypes.find(contentTypeStr) == supportedContentTypes.end()) {
                    continue;
                }

                RmRepoFile repoFile;
                repoFile._name = result->at(L"Name")->AsString()->GetString();
                const std::wstring& path = result->at(L"Path")->AsString()->GetString();
                const auto pos = path.find(L"/", 1);
                const std::wstring& pathUnderCollection = (pos == path.npos ? std::wstring() : path.substr(pos));
                repoFile._id = L'/' + collection + pathUnderCollection + L'/' + repoFile._name;
                repoFile._path = L'/' + *collectionDisplayName + pathUnderCollection + L'/' + repoFile._name;
                repoFile._folder = (contentTypeStr == L"Folder");
                const std::wstring& modified = result->at(L"Modified")->AsString()->GetString();
                const auto posStart = std::wstring(L"/Date(").length();
                const auto posEnd = modified.find(L")", posStart);
                repoFile._time = std::stoll(modified.substr(posStart, posEnd - posStart));

                try {
                    // File size is returned in "VirusStatus".
                    repoFile._size = std::stoll(result->at(L"VirusStatus")->AsString()->GetString());
                }
                catch (...) {
                    // Ignore
                }

                const __int64 id = result->at(L"Id")->AsNumber()->ToInt64();
                const std::wstring& sourceUrl = result->at(L"__metadata")->AsObject()->at(L"media_src")->AsString()->GetString();

                files.push_back(repoFile);
                ids.push_back(id);
                sourceUrls.push_back(sourceUrl);
                if (serverRelUrls) {
                    serverRelUrls->push_back(path + L'/' + repoFile._name);
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

Result RmRestClient::SharePointOnlineCreateFolder(const std::wstring& siteUrl,
    const std::wstring& accessToken,
    const RmRepoFile& repoParentDir,
    const std::wstring& parentDirServerRelUrl,
    const std::wstring& folderName,
    RmRestStatus& status,
    RmRepoFile& folder)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(siteUrl, res));
    if (p) {
        res = SharePointOnlineCreateFolder(p.get(), accessToken, repoParentDir, parentDirServerRelUrl, folderName, status, folder);
    }
    return res;
}

Result RmRestClient::SharePointOnlineCreateFolder(NX::REST::http::Connection* conn,
    const std::wstring& accessToken,
    const RmRepoFile& repoParentDir,
    const std::wstring& parentDirServerRelUrl,
    const std::wstring& folderName,
    RmRestStatus& status,
    RmRepoFile& folder)
{
    RequestCreateFolder request(accessToken, parentDirServerRelUrl + L'/' + folderName);
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

        folder._name = folderName;
        folder._id = repoParentDir.GetId() + L'/' + folderName;
        folder._path = repoParentDir.GetPath() + L'/' + folderName;
        folder._folder = true;

    } while (FALSE);


    return RESULT(0);
}

Result RmRestClient::SharePointOnlineDeleteFile(const std::wstring& siteUrl,
    const std::wstring& accessToken,
    const std::wstring& collection,
    __int64 fileId,
    RmRestStatus& status)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(siteUrl, res));
    if (p) {
        res = SharePointOnlineDeleteFile(p.get(), accessToken, collection, fileId, status);
    }
    return res;
}

Result RmRestClient::SharePointOnlineDeleteFile(NX::REST::http::Connection* conn,
    const std::wstring& accessToken,
    const std::wstring& collection,
    __int64 fileId,
    RmRestStatus& status)
{
    RequestDeleteFile request(accessToken, collection, fileId);
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

Result RmRestClient::SharePointOnlineDownloadFile(const std::wstring& siteUrl,
	_In_opt_ bool* cancelFlag,
	const ULONG requestedLength,
    const std::wstring& accessToken,
    const std::wstring& sourceFileSourceUrl,
    const std::wstring& target,
    RmRestStatus& status)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(siteUrl, res));
    if (p) {
        res = SharePointOnlineDownloadFile(p.get(), cancelFlag, requestedLength, accessToken, sourceFileSourceUrl, target, status);
    }
    return res;
}

Result RmRestClient::SharePointOnlineDownloadFile(NX::REST::http::Connection* conn,
	_In_opt_ bool* cancelFlag,
	const ULONG requestedLength,
    const std::wstring& accessToken,
    const std::wstring& sourceFileSourceUrl,
    const std::wstring& target,
    RmRestStatus& status)
{
    RequestDownloadFile request(accessToken, sourceFileSourceUrl, cancelFlag, requestedLength);
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

Result RmRestClient::SharePointOnlineGetFolderUniqId(const std::wstring& siteUrl,
    const std::wstring& accessToken,
    const std::wstring& serverRelUrl,
    RmRestStatus& status,
    std::wstring& uniqId)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(siteUrl, res));
    if (p) {
        res = SharePointOnlineGetFolderUniqId(p.get(), accessToken, serverRelUrl, status, uniqId);
    }
    return res;
}

Result RmRestClient::SharePointOnlineGetFolderUniqId(NX::REST::http::Connection* conn,
    const std::wstring& accessToken,
    const std::wstring& serverRelUrl,
    RmRestStatus& status,
    std::wstring& uniqId)
{
    RequestGetFolderUniqId request(accessToken, serverRelUrl);
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
            uniqId = root->AsObject()->at(L"UniqueId")->AsString()->GetString();
        }
        catch (...) {
            // The JSON data is NOT correct
            res = RESULT2(ERROR_INVALID_DATA, "JSON response is not correct");
        }

    } while (FALSE);


    return RESULT(0);
}

Result RmRestClient::SharePointOnlineUploadFile(const std::wstring& siteUrl,
	_In_opt_ bool* cancelFlag,
    const std::wstring& accessToken,
    const std::wstring& file,
    const RmRepoFile& repoParentFolder,
    const std::wstring& repoParentFolderUniqId,
    const std::wstring& repoFileName,
    RmRestStatus& status,
    RmRepoFile& repoFile)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(siteUrl, res));
    if (p) {
        res = SharePointOnlineUploadFile(p.get(), cancelFlag, accessToken, file, repoParentFolder, repoParentFolderUniqId, repoFileName, status, repoFile);
    }
    return res;
}

Result RmRestClient::SharePointOnlineUploadFile(NX::REST::http::Connection* conn,
	_In_opt_ bool* cancelFlag,
    const std::wstring& accessToken,
    const std::wstring& file,
    const RmRepoFile& repoParentFolder,
    const std::wstring& repoParentFolderUniqId,
    const std::wstring& repoFileName,
    RmRestStatus& status,
    RmRepoFile& repoFile)
{
    RequestUploadFile request(accessToken, repoParentFolderUniqId, repoFileName, file);
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
            repoFile._name = repoFileName;
            repoFile._id = repoParentFolder.GetId() + L'/' + repoFileName;
            repoFile._path = repoParentFolder.GetPath() + L'/' + repoFileName;
            repoFile._folder = false;
            repoFile._size = std::stoll(root->AsObject()->at(L"Length")->AsString()->GetString());
        }
        catch (...) {
            // The JSON data is NOT correct
            res = RESULT2(ERROR_INVALID_DATA, "JSON response is not correct");
        }

    } while (FALSE);


    return RESULT(0);
}
