

#include "stdafx.h"

#include <winsock2.h>
#include <iphlpapi.h>
#include <Ws2tcpip.h>

#include <nx\common\stringex.h>
#include <nx\winutil\network.h>

using namespace NX;
using namespace NX::win;


namespace {

	typedef PWSTR(NTAPI* FpRtlIpv4AddressToString)(_In_ const IN_ADDR *Addr, _Out_ PWSTR S);
	typedef PWSTR(NTAPI* FpRtlIpv6AddressToString)(_In_ const IN6_ADDR *Addr, _Out_ PWSTR S);

	class RtlIpAddressToStringW
	{
	public:
		RtlIpAddressToStringW() : _fp_ipv4(NULL), _fp_ipv6(NULL)
		{
			HMODULE mod = GetModuleHandleW(L"ntdll.dll");
			if (NULL != mod) {
				_fp_ipv4 = (FpRtlIpv4AddressToString)GetProcAddress(mod, "RtlIpv4AddressToStringW");
				_fp_ipv6 = (FpRtlIpv6AddressToString)GetProcAddress(mod, "RtlIpv6AddressToStringW");
			}
		}

		~RtlIpAddressToStringW() {}

		PWSTR operator () (_In_ const IN_ADDR *Addr, _Out_ PWSTR S)
		{
			return (NULL != _fp_ipv4) ? _fp_ipv4(Addr, S) : NULL;
		}

		PWSTR operator () (_In_ const IN6_ADDR *Addr, _Out_ PWSTR S)
		{
			return (NULL != _fp_ipv6) ? _fp_ipv6(Addr, S) : NULL;
		}

	private:
		FpRtlIpv4AddressToString    _fp_ipv4;
		FpRtlIpv6AddressToString    _fp_ipv6;
	};

	bool IsIpv6LinkLocalOrSpecialUse(const std::wstring& ipv6_address)
	{
		if (0 == ipv6_address.find(L"fe")) {
			wchar_t c = ipv6_address[2];
			if (c == '8' || c == '9' || c == 'a' || c == 'b') {
				// local link
				return true;
			}
		}
		else if (0 == ipv6_address.find(L"2001:0:")) {
			// special use
			return true;
		}
		else {
			; // nothing
		}

		return false;
	}

	std::wstring IfTypeToString(unsigned long if_type)
	{
		std::wstring s;

		switch (if_type)
		{
		case IF_TYPE_OTHER:
			s = L"Others";
			break;
		case IF_TYPE_ETHERNET_CSMACD:
			s = L"Ethernet network interface";
			break;
		case IF_TYPE_ISO88025_TOKENRING:
			s = L"Token ring network interface";
			break;
		case IF_TYPE_PPP:
			s = L"PPP network interface";
			break;
		case IF_TYPE_SOFTWARE_LOOPBACK:
			s = L"Software loopback network interface";
			break;
		case IF_TYPE_ATM:
			s = L"ATM network interface";
			break;
		case IF_TYPE_IEEE80211:
			s = L"IEEE 802.11 wireless network interface";
			break;
		case IF_TYPE_TUNNEL:
			s = L"Tunnel type encapsulation network interface";
			break;
		case IF_TYPE_IEEE1394:
			s = L"IEEE 1394 (Firewire) high performance serial bus network interface";
			break;
		default:
			s = L"Unknown";
			break;
		}

		return s;
	}

	std::wstring OperStatusToString(unsigned long oper_status)
	{
		std::wstring s;

		switch (oper_status)
		{
		case IfOperStatusUp:
			s = L"Up";
			break;
		case IfOperStatusDown:
			s = L"Down";
			break;
		case IfOperStatusTesting:
			s = L"Testing Mode";
			break;
		case IfOperStatusDormant:
			s = L"Pending";
			break;
		case IfOperStatusNotPresent:
			s = L"Down (Component not present)";
			break;
		case IfOperStatusLowerLayerDown:
			s = L"Down (Lower layer interface is down)";
			break;
		case IfOperStatusUnknown:
		default:
			s = L"Unknown";
			break;
		}

		return s;
	}

