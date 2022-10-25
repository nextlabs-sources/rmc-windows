
#include "stdafx.h"

#include <nx\rmc\rest.h>
#include <nx\rmc\repo.h>
#include <nx\rmc\project.h>

#include <nx\common\rapidjson.h>
#include <nx\common\stringex.h>
#include <nx\crypt\md5.h>


using namespace NX;
using namespace NX::rapidjson;
using namespace NX::REST::http;


namespace {

	typedef enum PROJECTFILTER {
		ALL = 0,
		OWNEDBYME,
		OWNEDBYOTHER
	} PROJECTFILTER;

	class RequestGetProjects : public REST::http::StringBodyRequest
	{
	public:

		RequestGetProjects(const RmRestCommonParams& param, PROJECTFILTER filter, _In_opt_ bool* cancelFlag)
			: StringBodyRequest(REST::http::methods::GET,
				BuildUrl(filter),
				BuildHeader(param, filter),
				std::vector<LPCWSTR>({ REST::http::mime_types::application_json.c_str(), REST::http::mime_types::text.c_str(), NULL }),
				std::string(), cancelFlag)
		{
		}
		virtual ~RequestGetProjects() {}

	protected:
		std::wstring BuildUrl(PROJECTFILTER filter)
		{
			std::wstring url(L"/rms/rs/project");
			if (ALL != filter) {
				url.append(L"?");
				url.append((OWNEDBYME == filter) ? L"ownedByMe=true" : L"ownedByMe=false");
			}
			return url;
		}
		REST::http::HttpHeaders BuildHeader(const RmRestCommonParams& param, PROJECTFILTER filter)
		{
			REST::http::HttpHeaders headers;
			headers.push_back(std::pair<std::wstring, std::wstring>(L"clientId", param.GetClientId()));
			headers.push_back(std::pair<std::wstring, std::wstring>(L"platformId", param.GetPlatformId()));
			headers.push_back(std::pair<std::wstring, std::wstring>(L"deviceId", param.GetDeviceId()));
			headers.push_back(std::pair<std::wstring, std::wstring>(L"tenant", param.GetTenant()));
			headers.push_back(std::pair<std::wstring, std::wstring>(L"userId", param.GetUserId()));
			headers.push_back(std::pair<std::wstring, std::wstring>(L"ticket", param.GetTicket()));
			if (ALL != filter) {
				headers.push_back(std::pair<std::wstring, std::wstring>(L"ownedByMe", (OWNEDBYME == filter) ? L"true" : L"false"));
			}
			headers.push_back(std::pair<std::wstring, std::wstring>(REST::http::header_names::content_type, REST::http::mime_types::application_json));
			return headers;
		}
	};

	class RequestListProjects : public REST::http::StringBodyRequest
	{
	public:
		RequestListProjects(const RmRestCommonParams& param, __int64 pageId, __int64 pageSize, const std::wstring& orderBy, const std::wstring& searchValue, PROJECTFILTER filter, _In_opt_ bool* cancelFlag)
			: StringBodyRequest(REST::http::methods::GET,
				BuildUrl(pageId, pageSize, orderBy, searchValue, filter),
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
		virtual ~RequestListProjects() {}

	protected:
		std::wstring BuildUrl(__int64 pageId, __int64 pageSize, const std::wstring& orderBy, const std::wstring& searchValue, PROJECTFILTER filter)
		{
			std::wstring url(L"/rms/rs/project?page=");
			url.append(std::to_wstring(pageId));
			url.append(L"&size=");
			url.append(std::to_wstring(pageSize));
			if (!orderBy.empty()) {
				url.append(L"&orderBy=");
				url.append(orderBy);
			}
			if (!searchValue.empty()) {
				url.append(L"&q=name&searchString=");
				url.append(searchValue);
			}
			if (ALL != filter) {
				url.append(L"&ownedByMe=");
				url.append((OWNEDBYME == filter) ? L"true" : L"false");
			}
			return url;
		}
	};

	class RequestGetProjectMetadata : public REST::http::StringBodyRequest
	{
	public:
		RequestGetProjectMetadata(const RmRestCommonParams& param, const std::wstring& projectId)
			: StringBodyRequest(REST::http::methods::GET,
				std::wstring(L"/rms/rs/project/" + projectId),
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
		virtual ~RequestGetProjectMetadata() {}
	};

	class RequestCreateProject : public REST::http::StringBodyRequest
	{
	public:
		RequestCreateProject(const RmRestCommonParams& param,
			const std::wstring& projectName,
			const std::wstring& projectDescription,
			const std::wstring& inviteMsg,
			const std::vector<std::wstring>& inviteMembers)
			: StringBodyRequest(REST::http::methods::PUT,
				std::wstring(L"/rms/rs/project"),
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
				BuildBody(projectName, projectDescription, inviteMsg, inviteMembers))
		{
		}
		virtual ~RequestCreateProject() {}

	protected:
		std::string BuildBody(const std::wstring& projectName, const std::wstring& projectDescription, const std::wstring& inviteMsg, const std::vector<std::wstring>& inviteMembers)
		{
			std::string s;

			std::shared_ptr<JsonObject> root(JsonValue::CreateObject());
			if (root == NULL)
				return s;

			JsonObject* params = JsonValue::CreateObject();
			if (NULL == params)
				return s;
			root->set(L"parameters", params);

			params->set(L"projectName", JsonValue::CreateString(projectName));
			params->set(L"projectDescription", JsonValue::CreateString(projectDescription));
			params->set(L"invitationMsg", JsonValue::CreateString(inviteMsg));

			JsonArray* members = JsonValue::CreateArray();
			if (NULL == members)
				return s;
			params->set(L"emails", members);
			std::for_each(inviteMembers.cbegin(), inviteMembers.cend(), [&](const std::wstring& email) {
				if (!email.empty()) {
					members->push_back(JsonValue::CreateString(email));
				}
			});

			JsonStringWriter<char> writer;
			s = writer.Write(root.get());

			return s;
		}
	};

	class RequestSendInvitation : public REST::http::StringBodyRequest
	{
	public:
		RequestSendInvitation(const RmRestCommonParams& param,
			const std::wstring& projectId,
			const std::vector<std::wstring>& recipients)
			: StringBodyRequest(REST::http::methods::POST,
				std::wstring(L"/rms/rs/project/" + projectId + L"/invite"),
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
				BuildBody(recipients))
		{
		}
		virtual ~RequestSendInvitation() {}

	protected:
		std::string BuildBody(const std::vector<std::wstring>& recipients)
		{
			std::string s;

			std::shared_ptr<JsonObject> root(JsonValue::CreateObject());
			if (root == NULL)
				return s;

			JsonObject* params = JsonValue::CreateObject();
			if (NULL == params)
				return s;
			root->set(L"parameters", params);

			JsonArray* members = JsonValue::CreateArray();
			if (NULL == members)
				return s;
			params->set(L"emails", members);
			std::for_each(recipients.cbegin(), recipients.cend(), [&](const std::wstring& email) {
				if (!email.empty()) {
					members->push_back(JsonValue::CreateString(email));
				}
			});

			JsonStringWriter<char> writer;
			s = writer.Write(root.get());

			return s;
		}
	};

