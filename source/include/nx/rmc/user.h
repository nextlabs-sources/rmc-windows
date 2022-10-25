

#ifndef __LIBRMC_USER_H__
#define __LIBRMC_USER_H__

#include <nx\common\result.h>
#include <nx\common\rapidjson.h>
#include <nx\nxl\nxlfile.h>
#include <string>
#include <list>

namespace NX {

	class RmRestClient;
	class RmUser;

    class RmUserMembership
    {
    public:
        RmUserMembership();
        RmUserMembership(const std::wstring& id, int type, const std::wstring& tenantId);
        RmUserMembership(const RmUserMembership& rhs);
        ~RmUserMembership();

        void Clear();
		
		int GetFileTokenCount();
		NXL::FileToken GetFileToken();
		int AddFileTokens(const std::vector<NXL::FileToken>& tokens);
		std::string SerializeFileTokensCache() const;
		Result DeserializeFileTokensCache(const std::string& s);

		NX::rapidjson::PJSONVALUE ToJsonValue() const;
		void FromJsonValue(NX::rapidjson::PCJSONVALUE pv);
		template<typename T>
		std::basic_string<T> ToJsonString() const
		{
			std::shared_ptr<NX::rapidjson::JsonValue> root(ToJsonValue());
			if (NULL == root)
				return std::basic_string<T>();
			NX::rapidjson::JsonStringWriter<T> writer;
			return writer.Write(root.get());
		}
		template<typename T>
		void FromJsonString(const std::basic_string<T>& s)
		{
			NX::rapidjson::JsonDocument doc;
			if (!doc.LoadJsonString<char>(s))
				return RESULT(ERROR_INVALID_DATA);
			FromJsonObject(doc.GetRoot());
		}

        RmUserMembership& operator = (const RmUserMembership& rhs);


        inline bool Empty() const { return _id.empty(); }
        inline const std::wstring& GetTenantId() const { return _tenantId; }
        inline const std::wstring& GetId() const { return _id; }
        inline const std::wstring& GetIdHash() const { return _idHash; }
        inline int GetType() const { return _type; }
        inline const std::vector<UCHAR>& GetAgreement0() const { return _agreement0; }
        inline const std::vector<UCHAR>& GetAgreement1() const { return _agreement1; }

		inline void SetAgreement0(const std::vector<UCHAR>& agreement) { _agreement0 = agreement; }
		inline void SetAgreement1(const std::vector<UCHAR>& agreement) { _agreement1 = agreement; }

	protected:
		std::wstring CreateIdHash(const std::wstring& id);

    private:
        std::wstring    _id;
        std::wstring    _idHash;
        std::wstring    _tenantId;
        int             _type;
		std::vector<UCHAR> _agreement0;
		std::vector<UCHAR> _agreement1;
		std::list<NXL::FileToken> _tokens;
		mutable CRITICAL_SECTION _tokensLock;

		friend class RmUser;
		friend class RmSession;
    };

    class RmUserPreferences
    {
    public:
        RmUserPreferences();
        RmUserPreferences(const std::wstring& defaultTenant, const std::wstring& defaultMember, int securityMode);
        RmUserPreferences(const RmUserPreferences& rhs);
        ~RmUserPreferences();

        void Clear();

        RmUserPreferences& operator = (const RmUserPreferences& rhs);

		NX::rapidjson::PJSONVALUE ToJsonValue() const;
		void FromJsonValue(NX::rapidjson::PCJSONVALUE pv);
		template<typename T>
		std::basic_string<T> ToJsonString() const
		{
			std::shared_ptr<NX::rapidjson::JsonValue> root(ToJsonValue());
			if (NULL == root)
				return std::basic_string<T>();
			NX::rapidjson::JsonStringWriter<T> writer;
			return writer.Write(root.get());
		}
		template<typename T>
		void FromJsonString(const std::basic_string<T>& s)
		{
			NX::rapidjson::JsonDocument doc;
			if (!doc.LoadJsonString<char>(s))
				return RESULT(ERROR_INVALID_DATA);
			FromJsonObject(doc.GetRoot());
		}

