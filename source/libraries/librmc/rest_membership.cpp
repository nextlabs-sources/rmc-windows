
#include "stdafx.h"

#include <nx\rmc\keym.h>
#include <nx\rmc\rest.h>

#include <nx\common\rapidjson.h>
#include <nx\common\stringex.h>
#include <nx\crypt\md5.h>
#include <nx\crypt\dh.h>


using namespace NX;
using namespace NX::REST::http;


namespace {

    class RequestMembersipCerts : public REST::http::StringBodyRequest
    {
    public:
        RequestMembersipCerts(const RmRestCommonParams& param, const std::wstring& membership)
            : StringBodyRequest(REST::http::methods::PUT,
                std::wstring(L"/rms/rs/membership"),
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
                std::string(BuildBody(param, membership, GenerateDhKey())))
        {
        }
        virtual ~RequestMembersipCerts() {}

    protected:
        std::string BuildBody(const RmRestCommonParams& param, const std::wstring& membership, const std::wstring& publicKey)
        {
            std::wstring s = L"{\"parameters\": {\"userId\":" + param.GetUserId() + L",";
            s += L"\"ticket\":\"" + param.GetTicket() + L"\",";
            s += L"\"membership\":\"" + membership + L"\",";
            s += L"\"publicKey\":\"" + publicKey + L"\"}}";
            return NX::conv::utf16toutf8(s);
        }

		std::wstring GenerateDhKey()
		{
			NX::crypt::DiffieHellmanKey dhkey;
			Result res = dhkey.Generate(DH_P_2048, DH_G_2048, 2048);
			if (!res)
				return std::wstring();

			NX::crypt::DhKeyBlob keyblob;
			res = dhkey.Export(keyblob);
			if (!res)
				return std::wstring();

			std::vector<UCHAR> x509PublicKey;
			res = keyblob.CreateX509EncodedPublicKey(x509PublicKey);
			if (!res)
				return std::wstring();

			std::string sPublicKey = NX::conv::Base64Encode(x509PublicKey);
			return std::wstring(sPublicKey.begin(), sPublicKey.end());
		}
    };
}


Result RmRestClient::MembershipGetCertificates(const std::wstring& rms_url,
    const std::wstring& membership,
    RmRestStatus& status,
    std::vector<std::wstring>& certificates)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(rms_url, res));
    if (p) {
        res = MembershipGetCertificates(p.get(), membership, status, certificates);
    }
    return res;
}

Result RmRestClient::MembershipGetCertificates(NX::REST::http::Connection* conn,
	const std::wstring& membership,
    RmRestStatus& status,
    std::vector<std::wstring>& certificates)
{
	RequestMembersipCerts request(_commonParams, membership);
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
				rapidjson::JsonValue* certs = results->AsObject()->at(L"certficates");
				if (certs && certs->IsString()) {

					static const std::wstring CERT_HEADER(L"-----BEGIN CERTIFICATE-----");
					static const std::wstring CERT_FOOTER(L"-----END CERTIFICATE-----");
					std::wstring sCerts = certs->AsString()->GetString();

					// Parse all certificates
					std::vector<std::wstring> receivedCerts;
					while (!sCerts.empty()) {
						auto pos = sCerts.find(CERT_HEADER);
						if (pos == std::wstring::npos) {
							break;
						}
						if (pos != 0) {
							sCerts = sCerts.substr(pos);
						}
						pos = sCerts.find(CERT_HEADER, CERT_HEADER.length());
						if (pos != std::wstring::npos) {
							receivedCerts.push_back(sCerts.substr(0, pos));
							sCerts = sCerts.substr(pos);
						}
						else {
							receivedCerts.push_back(sCerts);
							sCerts.clear();
						}
					}

					// Push it back
					if (receivedCerts.size() < 2) {
						; // Too few certificates
					}
					else if (receivedCerts.size() == 2) {
						certificates.push_back(receivedCerts[1]);
					}
					else if (receivedCerts.size() == 3) {
						certificates.push_back(receivedCerts[2]);
						certificates.push_back(receivedCerts[1]);
					}
					else {
						certificates.push_back(receivedCerts[2]);
						certificates.push_back(receivedCerts[1]);
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