	class RequestAcceptInvitation : public REST::http::StringBodyRequest
	{
	public:
		RequestAcceptInvitation(const RmRestCommonParams& param, const std::wstring& invitationId, const std::wstring& invitationCode)
			: StringBodyRequest(REST::http::methods::GET,
				std::wstring(L"/rms/rs/project/accept?id=" + invitationId + L"&code=" + invitationCode),
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
		virtual ~RequestAcceptInvitation() {}
	};

	class RequestDeclineInvitation : public REST::http::StringBodyRequest
	{
	public:
		RequestDeclineInvitation(const RmRestCommonParams& param, const std::wstring& invitationId, const std::wstring& invitationCode, const std::wstring& reason)
			: StringBodyRequest(REST::http::methods::POST,
				std::wstring(L"/rms/rs/project/decline"),
				REST::http::HttpHeaders({
					std::pair<std::wstring, std::wstring>(L"clientId", param.GetClientId()),
					std::pair<std::wstring, std::wstring>(L"platformId", param.GetPlatformId()),
					std::pair<std::wstring, std::wstring>(L"deviceId", param.GetDeviceId()),
					std::pair<std::wstring, std::wstring>(L"tenant", param.GetTenant()),
					std::pair<std::wstring, std::wstring>(L"userId", param.GetUserId()),
					std::pair<std::wstring, std::wstring>(L"ticket", param.GetTicket()),
					std::pair<std::wstring, std::wstring>(REST::http::header_names::content_type, REST::http::mime_types::application_x_www_form_urlencoded)
				}),
				std::vector<LPCWSTR>({ REST::http::mime_types::application_json.c_str(), REST::http::mime_types::text.c_str(), NULL }),
				BuildBody(invitationId, invitationCode, reason))
		{
		}
		virtual ~RequestDeclineInvitation() {}

	protected:
		std::string BuildBody(const std::wstring& invitationId, const std::wstring& invitationCode, const std::wstring& reason)
		{
			std::wstring ws(L"id=");
			ws.append(invitationId);
			ws.append(L"&code=");
			ws.append(invitationCode);
			std::string s = NX::conv::utf16toutf8(ws);
			s = NX::conv::UrlEncode(s, true);
			if (!reason.empty()) {
				s.append("&declineReason=");
				s.append(NX::conv::UrlEncode(NX::conv::utf16toutf8(reason), false));
			}
			return s;
		}
	};

	class RequestSendInvitationReminder : public REST::http::StringBodyRequest
	{
	public:
		RequestSendInvitationReminder(const RmRestCommonParams& param, const std::wstring& invitationId)
			: StringBodyRequest(REST::http::methods::POST,
				std::wstring(L"/rms/rs/project/sendReminder"),
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
				BuildBody(invitationId))
		{
		}
		virtual ~RequestSendInvitationReminder() {}

	protected:
		std::string BuildBody(const std::wstring& invitationId)
		{
			std::string s("{\"parameters\":{\"invitationId\":");
			s.append(NX::conv::utf16toutf8(invitationId));
			s.append("}}");
			return s;
		}
	};

	class RequestRevokeInvitation : public REST::http::StringBodyRequest
	{
	public:
		RequestRevokeInvitation(const RmRestCommonParams& param, const std::wstring& invitationId)
			: StringBodyRequest(REST::http::methods::POST,
				std::wstring(L"/rms/rs/project/revokeInvite"),
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
				BuildBody(invitationId))
		{
		}
		virtual ~RequestRevokeInvitation() {}

	protected:
		std::string BuildBody(const std::wstring& invitationId)
		{
			std::string s("{\"parameters\":{\"invitationId\":");
			s.append(NX::conv::utf16toutf8(invitationId));
			s.append("}}");
			return s;
		}
	};

	class RequestQueryPendingInvitations : public REST::http::StringBodyRequest
	{
	public:
		RequestQueryPendingInvitations(const RmRestCommonParams& param,
			const std::wstring& projectId,
			__int64 pageId,
			__int64 pageSize,
			const std::wstring& orderBy,
			const std::wstring& searchField,
			const std::wstring& searchValue)
			: StringBodyRequest(REST::http::methods::GET,
				RequestQueryPendingInvitations::BuildUrl(projectId, pageId, pageSize, orderBy, searchField, searchValue),
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
		virtual ~RequestQueryPendingInvitations() {}

	private:
		static std::wstring BuildUrl(const std::wstring& projectId,
			__int64 pageId,
			__int64 pageSize,
			const std::wstring& orderBy,
			const std::wstring& searchField,
			const std::wstring& searchValue)
		{
			// /rms/rs/project/{projectId}/invitation/pending?page=1&size=10&orderBy=creationTime&q=email&searchString=abraham
			std::wstring s(L"/rms/rs/project/");
			s.append(projectId);
			s.append(L"/invitation/pending?page=");
			s.append(NX::i64tos<wchar_t>(pageId));
			s.append(L"&size=");
			s.append(NX::i64tos<wchar_t>(pageSize));
			if (!orderBy.empty()) {
				s.append(L"&orderBy=");
				s.append(orderBy);
			}
			if (!searchField.empty() && !searchValue.empty()) {
				s.append(L"&q=");
				s.append(searchField);
				s.append(L"&searchString=");
				s.append(searchValue);
			}
			return s;
		}
	};

	class RequestQueryPendingUserInvitations : public REST::http::StringBodyRequest
	{
	public:
		RequestQueryPendingUserInvitations(const RmRestCommonParams& param, _In_opt_ bool* cancelFlag)
			: StringBodyRequest(REST::http::methods::GET,
				std::wstring(L"/rms/rs/project/user/invitation/pending"),
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
				std::string(),
				cancelFlag)
		{
		}
		virtual ~RequestQueryPendingUserInvitations() {}
	};
	
	class RequestListFiles : public REST::http::StringBodyRequest
	{
	public:
		RequestListFiles(const RmRestCommonParams& param,
			const std::wstring& projectId,
			__int64 pageId,
			__int64 pageSize,
			const std::wstring& orderBy,
			const std::wstring& parentPath,
			const std::wstring& keywords, _In_opt_ bool* cancelFlag)
			: StringBodyRequest(REST::http::methods::GET,
				std::wstring(L"/rms/rs/project/" + projectId + L"/files?" + NX::conv::utf8toutf16(BuildBody(pageId, pageSize, orderBy, parentPath, keywords))),
				REST::http::HttpHeaders({
					std::pair<std::wstring, std::wstring>(L"clientId", param.GetClientId()),
					std::pair<std::wstring, std::wstring>(L"platformId", param.GetPlatformId()),
					std::pair<std::wstring, std::wstring>(L"deviceId", param.GetDeviceId()),
					std::pair<std::wstring, std::wstring>(L"tenant", param.GetTenant()),
					std::pair<std::wstring, std::wstring>(L"userId", param.GetUserId()),
					std::pair<std::wstring, std::wstring>(L"ticket", param.GetTicket()),
					std::pair<std::wstring, std::wstring>(REST::http::header_names::content_type, REST::http::mime_types::application_x_www_form_urlencoded)
				}),
				std::vector<LPCWSTR>({ REST::http::mime_types::application_json.c_str(), REST::http::mime_types::text_plain.c_str(), NULL }),
				std::string(), cancelFlag)
		{
		}
		virtual ~RequestListFiles() {}
	private:
		std::string BuildBody(__int64 pageId, __int64 pageSize, const std::wstring& orderBy, const std::wstring& parentPath, const std::wstring& keywords)
		{
			std::wstring ws(L"page=");
			ws.append(NX::i64tos<wchar_t>(pageId));
			ws.append(L"&size=");
			ws.append(NX::i64tos<wchar_t>(pageSize));
			ws.append(L"&orderBy=");
			ws.append(orderBy.empty()?L"creationTime,fileName": orderBy);
			if (!orderBy.empty()) {
			}
			if (!parentPath.empty()) {
				ws.append(L"&pathId=");
				ws.append(parentPath);
			}
			if (!keywords.empty()) {
				ws.append(L"&q=name&searchString=");
				ws.append(keywords);
			}
			std::string encoded_url = NX::conv::UrlEncode(NX::conv::utf16toutf8(ws));
			return encoded_url;
		}
	};

