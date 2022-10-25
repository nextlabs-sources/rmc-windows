

#ifndef __NX_WINUTIL_NETWORK_H__
#define __NX_WINUTIL_NETWORK_H__

#include <nx\common\result.h>

namespace NX {

	namespace win {

		class NetAdapterEnumerator;

		class NetAdapter
		{
		public:
			NetAdapter();
			NetAdapter(const NetAdapter& rhs);
			virtual ~NetAdapter();

			NetAdapter& operator = (const NetAdapter& rhs);

			std::wstring GetIfTypeName() const;
			std::wstring GetOperStatusName() const;
			std::wstring SerializeMacAddress() const;

			bool IsEthernetAdapter() const;
			bool IsPppAdapter() const;
			bool Is80211Adapter() const;
			bool Is1394Adapter() const;
			bool IsNetworkAdapter() const;

			bool IsActive() const;
			bool IsConnected() const;

			inline bool Empty() const { return _adapter_name.empty(); }
			inline const std::wstring& GetAdapterName() const { return _adapter_name; }
			inline const std::wstring& GetFriendlyName() const { return _friendly_name; }
			inline const std::wstring& GetDescription() const { return _description; }
			inline unsigned long GetIfType() const { return _if_type; }
			inline unsigned long GetOperStatus() const { return _oper_status; }
			inline bool IsIpv4Enabled() const { return _ipv4_enabled; }
			inline bool IsIpv6Enabled() const { return _ipv6_enabled; }
			inline const std::vector<unsigned char>& GetPhysicalAddress() const { return _phys_address; }
			inline const std::vector<unsigned char>& GetMacAddress() const { return _phys_address; }
			inline const std::vector<std::wstring>& GetIpv4Addresses() const { return _ipv4_addresses; }
			inline const std::vector<std::wstring>& GetIpv6Addresses() const { return _ipv6_addresses; }

		protected:
			NetAdapter(void* p);

		private:
			std::wstring        _adapter_name;
			std::wstring        _friendly_name;
			std::wstring        _description;
			unsigned long       _if_type;
			unsigned long       _oper_status;
			bool                _ipv4_enabled;
			bool                _ipv6_enabled;
			std::vector<unsigned char>	_phys_address;
			std::vector<std::wstring>   _ipv4_addresses;
			std::vector<std::wstring>   _ipv6_addresses;

			friend class NetAdapterEnumerator;
		};

		class NetAdapterEnumerator
		{
		public:
			NetAdapterEnumerator();
			virtual ~NetAdapterEnumerator();

			std::vector<NetAdapter> Enum();

		protected:
			virtual bool Filter(const NetAdapter& adapter);
		};

		std::vector<NetAdapter> GetAllNetAdapters();
		std::vector<NetAdapter> GetNetworkAdapters(bool activeOnly);
		std::vector<NetAdapter> GetEthernetAdapters(bool activeOnly);
		std::vector<NetAdapter> GetWirelessAdapters(bool activeOnly);
		std::vector<NetAdapter> GetPppAdapters(bool activeOnly);
		std::vector<NetAdapter> Get1394Adapters(bool activeOnly);

	}	// NX::win

}   // NX

#endif
