/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "igtlProtocol/RawMessage.hpp"

#include <boost/numeric/conversion/cast.hpp>

#include <algorithm>
#include <cstdint>

namespace igtlProtocol
{

RawMessage::RawMessage(std::string const& bodyType)
{
    m_DefaultBodyType = bodyType;
}

//-----------------------------------------------------------------------------

RawMessage::~RawMessage()
{
}

//-----------------------------------------------------------------------------

void RawMessage::append(const RawDataType& msg)
{
    m_msg.insert(m_msg.end(), msg.begin(), msg.end());
}

//-----------------------------------------------------------------------------

void RawMessage::append(const char* data, std::size_t size)
{
    m_msg.insert(m_msg.end(), data, data + size);
}

//-----------------------------------------------------------------------------

RawMessage::Pointer RawMessage::New(std::string const& bodyType)
{
    return RawMessage::Pointer(new RawMessage(bodyType));
}

//-----------------------------------------------------------------------------

RawMessage::RawDataType const& RawMessage::getMessage() const
{
    return m_msg;
}

//-----------------------------------------------------------------------------

RawMessage::RawDataType& RawMessage::getMessage()
{
    return m_msg;
}

//-----------------------------------------------------------------------------

int RawMessage::GetBodyPackSize()
{
    return ::boost::numeric_cast<int>(sizeof(uint32_t) + m_msg.size());
}

//-----------------------------------------------------------------------------

int RawMessage::PackBody()
{
    uint32_t* size;
    char* str;

    this->AllocatePack();
    size  = (uint32_t*)(m_Body);
    *size = m_msg.size();
    str   = (char*)(m_Body + sizeof(uint32_t));
    if (*size > 0)
    {
        std::copy(m_msg.begin(), m_msg.end(), str);
    }
    return 1;
}

//-----------------------------------------------------------------------------

int RawMessage::UnpackBody()
{
    uint32_t* size;
    char* str;

    m_msg.clear();
    size = (uint32_t*)(m_Body);
    str  = (char*)(m_Body + sizeof(uint32_t));
    if (*size > 0)
    {
        m_msg.resize(*size);
        std::copy(str, str + *size, m_msg.begin());
    }
    return 1;
}

}//namespace igtlProtocol
