/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWTOOLS_TYPE_HPP_
#define _FWTOOLS_TYPE_HPP_

#include <limits>
#include <map>
#include <string>

#include <boost/cstdint.hpp>

#include <boost/type_traits/is_signed.hpp>
#include <boost/any.hpp>

#include "fwTools/config.hpp"


namespace fwTools {

/**
 * @brief   Class defining an elementary C++ type aka unsigned char, signed char, .... signed long, float, double
 * @class   Type
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 *
 **/
class FWTOOLS_CLASS_API Type
{
public:
    typedef std::map<std::string, Type> TypeMapType;
    
    /// Default constructor
    FWTOOLS_API Type();

    /**
     * @brief comparison operator
     */
    FWTOOLS_API bool operator==(const Type &) const;

    /**
     * @brief comparison operator
     */
    FWTOOLS_API bool operator!=(const Type &) const;


    /**
     * @brief  define an order (lexicographic) for Type
     */
    FWTOOLS_API bool operator<( const Type& ) const;

    /**
     * @brief   Set Type value according given template
     * @note    A BOOST_ASSERTION can be raised if TYPE is not managed by isMapping
     **/
    template< class TYPE>
    void setType();

    /**
     * @brief   Return true iff the Type value represents the TYPE
     * @note    A BOOST_ASSERTION can be raised if TYPE is not managed by isMapping
     **/
    template< class TYPE>
    bool isOfType() const;

    /**
     * @brief   Return the sizeof of the type
     */
    FWTOOLS_API unsigned char sizeOf() const;


    /// Default destrucor : do nothing
    //FWTOOLS_API virtual ~Type();

    /// Return a human readable string
    FWTOOLS_API const std::string &string() const;

    /**
     * @brief return the min and max storable in the Type. take care that min/max value are casted into template T
     */
    template<class T>
    const std::pair<T,T> minMax() const;

    /// return true iff the type use a fixed precision
    FWTOOLS_API bool isFixedPrecision() const;

    /// return true iff the type is signed
    FWTOOLS_API bool isSigned() const;

    template <typename T>
    static const std::string typeToString();

    template <typename T>
    static Type create();

    FWTOOLS_API static Type create(std::string name);

protected :

    std::string m_name;
    unsigned char m_sizeof;
    bool m_isSigned;
    bool m_isFixedPrecision;
    ::boost::any m_min;
    ::boost::any m_max;


    /// Value for not specified type
    FWTOOLS_API static const std::string s_unspecifiedTypeName;
    FWTOOLS_API static const Type s_unspecifiedType;
    FWTOOLS_API static const TypeMapType s_typeMap;

};


//-----------------------------------------------------------------------------

template <typename T>
Type Type::create()
{
    Type t;
    t.setType<T>();
    return t;
}


//-----------------------------------------------------------------------------

template <typename T>
bool Type::isOfType() const
{
    return *this == create<T>();
}
//-----------------------------------------------------------------------------

template <typename T>
void Type::setType()
{
    m_name = Type::typeToString<T>();
    m_sizeof = sizeof(T);
    m_isSigned = ::boost::is_signed<T>::value;
    m_isFixedPrecision = ::boost::is_integral<T>::value;

    T min = static_cast< T >( std::numeric_limits< T >::min() );
    T max = static_cast< T >( std::numeric_limits< T >::max() );

    if(!m_isFixedPrecision)
    {
        // std::numeric_limits::min() returns the smallest positive value for floating types
        min = max * -1;
    }

    m_min = min;
    m_max = max;
}

//-----------------------------------------------------------------------------

template <typename T>
const std::string Type::typeToString()
{
    return Type::s_unspecifiedTypeName;
}

//-----------------------------------------------------------------------------

template <typename T>
const std::pair<T,T> Type::minMax() const
{
    return std::pair<T,T>(
            boost::any_cast< T >(m_min),
            boost::any_cast< T >(m_max)
            );
}


//-----------------------------------------------------------------------------

template<> FWTOOLS_API const std::string Type::typeToString< ::boost::uint8_t  >();
template<> FWTOOLS_API const std::string Type::typeToString< ::boost::uint16_t >();
template<> FWTOOLS_API const std::string Type::typeToString< ::boost::uint32_t >();
template<> FWTOOLS_API const std::string Type::typeToString< ::boost::uint64_t >();

template<> FWTOOLS_API const std::string Type::typeToString< ::boost::int8_t  > ();
template<> FWTOOLS_API const std::string Type::typeToString< ::boost::int16_t > ();
template<> FWTOOLS_API const std::string Type::typeToString< ::boost::int32_t > ();
template<> FWTOOLS_API const std::string Type::typeToString< ::boost::int64_t > ();

template<> FWTOOLS_API const std::string Type::typeToString< float  >  ();
template<> FWTOOLS_API const std::string Type::typeToString< double >  ();

} //end namespace fwTools


#endif /*_FWTOOLS_TYPE_H_*/

