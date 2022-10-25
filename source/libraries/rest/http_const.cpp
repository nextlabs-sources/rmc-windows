
#include "stdafx.h"

#include <nx\rest\http_const.h>

#include <map>

using namespace NX::REST;

const std::wstring http::methods::GET(L"GET");
const std::wstring http::methods::POST(L"POST");
const std::wstring http::methods::PUT(L"PUT");
const std::wstring http::methods::DEL(L"DELETE");
const std::wstring http::methods::HEAD(L"HEAD");
const std::wstring http::methods::OPTIONS(L"OPTIONS");
const std::wstring http::methods::TRCE(L"TRACE");
const std::wstring http::methods::CONNECT(L"CONNECT");
const std::wstring http::methods::MERGE(L"MERGE");
const std::wstring http::methods::PATCH(L"PATCH");


// 1xx Informational
const http::http_status_and_phrase http::status_codes::Continue = { 100, L"Continue" };
const http::http_status_and_phrase http::status_codes::SwitchingProtocols = { 101, L"Switching Protocols" };
const http::http_status_and_phrase http::status_codes::Processing = { 102, L"Processing" };
// 2xx Success
const http::http_status_and_phrase http::status_codes::OK = { 200, L"OK" };
const http::http_status_and_phrase http::status_codes::Created = { 201, L"Created" };
const http::http_status_and_phrase http::status_codes::Accepted = { 202, L"Accepted" };
const http::http_status_and_phrase http::status_codes::NonAuthInfo = { 203, L"Non-Authoritative Information" };
const http::http_status_and_phrase http::status_codes::NoContent = { 204, L"No Content" };
const http::http_status_and_phrase http::status_codes::ResetContent = { 205, L"Reset Content" };
const http::http_status_and_phrase http::status_codes::PartialContent = { 206, L"Partial Content" };
const http::http_status_and_phrase http::status_codes::MultiStatus = { 207, L"Multi-Status" };  // WebDAV; RFC 4918
const http::http_status_and_phrase http::status_codes::AlreadyReported = { 208, L"Already Reported" };  // WebDAV; RFC 5842
const http::http_status_and_phrase http::status_codes::IMUsed = { 226, L"IM Used" };  // RFC 3229
// 3xx Redirection
const http::http_status_and_phrase http::status_codes::MultipleChoices = { 300, L"Multiple Choices" };
const http::http_status_and_phrase http::status_codes::MovedPermanently = { 301, L"Moved Permanently" };
const http::http_status_and_phrase http::status_codes::Found = { 302, L"Found" };
const http::http_status_and_phrase http::status_codes::SeeOther = { 303, L"See Other" };
const http::http_status_and_phrase http::status_codes::NotModified = { 304, L"Not Modified" };
const http::http_status_and_phrase http::status_codes::UseProxy = { 305, L"Use Proxy" };
const http::http_status_and_phrase http::status_codes::SwitchProxy = { 306, L"Switch Proxy" };
const http::http_status_and_phrase http::status_codes::TemporaryRedirect = { 307, L"Temporary Redirect" };
const http::http_status_and_phrase http::status_codes::PermanentRedirect = { 308, L"Permanent Redirect" };    // RFC 7538
// 4xx Client Error
const http::http_status_and_phrase http::status_codes::BadRequest = { 400, L"Bad Request" };
const http::http_status_and_phrase http::status_codes::Unauthorized = { 401, L"Unauthorized" };
const http::http_status_and_phrase http::status_codes::PaymentRequired = { 402, L"Payment Required" };
const http::http_status_and_phrase http::status_codes::Forbidden = { 403, L"Forbidden" };
const http::http_status_and_phrase http::status_codes::NotFound = { 404, L"Not Found" };
const http::http_status_and_phrase http::status_codes::MethodNotAllowed = { 405, L"Method Not Allowed" };
const http::http_status_and_phrase http::status_codes::NotAcceptable = { 406, L"Not Acceptable" };
const http::http_status_and_phrase http::status_codes::ProxyAuthRequired = { 407, L"Proxy Authentication Required" };
const http::http_status_and_phrase http::status_codes::RequestTimeout = { 408, L"Request Time-out" };
const http::http_status_and_phrase http::status_codes::Conflict = { 409, L"Conflict" };
const http::http_status_and_phrase http::status_codes::Gone = { 410, L"Gone" };
const http::http_status_and_phrase http::status_codes::LengthRequired = { 411, L"Length Required" };
const http::http_status_and_phrase http::status_codes::PreconditionFailed = { 412, L"Precondition Failed" };
const http::http_status_and_phrase http::status_codes::PayloadTooLarge = { 413, L"Payload Too Large" };
const http::http_status_and_phrase http::status_codes::UriTooLong = { 414, L"URI Too Long" };
const http::http_status_and_phrase http::status_codes::UnsupportedMediaType = { 415, L"Unsupported Media Type" };
const http::http_status_and_phrase http::status_codes::RangeNotSatisfiable = { 416, L"Range Not Satisfiable" };
const http::http_status_and_phrase http::status_codes::ExpectationFailed = { 417, L"Expectation Failed" };
const http::http_status_and_phrase http::status_codes::IAmATeapot = { 418, L"I'm a teapot" };    // RFC 2324
const http::http_status_and_phrase http::status_codes::MisdirectedRequest = { 421, L"Misdirected Request" };    // RFC 7540
const http::http_status_and_phrase http::status_codes::UnprocessableEntity = { 422, L"Unprocessable Entity" };    // WebDAV; RFC 4918
const http::http_status_and_phrase http::status_codes::Locked = { 423, L"Locked" };    // WebDAV; RFC 4918
const http::http_status_and_phrase http::status_codes::FailedDependency = { 424, L"Failed Dependency" };    // WebDAV; RFC 4918
const http::http_status_and_phrase http::status_codes::UpgradeRequired = { 426, L"Upgrade Required" };
const http::http_status_and_phrase http::status_codes::PreconditionRequired = { 428, L"Precondition Required" };    // RFC 6585
const http::http_status_and_phrase http::status_codes::TooManyRequests = { 429, L"Too Many Requests" };    // RFC 6585
const http::http_status_and_phrase http::status_codes::RequestHeaderFieldsTooLarge = { 431, L"Request Header Fields Too Large" };    // RFC 6585
const http::http_status_and_phrase http::status_codes::UnavailableForLegalReasons = { 451, L"Unavailable For Legal Reasons" };    // RFC 7725
// 5xx Server Error
const http::http_status_and_phrase http::status_codes::InternalError = { 500, L"Internal Server Error" };
const http::http_status_and_phrase http::status_codes::NotImplemented = { 501, L"Not Implemented" };
const http::http_status_and_phrase http::status_codes::BadGateway = { 502, L"Bad Gateway" };
const http::http_status_and_phrase http::status_codes::ServiceUnavailable = { 503, L"Service Unavailable" };
const http::http_status_and_phrase http::status_codes::GatewayTimeout = { 504, L"Gateway Time-out" };
const http::http_status_and_phrase http::status_codes::HttpVersionNotSupported = { 505, L"HTTP Version Not Supported" };
const http::http_status_and_phrase http::status_codes::VariantAlsoNegotiates = { 506, L"Variant Also Negotiates" };    // RFC 2295
const http::http_status_and_phrase http::status_codes::InsufficientStorage = { 507, L"Insufficient Storage" };    // WebDAV; RFC 4918
const http::http_status_and_phrase http::status_codes::LoopDetected = { 508, L"Loop Detected" };    // WebDAV; RFC 5842
const http::http_status_and_phrase http::status_codes::NotExtended = { 510, L"Not Extended" };    // RFC 2774
const http::http_status_and_phrase http::status_codes::NetworkAuthenticationRequired = { 511, L"Network Authentication Required" };    // RFC 6585

