

#ifndef __NX_REST_HTTP_CONST_H__
#define __NX_REST_HTTP_CONST_H__

#include <string>

namespace NX {
    namespace REST {

        namespace http {

            class methods
            {
            public:
                const static std::wstring GET;
                const static std::wstring POST;
                const static std::wstring PUT;
                const static std::wstring DEL;
                const static std::wstring HEAD;
                const static std::wstring OPTIONS;
                const static std::wstring TRCE;
                const static std::wstring CONNECT;
                const static std::wstring MERGE;
                const static std::wstring PATCH;
            };

            typedef struct _http_status_and_phrase
            {
                unsigned short  id;
                const wchar_t*  phrase;
            } http_status_and_phrase;
            
            class status_codes
            {
            public:
                // 1xx Informational
                const static http_status_and_phrase Continue;                       // 100;
                const static http_status_and_phrase SwitchingProtocols;             // 101;
                const static http_status_and_phrase Processing;                     // 102;

                // 2xx Success
                const static http_status_and_phrase OK;                             // 200;
                const static http_status_and_phrase Created;                        // 201;
                const static http_status_and_phrase Accepted;                       // 202;
                const static http_status_and_phrase NonAuthInfo;                    // 203;
                const static http_status_and_phrase NoContent;                      // 204;
                const static http_status_and_phrase ResetContent;                   // 205;
                const static http_status_and_phrase PartialContent;                 // 206;
                const static http_status_and_phrase MultiStatus;                    // 207;  // WebDAV; RFC 4918
                const static http_status_and_phrase AlreadyReported;                // 208;  // WebDAV; RFC 5842
                const static http_status_and_phrase IMUsed;                         // 226;  // RFC 3229

                // 3xx Redirection
                const static http_status_and_phrase MultipleChoices;                // 300;
                const static http_status_and_phrase MovedPermanently;               // 301;
                const static http_status_and_phrase Found;                          // 302;
                const static http_status_and_phrase SeeOther;                       // 303;
                const static http_status_and_phrase NotModified;                    // 304;
                const static http_status_and_phrase UseProxy;                       // 305;
                const static http_status_and_phrase SwitchProxy;                    // 306;
                const static http_status_and_phrase TemporaryRedirect;              // 307;
                const static http_status_and_phrase PermanentRedirect;              // 308;    // RFC 7538

                // 4xx Client Error
                const static http_status_and_phrase BadRequest;                     // 400;
                const static http_status_and_phrase Unauthorized;                   // 401;
                const static http_status_and_phrase PaymentRequired;                // 402;
                const static http_status_and_phrase Forbidden;                      // 403;
                const static http_status_and_phrase NotFound;                       // 404;
                const static http_status_and_phrase MethodNotAllowed;               // 405;
                const static http_status_and_phrase NotAcceptable;                  // 406;
                const static http_status_and_phrase ProxyAuthRequired;              // 407;
                const static http_status_and_phrase RequestTimeout;                 // 408;
                const static http_status_and_phrase Conflict;                       // 409;
                const static http_status_and_phrase Gone;                           // 410;
                const static http_status_and_phrase LengthRequired;                 // 411;
                const static http_status_and_phrase PreconditionFailed;             // 412;
                const static http_status_and_phrase PayloadTooLarge;                // 413;
                const static http_status_and_phrase UriTooLong;                     // 414;
                const static http_status_and_phrase UnsupportedMediaType;           // 415;
                const static http_status_and_phrase RangeNotSatisfiable;            // 416;
                const static http_status_and_phrase ExpectationFailed;              // 417;
                const static http_status_and_phrase IAmATeapot;                     // 418;    // RFC 2324
                const static http_status_and_phrase MisdirectedRequest;             // 421;    // RFC 7540
                const static http_status_and_phrase UnprocessableEntity;            // 422;    // WebDAV; RFC 4918
                const static http_status_and_phrase Locked;                         // 423;    // WebDAV; RFC 4918
                const static http_status_and_phrase FailedDependency;               // 424;    // WebDAV; RFC 4918
                const static http_status_and_phrase UpgradeRequired;                // 426;
                const static http_status_and_phrase PreconditionRequired;           // 428;    // RFC 6585
                const static http_status_and_phrase TooManyRequests;                // 429;    // RFC 6585
                const static http_status_and_phrase RequestHeaderFieldsTooLarge;    // 431;    // RFC 6585
                const static http_status_and_phrase UnavailableForLegalReasons;     // 451;    // RFC 7725

