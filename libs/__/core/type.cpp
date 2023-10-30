/************************************************************************
 *
 * Copyright (C) 2022-2023 IRCAD France
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

#include "core/type.hpp"

#include <array>
#include <sstream>

namespace sight::core
{

//------------------------------------------------------------------------------

const std::array<std::tuple<size_t, bool, std::string, type::type_t>,
                 size_t(type::type_t::none) + 1> type::TYPE_PROPERTIES =
{{
    {sizeof(std::int8_t), true, "int8", type_t::in_t8},
    {sizeof(std::int16_t), true, "int16", type_t::in_t16},
    {sizeof(std::int32_t), true, "int32", type_t::in_t32},
    {sizeof(std::int64_t), true, "int64", type_t::in_t64},
    {sizeof(std::uint8_t), false, "uint8", type_t::uin_t8},
    {sizeof(std::uint16_t), false, "uint16", type_t::uin_t16},
    {sizeof(std::uint32_t), false, "uint32", type_t::uin_t32},
    {sizeof(std::uint64_t), false, "uint64", type_t::uin_t64},
    {sizeof(float), true, "float", type_t::FLOAT},
    {sizeof(double), true, "double", type_t::DOUBLE},
    {0, false, "unspecified", type_t::none}
}
};

//------------------------------------------------------------------------------

type::type(const std::string& _str)
{
    for(const auto& [size, is_signed, name, type] : TYPE_PROPERTIES)
    {
        if(name == _str)
        {
            m_type = type;
            break;
        }
    }
}

//------------------------------------------------------------------------------

size_t type::size() const
{
    return std::get<0>(TYPE_PROPERTIES[size_t(m_type)]);
}

//------------------------------------------------------------------------------

bool type::is_signed() const
{
    return std::get<1>(TYPE_PROPERTIES[size_t(m_type)]);
}

//------------------------------------------------------------------------------

const std::string& type::name() const
{
    return std::get<2>(TYPE_PROPERTIES[size_t(m_type)]);
}

//------------------------------------------------------------------------------

std::string type::to_string(const void* _value) const
{
    switch(m_type)
    {
        case type_t::in_t8:
        {
            const auto& v = *(static_cast<const std::int8_t*>(_value));
            return std::to_string(v);
        }

        case type_t::in_t16:
        {
            const auto& v = *(static_cast<const std::int16_t*>(_value));
            return std::to_string(v);
        }

        case type_t::in_t32:
        {
            const auto& v = *(static_cast<const std::int32_t*>(_value));
            return std::to_string(v);
        }

        case type_t::in_t64:
        {
            const auto& v = *(static_cast<const std::int64_t*>(_value));
            return std::to_string(v);
        }

        case type_t::uin_t8:
        {
            const auto& v = *(static_cast<const std::uint8_t*>(_value));
            return std::to_string(v);
        }

        case type_t::uin_t16:
        {
            const auto& v = *(static_cast<const std::uint16_t*>(_value));
            return std::to_string(v);
        }

        case type_t::uin_t32:
        {
            const auto& v = *(static_cast<const std::uint32_t*>(_value));
            return std::to_string(v);
        }

        case type_t::uin_t64:
        {
            const auto& v = *(static_cast<const std::uint64_t*>(_value));
            return std::to_string(v);
        }

        case type_t::FLOAT:
        {
            const auto& v = *(static_cast<const float*>(_value));
            return std::to_string(v);
        }

        case type_t::DOUBLE:
        {
            const auto& v = *(static_cast<const double*>(_value));
            return std::to_string(v);
        }

        case type_t::none:
            return "";
    }

    return "";
}

//------------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& _os, const sight::core::type& _type)
{
    _os << _type.name();
    return _os;
}

} // namespace sight::core