	class ActiveNetAdapterEnumerator : public NetAdapterEnumerator
	{
	public:
		ActiveNetAdapterEnumerator() : NetAdapterEnumerator() {}
		virtual ~ActiveNetAdapterEnumerator() {}

	protected:
		virtual bool Filter(const NetAdapter& adapter)
		{
			return adapter.IsActive();
		}
	};

	class NetworkAdapterEnumerator : public NetAdapterEnumerator
	{
	public:
		NetworkAdapterEnumerator() : NetAdapterEnumerator() {}
		NetworkAdapterEnumerator(bool activeOnly) : NetAdapterEnumerator(), _activeOnly(activeOnly){}
		virtual ~NetworkAdapterEnumerator() {}

	protected:
		virtual bool Filter(const NetAdapter& adapter)
		{
			if (_activeOnly)
				return (adapter.IsActive() && adapter.IsNetworkAdapter());
			else
				return adapter.IsNetworkAdapter();
		}
	private:
		bool _activeOnly;
	};

	class WirelessAdapterEnumerator : public NetAdapterEnumerator
	{
	public:
		WirelessAdapterEnumerator() : NetAdapterEnumerator() {}
		WirelessAdapterEnumerator(bool activeOnly) : NetAdapterEnumerator(), _activeOnly(activeOnly){}
		virtual ~WirelessAdapterEnumerator() {}

	protected:
		virtual bool Filter(const NetAdapter& adapter)
		{
			if (_activeOnly)
				return (adapter.IsActive() && adapter.Is80211Adapter());
			else
				return adapter.Is80211Adapter();
		}
	private:
		bool _activeOnly;
	};

	class EthernetAdapterEnumerator : public NetAdapterEnumerator
	{
	public:
		EthernetAdapterEnumerator() : NetAdapterEnumerator() {}
		EthernetAdapterEnumerator(bool activeOnly) : NetAdapterEnumerator(), _activeOnly(activeOnly){}
		virtual ~EthernetAdapterEnumerator() {}

	protected:
		virtual bool Filter(const NetAdapter& adapter)
		{
			if (_activeOnly)
				return (adapter.IsActive() && adapter.IsEthernetAdapter());
			else
				return adapter.IsEthernetAdapter();
		}
	private:
		bool _activeOnly;
	};

	class PppAdapterEnumerator : public NetAdapterEnumerator
	{
	public:
		PppAdapterEnumerator() : NetAdapterEnumerator() {}
		PppAdapterEnumerator(bool activeOnly) : NetAdapterEnumerator(), _activeOnly(activeOnly){}
		virtual ~PppAdapterEnumerator() {}

	protected:
		virtual bool Filter(const NetAdapter& adapter)
		{
			if (_activeOnly)
				return (adapter.IsActive() && adapter.IsPppAdapter());
			else
				return adapter.IsPppAdapter();
		}
	private:
		bool _activeOnly;
	};

	class Adapter1394Enumerator : public NetAdapterEnumerator
	{
	public:
		Adapter1394Enumerator() : NetAdapterEnumerator() {}
		Adapter1394Enumerator(bool activeOnly) : NetAdapterEnumerator(), _activeOnly(activeOnly){}
		virtual ~Adapter1394Enumerator() {}

	protected:
		virtual bool Filter(const NetAdapter& adapter)
		{
			if (_activeOnly)
				return (adapter.IsActive() && adapter.Is1394Adapter());
			else
				return adapter.Is1394Adapter();
		}
	private:
		bool _activeOnly;
	};

}

NetAdapter::NetAdapter()
{
}

