/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <algorithm>
#include <limits>
#include <fwCore/base.hpp>

#include "fwTools/StringKeyTypeMapping.hpp"
#include "fwTools/Dispatcher.hpp"

namespace fwTools {


template<class TYPE>
DynamicType makeDynamicType()
{
    DynamicType d;
    d.template setType<TYPE>();
    return d;
}




struct TypeSetter
{
    template<class TYPE>
    void operator()( DynamicType & dynamicType )
    {
        dynamicType.setType<TYPE>();
    }
};



template<class KEYTYPE>
DynamicType makeDynamicType(const KEYTYPE &keyType)
{

    DynamicType d;

    Dispatcher<DynamicType::SupportedTypes,TypeSetter >::invoke(keyType,d);

    assert ( d != DynamicType() ); // a type must be found !!

    return d;
}



template<class T>
class MinMaxFunctor
{
public:

    template< typename PIXEL >
    void operator()( std::pair<T,T> &minMax )
    {
        minMax.first = static_cast< T >( std::numeric_limits< PIXEL >::min() );
        minMax.second = static_cast< T >( std::numeric_limits< PIXEL >::max() );

        ::fwTools::DynamicType type = ::fwTools::makeDynamicType< PIXEL >();
        if(!std::numeric_limits< PIXEL >::is_integer)
        {
            // std::numeric_limits::min() returns the smallest positive value for floating types
            minMax.first = minMax.second * -1;
        }
    }
};




template< class TYPE>
void DynamicType::setType() throw(std::invalid_argument)
{
     std::list< std::string>::const_iterator  supportedTypesIter;

     supportedTypesIter = m_managedTypes.begin();
     while ( supportedTypesIter !=  m_managedTypes.end() )
     {
         if ( isMapping<TYPE>( *supportedTypesIter) )
         {
             m_value  = *supportedTypesIter;
             m_sizeof = sizeof(TYPE);
             return;

         }
         ++supportedTypesIter;
     }

     throw std::invalid_argument("DynamicType::setType<TYPE> incorrect TYPE");
}




template< class TYPE>
bool DynamicType::isType() const
{
    return isMapping<TYPE>(m_value);
}




template<class TYPE>
const std::string DynamicType::string()
{
    DynamicType d;
    d.setType<TYPE>();
    return d.string();
}




template<class NEWTYPE>
void DynamicType::registerNewType(const std::string &newKey) throw(std::invalid_argument)
{
    // ensure isMapping present and well defined
    // if prog trap here it is because is Mapping is not well defined !!!
    if ( isMapping<NEWTYPE>(newKey) == false )
    {
        throw std::invalid_argument("Dynamic::registerNewType misconception with isMapping");
    }

    // ensure that newKey is not already used
    if ( std::find( m_managedTypes.begin(),m_managedTypes.end(), newKey ) != m_managedTypes.end() )
    {
        throw std::invalid_argument("Dynamic::registerNewType newKey already used");
    }

    // ensure that no other mapping respond to newkey
    try
    {
        DynamicType dummy;
        dummy.setType<NEWTYPE>();
    }
    catch ( std::exception )
    {
        // OK it is really a new type, insert it
        m_managedTypes.push_back(newKey);
        return;
    }

    throw std::invalid_argument("Dynamic::registerNewType another isMapping is responding");
}


template<class T>
std::pair<T,T> DynamicType::minMax()
{
    SLM_ASSERT("Unable to have minMax for UnSpecifiedType", this->string() != DynamicType::m_unSpecifiedType);
    std::pair<T,T> minMax;
    Dispatcher<DynamicType::SupportedTypes,MinMaxFunctor<T> >::invoke(*this,minMax);
    return minMax;
}



} //end namespace fwTools
