/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "core/tools/Type.hpp"

namespace std
{

//------------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& os, const ::sight::core::tools::Type& type)
{
    os << type.string();
    return os;
}

}

namespace sight::core::tools
{

const std::string Type::s_UNSPECIFIED_TYPENAME("UNSPECIFIED TYPE");

const std::string Type::s_INT8_TYPENAME("int8");
const std::string Type::s_INT16_TYPENAME("int16");
const std::string Type::s_INT32_TYPENAME("int32");
const std::string Type::s_INT64_TYPENAME("int64");

const std::string Type::s_UINT8_TYPENAME("uint8");
const std::string Type::s_UINT16_TYPENAME("uint16");
const std::string Type::s_UINT32_TYPENAME("uint32");
const std::string Type::s_UINT64_TYPENAME("uint64");

const std::string Type::s_FLOAT_TYPENAME("float");
const std::string Type::s_DOUBLE_TYPENAME("double");

const Type::TypeMapType Type::s_TYPEMAP = {{
    {Type::s_INT8_TYPENAME, Type::create<core::tools::Type::Int8Type>()},
    {Type::s_INT16_TYPENAME, Type::create<core::tools::Type::Int16Type>()},
    {Type::s_INT32_TYPENAME, Type::create<core::tools::Type::Int32Type>()},
    {Type::s_INT64_TYPENAME, Type::create<core::tools::Type::Int64Type>()},

    {Type::s_UINT8_TYPENAME, Type::create<core::tools::Type::UInt8Type>()},
    {Type::s_UINT16_TYPENAME,
     Type::create<core::tools::Type::UInt16Type>()
    },
    {Type::s_UINT32_TYPENAME,
     Type::create<core::tools::Type::UInt32Type>()
    },
    {Type::s_UINT64_TYPENAME,
     Type::create<core::tools::Type::UInt64Type>()
    },

    {Type::s_FLOAT_TYPENAME, Type::create<core::tools::Type::FloatType>()},
    {Type::s_DOUBLE_TYPENAME,
     Type::create<core::tools::Type::DoubleType>()
    }
}
};

const Type Type::s_UNSPECIFIED_TYPE;

const Type Type::s_INT8  = core::tools::Type::create<core::tools::Type::Int8Type>();
const Type Type::s_INT16 = core::tools::Type::create<core::tools::Type::Int16Type>();
const Type Type::s_INT32 = core::tools::Type::create<core::tools::Type::Int32Type>();
const Type Type::s_INT64 = core::tools::Type::create<core::tools::Type::Int64Type>();

const Type Type::s_UINT8  = core::tools::Type::create<core::tools::Type::UInt8Type>();
const Type Type::s_UINT16 = core::tools::Type::create<core::tools::Type::UInt16Type>();
const Type Type::s_UINT32 = core::tools::Type::create<core::tools::Type::UInt32Type>();
const Type Type::s_UINT64 = core::tools::Type::create<core::tools::Type::UInt64Type>();

const Type Type::s_FLOAT  = core::tools::Type::create<core::tools::Type::FloatType>();
const Type Type::s_DOUBLE = core::tools::Type::create<core::tools::Type::DoubleType>();

//------------------------------------------------------------------------------

template<>
const std::string& Type::traitsToString<1, true, true>()
{
    return Type::s_INT8_TYPENAME;
}

//------------------------------------------------------------------------------

template<>
const std::string& Type::traitsToString<2, true, true>()
{
    return Type::s_INT16_TYPENAME;
}

//------------------------------------------------------------------------------

template<>
const std::string& Type::traitsToString<4, true, true>()
{
    return Type::s_INT32_TYPENAME;
}

//------------------------------------------------------------------------------

template<>
const std::string& Type::traitsToString<8, true, true>()
{
    return Type::s_INT64_TYPENAME;
}

//------------------------------------------------------------------------------

template<>
const std::string& Type::traitsToString<1, false, true>()
{
    return Type::s_UINT8_TYPENAME;
}

//------------------------------------------------------------------------------

template<>
const std::string& Type::traitsToString<2, false, true>()
{
    return Type::s_UINT16_TYPENAME;
}

//------------------------------------------------------------------------------

template<>
const std::string& Type::traitsToString<4, false, true>()
{
    return Type::s_UINT32_TYPENAME;
}

//------------------------------------------------------------------------------

template<>
const std::string& Type::traitsToString<8, false, true>()
{
    return Type::s_UINT64_TYPENAME;
}

//------------------------------------------------------------------------------

template<>
const std::string& Type::traitsToString<4, true, false>()
{
    return Type::s_FLOAT_TYPENAME;
}

//------------------------------------------------------------------------------

template<>
const std::string& Type::traitsToString<8, true, false>()
{
    return Type::s_DOUBLE_TYPENAME;
}

//------------------------------------------------------------------------------

Type::Type()
{
    Type::setType<void>();
}

//------------------------------------------------------------------------------

Type::Type(const std::string& type)
{
    *this = Type::create(type);
}

//------------------------------------------------------------------------------

bool Type::operator==(const Type& _other) const
{
    return m_name == _other.m_name;
}

//------------------------------------------------------------------------------

bool Type::operator!=(const Type& _other) const
{
    return m_name != _other.m_name;
}

//------------------------------------------------------------------------------

bool Type::operator<(const Type& _other) const
{
    return m_name < _other.m_name;
}

//------------------------------------------------------------------------------

unsigned char Type::sizeOf() const
{
    return m_sizeof;
}

//------------------------------------------------------------------------------

const std::string& Type::string() const
{
    return m_name;
}

//------------------------------------------------------------------------------

const std::type_info& Type::typeId() const
{
    return m_tool->m_typeinfo;
}

//------------------------------------------------------------------------------

bool Type::isFixedPrecision() const
{
    return m_isFixedPrecision;
}

//------------------------------------------------------------------------------

bool Type::isSigned() const
{
    return m_isSigned;
}

//-----------------------------------------------------------------------------

Type Type::create(std::string name)
{
    TypeMapType::const_iterator iter = s_TYPEMAP.find(name);
    if(iter != s_TYPEMAP.end())
    {
        return iter->second;
    }

    return s_UNSPECIFIED_TYPE;
}

//-----------------------------------------------------------------------------

std::string Type::toString(const void* value) const
{
    return m_tool->toString(value);
}

//-----------------------------------------------------------------------------

Type::ToolBase::ToolBase() :
    m_typeinfo(typeid(void))
{
}

//-----------------------------------------------------------------------------

Type::ToolBase::ToolBase(const std::type_info& typeinfo) :
    m_typeinfo(typeinfo)
{
}

//-----------------------------------------------------------------------------

std::string Type::ToolBase::toString(std::any value) const
{
    SIGHT_NOT_USED(value);
    SIGHT_ASSERT("unable to convert an unspecified type value", 0);
    return "";
}

//-----------------------------------------------------------------------------

std::string Type::ToolBase::toString(const void* value) const
{
    SIGHT_NOT_USED(value);
    SIGHT_ASSERT("unable to convert an unspecified type value", 0);
    return "";
}

//-----------------------------------------------------------------------------

template<>
void Type::setType<char>()
{
    this->setType<signed char>();
}

//-----------------------------------------------------------------------------

#if (defined(linux) || defined(__linux))
template<>
void Type::setType<std::int64_t>()
{
    this->setType<long long>();
}

//-----------------------------------------------------------------------------

template<>
void Type::setType<std::uint64_t>()
{
    this->setType<unsigned long long>();
}
#endif

//-----------------------------------------------------------------------------

template<>
void Type::setType<void>()
{
    m_name             = s_UNSPECIFIED_TYPENAME;
    m_sizeof           = 0;
    m_isSigned         = false;
    m_isFixedPrecision = false;

    m_tool = SPTR(ToolBase)(new Type::ToolBase());

    m_min = 0;
    m_max = 0;
}

} // namespace sight::core::tools