NetAdapter::NetAdapter(void* p)
{
	PIP_ADAPTER_ADDRESSES pAdapter = (PIP_ADAPTER_ADDRESSES)p;

	// adapter name
	_adapter_name = NX::conv::ansitoutf16(pAdapter->AdapterName);

	// adapter friendly name
	if (NULL != pAdapter->FriendlyName) {
		_friendly_name = pAdapter->FriendlyName;
	}

	// adapter description
	if (NULL != pAdapter->Description) {
		_description = pAdapter->Description;
	}

	// adapter mac address
	if (NULL != pAdapter->PhysicalAddress && 0 != pAdapter->PhysicalAddressLength) {
		std::for_each(pAdapter->PhysicalAddress, pAdapter->PhysicalAddress + pAdapter->PhysicalAddressLength, [&](const BYTE v) {
			_phys_address.push_back(v);
		});
	}

	// adapter IfType and OperStatus
	_if_type = pAdapter->IfType;
	_oper_status = pAdapter->OperStatus;
	// ipv4/ipv6 status
	_ipv4_enabled = pAdapter->Ipv4Enabled;
	_ipv6_enabled = pAdapter->Ipv6Enabled;

	// ip addresses
	static RtlIpAddressToStringW ip_conv;
	PIP_ADAPTER_UNICAST_ADDRESS ip_address = pAdapter->FirstUnicastAddress;
	while (NULL != ip_address) {
		if (ip_address->Address.lpSockaddr->sa_family == AF_INET) {
			SOCKADDR_IN* ipv4 = reinterpret_cast<SOCKADDR_IN*>(ip_address->Address.lpSockaddr);
			wchar_t str_buffer[INET_ADDRSTRLEN] = { 0 };
			ip_conv(&(ipv4->sin_addr), str_buffer);
			if (L'\0' != str_buffer[0]) {
				_ipv4_addresses.push_back(std::wstring(str_buffer));
			}
		}
		else if (ip_address->Address.lpSockaddr->sa_family == AF_INET6) {
			SOCKADDR_IN6* ipv6 = reinterpret_cast<SOCKADDR_IN6*>(ip_address->Address.lpSockaddr);
			wchar_t str_buffer[INET6_ADDRSTRLEN] = { 0 };
			ip_conv(&(ipv6->sin6_addr), str_buffer);
			std::wstring ws_ip(str_buffer);
			if (!ws_ip.empty() && !IsIpv6LinkLocalOrSpecialUse(ws_ip)) {
				_ipv6_addresses.push_back(std::wstring(ws_ip));
			}
		}
		else {
			; // unknown
		}

		ip_address = ip_address->Next;
	}
}

NetAdapter::NetAdapter(const NetAdapter& rhs)
	: _adapter_name(rhs._adapter_name),
	_friendly_name(rhs._friendly_name),
	_description(rhs._description),
	_if_type(rhs._if_type),
	_oper_status(rhs._oper_status),
	_ipv4_enabled(rhs._ipv4_enabled),
	_ipv6_enabled(rhs._ipv6_enabled),
	_phys_address(rhs._phys_address),
	_ipv4_addresses(rhs._ipv4_addresses),
	_ipv6_addresses(rhs._ipv6_addresses)
{
}

NetAdapter::~NetAdapter()
{
}

NetAdapter& NetAdapter::operator = (const NetAdapter& rhs)
{
	if (this != &rhs) {
		_adapter_name	= rhs._adapter_name;
		_friendly_name	= rhs._friendly_name;
		_description	= rhs._description;
		_if_type		= rhs._if_type;
		_oper_status	= rhs._oper_status;
		_ipv4_enabled	= rhs._ipv4_enabled;
		_ipv6_enabled	= rhs._ipv6_enabled;
		_phys_address	= rhs._phys_address;
		_ipv4_addresses = rhs._ipv4_addresses;
		_ipv6_addresses	= rhs._ipv6_addresses;
	}
	return *this;
}

std::wstring NetAdapter::GetIfTypeName() const
{
	return IfTypeToString(_if_type);
}

std::wstring NetAdapter::GetOperStatusName() const
{
	return OperStatusToString(_oper_status);
}

std::wstring NetAdapter::SerializeMacAddress() const
{
	std::wstring s;
	std::for_each(_phys_address.begin(), _phys_address.end(), [&](const unsigned char& c) {
		if (!s.empty())
			s.append(L"-");
		s.append(NX::utohs<wchar_t>(c));
	});
	return s;
}

bool NetAdapter::IsEthernetAdapter() const
{
	return (!Empty() && IF_TYPE_ETHERNET_CSMACD == GetIfType());
}

