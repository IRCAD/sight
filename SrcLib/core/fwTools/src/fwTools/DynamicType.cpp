/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/assign/list_of.hpp>
#include <boost/type_traits/is_unsigned.hpp>

#include "fwCore/spyLog.hpp"
#include "fwTools/DynamicType.hpp"
#include "fwTools/DynamicTypeKeyTypeMapping.hpp"
#include "fwTools/Dispatcher.hpp"

namespace fwTools {

std::list< std::string > DynamicType::m_managedTypes = boost::assign::list_of
    ("unsigned char")
    ("signed char")
    ("unsigned short")
    ("signed short")
    ("unsigned int")
    ("signed int")
    ("unsigned long")
    ("signed long")
    ("float")
    ("double");

const std::string  DynamicType::m_unSpecifiedType = std::string("UNSPECIFIED TYPE");

typedef boost::mpl::vector< signed char, unsigned char, signed short, unsigned short,  signed int, unsigned int, float, double >::type SupportedTypes;



bool DynamicType::operator==(const DynamicType &d2) const
{
    return m_value == d2.m_value;
}



bool DynamicType::operator!=(const DynamicType &d2) const
{
    return m_value != d2.m_value;
}


bool DynamicType::operator<(const DynamicType &d2) const
{
    return m_value <  d2.m_value;
}




DynamicType::DynamicType() : m_value( m_unSpecifiedType), m_sizeof(0)
{
}


DynamicType::DynamicType(const DynamicType &d2)
{
    this->m_value = d2.m_value;
    this->m_sizeof = d2.m_sizeof;
}


DynamicType::~DynamicType()
{
}



const std::string &DynamicType::string() const
{
    return m_value;
}



unsigned char  DynamicType::sizeOf() const
{
    SLM_ASSERT("Sizeof not specified.", m_sizeof!=0);
    return m_sizeof;
}


bool DynamicType::isFixedPrecision()
{
    return !isType<float>() && !isType<double>();
}



class isSignedFunctor
{
public:

    template< typename PIXEL >
    void operator()( bool &isSigned )
    {
        isSigned =  !::boost::is_unsigned<PIXEL>::value;
    }
};


bool DynamicType::isSigned()
{
    bool isSigned;
    Dispatcher<SupportedTypes,isSignedFunctor >::invoke(*this,isSigned);
    return isSigned;
}

template<>
std::string getString(const DynamicType &dt)
{
    return dt.string();
}


template<>
std::string getString(const std::vector<DynamicType> &dtv)
{
    return getString(dtv.begin(), dtv.end());
}





} // end namespace fwTools {