#ifndef NX_HTTP_NO_STATUS_PHRASE_MAP
const wchar_t* http::HttpStatusToPhrase(unsigned short status)
{
    static const std::map<unsigned short, const wchar_t*> status_to_phrase_map = {
        // 1xx Informational
        { http::status_codes::Continue.id, http::status_codes::Continue.phrase },
        { http::status_codes::SwitchingProtocols.id, http::status_codes::SwitchingProtocols.phrase },
        { http::status_codes::Processing.id, http::status_codes::Processing.phrase },
        // 2xx Success
        { http::status_codes::OK.id, http::status_codes::OK.phrase },
        { http::status_codes::Created.id, http::status_codes::Created.phrase },
        { http::status_codes::Accepted.id, http::status_codes::Accepted.phrase },
        { http::status_codes::NonAuthInfo.id, http::status_codes::NonAuthInfo.phrase },
        { http::status_codes::NoContent.id, http::status_codes::NoContent.phrase },
        { http::status_codes::ResetContent.id, http::status_codes::ResetContent.phrase },
        { http::status_codes::PartialContent.id, http::status_codes::PartialContent.phrase },
        { http::status_codes::MultiStatus.id, http::status_codes::MultiStatus.phrase },
        { http::status_codes::AlreadyReported.id, http::status_codes::AlreadyReported.phrase },
        { http::status_codes::IMUsed.id, http::status_codes::IMUsed.phrase },
        // 3xx Redirection
        { http::status_codes::MultipleChoices.id, http::status_codes::MultipleChoices.phrase },
        { http::status_codes::MovedPermanently.id, http::status_codes::MovedPermanently.phrase },
        { http::status_codes::Found.id, http::status_codes::Found.phrase },
        { http::status_codes::SeeOther.id, http::status_codes::SeeOther.phrase },
        { http::status_codes::NotModified.id, http::status_codes::NotModified.phrase },
        { http::status_codes::UseProxy.id, http::status_codes::UseProxy.phrase },
        { http::status_codes::SwitchProxy.id, http::status_codes::SwitchProxy.phrase },
        { http::status_codes::TemporaryRedirect.id, http::status_codes::TemporaryRedirect.phrase },
        { http::status_codes::PermanentRedirect.id, http::status_codes::PermanentRedirect.phrase },
        // 4xx Client Error
        { http::status_codes::BadRequest.id, http::status_codes::BadRequest.phrase },
        { http::status_codes::Unauthorized.id, http::status_codes::Unauthorized.phrase },
        { http::status_codes::PaymentRequired.id, http::status_codes::PaymentRequired.phrase },
        { http::status_codes::Forbidden.id, http::status_codes::Forbidden.phrase },
        { http::status_codes::NotFound.id, http::status_codes::NotFound.phrase },
        { http::status_codes::MethodNotAllowed.id, http::status_codes::MethodNotAllowed.phrase },
        { http::status_codes::NotAcceptable.id, http::status_codes::NotAcceptable.phrase },
        { http::status_codes::ProxyAuthRequired.id, http::status_codes::ProxyAuthRequired.phrase },
        { http::status_codes::RequestTimeout.id, http::status_codes::RequestTimeout.phrase },
        { http::status_codes::Conflict.id, http::status_codes::Conflict.phrase },
        { http::status_codes::Gone.id, http::status_codes::Gone.phrase },
        { http::status_codes::LengthRequired.id, http::status_codes::LengthRequired.phrase },
        { http::status_codes::PreconditionFailed.id, http::status_codes::PreconditionFailed.phrase },
        { http::status_codes::PayloadTooLarge.id, http::status_codes::PayloadTooLarge.phrase },
        { http::status_codes::UriTooLong.id, http::status_codes::UriTooLong.phrase },
        { http::status_codes::UnsupportedMediaType.id, http::status_codes::UnsupportedMediaType.phrase },
        { http::status_codes::RangeNotSatisfiable.id, http::status_codes::RangeNotSatisfiable.phrase },
        { http::status_codes::ExpectationFailed.id, http::status_codes::ExpectationFailed.phrase },
        { http::status_codes::IAmATeapot.id, http::status_codes::IAmATeapot.phrase },
        { http::status_codes::MisdirectedRequest.id, http::status_codes::MisdirectedRequest.phrase },
        { http::status_codes::UnprocessableEntity.id, http::status_codes::UnprocessableEntity.phrase },
        { http::status_codes::FailedDependency.id, http::status_codes::FailedDependency.phrase },
        { http::status_codes::UpgradeRequired.id, http::status_codes::UpgradeRequired.phrase },
        { http::status_codes::PreconditionRequired.id, http::status_codes::PreconditionRequired.phrase },
        { http::status_codes::TooManyRequests.id, http::status_codes::TooManyRequests.phrase },
        { http::status_codes::RequestHeaderFieldsTooLarge.id, http::status_codes::RequestHeaderFieldsTooLarge.phrase },
        { http::status_codes::UnavailableForLegalReasons.id, http::status_codes::UnavailableForLegalReasons.phrase },
        // 5xx Server Error
        { http::status_codes::InternalError.id, http::status_codes::InternalError.phrase },
        { http::status_codes::NotImplemented.id, http::status_codes::NotImplemented.phrase },
        { http::status_codes::BadGateway.id, http::status_codes::BadGateway.phrase },
        { http::status_codes::ServiceUnavailable.id, http::status_codes::ServiceUnavailable.phrase },
        { http::status_codes::GatewayTimeout.id, http::status_codes::GatewayTimeout.phrase },
        { http::status_codes::HttpVersionNotSupported.id, http::status_codes::HttpVersionNotSupported.phrase },
        { http::status_codes::VariantAlsoNegotiates.id, http::status_codes::VariantAlsoNegotiates.phrase },
        { http::status_codes::InsufficientStorage.id, http::status_codes::InsufficientStorage.phrase },
        { http::status_codes::LoopDetected.id, http::status_codes::LoopDetected.phrase },
        { http::status_codes::NotExtended.id, http::status_codes::NotExtended.phrase },
        { http::status_codes::NetworkAuthenticationRequired.id, http::status_codes::NetworkAuthenticationRequired.phrase }
    };

    auto it = status_to_phrase_map.find(status);
    return (it == status_to_phrase_map.end()) ? L"Unknown Error" : (*it).second;
}
#endif