bool NetAdapter::IsPppAdapter() const
{
	return (!Empty() && IF_TYPE_PPP == GetIfType());
}

bool NetAdapter::Is80211Adapter() const
{
	return (!Empty() && IF_TYPE_IEEE80211 == GetIfType());
}

bool NetAdapter::Is1394Adapter() const
{
	return (!Empty() && IF_TYPE_IEEE1394 == GetIfType());
}

bool NetAdapter::IsNetworkAdapter() const
{
	return (IsEthernetAdapter() || IsPppAdapter() || Is80211Adapter());
}

bool NetAdapter::IsActive() const
{
	return (!Empty() && IfOperStatusUp == GetOperStatus());
}

bool NetAdapter::IsConnected() const
{
	return (!Empty() && IfOperStatusUp == GetOperStatus());
}

NetAdapterEnumerator::NetAdapterEnumerator()
{
}

NetAdapterEnumerator::~NetAdapterEnumerator()
{
}

std::vector<NetAdapter> NetAdapterEnumerator::Enum()
{
	std::vector<NetAdapter> adapters;

	do {

		std::vector<unsigned char> buf;
		PIP_ADAPTER_ADDRESSES pAddresses = NULL;
		PIP_ADAPTER_ADDRESSES pCurAddress = NULL;
		ULONG dwSize = sizeof(IP_ADAPTER_ADDRESSES);
		ULONG dwRetVal = 0;

		buf.resize(dwSize, 0);
		pAddresses = (PIP_ADAPTER_ADDRESSES)buf.data();
		memset(pAddresses, 0, dwSize);

		dwRetVal = GetAdaptersAddresses(AF_UNSPEC, 0, NULL, pAddresses, &dwSize);
		if (ERROR_SUCCESS != dwRetVal) {
			if (ERROR_BUFFER_OVERFLOW != dwRetVal) {
				// Some other error happens
				break;
			}

			dwSize += sizeof(IP_ADAPTER_ADDRESSES);
			buf.resize(dwSize, 0);
			pAddresses = (PIP_ADAPTER_ADDRESSES)buf.data();
			memset(pAddresses, 0, dwSize);
		}

		if (NULL == pAddresses) {
			break;
		}

		dwRetVal = GetAdaptersAddresses(AF_UNSPEC, 0, NULL, pAddresses, &dwSize);
		if (ERROR_SUCCESS != dwRetVal) {
			break;
		}

		// Go through all adapters
		pCurAddress = pAddresses;
		do {
			if (IF_TYPE_SOFTWARE_LOOPBACK == pCurAddress->IfType) {
				continue;
			}
			NetAdapter adapter((void*)pCurAddress);
			if (Filter(adapter)) {
				adapters.push_back(adapter);
			}
		} while (NULL != (pCurAddress = pCurAddress->Next));

	} while (FALSE);

	return adapters;
}

bool NetAdapterEnumerator::Filter(const NetAdapter& adapter)
{
	return true;
}


std::vector<NetAdapter> NX::win::GetAllNetAdapters()
{
	NetAdapterEnumerator netenum;
	return netenum.Enum();
}

std::vector<NetAdapter> NX::win::GetNetworkAdapters(bool activeOnly)
{
	NetworkAdapterEnumerator netenum(activeOnly);
	return netenum.Enum();
}

std::vector<NetAdapter> NX::win::GetEthernetAdapters(bool activeOnly)
{
	EthernetAdapterEnumerator netenum(activeOnly);
	return netenum.Enum();
}

std::vector<NetAdapter> NX::win::GetWirelessAdapters(bool activeOnly)
{
	WirelessAdapterEnumerator netenum(activeOnly);
	return netenum.Enum();
}

std::vector<NetAdapter> NX::win::GetPppAdapters(bool activeOnly)
{
	PppAdapterEnumerator netenum(activeOnly);
	return netenum.Enum();
}

std::vector<NetAdapter> NX::win::Get1394Adapters(bool activeOnly)
{
	Adapter1394Enumerator netenum(activeOnly);
	return netenum.Enum();
}