	class RequestCreateFolder : public REST::http::StringBodyRequest
	{
	public:
		RequestCreateFolder(const RmRestCommonParams& param, const std::wstring& projectId, const std::wstring& parentDir, const std::wstring& folderName, bool autoRename)
			: StringBodyRequest(REST::http::methods::POST,
				std::wstring(L"/rms/rs/project/" + projectId + L"/createFolder"),
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
				BuildBody(parentDir, folderName, autoRename))
		{
		}
		virtual ~RequestCreateFolder() {}

	protected:
		std::string BuildBody(const std::wstring& parentDir, const std::wstring& folderName, bool autoRename)
		{
			std::string s;

			std::shared_ptr<JsonObject> root(JsonValue::CreateObject());
			if (root == NULL)
				return s;

			JsonObject* params = JsonValue::CreateObject();
			if (NULL == params)
				return s;
			root->set(L"parameters", params);
			params->set(L"parentPathId", JsonValue::CreateString(parentDir));
			params->set(L"name", JsonValue::CreateString(folderName));
			params->set(L"autorename", JsonValue::CreateBool(autoRename));

			JsonStringWriter<char> writer;
			s = writer.Write(root.get());

			return s;
		}
	};

	class RequestDeleteFile : public REST::http::StringBodyRequest
	{
	public:
		RequestDeleteFile(const RmRestCommonParams& param, const std::wstring& projectId, const std::wstring& path)
			: StringBodyRequest(REST::http::methods::POST,
				std::wstring(L"/rms/rs/project/" + projectId + L"/delete"),
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

	protected:
		std::string BuildBody(const std::wstring& path)
		{
			std::string s;

			std::shared_ptr<JsonObject> root(JsonValue::CreateObject());
			if (root == NULL)
				return s;

			JsonObject* params = JsonValue::CreateObject();
			if (NULL == params)
				return s;
			root->set(L"parameters", params);
			params->set(L"pathId", JsonValue::CreateString(path));

			JsonStringWriter<char> writer;
			s = writer.Write(root.get());

			return s;
		}
	};

	class RequestGetFolderMetadata : public REST::http::StringBodyRequest
	{
	public:
		RequestGetFolderMetadata(const RmRestCommonParams& param,
			const std::wstring& projectId,
			const std::wstring& folderId,
			__int64 pageId,
			__int64 pageSize,
			const std::wstring& orderBy,
			__int64 lastModifiedTime,
			_In_opt_ bool* cancelFlag)
			: StringBodyRequest(REST::http::methods::GET,
				std::wstring(L"/rms/rs/project/" + projectId + L"/folderMetadata?" + NX::conv::utf8toutf16(BuildBody(pageId, pageSize, orderBy, folderId, lastModifiedTime))),
				REST::http::HttpHeaders({
			std::pair<std::wstring, std::wstring>(L"clientId", param.GetClientId()),
			std::pair<std::wstring, std::wstring>(L"platformId", param.GetPlatformId()),
			std::pair<std::wstring, std::wstring>(L"deviceId", param.GetDeviceId()),
			std::pair<std::wstring, std::wstring>(L"tenant", param.GetTenant()),
			std::pair<std::wstring, std::wstring>(L"userId", param.GetUserId()),
			std::pair<std::wstring, std::wstring>(L"ticket", param.GetTicket()),
			std::pair<std::wstring, std::wstring>(REST::http::header_names::content_type, REST::http::mime_types::application_x_www_form_urlencoded)
		}),
				std::vector<LPCWSTR>({ REST::http::mime_types::application_json.c_str(), REST::http::mime_types::text_plain.c_str(), NULL }),
				std::string(), cancelFlag)
		{
		}
		virtual ~RequestGetFolderMetadata() {}
	private:
		std::string BuildBody(__int64 pageId, __int64 pageSize, const std::wstring& orderBy, const std::wstring& folderId, __int64 lastModifiedTime)
		{
			std::wstring ws(L"page=");
			ws.append(NX::i64tos<wchar_t>(pageId));
			ws.append(L"&size=");
			ws.append(NX::i64tos<wchar_t>(pageSize));
			ws.append(L"&orderBy=");
			ws.append(orderBy.empty() ? L"creationTime,name" : orderBy);
			ws.append(L"&pathId=");
			ws.append(folderId.empty() ? L"/" : folderId);
			ws.append(L"&lastModified=");
			ws.append(NX::i64tos<wchar_t>(lastModifiedTime));

			std::string encoded_url = NX::conv::UrlEncode(NX::conv::utf16toutf8(ws));
			return encoded_url;
		}
	};

	class RequestDownloadFile : public REST::http::StringBodyRequest
	{
	public:
		RequestDownloadFile(const RmRestCommonParams& param, const std::wstring& projectId, const std::wstring& path, bool forViewer, bool* cancelFlag)
			: StringBodyRequest(REST::http::methods::POST,
				std::wstring(L"/rms/rs/project/" + projectId + L"/download"),
				REST::http::HttpHeaders({
					std::pair<std::wstring, std::wstring>(L"clientId", param.GetClientId()),
					std::pair<std::wstring, std::wstring>(L"platformId", param.GetPlatformId()),
					std::pair<std::wstring, std::wstring>(L"deviceId", param.GetDeviceId()),
					std::pair<std::wstring, std::wstring>(L"tenant", param.GetTenant()),
					std::pair<std::wstring, std::wstring>(L"userId", param.GetUserId()),
					std::pair<std::wstring, std::wstring>(L"ticket", param.GetTicket()),
					std::pair<std::wstring, std::wstring>(REST::http::header_names::content_type, REST::http::mime_types::application_json)
				}),
				std::vector<LPCWSTR>({ REST::http::mime_types::application_json.c_str(), REST::http::mime_types::application_octetstream.c_str(), REST::http::mime_types::text.c_str(), NULL }),
				BuildBody(path, forViewer),
				cancelFlag)
		{
		}
		virtual ~RequestDownloadFile() {}

	protected:
		std::string BuildBody(const std::wstring& path, bool forViewer)
		{
			std::string s;

			std::shared_ptr<JsonObject> root(JsonValue::CreateObject());
			if (root == NULL)
				return s;

			JsonObject* params = JsonValue::CreateObject();
			if (NULL == params)
				return s;
			root->set(L"parameters", params);
			params->set(L"pathId", JsonValue::CreateString(path));
			params->set(L"forViewer", JsonValue::CreateBool(forViewer));

			JsonStringWriter<char> writer;
			s = writer.Write(root.get());

			return s;
		}
	};

