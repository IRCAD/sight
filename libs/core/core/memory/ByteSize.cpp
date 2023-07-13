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

#include "core/memory/ByteSize.hpp"

#include "core/memory/exception/BadCast.hpp"

#include <boost/phoenix/operator.hpp>
#include <boost/spirit/include/qi.hpp>

#include <array>
#include <sstream>

namespace sight::core::memory
{

ByteSize::ByteSize()
= default;

//------------------------------------------------------------------------------

ByteSize::ByteSize(SizeType size, UnitType unit)
{
    SIGHT_ASSERT(
        "Bad Unit",
        (unit == Bytes) || (unit == KB) || (unit == MB) || (unit == GB) || (unit == TB) || (unit == PB)
        || (unit == KiB) || (unit == MiB) || (unit == GiB) || (unit == TiB) || (unit == PiB)
    );
    this->setSize(size, unit);
}

//------------------------------------------------------------------------------

ByteSize::ByteSize(double size, UnitType unit)
{
    SIGHT_ASSERT(
        "Bad Unit",
        (unit == Bytes) || (unit == KB) || (unit == MB) || (unit == GB) || (unit == TB) || (unit == PB)
        || (unit == KiB) || (unit == MiB) || (unit == GiB) || (unit == TiB) || (unit == PiB)
    );
    if(size < 0)
    {
        SIGHT_THROW_EXCEPTION_MSG(core::memory::exception::BadCast, "Bad size : " << size << " < 0");
    }

    this->setSize(size, unit);
}

//------------------------------------------------------------------------------

ByteSize::ByteSize(const std::string& size)
{
    this->setSize(size);
}

//------------------------------------------------------------------------------

ByteSize& ByteSize::operator=(SizeType size)
{
    this->setSize(size);
    return *this;
}

//------------------------------------------------------------------------------

ByteSize& ByteSize::operator=(double size)
{
    if(size < 0)
    {
        SIGHT_THROW_EXCEPTION_MSG(core::memory::exception::BadCast, "Bad size : " << size << " < 0");
    }

    this->setSize(size);
    return *this;
}

//------------------------------------------------------------------------------

ByteSize& ByteSize::operator=(const std::string& size)
{
    this->setSize(size);
    return *this;
}

//------------------------------------------------------------------------------

void ByteSize::setSize(SizeType size, UnitType unit)
{
    SIGHT_ASSERT(
        "Bad Unit",
        (unit == Bytes) || (unit == KB) || (unit == MB) || (unit == GB) || (unit == TB) || (unit == PB)
        || (unit == KiB) || (unit == MiB) || (unit == GiB) || (unit == TiB) || (unit == PiB)
    );
    m_size = size * unit;
}

//------------------------------------------------------------------------------

void ByteSize::setSize(double size, UnitType unit)
{
    if(size < 0)
    {
        SIGHT_THROW_EXCEPTION_MSG(core::memory::exception::BadCast, "Bad size : " << size << " < 0");
    }

    SIGHT_ASSERT(
        "Bad Unit",
        (unit == Bytes) || (unit == KB) || (unit == MB) || (unit == GB) || (unit == TB) || (unit == PB)
        || (unit == KiB) || (unit == MiB) || (unit == GiB) || (unit == TiB) || (unit == PiB)
    );
    m_size = static_cast<SizeType>(size * static_cast<double>(unit));
}

//------------------------------------------------------------------------------

void ByteSize::setSize(const std::string& size)
{
    SizeType newSize = 0;
    bool r           = parseSize(size, newSize);
    if(r)
    {
        m_size = newSize;
    }
    else
    {
        SIGHT_THROW_EXCEPTION_MSG(core::memory::exception::BadCast, "Bad size : " << size);
    }
}

//------------------------------------------------------------------------------

std::string ByteSize::unitToString(ByteSize::UnitType unit)
{
    switch(unit)
    {
        case Bytes:
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

        case KiB:
            return "KiB";

        case GiB:
            return "GiB";

        case MiB:
            return "MiB";

        case TiB:
            return "TiB";

        case PiB:
            return "PiB";

        default:
            SIGHT_ASSERT("Bad Unit", false);
            return "?";
    }
}

//------------------------------------------------------------------------------

bool ByteSize::parseSize(const std::string& s, SizeType& size)
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

