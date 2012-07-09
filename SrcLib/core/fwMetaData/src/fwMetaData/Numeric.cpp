#include "fwMetaData/Numeric.hpp"


namespace fwMetaData
{


Numeric::Numeric(std::string& value) : Base(), m_value(value)
{}

Numeric::sptr Numeric::New(std::string value)
{
    Numeric::sptr valueSptr(new Numeric(value));
    return valueSptr;

}


bool Numeric::isSigned()
{

    return m_value[0] == '-';

}
bool Numeric::isReal()
{
    return m_value.find('.') != std::string::npos;
}

//------------------------------------------------------------------------------

Base::sptr Numeric::clone()
{
    return Numeric::New(m_value);
}

bool Numeric::isEqual(Numeric::sptr obj)
{
    return obj->getString() == m_value;
}

bool Numeric::isHigher(Numeric::sptr obj)
{
    std::string value = obj->getString();

    bool result;
    double thisValue = boost::lexical_cast<double>(m_value);
    double compValue = boost::lexical_cast<double>(value);

    result = thisValue > compValue;
    return result;
}


bool Numeric::isLower(Numeric::sptr obj)
{
    return !this->isEqual(obj) && !this->isHigher(obj);
}

}