	class RequestGetFileMetadata : public REST::http::StringBodyRequest
	{
	public:
		RequestGetFileMetadata(const RmRestCommonParams& param, const std::wstring& projectId, const std::wstring& path)
			: StringBodyRequest(REST::http::methods::POST,
				std::wstring(L"/rms/rs/project/" + projectId + L"/file/metadata"),
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
		virtual ~RequestGetFileMetadata() {}

	protected:
		std::string BuildBody(const std::wstring& path)
		{
			std::string s;

			std::shared_ptr<JsonObject> root(JsonValue::CreateObject());
			if (root == NULL)
				return s;

			JsonObject* params = JsonValue::CreateObject();
			if (NULL == params)
				return s;
			root->set(L"parameters", params);
			params->set(L"pathId", JsonValue::CreateString(path));

			JsonStringWriter<char> writer;
			s = writer.Write(root.get());

			return s;
		}
	};

	static std::wstring BuildSearchUrl(const std::wstring& projectId,
		__int64 pageId,
		__int64 pageSize,
		const std::wstring& orderBy,
		const std::wstring& searchField,
		const std::wstring& searchValue,
		bool returnPicture)
	{
		std::wstring s(L"/rms/rs/project/");
		s.append(projectId);
		s.append(L"/members?page=");
		s.append(NX::i64tos<wchar_t>(pageId));
		s.append(L"&size=");
		s.append(NX::i64tos<wchar_t>(pageSize));
		s.append(L"&orderBy=");
		s.append(orderBy);
		s.append(L"&picture=");
		s.append(returnPicture?L"true":L"false");
		if (!searchField.empty() && !searchValue.empty()) {
			s.append(L"&q=");
			s.append(searchField);
			s.append(L"&searchString=");
			s.append(searchValue);
		}
		return s;
	}
	class RequestListMembers : public REST::http::StringBodyRequest
	{
	public:
		RequestListMembers(const RmRestCommonParams& param,
			const std::wstring& projectId,
			__int64 pageId,
			__int64 pageSize,
			const std::wstring& orderBy,
			const std::wstring& searchField,
			const std::wstring& searchValue,
			bool returnPicture)
			: StringBodyRequest(REST::http::methods::GET,
				BuildSearchUrl(projectId, pageId, pageSize, orderBy, searchField, searchValue, returnPicture),
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
		virtual ~RequestListMembers() {}
	};

	class RequestRemoveMember : public REST::http::StringBodyRequest
	{
	public:
		RequestRemoveMember(const RmRestCommonParams& param, const std::wstring& projectId, const std::wstring& memberId)
			: StringBodyRequest(REST::http::methods::POST,
				std::wstring(L"/rms/rs/project/" + projectId + L"/members/remove"),
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
				BuildBody(memberId))
		{
		}
		virtual ~RequestRemoveMember() {}
	protected:
		std::string BuildBody(const std::wstring& memberId)
		{
			std::string s("{\"parameters\":{\"memberId\":");
			s.append(std::string(memberId.begin(), memberId.end()));
			s.append("}}");
			return s;
		}
	};

