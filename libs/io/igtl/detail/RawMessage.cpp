/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#include "RawMessage.hpp"

#include <boost/numeric/conversion/cast.hpp>

#include <algorithm>
#include <cstdint>

namespace sight::io::igtl::detail
{

raw_message::raw_message(std::string const& _body_type)
{
#if defined(OpenIGTLink_HEADER_VERSION)
    m_SendMessageType    = _body_type;
    m_ReceiveMessageType = _body_type;
#else
    m_DefaultBodyType = _body_type;
#endif
}

//-----------------------------------------------------------------------------

raw_message::~raw_message()
= default;

//-----------------------------------------------------------------------------

void raw_message::append(const raw_data_t& _msg)
{
    m_msg.insert(m_msg.end(), _msg.begin(), _msg.end());
}

//-----------------------------------------------------------------------------

void raw_message::append(const char* _data, std::size_t _size)
{
    m_msg.insert(m_msg.end(), _data, _data + _size);
}

//-----------------------------------------------------------------------------

raw_message::Pointer raw_message::New(std::string const& _body_type)
{
    return {new raw_message(_body_type)};
}

//-----------------------------------------------------------------------------

raw_message::raw_data_t const& raw_message::get_message() const
{
    return m_msg;
}

//-----------------------------------------------------------------------------

raw_message::raw_data_t& raw_message::get_message()
{
    return m_msg;
}

//-----------------------------------------------------------------------------

int raw_message::GetBodyPackSize()
{
    return boost::numeric_cast<int>(sizeof(uint32_t) + m_msg.size());
}

//-----------------------------------------------------------------------------

int raw_message::PackBody()
{
    uint32_t* size = nullptr;
    char* str      = nullptr;

    this->AllocatePack();
    size  = reinterpret_cast<uint32_t*>(m_Body);
    *size = std::uint32_t(m_msg.size());
    str   = reinterpret_cast<char*>(m_Body + sizeof(uint32_t));
    if(*size > 0)
    {
        std::copy(m_msg.begin(), m_msg.end(), str);
    }

    return 1;
}

//-----------------------------------------------------------------------------

int raw_message::UnpackBody()
{
    uint32_t* size = nullptr;
    char* str      = nullptr;

    m_msg.clear();
    size = reinterpret_cast<uint32_t*>(m_Body);
    str  = reinterpret_cast<char*>(m_Body + sizeof(uint32_t));
    if(*size > 0)
    {
        m_msg.resize(*size);
        std::copy(str, str + *size, m_msg.begin());
    }

    return 1;
}

} //namespace sight::io::igtl::detail
