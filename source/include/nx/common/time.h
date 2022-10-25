

#pragma once
#ifndef __LIBRMC_TIME_HPP__
#define __LIBRMC_TIME_HPP__

#include <nx\common\result.h>

#include <string>

namespace NX {

    namespace time {

        __int64 filetime_to_time(const FILETIME* t);
        __int64 systemtime_to_time(const SYSTEMTIME* t);

        bool is_iso_8601_time_string(const std::wstring& s);
        bool is_iso_8601_zone_time_string(const std::wstring& s);
        bool is_rfc_1123_0_time_string(const std::wstring& s);
        bool is_rfc_1123_1_time_string(const std::wstring& s);
        bool is_special_1_time_string(const std::wstring& s);
        bool is_time_only_string(const std::wstring& s);

		class formatted_span
		{
		public:
			formatted_span();
			explicit formatted_span(__int64 seconds);
			explicit formatted_span(const formatted_span& rhs);
			~formatted_span();

			formatted_span& operator = (const formatted_span& rhs);

			std::wstring serialize();
			std::wstring serialize_timepassed();

		private:
			int _days;
			int _hours;
			int _minutes;
			int _seconds;
		};

        class span
        {
        public:
            span();
            explicit span(const span& s);
            explicit span(const std::wstring& s);
            span(__int64 s);
            ~span();

            static span seconds_span(__int64 seconds);
            static span minutes_span(__int64 minutes);
            static span hours_span(__int64 hours);
            static span days_span(__int64 days);

            inline operator __int64() const { return _span; }
            inline span& operator = (const span& other)
            {
                if (this != &other) {
                    _span = (__int64)other;
                }
                return *this;
            }

            __int64 to_seconds() const;

        private:
            __int64 _span;
        };

        class timezone
        {
        public:
            timezone();
            timezone(long zone_bias, const std::wstring& zone_name);
            timezone(const std::wstring& zone_bias, const std::wstring& zone_name);
            ~timezone();

            inline long bias() const { return _bias; }
            inline const std::wstring& name() const { return _name; }
            inline timezone& operator = (const timezone& other)
            {
                if (this != &other) {
                    _bias = other.bias();
                    _name = other.name();
                }
                return *this;
            }

            std::wstring serialize_bias() const;

            static timezone current_timezone();

        private:
            long parse(const std::wstring& zone_bias);

        private:
            long _bias;
            std::wstring _name;
        };

        class datetime
        {
        public:
            datetime();
            datetime(const datetime& other);
            explicit datetime(__int64 t);
            explicit datetime(const FILETIME* t);
            explicit datetime(const SYSTEMTIME* t);
            explicit datetime(const std::string& s);
            explicit datetime(const std::wstring& s);
            virtual ~datetime();

            inline operator __int64() const { return _t; }
            inline bool empty() const { return (0 == _t); }
            inline void clear() { _t = 0; }

            datetime& operator = (const datetime& other);

            datetime& operator += (const span& other);
            datetime& operator -= (const span& other);
            datetime& add(const span& other);
            datetime& sub(const span& other);
            span operator - (const datetime& other) const;
            bool operator == (const datetime& other) const;
            bool operator > (const datetime& other) const;
            bool operator >= (const datetime& other) const;
            bool operator < (const datetime& other) const;
            bool operator <= (const datetime& other) const;

            void from_filetime(const FILETIME* ft, bool local_time = false);
            void from_systemtime(const SYSTEMTIME* st, bool local_time = false);
            void to_filetime(FILETIME* ft, bool local_time = false) const;
            void to_systemtime(SYSTEMTIME* st, bool local_time = false) const;

            // Seconds since 1970-01-01 00:00:00
            static time::datetime from_dos_time(__int64 dos_time);
            __int64 to_dos_time() const;

            // Milliseconds since 1970-01-01 00:00:00
            static time::datetime from_unix_time(__int64 dos_time);
            __int64 to_java_time() const;

            static time::datetime current_time();


            // By default, it is in ISO 8601 format
            virtual Result parse(const std::wstring& s);
            virtual std::wstring serialize(bool local_time = false, bool with_milliseconds = false) const;

        private:
            __int64 _t;
        };

        class datetime_rfc1123 : public datetime
        {
        public:
            datetime_rfc1123() : datetime() {}
            datetime_rfc1123(__int64 t) : datetime(t) {}
            virtual ~datetime_rfc1123() {}
            virtual Result parse(const std::wstring& s);
            virtual std::wstring serialize(bool local_time = false, bool with_milliseconds = false) const;
        };

        class datetime_special_1 : public datetime
        {
        public:
            datetime_special_1() : datetime() {}
            datetime_special_1(__int64 t) : datetime(t) {}
            virtual ~datetime_special_1() {}
            virtual Result parse(const std::wstring& s);
            virtual std::wstring serialize(bool local_time = false, bool with_milliseconds = false) const;
        };


    }

}


#endif