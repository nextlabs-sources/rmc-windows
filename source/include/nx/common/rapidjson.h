

#ifndef __NX_RAPIDJSON_H__
#define __NX_RAPIDJSON_H__


#include <windows.h>

#include <vector>
#include <string>
#include <sstream>
#include <memory>
#include <algorithm>
#include <fstream>
#include <exception>
#include <nx\common\stringex.h>


namespace NX {

    namespace rapidjson {

        typedef enum ValueType {
            JNULL = 0,
            JBOOL,
            JNUMBER,
            JSTRING,
            JOBJECT,
            JARRAY
        } ValueType;

        // Forward
        class JsonNull;
        class JsonBool;
        class JsonNumber;
        class JsonString;
        class JsonObject;
        class JsonArray;
        template<typename T> class JsonParser;

        class JsonValue
        {
        public:
            virtual ~JsonValue() { clear(); }

            static JsonNull* CreateNull();
            static JsonBool* CreateBool(bool v);
            static JsonNumber* CreateNumber(int v);
            static JsonNumber* CreateNumber(__int64 v);
            static JsonNumber* CreateNumber(unsigned int v);
            static JsonNumber* CreateNumber(unsigned __int64 v);
            static JsonNumber* CreateNumber(float v);
            static JsonNumber* CreateNumber(double v);
            static JsonString* CreateString(const std::string& v);
            static JsonString* CreateString(const std::wstring& v);
            static JsonObject* CreateObject();
            static JsonArray* CreateArray();

            inline ValueType GetType() const { return _type; }
            inline bool IsNull() const { return (JNULL == _type); }
            inline bool IsBool() const { return (JBOOL == _type); }
            inline bool IsNumber() const { return (JNUMBER == _type); }
            inline bool IsString() const { return (JSTRING == _type); }
            inline bool IsObject() const { return (JOBJECT == _type); }
            inline bool IsArray() const { return (JARRAY == _type); }

            virtual JsonNull* AsNull() { return nullptr; }
            virtual const JsonNull* AsNull() const { return nullptr; }
            virtual JsonBool* AsBool() { return nullptr; }
            virtual const JsonBool* AsBool() const { return nullptr; }
            virtual JsonNumber* AsNumber() { return nullptr; }
            virtual const JsonNumber* AsNumber() const { return nullptr; }
            virtual JsonString* AsString() { return nullptr; }
            virtual const JsonString* AsString() const { return nullptr; }
            virtual JsonObject* AsObject() { return nullptr; }
            virtual const JsonObject* AsObject() const { return nullptr; }
            virtual JsonArray* AsArray() { return nullptr; }
            virtual const JsonArray* AsArray() const { return nullptr; }

            virtual void clear() {}
            virtual bool empty() const { return true; }

        protected:
            JsonValue() : _type(JNULL) {}
            JsonValue(ValueType type) : _type(type) {}

        private:
            // No copy/move allowed
            JsonValue(const JsonValue& rhs) : _type(JNULL) {}
            JsonValue(JsonValue&& rhs) : _type(JNULL) {}

        private:
            ValueType   _type;
        };


		inline bool IsValidJsonValue(const JsonValue* p) { return (p && !p->IsNull()); }
		inline bool IsValidJsonBool(const JsonValue* p) { return (p && p->IsBool()); }
		inline bool IsValidJsonNumber(const JsonValue* p) { return (p && p->IsNumber()); }
		inline bool IsValidJsonString(const JsonValue* p) { return (p && p->IsString()); }
		inline bool IsValidJsonObject(const JsonValue* p) { return (p && p->IsObject()); }
		inline bool IsValidJsonArray(const JsonValue* p) { return (p && p->IsArray()); }

		typedef JsonValue*			PJSONVALUE;
		typedef const JsonValue*	PCJSONVALUE;
		typedef JsonNull*			PJSONNULL;
		typedef const JsonNull*		PCJSONNULL;
		typedef JsonBool*			PJSONBOOL;
		typedef const JsonBool*		PCJSONBOOL;
		typedef JsonNumber*			PJSONNUMBER;
		typedef const JsonNumber*	PCJSONNUMBER;
		typedef JsonString*			PJSONSTRING;
		typedef const JsonString*	PCJSONSTRING;
		typedef JsonObject*			PJSONOBJECT;
		typedef const JsonObject*	PCJSONOBJECT;
		typedef JsonArray*			PJSONARRAY;
		typedef const JsonArray*	PCJSONARRAY;

        class JsonNull : public JsonValue
        {
        public:
            virtual ~JsonNull() {}
            virtual JsonNull* AsNull() { return dynamic_cast<JsonNull*>(this); }
            virtual const JsonNull* AsNull() const { return dynamic_cast<const JsonNull*>(this); }

