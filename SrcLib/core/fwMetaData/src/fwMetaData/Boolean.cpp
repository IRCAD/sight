/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwMetaData/Boolean.hpp"

fwCampImplementMacro((fwMetaData)(Boolean))
{
    builder
        .tag("object_version", "1")
        .tag("lib_name", "fwMetaData")
        .base< ::fwMetaData::Base >()
        .property("value", &::fwMetaData::Boolean::getString, &::fwMetaData::Boolean::setString);
}

namespace fwMetaData
{


Boolean::Boolean(bool value)
{
    m_value = value;
}

Boolean::Boolean(std::string& value)
{
    if(value.compare("true") == 0)
    {
        m_value = true;
    }
    else
    {
        m_value = false;
    }
}

Boolean::sptr Boolean::New(std::string value)
{
    Boolean::sptr valueSptr(new Boolean(value));
    return valueSptr;

}

Boolean::sptr Boolean::New(bool value)
{
    Boolean::sptr valueSptr(new Boolean(value));
    return valueSptr;

}

std::string Boolean::getString() const
{
    std::string result = "false";
    if(m_value)
    {
        result = "true";
    }

    return result;
}

void Boolean::setString(const std::string& value)
{
    if(!value.compare("true"))
    {
        m_value = true;
    }
    else
    {
        m_value = false;
    }
}
//------------------------------------------------------------------------------

Base::sptr Boolean::clone()
{

    return Boolean::New(m_value);
}
}

