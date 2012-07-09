#include "fwMetaData/Boolean.hpp"

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