        protected:
            JsonNull() : JsonValue(JNULL) {}
            friend class JsonValue;
        };

        class JsonBool : public JsonValue
        {
        public:
            virtual ~JsonBool() {}

            virtual JsonBool* AsBool() { return this; }
            virtual const JsonBool* AsBool() const { return this; }

            virtual void clear() {}
            virtual bool empty() const { return false; }

            inline bool GetBool() const { return _v; }

        protected:
            JsonBool() : JsonValue(JBOOL), _v(false)
            {
            }

            JsonBool(bool b) : JsonValue(JBOOL), _v(b)
            {
            }

        private:
            bool _v;

            friend class JsonValue;
        };

        class JsonNumber : public JsonValue
        {
        public:
            virtual ~JsonNumber() {}

            virtual JsonNumber* AsNumber() { return this; }
            virtual const JsonNumber* AsNumber() const { return this; }

            virtual void clear() { _v.n = 0; }
            virtual bool empty() const { return false; }

            bool IsDecimal() const { return _decimal; }
            int ToInt() const
            {
                return static_cast<int>(ToInt64());
            }
            __int64 ToInt64() const
            {
                if (IsDecimal()) {
                    return static_cast<__int64>(_v.f);
                }
                else {
                    return _v.n;
                }
            }
            unsigned int ToUint() const
            {
                return (unsigned int)ToUint64();
            }
            unsigned __int64 ToUint64() const
            {
                return (unsigned __int64)ToInt64();
            }
            double ToFloat() const
            {
                if (IsDecimal()) {
                    return _v.f;
                }
                else {
                    return static_cast<double>(_v.n);
                }
            }


        protected:
            JsonNumber() : JsonValue(JNUMBER), _v({ 0 }), _decimal(false)
            {
            }

            JsonNumber(int n) : JsonValue(JNUMBER), _v({ 0 }), _decimal(false)
            {
                _v.n = static_cast<__int64>(n);
            }

            JsonNumber(__int64 n) : JsonValue(JNUMBER), _v({ 0 }), _decimal(false)
            {
                _v.n = n;
            }

            JsonNumber(unsigned int n) : JsonValue(JNUMBER), _v({ 0 }), _decimal(false)
            {
                memcpy(&_v.n, &n, sizeof(unsigned int));
            }

            JsonNumber(unsigned __int64 n) : JsonValue(JNUMBER), _v({ 0 }), _decimal(false)
            {
                _v.n = static_cast<__int64>(n);
            }

            JsonNumber(float f) : JsonValue(JNUMBER), _v({ 0 }), _decimal(true)
            {
                _v.f = f;
            }

            JsonNumber(double f) : JsonValue(JNUMBER), _v({ 0 }), _decimal(true)
            {
                _v.f = f;
            }

        private:
            bool _decimal;
            union {
                __int64 n;
                double  f;
            }   _v;

            friend class JsonValue;
        };

        class JsonString : public JsonValue
        {
        public:
            virtual ~JsonString() {}

            virtual JsonString* AsString() { return this; }
            virtual const JsonString* AsString() const { return this; }

            virtual void clear() { _v.clear(); }
            virtual bool empty() const { return _v.empty(); }

            const std::wstring& GetString() const { return _v; }

        protected:
            JsonString() : JsonValue(JSTRING) {}
            JsonString(const std::string& s) : JsonValue(JSTRING), _v(s.begin(), s.end()) {}
            JsonString(const std::wstring& s) : JsonValue(JSTRING), _v(s) {}

        private:
            std::wstring    _v;

            friend class JsonValue;
        };

        class JsonObject : public JsonValue
        {
        public:
            virtual ~JsonObject()
			{
				clear();
			}

            typedef std::vector<std::pair<std::wstring, JsonValue*>>::iterator        iterator;
            typedef std::vector<std::pair<std::wstring, JsonValue*>>::const_iterator  const_iterator;
            typedef std::pair<std::wstring, JsonValue*>                             value_type;

            virtual JsonObject* AsObject() { return this; }
            virtual const JsonObject* AsObject() const { return this; }

            inline iterator begin() { return _v.begin(); }
            inline iterator end() { return _v.end(); }
            inline const_iterator cbegin() const { return _v.cbegin(); }
            inline const_iterator cend()const { return _v.cend(); }

            virtual void clear()
            {
				while (!_v.empty()) {
					auto it = _v.begin();
					if (NULL != (*it).second) {
						delete (*it).second;
						(*it).second = NULL;
					}
					_v.erase(it);
				}
            }
            virtual bool empty() const { return _v.empty(); }

            size_t size() const { return _v.size(); }