const std::wstring http::header_names::accept(L"Accept");
const std::wstring http::header_names::accept_charset(L"Accept-Charset");
const std::wstring http::header_names::accept_encoding(L"Accept-Encoding");
const std::wstring http::header_names::accept_language(L"Accept-Language");
const std::wstring http::header_names::accept_ranges(L"Accept-Ranges");
const std::wstring http::header_names::age(L"Age");
const std::wstring http::header_names::allow(L"Allow");
const std::wstring http::header_names::authorization(L"Authorization");
const std::wstring http::header_names::cache_control(L"Cache-Control");
const std::wstring http::header_names::connection(L"Connection");
const std::wstring http::header_names::content_disposition(L"Content-Disposition");
const std::wstring http::header_names::content_encoding(L"Content-Encoding");
const std::wstring http::header_names::content_language(L"Content-Language");
const std::wstring http::header_names::content_length(L"Content-Length");
const std::wstring http::header_names::content_location(L"Content-Location");
const std::wstring http::header_names::content_md5(L"Content-MD5");
const std::wstring http::header_names::content_range(L"Content-Range");
const std::wstring http::header_names::content_type(L"Content-Type");
const std::wstring http::header_names::date(L"Date");
const std::wstring http::header_names::etag(L"ETag");
const std::wstring http::header_names::expect(L"Expect");
const std::wstring http::header_names::expires(L"Expires");
const std::wstring http::header_names::from(L"From");
const std::wstring http::header_names::host(L"Host");
const std::wstring http::header_names::if_match(L"If-Match");
const std::wstring http::header_names::if_modified_since(L"If-Modified-Since");
const std::wstring http::header_names::if_none_match(L"If-None-Match");
const std::wstring http::header_names::if_range(L"If-Range");
const std::wstring http::header_names::if_unmodified_since(L"If-Unmodified-Since");
const std::wstring http::header_names::last_modified(L"Last-Modified");
const std::wstring http::header_names::location(L"Location");
const std::wstring http::header_names::max_forwards(L"Max-Forwards");
const std::wstring http::header_names::pragma(L"Pragma");
const std::wstring http::header_names::proxy_authenticate(L"Proxy-Authenticate");
const std::wstring http::header_names::proxy_authorization(L"Proxy-Authorization");
const std::wstring http::header_names::range(L"Range");
const std::wstring http::header_names::referer(L"Referer");
const std::wstring http::header_names::retry_after(L"Retry-After");
const std::wstring http::header_names::server(L"Server");
const std::wstring http::header_names::te(L"TE");
const std::wstring http::header_names::trailer(L"Trailer");
const std::wstring http::header_names::transfer_encoding(L"Transfer-Encoding");
const std::wstring http::header_names::upgrade(L"Upgrade");
const std::wstring http::header_names::user_agent(L"User-Agent");
const std::wstring http::header_names::vary(L"Vary");
const std::wstring http::header_names::via(L"Via");
const std::wstring http::header_names::warning(L"Warning");
const std::wstring http::header_names::www_authenticate(L"WWW-Authenticate");