	class RequestGetMemberMetadata : public REST::http::StringBodyRequest
	{
	public:
		RequestGetMemberMetadata(const RmRestCommonParams& param, const std::wstring& projectId, const std::wstring& memberId)
			: StringBodyRequest(REST::http::methods::GET,
				std::wstring(L"/rms/rs/project/" + projectId + L"/member/" + memberId),
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
		virtual ~RequestGetMemberMetadata() {}
	};

}

Result RmRestClient::ProjectListProjects(const std::wstring& rms_url, _In_opt_ bool* cancelFlag,
    bool ownedByMe,
    bool ownedByOther,
    RmRestStatus& status,
    std::vector<RmProject>& projects)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(rms_url, res));
    if (p) {
        res = ProjectListProjects(p.get(), cancelFlag, ownedByMe, ownedByOther, status, projects);
    }
    return res;
}

Result RmRestClient::ProjectListProjects(NX::REST::http::Connection* conn, _In_opt_ bool* cancelFlag,
    bool ownedByMe,
	bool ownedByOther,
    RmRestStatus& status,
	std::vector<RmProject>& projects)
{
	PROJECTFILTER filter = (ownedByMe && ownedByOther) ? PROJECTFILTER::ALL : (ownedByMe ? PROJECTFILTER::OWNEDBYME : PROJECTFILTER::OWNEDBYOTHER);
	RequestGetProjects   request(_commonParams, filter, cancelFlag);
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
					if (detail && detail->IsArray()) {
						const size_t count = detail->AsArray()->size();
						for (size_t i = 0; i < count; i++) {
							PCJSONVALUE pv = detail->AsArray()->at(i);
							RmProject project;
							project.FromJsonValue(pv);
							if (!project.Empty())
								projects.push_back(project);
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

Result RmRestClient::ProjectListProjectsEx(const std::wstring& rms_url, bool* cancelFlag,
	__int64 pageId,
	__int64 pageSize,
	const std::wstring& orderBy,
	const std::wstring& searchValue,
	bool ownedByMe,
	bool ownedByOther,
	RmRestStatus& status,
	_Out_opt_ __int64* total,
	std::vector<RmProject>& projects)
{
	Result res = RESULT(0);
	std::shared_ptr<Connection> p(Connect(rms_url, res));
	if (p) {
		res = ProjectListProjectsEx(p.get(), cancelFlag, pageId, pageSize, orderBy, searchValue, ownedByMe, ownedByOther, status, total, projects);
	}
	return res;
}

Result RmRestClient::ProjectListProjectsEx(NX::REST::http::Connection* conn, bool* cancelFlag,
	__int64 pageId,
	__int64 pageSize,
	const std::wstring& orderBy,
	const std::wstring& searchValue,
	bool ownedByMe,
	bool ownedByOther,
	RmRestStatus& status,
	_Out_opt_ __int64* total,
	std::vector<RmProject>& projects)
{
	PROJECTFILTER filter = (ownedByMe && ownedByOther) ? PROJECTFILTER::ALL : (ownedByMe ? PROJECTFILTER::OWNEDBYME : PROJECTFILTER::OWNEDBYOTHER);
	RequestListProjects   request(_commonParams, pageId, pageSize, orderBy, searchValue, filter, cancelFlag);
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
					if (NULL != total) {
						PCJSONVALUE pTotalProjects = results->AsObject()->at(L"totalProjects");
						if (IsValidJsonNumber(pTotalProjects))
							*total = pTotalProjects->AsNumber()->ToInt64();
					}
					PCJSONVALUE pProjectsArray = results->AsObject()->at(L"detail");
					if (IsValidJsonArray(pProjectsArray)) {
						const size_t count = pProjectsArray->AsArray()->size();
						for (size_t i = 0; i < count; i++) {
							PCJSONVALUE pv = pProjectsArray->AsArray()->at(i);
							RmProject project;
							project.FromJsonValue(pv);
							if (!project.Empty())
								projects.push_back(project);
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

Result RmRestClient::ProjectCreateProject(const std::wstring& rms_url,
	const std::wstring& name,
	const std::wstring& description,
	const std::wstring& inviteMsg,
	const std::vector<std::wstring>& members,
    RmRestStatus& status,
    std::wstring& projectId)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(rms_url, res));
    if (p) {
        res = ProjectCreateProject(p.get(), name, description, inviteMsg, members, status, projectId);
    }
    return res;
}

Result RmRestClient::ProjectCreateProject(NX::REST::http::Connection* conn,
    const std::wstring& name,
    const std::wstring& description,
	const std::wstring& inviteMsg,
    const std::vector<std::wstring>& members,
    RmRestStatus& status,
    std::wstring& projectId)
{
	RequestCreateProject   request(_commonParams, name, description, inviteMsg, members);
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
					PCJSONVALUE pId = results->AsObject()->at(L"projectId");
					if (pId && pId->IsNumber()) {
						projectId = NX::i64tos<wchar_t>(pId->AsNumber()->ToInt64());
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

Result RmRestClient::ProjectGetProjectMetadata(const std::wstring& rms_url,
    const std::wstring& projectId,
    RmRestStatus& status,
	RmProject& project)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(rms_url, res));
    if (p) {
        res = ProjectGetProjectMetadata(p.get(), projectId, status, project);
    }
    return res;
}

Result RmRestClient::ProjectGetProjectMetadata(NX::REST::http::Connection* conn,
    const std::wstring& projectId,
    RmRestStatus& status,
	RmProject& project)
{
	RequestGetProjectMetadata   request(_commonParams, projectId);
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
						project.FromJsonValue(detail);
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

Result RmRestClient::ProjectInvitePeople(const std::wstring& rms_url,
    const std::wstring& projectId,
    const std::vector<std::wstring>& inviteMembers,
    RmRestStatus& status,
	std::vector<std::wstring>& alreadyInvited,
	std::vector<std::wstring>& nowInvited,
	std::vector<std::wstring>& alreadyMembers)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(rms_url, res));
    if (p) {
        res = ProjectInvitePeople(p.get(), projectId, inviteMembers, status, alreadyInvited, nowInvited, alreadyMembers);
    }
    return res;
}

Result RmRestClient::ProjectInvitePeople(NX::REST::http::Connection* conn,
    const std::wstring& projectId,
    const std::vector<std::wstring>& inviteMembers,
    RmRestStatus& status,
    std::vector<std::wstring>& alreadyInvited,
    std::vector<std::wstring>& nowInvited,
    std::vector<std::wstring>& alreadyMembers)
{
	RequestSendInvitation   request(_commonParams, projectId, inviteMembers);
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

					PCJSONVALUE pv = results->AsObject()->at(L"alreadyInvited");
					if (pv && pv->IsArray()) {
						const size_t count = pv->AsArray()->size();
						for (size_t i = 0; i < count; i++) {
							PCJSONVALUE item = pv->AsArray()->at(i);
							if (item && item->IsString()) {
								const std::wstring& email = item->AsString()->GetString();
								if (!email.empty())
									alreadyInvited.push_back(email);
							}
						}
					}

					pv = results->AsObject()->at(L"nowInvited");
					if (pv && pv->IsArray()) {
						const size_t count = pv->AsArray()->size();
						for (size_t i = 0; i < count; i++) {
							PCJSONVALUE item = pv->AsArray()->at(i);
							if (item && item->IsString()) {
								const std::wstring& email = item->AsString()->GetString();
								if (!email.empty())
									nowInvited.push_back(email);
							}
						}
					}

					pv = results->AsObject()->at(L"alreadyMembers");
					if (pv && pv->IsArray()) {
						const size_t count = pv->AsArray()->size();
						for (size_t i = 0; i < count; i++) {
							PCJSONVALUE item = pv->AsArray()->at(i);
							if (item && item->IsString()) {
								const std::wstring& email = item->AsString()->GetString();
								if (!email.empty())
									alreadyMembers.push_back(email);
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

Result RmRestClient::ProjectAcceptInvitation(const std::wstring& rms_url,
	const std::wstring& invitationId,
	const std::wstring& invitationCode,
	RmRestStatus& status,
	std::wstring& projectId)
{
	Result res = RESULT(0);
	std::shared_ptr<Connection> p(Connect(rms_url, res));
	if (p) {
		res = ProjectAcceptInvitation(p.get(), invitationId, invitationCode, status, projectId);
	}
	return res;
}

Result RmRestClient::ProjectAcceptInvitation(NX::REST::http::Connection* conn,
	const std::wstring& invitationId,
	const std::wstring& invitationCode,
	RmRestStatus& status,
	std::wstring& projectId)
{
	RequestAcceptInvitation   request(_commonParams, invitationId, invitationCode);
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

					PCJSONVALUE pv = results->AsObject()->at(L"projectId");
					if (pv && pv->IsString()) {
						projectId = pv->AsString()->GetString();
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

Result RmRestClient::ProjectDeclineInvitation(const std::wstring& rms_url,
	const std::wstring& invitationId,
	const std::wstring& invitationCode,
	const std::wstring& reason,
	RmRestStatus& status)
{
	Result res = RESULT(0);
	std::shared_ptr<Connection> p(Connect(rms_url, res));
	if (p) {
		res = ProjectDeclineInvitation(p.get(), invitationId, invitationCode, reason, status);
	}
	return res;
}

Result RmRestClient::ProjectDeclineInvitation(NX::REST::http::Connection* conn,
	const std::wstring& invitationId,
	const std::wstring& invitationCode,
	const std::wstring& reason,
	RmRestStatus& status)
{
	RequestDeclineInvitation   request(_commonParams, invitationId, invitationCode, reason);
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

Result RmRestClient::ProjectSendInvitationReminder(const std::wstring& rms_url,
	const std::wstring& invitationId,
	RmRestStatus& status)
{
	Result res = RESULT(0);
	std::shared_ptr<Connection> p(Connect(rms_url, res));
	if (p) {
		res = ProjectSendInvitationReminder(p.get(), invitationId, status);
	}
	return res;
}

Result RmRestClient::ProjectSendInvitationReminder(NX::REST::http::Connection* conn,
	const std::wstring& invitationId,
	RmRestStatus& status)
{
	RequestSendInvitationReminder   request(_commonParams, invitationId);
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

Result RmRestClient::ProjectRevokeInvitation(const std::wstring& rms_url,
	const std::wstring& invitationId,
	RmRestStatus& status)
{
	Result res = RESULT(0);
	std::shared_ptr<Connection> p(Connect(rms_url, res));
	if (p) {
		res = ProjectRevokeInvitation(p.get(), invitationId, status);
	}
	return res;
}

Result RmRestClient::ProjectRevokeInvitation(NX::REST::http::Connection* conn,
	const std::wstring& invitationId,
	RmRestStatus& status)
{
	RequestRevokeInvitation   request(_commonParams, invitationId);
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

Result RmRestClient::ProjectListPendingInvitations(const std::wstring& rms_url,
    const std::wstring& projectId,
    __int64 pageId,
    __int64 pageSize,
    const std::wstring& orderBy,
    const std::wstring& searchField,
    const std::wstring& searchValue,
    RmRestStatus& status,
	_Out_opt_ __int64* total,
	std::vector<RmProjectInvitation>& invitations)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(rms_url, res));
    if (p) {
        res = ProjectListPendingInvitations(p.get(), projectId, pageId, pageSize, orderBy, searchField, searchValue, status, total, invitations);
    }
    return res;
}

Result RmRestClient::ProjectListPendingInvitations(NX::REST::http::Connection* conn,
    const std::wstring& projectId,
    __int64 pageId,
    __int64 pageSize,
    const std::wstring& orderBy,
    const std::wstring& searchField,
    const std::wstring& searchValue,
    RmRestStatus& status,
	_Out_opt_ __int64* total,
	std::vector<RmProjectInvitation>& invitations)
{
	RequestQueryPendingInvitations request(_commonParams, projectId, pageId, pageSize, orderBy, searchField, searchValue);
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

					PCJSONVALUE pPendingList = results->AsObject()->at(L"pendingList");
					if (IsValidJsonObject(pPendingList)) {

						if (NULL != total) {
							PCJSONVALUE pTotalInvites = pPendingList->AsObject()->at(L"totalInvitations");
							if (IsValidJsonNumber(pTotalInvites)) {
								*total = pTotalInvites->AsNumber()->ToInt64();
							}
						}

						PCJSONVALUE pInvitations = pPendingList->AsObject()->at(L"invitations");
						if (pInvitations && pInvitations->IsArray()) {
							const size_t count = pInvitations->AsArray()->size();
							for (size_t i = 0; i < count; i++) {
								RmProjectInvitation invitation;
								invitation.FromJsonValue(pInvitations->AsArray()->at(i));
								if (!invitation.Empty())
									invitations.push_back(invitation);
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

Result RmRestClient::ProjectListUserPendingInvitations(const std::wstring& rms_url, _In_opt_ bool* cancelFlag,
	RmRestStatus& status,
	std::vector<RmProjectInvitation>& invitations)
{
	Result res = RESULT(0);
	std::shared_ptr<Connection> p(Connect(rms_url, res));
	if (p) {
		res = ProjectListUserPendingInvitations(p.get(), cancelFlag, status, invitations);
	}
	return res;
}

Result RmRestClient::ProjectListUserPendingInvitations(NX::REST::http::Connection* conn, _In_opt_ bool* cancelFlag,
	RmRestStatus& status,
	std::vector<RmProjectInvitation>& invitations)
{
	RequestQueryPendingUserInvitations   request(_commonParams, cancelFlag);
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
					PCJSONVALUE pInvitations = results->AsObject()->at(L"pendingInvitations");
					if (pInvitations && pInvitations->IsArray()) {
						const size_t count = pInvitations->AsArray()->size();
						for (size_t i = 0; i < count; i++) {
							RmProjectInvitation invitation;
							invitation.FromJsonValue(pInvitations->AsArray()->at(i));
							if (!invitation.Empty())
								invitations.push_back(invitation);
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

Result RmRestClient::ProjectListFiles(const std::wstring& rms_url, _In_opt_ bool* cancelFlag,
    const std::wstring& projectId,
    __int64 pageId,
    __int64 pageSize,
    const std::wstring& orderBy,
    const std::wstring& parentPath,
    const std::wstring& keywords,
    RmRestStatus& status,
	_Out_opt_ int* totalFiles,
	_Out_opt_ __int64* usage,
	_Out_opt_ __int64* quota,
	_Out_opt_ __int64* lastUpdatedTime,
    std::vector<RmProjectFile>& files)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(rms_url, res));
    if (p) {
        res = ProjectListFiles(p.get(), cancelFlag, projectId, pageId, pageSize, orderBy, parentPath, keywords, status, totalFiles, usage, quota, lastUpdatedTime, files);
    }
    return res;
}

Result RmRestClient::ProjectListFiles(NX::REST::http::Connection* conn, _In_opt_ bool* cancelFlag,
    const std::wstring& projectId,
    __int64 pageId,
    __int64 pageSize,
    const std::wstring& orderBy,
    const std::wstring& parentPath,
    const std::wstring& keywords,
    RmRestStatus& status,
	_Out_opt_ int* totalFiles,
	_Out_opt_ __int64* usage,
	_Out_opt_ __int64* quota,
	_Out_opt_ __int64* lastUpdatedTime,
	std::vector<RmProjectFile>& files)
{
	RequestListFiles   request(_commonParams, projectId, pageId, pageSize, orderBy, parentPath, keywords, cancelFlag);
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

					PCJSONVALUE pv = NULL;
					if (usage) {
						*usage = 0;
						pv = results->AsObject()->at(L"usage");
						if (pv && pv->IsNumber())
							*usage = pv->AsNumber()->ToInt64();
					}
					if (quota) {
						*quota = 0;
						pv = results->AsObject()->at(L"quota");
						if (pv && pv->IsNumber())
							*quota = pv->AsNumber()->ToInt64();
					}
					if (lastUpdatedTime) {
						*lastUpdatedTime = 0;
						pv = results->AsObject()->at(L"folderLastUpdatedTime");
						if (pv && pv->IsNumber())
							*lastUpdatedTime = pv->AsNumber()->ToInt64();
					}

					PCJSONVALUE detail = results->AsObject()->at(L"detail");
					if (detail && detail->IsObject()) {
						if (totalFiles) {
							pv = detail->AsObject()->at(L"totalFiles");
							if (pv && pv->IsNumber())
								*totalFiles = pv->AsNumber()->ToInt();
						}
						pv = detail->AsObject()->at(L"files");
						if (pv && pv->IsArray()) {
							const size_t count = pv->AsArray()->size();
							for (size_t i = 0; i < count; i++) {
								RmProjectFile file;
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

Result RmRestClient::ProjectCreateFolder(const std::wstring& rms_url,
    const std::wstring& projectId,
	const std::wstring& parentDir,
	const std::wstring& folderName,
	bool autoRename,
	RmRestStatus& status,
	RmRepoFile& folder)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(rms_url, res));
    if (p) {
        res = ProjectCreateFolder(p.get(), projectId, parentDir, folderName, autoRename, status, folder);
    }
    return res;
}

Result RmRestClient::ProjectCreateFolder(NX::REST::http::Connection* conn,
    const std::wstring& projectId,
    const std::wstring& parentDir,
    const std::wstring& folderName,
    bool autoRename,
    RmRestStatus& status,
    RmRepoFile& folder)
{
	RequestCreateFolder   request(_commonParams, projectId, parentDir, folderName, autoRename);
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
					PCJSONVALUE entry = results->AsObject()->at(L"entry");
					if (IsValidJsonObject(entry)) {

						PCJSONVALUE pv = entry->AsObject()->at(L"pathId");
						if (IsValidJsonString(pv))
							folder._id = pv->AsString()->GetString();

						pv = entry->AsObject()->at(L"pathDisplay");
						if (IsValidJsonString(pv))
							folder._path = pv->AsString()->GetString();

						pv = entry->AsObject()->at(L"name");
						if (IsValidJsonString(pv))
							folder._name = pv->AsString()->GetString();

						pv = entry->AsObject()->at(L"size");
						if (IsValidJsonNumber(pv))
							folder._size = pv->AsNumber()->ToInt64();

						pv = entry->AsObject()->at(L"lastModified");
						if (IsValidJsonNumber(pv))
							folder._time = pv->AsNumber()->ToInt64();

						pv = entry->AsObject()->at(L"folder");
						if (IsValidJsonBool(pv))
							folder._folder = pv->AsBool()->GetBool();
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

Result RmRestClient::ProjectGetFolderMetadata(const std::wstring& rms_url, _In_opt_ bool* cancelFlag,
	const std::wstring& projectId,
	const std::wstring& folderId,
	__int64 pageId,
	__int64 pageSize,
	const std::wstring& orderBy,
	__int64 lastModifiedTime,
	RmRestStatus& status,
	_Out_opt_ __int64* usage,
	_Out_opt_ __int64* quota,
	_Out_opt_ __int64* totalRecords,
	std::vector<RmProjectFile>& files)
{
	Result res = RESULT(0);
	std::shared_ptr<Connection> p(Connect(rms_url, res));
	if (p) {
		res = ProjectGetFolderMetadata(p.get(), cancelFlag, projectId, folderId, pageId, pageSize, orderBy, lastModifiedTime, status, usage, quota, totalRecords, files);
	}
	return res;
}

Result RmRestClient::ProjectGetFolderMetadata(NX::REST::http::Connection* conn, _In_opt_ bool* cancelFlag,
	const std::wstring& projectId,
	const std::wstring& folderId,
	__int64 pageId,
	__int64 pageSize,
	const std::wstring& orderBy,
	__int64 lastModifiedTime,
	RmRestStatus& status,
	_Out_opt_ __int64* usage,
	_Out_opt_ __int64* quota,
	_Out_opt_ __int64* totalRecords,
	std::vector<RmProjectFile>& files)
{
	RequestGetFolderMetadata request(_commonParams, projectId, folderId, pageId, pageSize, orderBy, lastModifiedTime, cancelFlag);
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
						if (totalRecords) {
							pv = detail->AsObject()->at(L"totalFiles");
							if (pv && pv->IsNumber())
								*totalRecords = pv->AsNumber()->ToInt();
						}
						if (usage) {
							*usage = 0;
							pv = detail->AsObject()->at(L"usage");
							if (pv && pv->IsNumber())
								*usage = pv->AsNumber()->ToInt64();
						}
						if (quota) {
							*quota = 0;
							pv = detail->AsObject()->at(L"quota");
							if (pv && pv->IsNumber())
								*quota = pv->AsNumber()->ToInt64();
						}
						pv = detail->AsObject()->at(L"files");
						if (pv && pv->IsArray()) {
							const size_t count = pv->AsArray()->size();
							for (size_t i = 0; i < count; i++) {
								RmProjectFile file;
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

Result RmRestClient::ProjectDeleteFile(const std::wstring& rms_url,
    const std::wstring& projectId,
    const std::wstring& filePath,
    RmRestStatus& status)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(rms_url, res));
    if (p) {
        res = ProjectDeleteFile(p.get(), projectId, filePath, status);
    }
    return res;
}

Result RmRestClient::ProjectDeleteFile(NX::REST::http::Connection* conn,
    const std::wstring& projectId,
    const std::wstring& filePath,
    RmRestStatus& status)
{
	RequestDeleteFile   request(_commonParams, projectId, filePath);
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

Result RmRestClient::ProjectUploadFile(const std::wstring& rms_url,
	_In_opt_ bool* cancelFlag,
    const std::wstring& projectId,
	const std::wstring& destDirPathId,
	const std::wstring& destFileName,
	const std::wstring& srcFilePath,
    const std::wstring& fileRights,
    const NXL::FileTags& fileTags,
    RmRestStatus& status,
	RmProjectFile& newFile)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(rms_url, res));
    if (p) {
        res = ProjectUploadFile(p.get(), cancelFlag, projectId, destDirPathId, destFileName, srcFilePath, fileRights, fileTags, status, newFile);
    }
    return res;
}

static std::string ProjectBuildUploadParameter(const std::wstring& fileName, const std::wstring& parentPathId, const std::wstring& fileRights, const NXL::FileTags& fileTags)
{
	std::wstring parentDirPathId(parentPathId);
	if (!NX::iend_with<wchar_t>(parentDirPathId, L"/"))
		parentDirPathId.append(L"/");
	std::shared_ptr<NX::rapidjson::JsonValue> root(NX::rapidjson::JsonValue::CreateObject());
	NX::rapidjson::JsonValue* params = NX::rapidjson::JsonValue::CreateObject();
	params->AsObject()->set(L"name", NX::rapidjson::JsonValue::CreateString(fileName));
	params->AsObject()->set(L"rightsJSON", NX::rapidjson::JsonValue::CreateString(fileRights));
	params->AsObject()->set(L"parentPathId", NX::rapidjson::JsonValue::CreateString(parentDirPathId));
	NX::rapidjson::JsonValue* tags = NX::rapidjson::JsonValue::CreateObject();
	if (tags) {
		std::for_each(fileTags.GetTags().cbegin(), fileTags.GetTags().cend(), [&](const NXL::FileTags::FileTag& item) {
			if (!item.first.empty()) {
				NX::rapidjson::JsonValue* tagValues = NX::rapidjson::JsonValue::CreateArray();
				std::for_each(item.second.begin(), item.second.end(), [&](const std::wstring& v) {
					if (!v.empty()) {
						tagValues->AsArray()->push_back(NX::rapidjson::JsonValue::CreateString(v));
					}
				});
				tags->AsObject()->set(item.first, tagValues);
			}
		});
		params->AsObject()->set(L"tags", tags);
	}
	root->AsObject()->set(L"parameters", params);
	NX::rapidjson::JsonStringWriter<char> writer;
	std::string s = writer.Write(root.get());
	root.reset();
	return s;
}

Result RmRestClient::ProjectUploadFile(NX::REST::http::Connection* conn,
	_In_opt_ bool* cancelFlag,
    const std::wstring& projectId,
	const std::wstring& destDirPathId,
	const std::wstring& destFileName,
	const std::wstring& srcFilePath,
    const std::wstring& fileRights,
    const NXL::FileTags& fileTags,
    RmRestStatus& status,
	RmProjectFile& newFile)
{
	auto pos = srcFilePath.rfind(L'\\');
	const std::wstring& srcFileName = (pos == std::wstring::npos) ? srcFilePath : srcFilePath.substr(pos+1);
	RmUploadRequest request(_commonParams, std::wstring(L"/rms/rs/project/" + projectId + L"/upload"), ProjectBuildUploadParameter(destFileName.empty() ? srcFileName : destFileName, destDirPathId, fileRights, fileTags), srcFilePath, cancelFlag);
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
						newFile._name = pv->AsString()->GetString();
					}

					pv = results->AsObject()->at(L"pathId");
					if (pv && pv->IsString()) {
						newFile._path = pv->AsString()->GetString();
					}

					pv = results->AsObject()->at(L"pathDisplay");
					if (pv && pv->IsString()) {
						newFile._displayPath = pv->AsString()->GetString();
					}

					pv = results->AsObject()->at(L"lastModified");
					if (pv && pv->IsNumber()) {
						newFile._lastModifiedTime = pv->AsNumber()->ToInt64();
					}

					pv = results->AsObject()->at(L"size");
					if (pv && pv->IsNumber()) {
						newFile._size = pv->AsNumber()->ToInt64();
					}

					pv = results->AsObject()->at(L"folder");
					if (pv && pv->IsBool()) {
						newFile._folder = pv->AsBool()->GetBool();
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

Result RmRestClient::ProjectDownloadFile(const std::wstring& rms_url,
	_In_opt_ bool* cancelFlag,
    const std::wstring& projectId,
    const std::wstring& filePath,
    const std::wstring& downloadPath,
    bool forViewer,
    RmRestStatus& status,
	_Out_ std::wstring& preferredFileName)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(rms_url, res));
    if (p) {
        res = ProjectDownloadFile(p.get(), cancelFlag, projectId, filePath, downloadPath, forViewer, status, preferredFileName);
    }
    return res;
}

Result RmRestClient::ProjectDownloadFile(NX::REST::http::Connection* conn,
	_In_opt_ bool* cancelFlag,
    const std::wstring& projectId,
    const std::wstring& filePath,
    const std::wstring& downloadPath,
    bool forViewer,
    RmRestStatus& status,
	_Out_ std::wstring& preferredFileName)
{
	RequestDownloadFile   request(_commonParams, projectId, filePath, forViewer, cancelFlag);
	FileResponse  response(downloadPath);

	Result res = conn->SendRequest(request, response);
	if (!res) {
		return res;
	}

	status._status = response.GetStatus();
	status._phrase = response.GetPhrase();

	if (status) {
		const HttpHeaders& headers = response.GetHeaders();
		auto it = std::find_if(headers.begin(), headers.end(), [](const std::pair<std::wstring, std::wstring>& item)->bool {
			return (0 == _wcsicmp(item.first.c_str(), L"Content-Disposition"));
		});
		if (it != headers.end()) {
			auto pos = (*it).second.find(L"UTF-8''");
			if (pos != std::wstring::npos) {
				preferredFileName = NX::conv::utf8toutf16(NX::conv::UrlDecode(NX::conv::utf16toutf8((*it).second.substr(pos + 7))));
			}
		}
	}

	return RESULT(0);
}

Result RmRestClient::ProjectGetFileMetadata(const std::wstring& rms_url,
    const std::wstring& projectId,
    const std::wstring& filePath,
    RmRestStatus& status,
	RmProjectFileMetadata& metadata)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(rms_url, res));
    if (p) {
        res = ProjectGetFileMetadata(p.get(), projectId, filePath, status, metadata);
    }
    return res;
}

Result RmRestClient::ProjectGetFileMetadata(NX::REST::http::Connection* conn,
    const std::wstring& projectId,
    const std::wstring& filePath,
    RmRestStatus& status,
	RmProjectFileMetadata& metadata)
{
	RequestGetFileMetadata   request(_commonParams, projectId, filePath);
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
					PCJSONVALUE fileInfo = results->AsObject()->at(L"fileInfo");
					if (fileInfo && fileInfo->IsObject()) {
						PCJSONVALUE pv = fileInfo->AsObject()->at(L"pathId");
						if (pv && pv->IsString()) {
							metadata._path = pv->AsString()->GetString();
						}
						pv = fileInfo->AsObject()->at(L"pathDisplay");
						if (pv && pv->IsString()) {
							metadata._displayPath = pv->AsString()->GetString();
						}
						pv = fileInfo->AsObject()->at(L"fileName");
						if (pv && pv->IsString()) {
							metadata._name = pv->AsString()->GetString();
						}
						pv = fileInfo->AsObject()->at(L"fileSize");
						if (pv && pv->IsNumber()) {
							metadata._size = pv->AsNumber()->ToInt64();
						}
						pv = fileInfo->AsObject()->at(L"lastModifiedTime");
						if (pv && pv->IsNumber()) {
							metadata._lastModifiedTime = pv->AsNumber()->ToInt64();
						}
						pv = fileInfo->AsObject()->at(L"rights");
						if (pv && pv->IsArray()) {
							std::for_each(pv->AsArray()->cbegin(), pv->AsArray()->cend(), [&](const JsonArray::value_type& item) {
								if (item && item->IsString())
									metadata._rights.push_back(item->AsString()->GetString());
							});
						}
						pv = fileInfo->AsObject()->at(L"nxl");
						if (pv && pv->IsBool()) {
							metadata._isNXL = pv->AsBool()->GetBool();
						}
						pv = fileInfo->AsObject()->at(L"owner");
						if (pv && pv->IsBool()) {
							metadata._ownedByMe = pv->AsBool()->GetBool();
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

Result RmRestClient::ProjectListMembers(const std::wstring& rms_url,
    const std::wstring& projectId,
    __int64 pageId,
    __int64 pageSize,
    const std::wstring& orderBy,
    const std::wstring& searchField,
    const std::wstring& searchValue,
    bool returnPicture,
    RmRestStatus& status,
	_Out_opt_ int* totalMembers,
	std::vector<RmProjectMember>& members)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(rms_url, res));
    if (p) {
        res = ProjectListMembers(p.get(), projectId, pageId, pageSize, orderBy, searchField, searchValue, returnPicture, status, totalMembers, members);
    }
    return res;
}

Result RmRestClient::ProjectListMembers(NX::REST::http::Connection* conn,
    const std::wstring& projectId,
    __int64 pageId,
    __int64 pageSize,
    const std::wstring& orderBy,
    const std::wstring& searchField,
    const std::wstring& searchValue,
    bool returnPicture,
    RmRestStatus& status,
	_Out_opt_ int* totalMembers,
	std::vector<RmProjectMember>& members)
{
	RequestListMembers   request(_commonParams, projectId, pageId, pageSize, orderBy, searchField, searchValue, returnPicture);
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
						if (totalMembers) {
							*totalMembers = 0;
							pv = detail->AsObject()->at(L"totalMembers");
							if (pv && pv->IsNumber()) {
								*totalMembers = pv->AsNumber()->ToInt();
							}
						}
						pv = detail->AsObject()->at(L"members");
						if (pv && pv->IsArray()) {
							std::for_each(pv->AsArray()->cbegin(), pv->AsArray()->cend(), [&](const JsonArray::value_type& item) {
								if (item && item->IsObject()) {
									RmProjectMember member;
									member.FromJsonValue(item);
									if(!member.Empty())
										members.push_back(member);
								}
							});
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

Result RmRestClient::ProjectRemoveMember(const std::wstring& rms_url,
    const std::wstring& projectId,
    const std::wstring& memberId,
    RmRestStatus& status)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(rms_url, res));
    if (p) {
        res = ProjectRemoveMember(p.get(), projectId, memberId, status);
    }
    return res;
}

Result RmRestClient::ProjectRemoveMember(NX::REST::http::Connection* conn,
    const std::wstring& projectId,
    const std::wstring& memberId,
    RmRestStatus& status)
{
	RequestRemoveMember   request(_commonParams, projectId, memberId);
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

Result RmRestClient::ProjectGetMemberMetadata(const std::wstring& rms_url,
    const std::wstring& projectId,
    const std::wstring& memberId,
    RmRestStatus& status,
	RmProjectMember& member)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(rms_url, res));
    if (p) {
        res = ProjectGetMemberMetadata(p.get(), projectId, memberId, status, member);
    }
    return res;
}

Result RmRestClient::ProjectGetMemberMetadata(NX::REST::http::Connection* conn,
    const std::wstring& projectId,
    const std::wstring& memberId,
    RmRestStatus& status,
	RmProjectMember& member)
{
	RequestGetMemberMetadata   request(_commonParams, projectId, memberId);
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
						member.FromJsonValue(detail);
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