            JsonValue* at(const std::wstring& key)
            {
                auto it = std::find_if(_v.begin(), _v.end(), [&](const value_type& it) -> bool {
                    return (0 == _wcsicmp(it.first.c_str(), key.c_str()));
                });
                return (it == _v.end()) ? nullptr : it->second;
            }

            const JsonValue* at(const std::wstring& key) const
            {
                auto it = std::find_if(_v.begin(), _v.end(), [&](const value_type& it) -> bool {
                    return (0 == _wcsicmp(it.first.c_str(), key.c_str()));
                });
                return (it == _v.end()) ? nullptr : it->second;
            }

            bool set(const std::wstring& key, JsonValue* v)
            {
                if (key.empty() || v == nullptr)
                    return false;

                auto it = std::find_if(_v.begin(), _v.end(), [&](const value_type& it) -> bool {
                    return (0 == _wcsicmp(it.first.c_str(), key.c_str()));
                });

                if (it == _v.end()) {
                    _v.push_back(std::pair<std::wstring,JsonValue*>(key, v));
                }
                else {
                    if (it->second) {
                        delete it->second;
                        it->second = nullptr;
                    }
                    it->second = v;
                }

                return true;
            }

            JsonValue* remove(const std::wstring& key)
            {
                JsonValue* p = nullptr;
                auto it = std::find_if(_v.begin(), _v.end(), [&](const value_type& it) -> bool {
                    return (0 == _wcsicmp(it.first.c_str(), key.c_str()));
                });
                if (it != _v.end()) {
                    p = it->second;
                    it->second = nullptr;
                }
                _v.erase(it);
                return p;
            }

            bool erase(const std::wstring& key)
            {
                bool erased = false;
                auto it = std::find_if(_v.begin(), _v.end(), [&](const value_type& it) -> bool {
                    return (0 == _wcsicmp(it.first.c_str(), key.c_str()));
                });
                if (it != _v.end()) {
                    if (it->second) {
                        delete it->second;
                        it->second = nullptr;
                    }
                    _v.erase(it);
                    erased = true;
                }
                return erased;
            }

        protected:
            JsonObject() : JsonValue(JOBJECT) {}


        private:
            std::vector<std::pair<std::wstring, JsonValue*>> _v;

            friend class JsonValue;
        };

        class JsonArray : public JsonValue
        {
        public:
            virtual ~JsonArray()
			{
				clear();
			}

            typedef std::vector<JsonValue*>::iterator       iterator;
            typedef std::vector<JsonValue*>::const_iterator const_iterator;
            typedef JsonValue*                              value_type;

            iterator begin() { return _v.begin(); }
            iterator end() { return _v.end(); }
            const_iterator cbegin() const { return _v.cbegin(); }
            const_iterator cend() const { return _v.cend(); }

            virtual JsonArray* AsArray() { return this; }
            virtual const JsonArray* AsArray() const { return this; }

            virtual void clear()
            {
                for (int i = 0; i < (int)_v.size(); i++)
                {
                    JsonValue* p = _v[i];
                    if (nullptr != p) {
                        delete p;
                        _v[i] = nullptr;
                    }
                }
                _v.clear();
            }
            virtual bool empty() const { return _v.empty(); }

            size_t size() const { return _v.size(); }

            JsonValue* at(size_t i)
            {
                if (i < _v.size())
                    return _v[i];
                else
                    return nullptr;
            }

            const JsonValue* at(size_t i) const
            {
                if (i < _v.size())
                    return _v[i];
                else
                    return nullptr;
            }

            bool push_back(JsonValue* v)
            {
                if (v == nullptr)
                    return false;
                _v.push_back(v);
                return true;
            }

            JsonValue* pop_back()
            {
                JsonValue* p = nullptr;
                if (!_v.empty()) {
                    p = _v[_v.size() - 1];
                    _v[_v.size() - 1] = nullptr;
                    _v.pop_back();
                }
                return p;
            }

            bool erase(size_t i)
            {
                bool erased = false;
                if (i < _v.size()) {
                    JsonValue* p = _v[i];
                    if (p) {
                        _v[i] = nullptr;
                        delete p;
                    }
                    _v.erase(_v.begin() + i);
                    erased = true;
                }
                return erased;
            }

        protected:
            JsonArray() : JsonValue(JARRAY) {}

        private:
            std::vector<JsonValue*> _v;

            friend class JsonValue;
        };

        typedef enum WriterType {
            Compacted = 0,
            Formatted
        } WriterType;

        template <typename T>
        class Writer
        {
        public:
            Writer() : _indent(0), _type(Compacted) {}
            virtual ~Writer() {}

