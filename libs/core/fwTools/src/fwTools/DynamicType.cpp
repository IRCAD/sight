/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "fwTools/DynamicType.hpp"

#include "fwCore/spyLog.hpp"

#include "fwTools/Dispatcher.hpp"
#include "fwTools/DynamicTypeKeyTypeMapping.hpp"

#include <type_traits>

namespace fwTools
{

std::list< std::string > DynamicType::m_managedTypes = {{ ("unsigned char"),
                                                          ("signed char"),
                                                          ("unsigned short"),
                                                          ("signed short"),
                                                          ("unsigned int"),
                                                          ("signed int"),
                                                          ("unsigned long"),
                                                          ("signed long"),
                                                          ("float"),
                                                          ("double")}};

const std::string DynamicType::m_unSpecifiedType = std::string("UNSPECIFIED TYPE");

typedef boost::mpl::vector< signed char, unsigned char, signed short, unsigned short,  signed int, unsigned int, float,
                            double >::type SupportedTypes;

//------------------------------------------------------------------------------

bool DynamicType::operator==(const DynamicType& d2) const
{
    return m_value == d2.m_value;
}

//------------------------------------------------------------------------------

bool DynamicType::operator!=(const DynamicType& d2) const
{
    return m_value != d2.m_value;
}

//------------------------------------------------------------------------------

bool DynamicType::operator<(const DynamicType& d2) const
{
    return m_value < d2.m_value;
}

DynamicType::DynamicType() :
    m_value( m_unSpecifiedType),
    m_sizeof(0)
{
    FW_DEPRECATED_MSG("'::fwTools::DynamicType' is no longer supported and will be removed in sight 22.0,"
                      " use '::fwTools::Type' instead",  "22.0")
}

DynamicType::DynamicType(const DynamicType& d2)
{
    FW_DEPRECATED_MSG("'::fwTools::DynamicType' is no longer supported and will be removed in sight 22.0,"
                      " use '::fwTools::Type' instead",  "22.0")
    this->m_value  = d2.m_value;
    this->m_sizeof = d2.m_sizeof;
}

DynamicType::~DynamicType()
{
}

//------------------------------------------------------------------------------

const std::string& DynamicType::string() const
{
    return m_value;
}

//------------------------------------------------------------------------------

unsigned char DynamicType::sizeOf() const
{
    SLM_ASSERT("Sizeof not specified.", m_sizeof != 0);
    return m_sizeof;
}

//------------------------------------------------------------------------------

bool DynamicType::isFixedPrecision()
{
    return !isType<float>() && !isType<double>();
}

class isSignedFunctor
{
public:

    //------------------------------------------------------------------------------

    template< typename PIXEL >
    void operator()( bool& isSigned )
    {
        isSigned = !std::is_unsigned<PIXEL>::value;
    }
};

//------------------------------------------------------------------------------

bool DynamicType::isSigned()
{
    bool isSigned;
    Dispatcher<SupportedTypes, isSignedFunctor >::invoke(*this, isSigned);
    return isSigned;
}

//------------------------------------------------------------------------------

template<>
std::string getString(const DynamicType& dt)
{
    return dt.string();
}

//------------------------------------------------------------------------------

template<>
std::string getString(const std::vector<DynamicType>& dtv)
{
    return getString(dtv.begin(), dtv.end());
}

} // end namespace fwTools {
