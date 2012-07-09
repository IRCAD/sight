#include "fwMetaData/String.hpp"

#include <string>
#include <boost/any.hpp>

namespace fwMetaData
{

String::String(const std::string& value) : m_value(value)
{}

String::sptr String::New(std::string value)
{
    String::sptr valueSptr(new String(value));
    return valueSptr;

}
//------------------------------------------------------------------------------

Base::sptr String::clone()
{
    return String::New(m_value);
}

}