        protected:
            void WriteValue(std::basic_ostream<T>& os, const JsonValue* v)
            {
                if (v->IsNull()) {
                    WriteNull(os, v->AsNull());
                }
                else if (v->IsBool()) {
                    WriteBool(os, v->AsBool());
                }
                else if (v->IsNumber()) {
                    WriteNumber(os, v->AsNumber());
                }
                else if (v->IsString()) {
                    WriteString(os, v->AsString());
                }
                else if (v->IsObject()) {
                    WriteObject(os, v->AsObject());
                }
                else if (v->IsArray()) {
                    WriteArray(os, v->AsArray());
                }
                else {
                    ;
                }
            }

            inline bool IsFormatted() const { return (WriterType::Formatted == _type); }

        protected:

            inline void SetType(WriterType type) { _type = type; }
            inline void IncreaseIndent() { _indent += 4; }
            inline void DecreaseIndent() { _indent = (_indent > 4) ? (_indent - 4) : 0; }
            inline void WriteIndent(std::basic_ostream<T>& os)
            {
                std::vector<T> indentbuf;
                indentbuf.resize(_indent, (T)' ');
                os << std::basic_string<T>(indentbuf.begin(), indentbuf.end());
            }
            inline void WriteLineEnd(std::basic_ostream<T>& os)
            {
//#ifdef _WIN32
//                os << ((T)'\r');
//#endif
                os << ((T)'\n');
            }

            void WriteNull(std::basic_ostream<T>& os, const JsonNull* v)
            {
                static const std::string name("null");
                os << std::basic_string<T>(name.begin(), name.end());
            }

            void WriteBool(std::basic_ostream<T>& os, const JsonBool* v)
            {
                static const std::string sTrue("true");
                static const std::string sFalse("false");
                os << (v->GetBool() ? std::basic_string<T>(sTrue.cbegin(), sTrue.cend()) : std::basic_string<T>(sFalse.cbegin(), sFalse.cend()));
            }

            void WriteNumber(std::basic_ostream<T>& os, const JsonNumber* v)
            {
                std::string s;
                if (v->IsDecimal()) {
                    sprintf_s(NX::string_buffer(s, 128), 128, "%f", v->ToFloat());
                }
                else {
                    sprintf_s(NX::string_buffer(s, 128), 128, "%I64d", v->ToInt64());
                }
                os << std::basic_string<T>(s.begin(), s.end());
            }

            void WriteString(std::basic_ostream<T>& os, const JsonString* v)
            {
                std::basic_string<T> s;
                std::wstring ws = NX::escape(v->GetString());
                if (sizeof(wchar_t) == sizeof(T)) {
                    s = std::basic_string<T>(ws.begin(), ws.end());
                }
                else {
                    const std::string& as = NX::conv::utf16toutf8(ws);
                    s = std::basic_string<T>(as.begin(), as.end());
                }
                os << ((T)'"');
                os << s;
                os << ((T)'"');
            }

            void WriteObject(std::basic_ostream<T>& os, const JsonObject* v)
            {
                os << ((T)'{');
                if (IsFormatted()) {
                    IncreaseIndent();
                }

                for (JsonObject::const_iterator it = v->cbegin(); it != v->cend(); ++it) {

                    if (it != v->cbegin()) {
                        os << (T)',';
                    }
                    if (IsFormatted()) {
                        WriteLineEnd(os);
                        WriteIndent(os);
                    }
                
                    std::wstring key = NX::escape(it->first);
                    std::basic_string<T> s;
                    if (sizeof(wchar_t) == sizeof(T)) {
                        s = std::basic_string<T>(key.begin(), key.end());
                    }
                    else {
                        const std::string& as = NX::conv::utf16toutf8(key);
                        s = std::basic_string<T>(as.begin(), as.end());
                    }
                    os << (T)'"';
                    os << s;
                    os << (T)'"';
                    os << (T)':';
                    if (IsFormatted()) {
                        os << (T)' ';
                    }
                    WriteValue(os, it->second);

                }

                if (IsFormatted()) {
                    DecreaseIndent();
                    WriteLineEnd(os);
                    WriteIndent(os);
                }
                os << ((T)'}');
            }

            void WriteArray(std::basic_ostream<T>& os, const JsonArray* v)
            {
                os << ((T)'[');
                if (IsFormatted()) {
                    IncreaseIndent();
                }

                for (JsonArray::const_iterator it = v->cbegin(); it != v->cend(); ++it) {

                    if (it != v->cbegin()) {
                        os << (T)',';
                    }
                    if (IsFormatted()) {
                        WriteLineEnd(os);
                        WriteIndent(os);
                    }
                    WriteValue(os, (*it));
                }

                if (IsFormatted()) {
                    DecreaseIndent();
                    WriteLineEnd(os);
                    WriteIndent(os);
                }
                os << ((T)']');
            }

        private:
            int _indent;
            WriterType _type;
        };

