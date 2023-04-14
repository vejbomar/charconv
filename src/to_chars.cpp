// Copyright 2020-2023 Junekey Jeon
// Copyright 2022 Peter Dimov
// Copyright 2023 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/charconv/detail/dragonbox.hpp>
#include <boost/charconv/detail/floff.hpp>
#include <boost/charconv/to_chars.hpp>
#include <limits>
#include <cstdio>
#include <cstring>
#include <cstdint>
#include <cmath>

boost::charconv::to_chars_result boost::charconv::to_chars(char* first, char* last, float value, boost::charconv::chars_format fmt, int precision) noexcept
{
    if (fmt == boost::charconv::chars_format::general || fmt == boost::charconv::chars_format::fixed)
    {
        const auto abs_value = std::abs(value);
        constexpr float max_value = static_cast<float>((std::numeric_limits<std::uint32_t>::max)());
        constexpr float min_value = 1.0f / max_value;
        if (abs_value < max_value && abs_value > min_value)
        {
            if (value < 0)
            {
                *first++ = '-';
            }
            return boost::charconv::to_chars(first, last, static_cast<std::uint32_t>(abs_value));
        }
        else
        {
            auto* ptr = boost::charconv::detail::floff<boost::charconv::detail::main_cache_full, boost::charconv::detail::extended_cache_long>(value, std::numeric_limits<float>::max_digits10 - 1, first, fmt);
            return { ptr, 0 };            
        }
    }
    else if (fmt == boost::charconv::chars_format::scientific)
    {
        if (precision == -1)
        {
            precision = std::numeric_limits<float>::max_digits10;
        }
        if (precision > static_cast<std::ptrdiff_t>(last - first))
        {
            return { first, EOVERFLOW };
        }
        auto* ptr = boost::charconv::detail::floff<boost::charconv::detail::main_cache_full, boost::charconv::detail::extended_cache_long>(value, precision, first, fmt);
        return { ptr, 0 };
    }
    else if (fmt == boost::charconv::chars_format::hex)
    {
        return boost::charconv::detail::to_chars_hex(first, last, value, precision);
    }
    return { first + std::strlen(first), 0 };
}

boost::charconv::to_chars_result boost::charconv::to_chars(char* first, char* last, double value, boost::charconv::chars_format fmt, int precision) noexcept
{
    const std::ptrdiff_t buffer_size = last - first;
    
    // Unspecified precision so we always go with shortest representation
    if (precision == -1)
    {
        if (fmt == boost::charconv::chars_format::general || fmt == boost::charconv::chars_format::fixed)
        {
            const auto abs_value = std::abs(value);
            if (abs_value >= 1 && abs_value < 1e18) // 1 x 10^(max_digits10 + 1)
            {
                auto value_struct = boost::charconv::detail::to_decimal(value);
                if (value_struct.is_negative)
                {
                    *first++ = '-';
                }

                auto r = boost::charconv::to_chars(first, last, value_struct.significand);
                if (r.ec != 0)
                {
                    return r;
                }
                
                // Bounds check
                if (value_struct.exponent < 0 && -value_struct.exponent < buffer_size)
                {
                    std::memmove(r.ptr + value_struct.exponent + 1, r.ptr + value_struct.exponent, -value_struct.exponent);
                    std::memset(r.ptr + value_struct.exponent, '.', 1);
                }

                return { r.ptr, 0 };
            }
            else
            {
                auto* ptr = boost::charconv::detail::to_chars(value, first);
                return { ptr, 0 };
            }
        }
        else if (fmt == boost::charconv::chars_format::scientific)
        {
            auto* ptr = boost::charconv::detail::to_chars(value, first);
            return { ptr, 0 };
        }
    }
    else
    {
        if (fmt != boost::charconv::chars_format::hex)
        {
            auto* ptr = boost::charconv::detail::floff<boost::charconv::detail::main_cache_full, boost::charconv::detail::extended_cache_long>(value, precision, first, fmt);
            return { ptr, 0 };
        }
    }

    // Hex handles both cases already
    return boost::charconv::detail::to_chars_hex(first, last, value, precision);
}

boost::charconv::to_chars_result boost::charconv::to_chars( char* first, char* last, long double value ) noexcept
{
    std::snprintf( first, last - first, "%.*Lg", std::numeric_limits<long double>::max_digits10, value );
    return { first + std::strlen(first), 0 };
}
