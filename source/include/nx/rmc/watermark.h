

#ifndef __NXRMC_WATERMARK_H__
#define __NXRMC_WATERMARK_H__

#include <nx\common\result.h>
#include <string>

namespace NX {

	class RmWatermarkConfig
	{
	public:
		typedef enum ROTATION_TYPE {
			NOROTATION = 0,
			CLOCKWISE,
			ANTICLOCKWISE
		} ROTATION_TYPE;

		RmWatermarkConfig();
		RmWatermarkConfig(const std::wstring& s);
		RmWatermarkConfig(const RmWatermarkConfig& rhs);
		~RmWatermarkConfig();

		RmWatermarkConfig& operator = (const RmWatermarkConfig& rhs);

		std::string serialize() const;
		Result deserialize(const std::string& s);
		Result deserialize(const std::wstring& s);

		inline const std::wstring& GetText() const { return _text; }
		inline const std::wstring& GetDensity() const { return _density; }
		inline const std::wstring& GetFontName() const { return _fontName; }
		inline int GetTransparency() const { return _transparency; }

	private:
		std::wstring	_text;
		std::wstring	_density;
		std::wstring	_fontName;
		int				_transparency;
		int				_fontSize;
		unsigned int	_fontColor;
		ROTATION_TYPE	_rotation;
		bool			_repeat;
	};

}


#endif