        template <typename T>
        class JsonStringWriter : public Writer<T>
        {
        public:
            JsonStringWriter() : Writer()
            {
            }
            virtual ~JsonStringWriter()
            {
            }

            std::basic_string<T> Write(const JsonValue* v, WriterType type = Compacted)
            {
                SetType(type);
                std::basic_ostringstream<T> os;
                WriteValue(os, v);
                return os.str();
            }
        };

        class FileWriter : public Writer<char>
        {
        public:
            FileWriter() : Writer()
            {
            }
            virtual ~FileWriter()
            {
            }

            bool Write(const std::wstring& file, const JsonValue* v, WriterType type = Compacted)
            {
                std::ofstream ofs;
                ofs.open(file, std::ios_base::out | std::ios_base::trunc);
                if (!ofs.is_open())
                    return false;
                SetType(type);
                WriteValue(ofs, v);
                ofs.close();
                return true;
            }
        };

        template <typename T>
        class JsonParser
        {
        public:
            JsonParser() : _errcode(0), _errpos(0)
            {
            }

            virtual ~JsonParser()
            {
            }

            inline size_t GetErrorPosition() const { return _errpos; }
            inline int GetErrorCode() const { return _errcode; }
            inline bool Succeeded() const { return (0 == _errcode); }

            typedef enum JsonParseError {
                JESuccess = 0,
                JEInvalidRootType,
                JEUnexpectedEnding,
                JEUnexpectedChar,
                JEInvalidNullValue,
                JEInvalidBoolValue,
                JEInvalidNumberValue,
                JEInvalidStringValue,
                JEInvalidObjectKey,
                JEInvalidObjectValue,
                JEInvalidArrayValue,
                JEInvalidEscapedChar,
                JEInvalidNumberChar,
                JEInvalidHexChar,
                JEMissingComma,
                JEMissingColon,
                JEMissingLeftQuotationMark,
                JEMissingRightQuotationMark,
                JEMissingObjectLeftBrace,
                JEMissingObjectRightBrace,
                JEMissingArrayLeftBracket,
                JEMissingArrayRightBracket,
                JEUnknown
            } JsonParseError;

            JsonValue* Parse()
            {
                _errcode = 0;
                _errpos = 0;
                T ch = PeekNextNwChar();
                if (ch == (T)'{') {
                    return ParseObject();
                }
                else if (ch == (T)'[') {
                    return ParseArray();
                }
                else {
                    _errcode = JEInvalidRootType;
                    return nullptr;
                }
            }

        protected:
            virtual T GetNextChar() = 0;
            virtual T PeekNextChar() = 0;
            virtual bool ReachEnd() = 0;

            virtual T GetNextNwChar()
            {
                T ch = 0;
                while (0 != (ch = GetNextChar()) && iswspace((int)ch));
                return ch;
            }

            virtual T PeekNextNwChar()
            {
                T ch = 0;
                while (0 != (ch = PeekNextChar()) && iswspace((int)ch)) {
                    (void)GetNextChar();
                }
                return ch;
            }

            void IncreasePos() { ++_errpos; }

        protected:
            JsonValue* ParseValue()
            {
                JsonValue* pv = nullptr;
                T ch = PeekNextNwChar();
                switch (ch)
                {
                case 0:
                    _errcode = JEUnexpectedEnding;
                    break;
                case (T)'{':
                    pv = ParseObject();
                    break;
                case (T)'[':
                    pv = ParseArray();
                    break;
                case (T)'"':
                    pv = ParseString();
                    break;
                case (T)'N':
                case (T)'n':
                    pv = ParseNull();
                    break;
                case (T)'T':
                case (T)'t':
                case (T)'F':
                case (T)'f':
                    pv = ParseBool();
                    break;
                case (T)'-':
                case (T)'.':
                case (T)'0':
                case (T)'1':
                case (T)'2':
                case (T)'3':
                case (T)'4':
                case (T)'5':
                case (T)'6':
                case (T)'7':
                case (T)'8':
                case (T)'9':
                    pv = ParseNumber();
                    break;
                default:
                    _errcode = JEUnexpectedChar;
                    break;
                }

                return pv;
            }

