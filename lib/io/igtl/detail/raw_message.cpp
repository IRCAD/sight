/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
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

#include "raw_message.hpp"

#include <boost/numeric/conversion/cast.hpp>

#include <cstdint>
#include <cstring>

namespace sight::io::igtl::detail
{

raw_message::raw_message(std::string const& _body_type)
{
#ifdef OpenIGTLink_HEADER_VERSION
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

#ifdef OpenIGTLink_HEADER_VERSION
int raw_message::CalculateContentBufferSize()
#else
//------------------------------------------------------------------------------

int raw_message::GetBodyPackSize()
#endif
{
    return boost::numeric_cast<int>(sizeof(std::uint32_t) + m_msg.size());
}

//-----------------------------------------------------------------------------

#ifdef OpenIGTLink_HEADER_VERSION
int raw_message::PackContent()
#else
//------------------------------------------------------------------------------

int raw_message::PackBody()
#endif
{
    this->AllocatePack();
#ifdef OpenIGTLink_HEADER_VERSION
    auto* content = m_Content;
#else
    auto* content = m_Body;
#endif

    // Keep the native byte order of the existing custom message format.
    const auto size = boost::numeric_cast<std::uint32_t>(m_msg.size());
    std::memcpy(content, &size, sizeof(size));
    if(size > 0)
    {
        std::memcpy(content + sizeof(size), m_msg.data(), m_msg.size());
    }

    return 1;
}

//-----------------------------------------------------------------------------

#ifdef OpenIGTLink_HEADER_VERSION
int raw_message::UnpackContent()
#else
//------------------------------------------------------------------------------

int raw_message::UnpackBody()
#endif
{
#ifdef OpenIGTLink_HEADER_VERSION
    const auto* content = m_Content;
#else
    const auto* content = m_Body;
#endif

    std::uint32_t size = 0;
    std::memcpy(&size, content, sizeof(size));
    m_msg.resize(size);
    if(size > 0)
    {
        std::memcpy(m_msg.data(), content + sizeof(size), size);
    }

    return 1;
}

} //namespace sight::io::igtl::detail
