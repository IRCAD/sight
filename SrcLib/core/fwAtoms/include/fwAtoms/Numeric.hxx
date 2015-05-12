/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWATOMS_NUMERIC_HXX__
#define __FWATOMS_NUMERIC_HXX__


#include <boost/numeric/conversion/cast.hpp>
#include <boost/variant/get.hpp>
#include <boost/mpl/and.hpp>
#include <boost/type_traits/is_floating_point.hpp>
#include <boost/type_traits/is_integral.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/type_traits/is_signed.hpp>
#include <boost/type_traits/is_unsigned.hpp>
#include <boost/type_traits/remove_const.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include <boost/utility/enable_if.hpp>


#include "fwAtoms/Exception.hpp"

namespace fwAtoms
{

class NumericSetter
{
friend class Numeric;


template< typename T >
static void setValue( Numeric::ValueType &v,
                      typename ::boost::enable_if_c<
                          ::boost::is_same<
                              typename ::boost::remove_reference< typename ::boost::remove_const <T>::type >::type,
                              std::string
                              >::value

                          , T>::type value )
{
    v = Numeric::valueFromString(value);
}


template< typename T >
static void setValue( Numeric::ValueType &v,
                      typename ::boost::enable_if_c<
                          ::boost::mpl::and_< ::boost::is_signed<T>, ::boost::is_integral<T> >::value
                          , T>::type value )
{
    v = static_cast< ::boost::int64_t>(value);
}


template< typename T >
static void setValue( Numeric::ValueType &v,
                      typename ::boost::enable_if_c<
                          ::boost::mpl::and_< ::boost::is_unsigned<T>,::boost::is_integral<T> >::value
                          , T>::type value )
{
    v = static_cast< ::boost::uint64_t>(value);
}

template< typename T >
static void setValue( Numeric::ValueType &v,
                      typename ::boost::enable_if_c< ::boost::is_floating_point<T>::value, T>::type value )
{
    v = value;
}

};

//--------------------------------------------------------------------------

template<typename T>
Numeric::sptr Numeric::New( T value )
{
    Numeric::sptr valueSptr = Numeric::New();
    NumericSetter::setValue<T>(valueSptr->m_value, value);
    return valueSptr;
}

//--------------------------------------------------------------------------

template<typename T>
class get_casted_value : public boost::static_visitor<T>
{
public:

    T operator()( ::boost::blank & ) const
    {
        FW_RAISE_EXCEPTION(::fwAtoms::Exception("Unable to get value of an empty numeric"));
        return T();
    }
    T operator()( const ::boost::blank & ) const
    {
        FW_RAISE_EXCEPTION(::fwAtoms::Exception("Unable to get value of an empty numeric"));
        return T();
    }

    template <typename U>
    T operator()( U & value ) const
    {
        return ::boost::numeric_cast<T>(value);
    }

};

template<typename T>
T Numeric::getValue() const
{
    T val = boost::apply_visitor( get_casted_value<T>(), m_value );
    return val;
}

}

#endif //__FWATOMS_NUMERIC_HXX__