            JsonValue* ParseObject()
            {
                T ch = GetNextChar();
                if ((T)'{' != ch) {
                    _errcode = ReachEnd() ? JEUnexpectedEnding : JEMissingObjectLeftBrace;
                    return nullptr;
                }

                JsonObject* po = JsonValue::CreateObject();
                if (nullptr == po) {
                    SetLastError(ERROR_NOT_ENOUGH_MEMORY);
                    return po;
                }

                do {

                    ch = PeekNextNwChar();

                    // End of Object
                    if ((T)'}' == ch) {
                        (void)GetNextChar();
                        break;
                    }

                    // Empty Object item
                    if ((T)',' == ch) {
                        (void)GetNextChar();
                        continue;
                    }

                    // First: Key
                    if ((T)'"' != ch) {
                        _errcode = ReachEnd() ? JEUnexpectedEnding : JEMissingLeftQuotationMark;
                        goto _error;
                    }

                    std::basic_string<T> key;

                    if (!ParseQuotaString(key)) {
                        goto _error;
                    }

                    if (key.empty()) {
                        _errcode = JEInvalidObjectKey;
                        goto _error;
                    }

                    ch = GetNextNwChar();
                    if ((T)':' != ch) {
                        _errcode = ReachEnd() ? JEUnexpectedEnding : JEMissingColon;
                        goto _error;
                    }

                    JsonValue* value = ParseValue();
                    if (nullptr == value)
                        goto _error;

                    if (!po->set(NX::conv::to_wstring(key), value)) {
                        goto _error;
                    }

                } while (TRUE);

                goto _succeed;
            _error:
                if (po) {
                    delete po;
                    po = nullptr;
                }

            _succeed:
                return po;
            }

            JsonValue* ParseArray()
            {
                T ch = GetNextChar();
                if ((T)'[' != ch) {
                    _errcode = ReachEnd() ? JEUnexpectedEnding : JEMissingArrayLeftBracket;
                    return nullptr;
                }

                JsonArray* pa = JsonValue::CreateArray();
                if (nullptr == pa) {
                    SetLastError(ERROR_NOT_ENOUGH_MEMORY);
                    return pa;
                }

                do {

                    ch = PeekNextNwChar();

                    // End of Array
                    if ((T)']' == ch) {
                        (void)GetNextChar();
                        break;
                    }

                    // Empty Array item
                    if ((T)',' == ch) {
                        (void)GetNextChar();
                        continue;
                    }

                    JsonValue* value = ParseValue();
                    if (nullptr == value)
                        goto _error;

                    if (!pa->push_back(value)) {
                        goto _error;
                    }

                } while (TRUE);

                goto _succeed;
            _error:
                if (pa) {
                    delete pa;
                    pa = nullptr;
                }

            _succeed:
                return pa;
            }

            JsonValue* ParseNull()
            {
                if (0 != NX::icompare(GetNextChar(), (T)'N')) {
                    _errcode = ReachEnd() ? JEUnexpectedEnding : JEInvalidNullValue;
                    return nullptr;
                }
                if (0 != NX::icompare(GetNextChar(), (T)'u')) {
                    _errcode = ReachEnd() ? JEUnexpectedEnding : JEInvalidNullValue;
                    return nullptr;
                }
                if (0 != NX::icompare(GetNextChar(), (T)'l')) {
                    _errcode = ReachEnd() ? JEUnexpectedEnding : JEInvalidNullValue;
                    return nullptr;
                }
                if (0 != NX::icompare(GetNextChar(), (T)'l')) {
                    _errcode = ReachEnd() ? JEUnexpectedEnding : JEInvalidNullValue;
                    return nullptr;
                }

                return JsonValue::CreateNull();
            }

            JsonValue* ParseBool()
            {
                bool v = false;
                T ch = GetNextChar();
                if (0 == NX::icompare(ch, (T)'T')) {
                    if (0 != NX::icompare(GetNextChar(), (T)'r')) {
                        _errcode = ReachEnd() ? JEUnexpectedEnding : JEInvalidBoolValue;
                        return nullptr;
                    }
                    if (0 != NX::icompare(GetNextChar(), (T)'u')) {
                        _errcode = ReachEnd() ? JEUnexpectedEnding : JEInvalidBoolValue;
                        return nullptr;
                    }
                    if (0 != NX::icompare(GetNextChar(), (T)'e')) {
                        _errcode = ReachEnd() ? JEUnexpectedEnding : JEInvalidBoolValue;
                        return nullptr;
                    }
                    v = true;
                }
                else if (0 == NX::icompare(ch, (T)'F')) {
                    if (0 != NX::icompare(GetNextChar(), (T)'a')) {
                        _errcode = ReachEnd() ? JEUnexpectedEnding : JEInvalidBoolValue;
                        return nullptr;
                    }
                    if (0 != NX::icompare(GetNextChar(), (T)'l')) {
                        _errcode = ReachEnd() ? JEUnexpectedEnding : JEInvalidBoolValue;
                        return nullptr;
                    }
                    if (0 != NX::icompare(GetNextChar(), (T)'s')) {
                        _errcode = ReachEnd() ? JEUnexpectedEnding : JEInvalidBoolValue;
                        return nullptr;
                    }
                    if (0 != NX::icompare(GetNextChar(), (T)'e')) {
                        _errcode = ReachEnd() ? JEUnexpectedEnding : JEInvalidBoolValue;
                        return nullptr;
                    }
                    v = false;
                }
                else {
                    _errcode = JEInvalidBoolValue;
                    return nullptr;
                }

                return JsonValue::CreateBool(v);
            }