    std::string::const_iterator first = s.begin();
    std::string::const_iterator last  = s.end();

    ByteSize::SizeType intSize    = 0;
    double floatSize              = 0;
    ByteSize::SizeType multiplier = ByteSize::Bytes;

    symbols<char, ByteSize::SizeType> unit;

    unit.add
        ("b", ByteSize::Bytes)("byte", ByteSize::Bytes)("bytes", ByteSize::Bytes)
        ("kb", ByteSize::KB)
        ("mb", ByteSize::MB)
        ("gb", ByteSize::GB)
        ("tb", ByteSize::TB)
        ("pb", ByteSize::PB)
        ("k", ByteSize::KiB)("kib", ByteSize::KiB)
        ("m", ByteSize::MiB)("mib", ByteSize::MiB)
        ("g", ByteSize::GiB)("gib", ByteSize::GiB)
        ("t", ByteSize::TiB)("tib", ByteSize::TiB)
        ("p", ByteSize::PiB)("pib", ByteSize::PiB)
    ;

    bool r = false;
    r = phrase_parse(
        first,
        last,
        //  Begin grammar
        (
            (*space
             >> (&((double_) >> no_case[-unit] >> *space >> eoi)
                 >> double_[ref(floatSize) = _1] >> no_case[-unit[ref(multiplier) = _1]])
             | (&((ulong_long) >> no_case[-unit] >> *space >> eoi)
                >> ulong_long[ref(intSize) = _1] >> no_case[-unit[ref(multiplier) = _1]])
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

    if(intSize != 0)
    {
        size = intSize * multiplier;
    }
    else if(floatSize != 0)
    {
        if(floatSize < 0)
        {
            SIGHT_THROW_EXCEPTION_MSG(core::memory::exception::BadCast, "Bad size : " << floatSize << " < 0");
        }

        size = static_cast<ByteSize::SizeType>(floatSize * static_cast<double>(multiplier));
    }
    else
    {
        return false;
    }

    return r;
}

//------------------------------------------------------------------------------

std::string ByteSize::getSizeAsString(UnitType unit) const
{
    SIGHT_ASSERT(
        "Bad Unit",
        (unit == Bytes) || (unit == KB) || (unit == MB) || (unit == GB) || (unit == TB) || (unit == PB)
        || (unit == KiB) || (unit == MiB) || (unit == GiB) || (unit == TiB) || (unit == PiB)
    );
    std::stringstream sstr;
    sstr << std::noshowpoint;

    if(unit == Bytes)
    {
        sstr << m_size;
    }
    else
    {
        sstr << (static_cast<double>(m_size) / static_cast<double>(unit));
    }

    sstr << " " << unitToString(unit);

    return sstr.str();
}

//------------------------------------------------------------------------------

std::string ByteSize::getHumanReadableSize(StandardType standard) const
{
    static const std::array si {Bytes, KB, MB, GB, TB, PB};
    static const std::array iec {Bytes, KiB, MiB, GiB, TiB, PiB};
    static_assert(si.size() == iec.size());
    const std::size_t sizeOfStandardSet = si.size();

    const auto* unitSet = &iec;
    if(standard == SI)
    {
        unitSet = &si;
    }

    std::size_t i = 0;
    for(i = 1 ; i < sizeOfStandardSet ; ++i)
    {
        if(m_size < (*unitSet)[i])
        {
            break;
        }
    }

    return getSizeAsString((*unitSet)[i - 1]);
}

//------------------------------------------------------------------------------

} //namespace sight::core::memory
