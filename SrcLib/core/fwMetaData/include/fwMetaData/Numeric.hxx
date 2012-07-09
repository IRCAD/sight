#ifndef __FWMETADATA_BASE_NUMERIC_HXX__
#define __FWMETADATA_BASE_NUMERIC_HXX__


#include <string>
#include <iomanip>
#include <boost/lexical_cast.hpp>

namespace fwMetaData
{

template <typename T>
Numeric::Numeric(T value)
{

    m_value = boost::lexical_cast<std::string>(value);
}


template<typename T>
Numeric::sptr Numeric::New(T value)
{
    Numeric::sptr valueSptr(new Numeric(value));
    return valueSptr;

}

}

#endif
