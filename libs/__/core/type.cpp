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
                 size_t(type::type_t::NONE) + 1> type::s_type_properties =
{{
    {sizeof(std::int8_t), true, "int8", type_t::INT8},
    {sizeof(std::int16_t), true, "int16", type_t::INT16},
    {sizeof(std::int32_t), true, "int32", type_t::INT32},
    {sizeof(std::int64_t), true, "int64", type_t::INT64},
    {sizeof(std::uint8_t), false, "uint8", type_t::UINT8},
    {sizeof(std::uint16_t), false, "uint16", type_t::UINT16},
    {sizeof(std::uint32_t), false, "uint32", type_t::UINT32},
    {sizeof(std::uint64_t), false, "uint64", type_t::UINT64},
    {sizeof(float), true, "float", type_t::FLOAT},
    {sizeof(double), true, "double", type_t::DOUBLE},
    {0, false, "unspecified", type_t::NONE}
}
};

//------------------------------------------------------------------------------

type::type(const std::string& str)
{
    for(const auto& [size, is_signed, name, type] : s_type_properties)
    {
        if(name == str)
        {
            m_type = type;
            break;
        }
    }
}

//------------------------------------------------------------------------------

size_t type::size() const
{
    return std::get<0>(s_type_properties[size_t(m_type)]);
}

//------------------------------------------------------------------------------

bool type::is_signed() const
{
    return std::get<1>(s_type_properties[size_t(m_type)]);
}

//------------------------------------------------------------------------------

const std::string& type::name() const
{
    return std::get<2>(s_type_properties[size_t(m_type)]);
}

//------------------------------------------------------------------------------

std::string type::to_string(const void* value) const
{
    switch(m_type)
    {
        case type_t::INT8:
        {
            const auto& v = *(static_cast<const std::int8_t*>(value));
            return std::to_string(v);
        }

        case type_t::INT16:
        {
            const auto& v = *(static_cast<const std::int16_t*>(value));
            return std::to_string(v);
        }

        case type_t::INT32:
        {
            const auto& v = *(static_cast<const std::int32_t*>(value));
            return std::to_string(v);
        }

        case type_t::INT64:
        {
            const auto& v = *(static_cast<const std::int64_t*>(value));
            return std::to_string(v);
        }

        case type_t::UINT8:
        {
            const auto& v = *(static_cast<const std::uint8_t*>(value));
            return std::to_string(v);
        }

        case type_t::UINT16:
        {
            const auto& v = *(static_cast<const std::uint16_t*>(value));
            return std::to_string(v);
        }

        case type_t::UINT32:
        {
            const auto& v = *(static_cast<const std::uint32_t*>(value));
            return std::to_string(v);
        }

        case type_t::UINT64:
        {
            const auto& v = *(static_cast<const std::uint64_t*>(value));
            return std::to_string(v);
        }

        case type_t::FLOAT:
        {
            const auto& v = *(static_cast<const float*>(value));
            return std::to_string(v);
        }

        case type_t::DOUBLE:
        {
            const auto& v = *(static_cast<const double*>(value));
            return std::to_string(v);
        }

        case type_t::NONE:
            return "";
    }

    return "";
}

//------------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& os, const sight::core::type& type)
{
    os << type.name();
    return os;
}

} // namespace sight::core