const std::wstring http::mime_types::application_atom_xml(L"application/atom+xml");
const std::wstring http::mime_types::application_http(L"application/http");
const std::wstring http::mime_types::application_javascript(L"application/javascript");
const std::wstring http::mime_types::application_json(L"application/json");
const std::wstring http::mime_types::application_xjson(L"application/x-json");
const std::wstring http::mime_types::application_octetstream(L"application/octet-stream");
const std::wstring http::mime_types::application_x_www_form_urlencoded(L"application/x-www-form-urlencoded");
const std::wstring http::mime_types::application_xjavascript(L"application/x-javascript");
const std::wstring http::mime_types::application_xml(L"application/xml");
const std::wstring http::mime_types::message_http(L"message/http");
const std::wstring http::mime_types::multipart_form_data(L"multipart/form-data");
const std::wstring http::mime_types::text(L"text");
const std::wstring http::mime_types::text_javascript(L"text/javascript");
const std::wstring http::mime_types::text_json(L"text/json");
const std::wstring http::mime_types::text_csv(L"text/csv");
const std::wstring http::mime_types::text_plain(L"text/plain");
const std::wstring http::mime_types::text_plain_utf16(L"text/plain; charset=utf-16");
const std::wstring http::mime_types::text_plain_utf16le(L"text/plain; charset=utf-16le");
const std::wstring http::mime_types::text_plain_utf8(L"text/plain; charset=utf-8");
const std::wstring http::mime_types::text_xjavascript(L"text/x-javascript");
const std::wstring http::mime_types::text_xjson(L"text/x-json");


