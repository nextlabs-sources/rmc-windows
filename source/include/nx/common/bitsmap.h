

#ifndef __NX_COMMON_BITS_MAP_H__
#define __NX_COMMON_BITS_MAP_H__


#include <vector>

namespace NX {
	
	class BitsMap
	{
	public:
		BitsMap() : _size(0) {}
		BitsMap(unsigned long size) : _size(size)
		{
			_map.resize(GetElementCount(), 0);
		}

		BitsMap(unsigned long size, const unsigned long* pbData, unsigned long cbCount) : _size(size)
		{
			_map.resize(GetElementCount(), 0);
			if (pbData && cbCount) {
				const unsigned long cbCopy = 4 * (GetElementCount() < cbCount ? GetElementCount() : cbCount);
				memcpy(_map.data(), pbData, cbCopy);
			}
		}

		BitsMap(const BitsMap& rhs) : _size(rhs._size), _map(rhs._map) {}
		BitsMap& operator = (const BitsMap& rhs)
		{
			if (this != &rhs) {
				_size = rhs._size;
				_map = rhs._map;
			}
			return *this;
		}

		inline const unsigned long* GetData() const { return _map.empty() ? NULL : _map.data(); }
		inline unsigned long GetBitsSize() const { return _size; }
		inline unsigned long GetElementCount() const { return ((_size + 31) / 32); }

		virtual ~BitsMap()
		{
		}

		inline bool CheckBit(_In_ unsigned long bit) const
		{
			if (bit >= GetBitsSize()) {
				return false;
			}
			return CheckBit(_map[bit / 32], bit % 32);
		}

		inline bool operator [](_In_ unsigned long bit) const
		{
			return CheckBit(bit);
		}

		inline void SetBit(_In_ unsigned long bit)
		{
			if (bit < GetBitsSize()) {
				SetBit(_map[bit / 32], bit % 32);
			}
		}

		inline void ClearBit(_In_ unsigned long bit)
		{
			if (bit < GetBitsSize()) {
				ClearBit(_map[bit / 32], bit % 32);
			}
		}

		inline void Clear()
		{
			if (!_map.empty()) {
				memset(_map.data(), 0, _map.size() * sizeof(unsigned long));
			}
		}

	protected:
		const std::vector<unsigned long>& GetMap() const { return _map; }
		std::vector<unsigned long>& GetMap() { return _map; }

	private:
		std::vector<unsigned long>	_map;
		unsigned long				_size;

		bool CheckBit(unsigned long value, unsigned long bit) const
		{
			return (0 != (value & BitsMaskMap[bit]));
		}

		void SetBit(unsigned long& value, unsigned long bit) const
		{
			value |= BitsMaskMap[bit];
		}

		void ClearBit(unsigned long& value, unsigned long bit) const
		{
			value &= (~BitsMaskMap[bit]);
		}

		const unsigned long BitsMaskMap[32] = {
			0x00000001, 0x00000002, 0x00000004, 0x00000008, 0x00000010, 0x00000020, 0x00000040, 0x00000080,
			0x00000100, 0x00000200, 0x00000400, 0x00000800, 0x00001000, 0x00002000, 0x00004000, 0x00008000,
			0x00010000, 0x00020000, 0x00040000, 0x00080000, 0x00100000, 0x00200000, 0x00400000, 0x00800000,
			0x01000000, 0x02000000, 0x04000000, 0x08000000, 0x10000000, 0x20000000, 0x40000000, 0x80000000
		};
	};

	template <unsigned long N>
	class FixedSizeBitsMap : public BitsMap
	{
	public:
		FixedSizeBitsMap() : BitsMap(N) {}
		virtual ~FixedSizeBitsMap() {}
	};

	class BitsMap32 : public BitsMap
	{
	public:
		BitsMap32() : BitsMap(32) {}
		BitsMap32(unsigned long data) : BitsMap(32, &data, 1)
		{
		}
		virtual ~BitsMap32() {}

		BitsMap32& operator = (_In_ unsigned long data)
		{
			GetMap()[0] = data;
			return *this;
		}

		BitsMap32& operator = (_In_ int data)
		{
			GetMap()[0] = (unsigned long)data;
			return *this;
		}
	};

}


#endif