                // 5xx Server Error
                const static http_status_and_phrase InternalError;                  // 500;
                const static http_status_and_phrase NotImplemented;                 // 501;
                const static http_status_and_phrase BadGateway;                     // 502;
                const static http_status_and_phrase ServiceUnavailable;             // 503;
                const static http_status_and_phrase GatewayTimeout;                 // 504;
                const static http_status_and_phrase HttpVersionNotSupported;        // 505;
                const static http_status_and_phrase VariantAlsoNegotiates;          // 506;    // RFC 2295
                const static http_status_and_phrase InsufficientStorage;            // 507;    // WebDAV; RFC 4918
                const static http_status_and_phrase LoopDetected;                   // 508;    // WebDAV; RFC 5842
                const static http_status_and_phrase NotExtended;                    // 510;    // RFC 2774
                const static http_status_and_phrase NetworkAuthenticationRequired;  // 511;    // RFC 6585
            };

            // Define NX_HTTP_NO_STATUS_PHRASE_MAP = 1 to save some memory
            #ifndef NX_HTTP_NO_STATUS_PHRASE_MAP
            const wchar_t* HttpStatusToPhrase(unsigned short status);
            #endif

            class header_names
            {
            public:
                const static std::wstring accept;
                const static std::wstring accept_charset;
                const static std::wstring accept_encoding;
                const static std::wstring accept_language;
                const static std::wstring accept_ranges;
                const static std::wstring age;
                const static std::wstring allow;
                const static std::wstring authorization;
                const static std::wstring cache_control;
                const static std::wstring connection;
                const static std::wstring content_disposition;
                const static std::wstring content_encoding;
                const static std::wstring content_language;
                const static std::wstring content_length;
                const static std::wstring content_location;
                const static std::wstring content_md5;
                const static std::wstring content_range;
                const static std::wstring content_type;
                const static std::wstring date;
                const static std::wstring etag;
                const static std::wstring expect;
                const static std::wstring expires;
                const static std::wstring from;
                const static std::wstring host;
                const static std::wstring if_match;
                const static std::wstring if_modified_since;
                const static std::wstring if_none_match;
                const static std::wstring if_range;
                const static std::wstring if_unmodified_since;
                const static std::wstring last_modified;
                const static std::wstring location;
                const static std::wstring max_forwards;
                const static std::wstring pragma;
                const static std::wstring proxy_authenticate;
                const static std::wstring proxy_authorization;
                const static std::wstring range;
                const static std::wstring referer;
                const static std::wstring retry_after;
                const static std::wstring server;
                const static std::wstring te;
                const static std::wstring trailer;
                const static std::wstring transfer_encoding;
                const static std::wstring upgrade;
                const static std::wstring user_agent;
                const static std::wstring vary;
                const static std::wstring via;
                const static std::wstring warning;
                const static std::wstring www_authenticate;
            };


            class mime_types
            {
            public:
                const static std::wstring application_atom_xml;
                const static std::wstring application_http;
                const static std::wstring application_javascript;
                const static std::wstring application_json;
                const static std::wstring application_xjson;
                const static std::wstring application_octetstream;
                const static std::wstring application_x_www_form_urlencoded;
                const static std::wstring application_xjavascript;
                const static std::wstring application_xml;
                const static std::wstring message_http;
                const static std::wstring multipart_form_data;
                const static std::wstring text;
                const static std::wstring text_javascript;
                const static std::wstring text_json;
                const static std::wstring text_csv;
                const static std::wstring text_plain;
                const static std::wstring text_plain_utf16;
                const static std::wstring text_plain_utf16le;
                const static std::wstring text_plain_utf8;
                const static std::wstring text_xjavascript;
                const static std::wstring text_xjson;
            };

            class charset_types
            {
            public:
                const static std::wstring ascii;
                const static std::wstring usascii;
                const static std::wstring latin1;
                const static std::wstring utf8;
                const static std::wstring utf16;
                const static std::wstring utf16le;
                const static std::wstring utf16be;
            };

            typedef std::wstring oauth1_method;
            class oauth1_methods
            {
            public:
                const static std::wstring hmac_sha1;
                const static std::wstring plaintext;
            };

            class oauth1_strings
            {
            public:
                const static std::wstring callback;
                const static std::wstring callback_confirmed;
                const static std::wstring consumer_key;
                const static std::wstring nonce;
                const static std::wstring realm;
                const static std::wstring signature;
                const static std::wstring signature_method;
                const static std::wstring timestamp;
                const static std::wstring token;
                const static std::wstring token_secret;
                const static std::wstring verifier;
                const static std::wstring version;
            };

            class oauth2_strings
            {
            public:
                const static std::wstring access_token;
                const static std::wstring authorization_code;
                const static std::wstring bearer;
                const static std::wstring client_id;
                const static std::wstring client_secret;
                const static std::wstring code;
                const static std::wstring expires_in;
                const static std::wstring grant_type;
                const static std::wstring redirect_uri;
                const static std::wstring refresh_token;
                const static std::wstring response_type;
                const static std::wstring scope;
                const static std::wstring state;
                const static std::wstring token;
                const static std::wstring token_type;
            };

        }
    }
}

#endif