        inline const std::wstring& GetDefaultTenant() const { return _defaultTenant; }
        inline const std::wstring& GetDefaultMember() const { return _defaultMember; }
        inline int GetSecurityMode() const { return _securityMode; }

    private:
        std::wstring    _defaultTenant;
        std::wstring    _defaultMember;
        int             _securityMode;

		friend class RmUser;
		friend class RmSession;
    };

    class RmUserTicket
    {
    public:
        RmUserTicket();
        RmUserTicket(const std::wstring& ticket, __int64 ttl);
        RmUserTicket(const RmUserTicket& rhs);
        RmUserTicket(RmUserTicket&& rhs);
        ~RmUserTicket();

        void Clear();
        bool Expired() const;

        RmUserTicket& operator = (const RmUserTicket& rhs);
        RmUserTicket& operator = (RmUserTicket&& rhs);

		NX::rapidjson::PJSONVALUE ToJsonValue() const;
		void FromJsonValue(NX::rapidjson::PCJSONVALUE pv);
		template<typename T>
		std::basic_string<T> ToJsonString() const
		{
			std::shared_ptr<NX::rapidjson::JsonValue> root(ToJsonValue());
			if (NULL == root)
				return std::basic_string<T>();
			NX::rapidjson::JsonStringWriter<T> writer;
			return writer.Write(root.get());
		}
		template<typename T>
		void FromJsonString(const std::basic_string<T>& s)
		{
			NX::rapidjson::JsonDocument doc;
			if (!doc.LoadJsonString<char>(s))
				return;
			FromJsonValue(doc.GetRoot());
		}

        inline const std::wstring& GetTicket() const { return _ticket; }
        inline __int64 GetTTL() const { return _ttl; }
        inline bool Empty() const { return _ticket.empty(); }

    private:
        std::wstring    _ticket;
        __int64         _ttl;

		friend class RmUser;
		friend class RmSession;
    };

	typedef enum IDPTYPE {
		SKYDRM = 0,
		SAML,
		GOOGLE,
		FACEBOOK,
		YAHOO
	} IDPTYPE;
	
    class RmUser
    {
    public:
        RmUser();
        RmUser(const RmUser& rhs);
        ~RmUser();

        void Clear();
		const RmUserMembership* Findmembership(const std::wstring& membershipId) const;
		RmUserMembership* Findmembership(const std::wstring& membershipId);

        RmUser& operator = (const RmUser& rhs);

		NX::rapidjson::PJSONVALUE ToJsonValue(bool withTicket = false) const;
		void FromJsonValue(NX::rapidjson::PCJSONVALUE pv);
		template<typename T>
		std::basic_string<T> ToJsonString() const
		{
			std::shared_ptr<NX::rapidjson::JsonValue> root(ToJsonValue());
			if (NULL == root)
				return std::basic_string<T>();
			NX::rapidjson::JsonStringWriter<T> writer;
			return writer.Write(root.get());
		}
		template<typename T>
		void FromJsonString(const std::basic_string<T>& s)
		{
			NX::rapidjson::JsonDocument doc;
			if (!doc.LoadJsonString<char>(s))
				return;
			FromJsonValue(doc.GetRoot());
		}


        inline bool Empty() const { return (_name.empty() || _id.empty()); }
        inline const std::wstring& GetName() const { return _name; }
        inline const std::wstring& GetId() const { return _id; }
        inline const std::wstring& GetEmail() const { return _email; }
        inline IDPTYPE GetIdpType() const { return _idpType; }
        inline const RmUserPreferences& GetPreferences() const { return _preferences; }
        inline const RmUserTicket& GetTicket() const { return _ticket; }
        inline RmUserTicket& GetTicket() { return _ticket; }
        inline const std::vector<RmUserMembership>& GetMemberships() const { return _memberships; }
        inline std::vector<RmUserMembership>& GetMemberships() { return _memberships; }
		inline void SetTicket(const RmUserTicket& ticket) { _ticket = ticket; }
        
    private:
        std::wstring        _id;
        std::wstring        _name;
        std::wstring        _email;
		IDPTYPE				_idpType;
        RmUserPreferences   _preferences;
        RmUserTicket        _ticket;
        std::vector<RmUserMembership>   _memberships;

        friend class RmRestClient;
        friend class RmSession;
    };
}


#endif