const std::wstring http::charset_types::ascii(L"ascii");
const std::wstring http::charset_types::usascii(L"us-ascii");
const std::wstring http::charset_types::latin1(L"iso-8859-1");
const std::wstring http::charset_types::utf8(L"utf-8");
const std::wstring http::charset_types::utf16(L"utf-16");
const std::wstring http::charset_types::utf16le(L"utf-16le");
const std::wstring http::charset_types::utf16be(L"utf-16be");


const std::wstring http::oauth1_methods::hmac_sha1(L"HMAC-SHA1");
const std::wstring http::oauth1_methods::plaintext(L"PLAINTEXT");


const std::wstring http::oauth1_strings::callback(L"oauth_callback");
const std::wstring http::oauth1_strings::callback_confirmed(L"oauth_callback_confirmed");
const std::wstring http::oauth1_strings::consumer_key(L"oauth_consumer_key");
const std::wstring http::oauth1_strings::nonce(L"oauth_nonce");
const std::wstring http::oauth1_strings::realm(L"realm"); // NOTE: No "oauth_" prefix.
const std::wstring http::oauth1_strings::signature(L"oauth_signature");
const std::wstring http::oauth1_strings::signature_method(L"oauth_signature_method");
const std::wstring http::oauth1_strings::timestamp(L"oauth_timestamp");
const std::wstring http::oauth1_strings::token(L"oauth_token");
const std::wstring http::oauth1_strings::token_secret(L"oauth_token_secret");
const std::wstring http::oauth1_strings::verifier(L"oauth_verifier");
const std::wstring http::oauth1_strings::version(L"oauth_version");


const std::wstring http::oauth2_strings::access_token(L"access_token");
const std::wstring http::oauth2_strings::authorization_code(L"authorization_code");
const std::wstring http::oauth2_strings::bearer(L"bearer");
const std::wstring http::oauth2_strings::client_id(L"client_id");
const std::wstring http::oauth2_strings::client_secret(L"client_secret");
const std::wstring http::oauth2_strings::code(L"code");
const std::wstring http::oauth2_strings::expires_in(L"expires_in");
const std::wstring http::oauth2_strings::grant_type(L"grant_type");
const std::wstring http::oauth2_strings::redirect_uri(L"redirect_uri");
const std::wstring http::oauth2_strings::refresh_token(L"refresh_token");
const std::wstring http::oauth2_strings::response_type(L"response_type");
const std::wstring http::oauth2_strings::scope(L"scope");
const std::wstring http::oauth2_strings::state(L"state");
const std::wstring http::oauth2_strings::token(L"token");
const std::wstring http::oauth2_strings::token_type(L"token_type");