            JsonValue* ParseNumber()
            {
                std::basic_string<T> s;
                bool    is_float = false;
                bool    is_exponent = false;
                bool    is_signed = false;

                T ch = GetNextChar();
                if (ch == T('-')) {
                    is_signed = true;
                    ch = GetNextChar();
                }
                if (ch == T('.')) {
                    s.push_back(T('0'));
                    is_float = true;
                    ch = GetNextChar();
                }
                if (!NX::isdigit(ch)) {
                    _errcode = ReachEnd() ? JEUnexpectedEnding: JEInvalidNumberChar;
                    return nullptr;
                }
                s.push_back(ch);

                do {

                    ch = PeekNextChar();
                    if (!NX::isdigit(ch) && ch != T('.') && ch != T('E') && ch != T('e')) {
                        break;
                    }

                    (void)GetNextChar();
                    if (ch == (T)'.') {
                        if (is_float) {
                            _errcode = JEInvalidNumberChar;
                            return nullptr;
                        }
                        is_float = true;
                        s.push_back(ch);
                    }
                    else if (ch == (T)'E' || ch == (T)'e') {
                        if (is_exponent) {
                            _errcode = JEInvalidNumberChar;
                            return nullptr;
                        }
                        is_exponent = true;
                        s.push_back(ch);
                    }
                    else {
                        s.push_back(ch);
                    }

                } while (TRUE);

                JsonValue* p = nullptr;
                try {
                    if (is_float || is_exponent) {
                        double f = std::stod(s);
                        p = JsonValue::CreateNumber(f);
                    }
                    else {
                        __int64 n = std::stoll(s);
                        p = JsonValue::CreateNumber(n);
                    }
                }
                catch (std::exception& e) {
                    UNREFERENCED_PARAMETER(e);
                    _errcode = JEInvalidNumberChar;
                }

                return p;
            }

            JsonValue* ParseString()
            {
                std::basic_string<T> s;

                if (!ParseQuotaString(s)) {
                    return nullptr;
                }

                return JsonValue::CreateString(s);
            }

            bool ParseQuotaString(std::basic_string<T>& s)
            {
                bool result = false;

                T ch = GetNextChar();
                if (ch != (T)'\"') {
                    _errcode = ReachEnd() ? JEUnexpectedEnding : JEMissingLeftQuotationMark;
                    return false;
                }

                do {

                    ch = GetNextChar();
                    if (ReachEnd()) {
                        _errcode = JEUnexpectedEnding;
                        return false;
                    }

                    // Finished
                    if (ch == (T)'\"') {
                        result = true;
                        break;
                    }

                    if (ch != (T)'\\') {
                        s.push_back(ch);
                    }
                    else {
                        // Skip '\\'
                        ch = GetNextChar();

                        switch (ch)
                        {
                        case 0:
                            _errcode = JEUnexpectedEnding;
                            break;
                        case T('\"'):
                            s.push_back(T('\"'));
                            break;
                        case T('\\'):
                            s.push_back(T('\\'));
                            break;
                        case T('/'):
                            s.push_back(T('/'));
                            break;
                        case T('b'):
                            s.push_back(T('\b'));
                            break;
                        case T('f'):
                            s.push_back(T('\f'));
                            break;
                        case T('r'):
                            s.push_back(T('\r'));
                            break;
                        case T('n'):
                            s.push_back(T('\n'));
                            break;
                        case T('t'):
                            s.push_back(T('\t'));
                            break;
                        case T('u'):
                            // A four-hexdigit Unicode character.
                            // Transform into a 16 bit code point.
                            {
                                wchar_t wc = 0;
                                T hex_str[4] = { 0, 0, 0, 0 };
                                for (int i = 0; i < 4; i++) {
                                    int n = (unsigned char)NX::ctoi(GetNextChar());
                                    if (n < 0) {
                                        _errcode = JEInvalidHexChar;
                                        return false;
                                    }
                                    wc <<= 4;
                                    wc |= (((unsigned char)n) & 0xF);
                                }
                                if (sizeof(T) == sizeof(wchar_t)) {
                                    s.push_back((T)wc);
                                }
                                else {
                                    std::wstring ws;
                                    ws.push_back(wc);
                                    std::string as = NX::conv::to_string(ws);
                                    s += std::basic_string<T>(as.begin(), as.end());
                                }
                            }
                            break;
                        default:
                            _errcode = JEInvalidEscapedChar;
                            break;
                        }

                        if (JESuccess != _errcode)
                            break;
                    }

                } while (TRUE);

                return result;
            }

