/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWATOMS_NUMERIC_HXX__
#define __FWATOMS_NUMERIC_HXX__

#include "fwAtoms/Exception.hpp"

#include <boost/mpl/and.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/variant/get.hpp>

#include <type_traits>

namespace fwAtoms
{

class NumericSetter
{
friend class Numeric;

//------------------------------------------------------------------------------

template< typename T >
static void setValue( Numeric::ValueType& v,
                      typename ::boost::enable_if_c<
                          std::is_same<
                              typename ::boost::remove_reference< typename std::remove_const <T>::type >::type,
                              std::string
                              >::value

                          , T>::type value )
{
    v = Numeric::valueFromString(value);
}

//------------------------------------------------------------------------------

template< typename T >
static void setValue( Numeric::ValueType& v,
                      typename ::boost::enable_if_c<
                          ::boost::mpl::and_< std::is_signed<T>, std::is_integral<T> >::value
                          , T>::type value )
{
    v = static_cast< std::int64_t>(value);
}

//------------------------------------------------------------------------------

template< typename T >
static void setValue( Numeric::ValueType& v,
                      typename ::boost::enable_if_c<
                          ::boost::mpl::and_< std::is_unsigned<T>, std::is_integral<T> >::value
                          , T>::type value )
{
    v = static_cast< std::uint64_t>(value);
}

//------------------------------------------------------------------------------

template< typename T >
static void setValue( Numeric::ValueType& v,
                      typename ::boost::enable_if_c< std::is_floating_point<T>::value, T>::type value )
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

    //------------------------------------------------------------------------------

    T operator()( ::boost::blank& ) const
    {
        FW_RAISE_EXCEPTION(::fwAtoms::Exception("Unable to get value of an empty numeric"));
        return T();
    }
    //------------------------------------------------------------------------------

    T operator()( const ::boost::blank& ) const
    {
        FW_RAISE_EXCEPTION(::fwAtoms::Exception("Unable to get value of an empty numeric"));
        return T();
    }

    //------------------------------------------------------------------------------

    template <typename U>
    T operator()( U& value ) const
    {
        return ::boost::numeric_cast<T>(value);
    }

};

//------------------------------------------------------------------------------

template<typename T>
T Numeric::getValue() const
{
    T val = boost::apply_visitor( get_casted_value<T>(), m_value );
    return val;
}

}

#endif //__FWATOMS_NUMERIC_HXX__

