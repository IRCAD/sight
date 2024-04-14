/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "core/memory/byte_size.hpp"

#include "core/memory/exception/bad_cast.hpp"

#include <boost/phoenix/operator.hpp>
#include <boost/spirit/include/qi.hpp>

#include <array>
#include <sstream>

namespace sight::core::memory
{

byte_size::byte_size()
= default;

//------------------------------------------------------------------------------

byte_size::byte_size(size_t _size, unit_t _unit)
{
    SIGHT_ASSERT(
        "Bad Unit",
        (_unit == BYTES) || (_unit == KB) || (_unit == MB) || (_unit == GB) || (_unit == TB) || (_unit == PB)
        || (_unit == KI_B) || (_unit == MI_B) || (_unit == GI_B) || (_unit == TI_B) || (_unit == PI_B)
    );
    this->set_size(_size, _unit);
}

//------------------------------------------------------------------------------

byte_size::byte_size(double _size, unit_t _unit)
{
    SIGHT_ASSERT(
        "Bad Unit",
        (_unit == BYTES) || (_unit == KB) || (_unit == MB) || (_unit == GB) || (_unit == TB) || (_unit == PB)
        || (_unit == KI_B) || (_unit == MI_B) || (_unit == GI_B) || (_unit == TI_B) || (_unit == PI_B)
    );
    if(_size < 0)
    {
        SIGHT_THROW_EXCEPTION_MSG(core::memory::exception::bad_cast, "Bad size : " << _size << " < 0");
    }

    this->set_size(_size, _unit);
}

//------------------------------------------------------------------------------

byte_size::byte_size(const std::string& _size)
{
    this->set_size(_size);
}

//------------------------------------------------------------------------------

byte_size& byte_size::operator=(size_t _size)
{
    this->set_size(_size);
    return *this;
}

//------------------------------------------------------------------------------

byte_size& byte_size::operator=(double _size)
{
    if(_size < 0)
    {
        SIGHT_THROW_EXCEPTION_MSG(core::memory::exception::bad_cast, "Bad size : " << _size << " < 0");
    }

    this->set_size(_size);
    return *this;
}

//------------------------------------------------------------------------------

byte_size& byte_size::operator=(const std::string& _size)
{
    this->set_size(_size);
    return *this;
}

//------------------------------------------------------------------------------

void byte_size::set_size(size_t _size, unit_t _unit)
{
    SIGHT_ASSERT(
        "Bad Unit",
        (_unit == BYTES) || (_unit == KB) || (_unit == MB) || (_unit == GB) || (_unit == TB) || (_unit == PB)
        || (_unit == KI_B) || (_unit == MI_B) || (_unit == GI_B) || (_unit == TI_B) || (_unit == PI_B)
    );
    m_size = _size * _unit;
}

//------------------------------------------------------------------------------

void byte_size::set_size(double _size, unit_t _unit)
{
    if(_size < 0)
    {
        SIGHT_THROW_EXCEPTION_MSG(core::memory::exception::bad_cast, "Bad size : " << _size << " < 0");
    }

    SIGHT_ASSERT(
        "Bad Unit",
        (_unit == BYTES) || (_unit == KB) || (_unit == MB) || (_unit == GB) || (_unit == TB) || (_unit == PB)
        || (_unit == KI_B) || (_unit == MI_B) || (_unit == GI_B) || (_unit == TI_B) || (_unit == PI_B)
    );
    m_size = static_cast<size_t>(_size * static_cast<double>(_unit));
}

//------------------------------------------------------------------------------

void byte_size::set_size(const std::string& _size)
{
    size_t new_size = 0;
    bool r          = parse_size(_size, new_size);
    if(r)
    {
        m_size = new_size;
    }
    else
    {
        SIGHT_THROW_EXCEPTION_MSG(core::memory::exception::bad_cast, "Bad size : " << _size);
    }
}

//------------------------------------------------------------------------------

std::string byte_size::unit_to_string(byte_size::unit_t _unit)
{
    switch(_unit)
    {
        case BYTES:
            return "Bytes";

        case KB:
            return "KB";

        case GB:
            return "GB";

        case MB:
            return "MB";

        case TB:
            return "TB";

        case PB:
            return "PB";

        case KI_B:
            return "KiB";

        case GI_B:
            return "GiB";

        case MI_B:
            return "MiB";

        case TI_B:
            return "TiB";

        case PI_B:
            return "PiB";

        default:
            SIGHT_ASSERT("Bad Unit", false);
            return "?";
    }
}

//------------------------------------------------------------------------------

bool byte_size::parse_size(const std::string& _s, size_t& _size)
{
    using boost::phoenix::ref;
    using boost::spirit::ascii::no_case;
    using boost::spirit::ascii::space;
    using boost::spirit::qi::as;
    using boost::spirit::qi::_1;
    using boost::spirit::qi::double_;
    using boost::spirit::qi::eoi;
    using boost::spirit::qi::phrase_parse;
    using boost::spirit::qi::symbols;
    using boost::spirit::qi::ulong_long;

    std::string::const_iterator first = _s.begin();
    std::string::const_iterator last  = _s.end();

    byte_size::size_t int_size   = 0;
    double float_size            = 0;
    byte_size::size_t multiplier = byte_size::BYTES;

    symbols<char, byte_size::size_t> unit;

    unit.add
        ("b", byte_size::BYTES)("byte", byte_size::BYTES)("bytes", byte_size::BYTES)
        ("kb", byte_size::KB)
        ("mb", byte_size::MB)
        ("gb", byte_size::GB)
        ("tb", byte_size::TB)
        ("pb", byte_size::PB)
        ("k", byte_size::KI_B)("kib", byte_size::KI_B)
        ("m", byte_size::MI_B)("mib", byte_size::MI_B)
        ("g", byte_size::GI_B)("gib", byte_size::GI_B)
        ("t", byte_size::TI_B)("tib", byte_size::TI_B)
        ("p", byte_size::PI_B)("pib", byte_size::PI_B)
    ;

    bool r = false;
    r = phrase_parse(
        first,
        last,
        //  Begin grammar
        (
            (*space
             >> (&((double_) >> no_case[-unit] >> *space >> eoi)
                 >> double_[ref(float_size) = _1] >> no_case[-unit[ref(multiplier) = _1]])
             | (&((ulong_long) >> no_case[-unit] >> *space >> eoi)
                >> ulong_long[ref(int_size) = _1] >> no_case[-unit[ref(multiplier) = _1]])
            )
            >> *space >> eoi
        ),
        //  End grammar

        space
    );

    if(!r || first != last) // fail if we did not get a full match
    {
        return false;
    }

    if(int_size != 0)
    {
        _size = int_size * multiplier;
    }
    else if(float_size != 0)
    {
        if(float_size < 0)
        {
            SIGHT_THROW_EXCEPTION_MSG(core::memory::exception::bad_cast, "Bad size : " << float_size << " < 0");
        }

        _size = static_cast<byte_size::size_t>(float_size * static_cast<double>(multiplier));
    }
    else
    {
        return false;
    }

    return r;
}

//------------------------------------------------------------------------------

std::string byte_size::get_size_as_string(unit_t _unit) const
{
    SIGHT_ASSERT(
        "Bad Unit",
        (_unit == BYTES) || (_unit == KB) || (_unit == MB) || (_unit == GB) || (_unit == TB) || (_unit == PB)
        || (_unit == KI_B) || (_unit == MI_B) || (_unit == GI_B) || (_unit == TI_B) || (_unit == PI_B)
    );
    std::stringstream sstr;
    sstr << std::noshowpoint;

    if(_unit == BYTES)
    {
        sstr << m_size;
    }
    else
    {
        sstr << (static_cast<double>(m_size) / static_cast<double>(_unit));
    }

    sstr << " " << unit_to_string(_unit);

    return sstr.str();
}

//------------------------------------------------------------------------------

std::string byte_size::get_human_readable_size(standard_type _standard) const
{
    static const std::array s_SI {BYTES, KB, MB, GB, TB, PB};
    static const std::array s_IEC {BYTES, KI_B, MI_B, GI_B, TI_B, PI_B};
    static_assert(s_SI.size() == s_IEC.size());
    const std::size_t size_of_standard_set = s_SI.size();

    const auto* unit_set = &s_IEC;
    if(_standard == si)
    {
        unit_set = &s_SI;
    }

    std::size_t i = 0;
    for(i = 1 ; i < size_of_standard_set ; ++i)
    {
        if(m_size < (*unit_set)[i])
        {
            break;
        }
    }

    return get_size_as_string((*unit_set)[i - 1]);
}

//------------------------------------------------------------------------------

} //namespace sight::core::memory