        private:
            size_t _errpos;
            int    _errcode;
        };

        template <typename T>
        class JsonStringParser : public JsonParser<T>
        {
        public:
            JsonStringParser() : JsonParser(){}
            virtual ~JsonStringParser() {}

            JsonValue* Parse(const std::basic_string<T>& s)
            {
                _s = s.c_str();
                return JsonParser::Parse();
            }

        protected:
            virtual T GetNextChar()
            {
                if (!ReachEnd()) {
                    IncreasePos();
                    return (*_s++);
                }
                return 0;
            }

            virtual T PeekNextChar()
            {
                return ReachEnd() ? 0 : (*_s);
            }

            virtual bool ReachEnd()
            {
                if (nullptr == _s) return true;
                return (0 == *_s) ? true : false;
            }

        private:
            const T* _s;
        };

        template <typename T>
        class JsonFileParser : public JsonParser<T>
        {
        public:
            JsonFileParser() : JsonParser() {}
            virtual ~JsonFileParser() {}

            JsonValue* Parse(const std::string& file)
            {
                JsonValue* p = nullptr;

                try {
                    _f.open(file.c_str());
                    if (!_f.is_open())
                        throw std::exception("fail to open file");
                    p = JsonParser::Parse();
                }
                catch (std::exception& e) {
                    UNREFERENCED_PARAMETER(e);
                    if (p) {
                        delete p;
                        p = nullptr;
                    }
                }

                return p;
            }

            JsonValue* Parse(const std::wstring& file)
            {
                JsonValue* p = nullptr;

                try {
                    _f.open(file.c_str());
                    if (!_f.is_open())
                        throw std::exception("fail to open file");
                    p = JsonParser::Parse();
                }
                catch (std::exception& e) {
                    UNREFERENCED_PARAMETER(e);
                    if (p) {
                        delete p;
                        p = nullptr;
                    }
                }

                return p;
            }

        protected:
            virtual T GetNextChar()
            {
                if (!ReachEnd()) {
                    IncreasePos();
                    return _f.get();
                }
                return 0;
            }

            virtual T PeekNextChar()
            {
                return ReachEnd() ? 0 : _f.peek();
            }

            virtual bool ReachEnd()
            {
                return _f.eof();
            }

        private:
            std::basic_ifstream<T> _f;
        };


        class JsonDocument
        {
        public:
            JsonDocument() : _root(nullptr)
            {
            }

            virtual ~JsonDocument()
            {
                Clear();
            }

            bool Create()
            {
                _root = (JsonValue*)JsonValue::CreateObject();
                return !Empty();
            }

            bool Empty() const
            {
                return (nullptr == _root);
            }

            void Clear()
            {
                if (nullptr != _root) {
                    delete _root;
                    _root = nullptr;
                }
            }

            inline JsonValue* GetRoot() { return _root; }
            inline const JsonValue* GetRoot() const { return _root; }

            template<typename T>
            bool LoadJsonFile(const std::string& file, int* err_code = nullptr, size_t* err_pos = nullptr)
            {
                JsonFileParser<T> parser;
                _root = parser.Parse(file);
                if (err_code) *err_code = parser.GetErrorCode();
                if (err_pos) *err_pos = (0 != parser.GetErrorCode()) ? parser.GetErrorPosition() : 0;
                return !Empty();
            }

            template<typename T>
            bool LoadJsonFile(const std::wstring& file, int* err_code = nullptr, size_t* err_pos = nullptr)
            {
                JsonFileParser<T> parser;
                _root = parser.Parse(file);
                if (err_code) *err_code = parser.GetErrorCode();
                if (err_pos) *err_pos = (0 != parser.GetErrorCode()) ? parser.GetErrorPosition() : 0;
                return !Empty();
            }

            template<typename T>
            bool LoadJsonString(const std::basic_string<T>& s, int* err_code = nullptr, size_t* err_pos = nullptr)
            {
                JsonStringParser<T> parser;
                _root = parser.Parse(s);
                if (err_code) *err_code = parser.GetErrorCode();
                if (err_pos) *err_pos = (0 != parser.GetErrorCode()) ? parser.GetErrorPosition() : 0;
                return !Empty();
            }
            
        protected:
            JsonValue* _Move()
            {
                JsonValue* tmp = _root;
                _root = nullptr;
                return tmp;
            }

        private:
            // No copy is allowed
            JsonDocument(const JsonDocument& rhs) {}
            JsonDocument& operator = (const JsonDocument& rhs) {}

        private:
            JsonValue*  _root;
        };

    }   // namespace rapidjson
}   // namespace NX


#endif