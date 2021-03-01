/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#pragma once

#include "core/config.hpp"
#include "core/tools/Stringizer.hpp"

#include <core/base.hpp>

#include <any>
#include <limits>
#include <map>
#include <string>
#include <type_traits>
#include <typeinfo>

namespace sight::core::tools
{

/**
 * @brief   Class describing an elementary C++ type aka unsigned char, signed char, .... int, float, double
 **/
class CORE_CLASS_API Type
{

public:
    struct CORE_CLASS_API ToolBase
    {
        CORE_API ToolBase();
        CORE_API virtual ~ToolBase()
        {
        }
        CORE_API ToolBase(const std::type_info& typeinfo);
        CORE_API virtual std::string toString( std::any value ) const;
        CORE_API virtual std::string toString( const void* ) const;

        const std::type_info& m_typeinfo;
    };

    template<typename T>
    struct  Tool : public ToolBase
    {
        Tool();
        virtual ~Tool()
        {
        }
        virtual std::string toString( std::any value ) const;
        virtual std::string toString( const void* ) const;
    };

    typedef std::map<std::string, Type> TypeMapType;

    /// Default constructor
    CORE_API Type();
    CORE_API Type(const std::string& type);

    /**
     * @brief comparison operator
     */
    CORE_API bool operator==(const Type&) const;

    /**
     * @brief comparison operator
     */
    CORE_API bool operator!=(const Type&) const;

    /**
     * @brief  define an order (lexicographic) for Type
     */
    CORE_API bool operator<( const Type& ) const;

    /**
     * @brief   Set Type value according given template
     **/
    template< class TYPE>
    void setType();

    /**
     * @brief   Return true ff the Type value represents the TYPE
     **/
    template< class TYPE>
    bool isOfType() const;

    /**
     * @brief   Return the sizeof of the type
     */
    CORE_API unsigned char sizeOf() const;

    /// Return a human readable string
    CORE_API const std::string& string() const;

    /// return type_info of represented type. If type is unspecified, return typeid(void)
    CORE_API const std::type_info& typeId() const;

    /**
     * @brief return the min and max storable in the Type. take care that min/max value are casted into template T
     */
    template<class T>
    const std::pair<T, T> minMax() const;

    /// return true iff the type use a fixed precision
    CORE_API bool isFixedPrecision() const;

    /// return true iff the type is signed
    CORE_API bool isSigned() const;

    CORE_API std::string toString( const void* ) const;

    template <int SIZEOF, bool SIGNED, bool ISINTEGRAL>
    static const std::string& traitsToString();

    template <typename T>
    static Type create();

    CORE_API static Type create(std::string name);

protected:

    std::string m_name;
    unsigned char m_sizeof;
    bool m_isSigned;
    bool m_isFixedPrecision;
    std::any m_min;
    std::any m_max;

    SPTR(ToolBase) m_tool;

    /// Value for not specified type

    CORE_API static const TypeMapType s_TYPEMAP;

public:

    CORE_API static const Type s_UNSPECIFIED_TYPE;

    CORE_API static const Type s_INT8;
    CORE_API static const Type s_INT16;
    CORE_API static const Type s_INT32;
    CORE_API static const Type s_INT64;

    CORE_API static const Type s_UINT8;
    CORE_API static const Type s_UINT16;
    CORE_API static const Type s_UINT32;
    CORE_API static const Type s_UINT64;

    CORE_API static const Type s_FLOAT;
    CORE_API static const Type s_DOUBLE;

    CORE_API static const std::string s_UNSPECIFIED_TYPENAME;

    CORE_API static const std::string s_INT8_TYPENAME;
    CORE_API static const std::string s_INT16_TYPENAME;
    CORE_API static const std::string s_INT32_TYPENAME;
    CORE_API static const std::string s_INT64_TYPENAME;

    CORE_API static const std::string s_UINT8_TYPENAME;
    CORE_API static const std::string s_UINT16_TYPENAME;
    CORE_API static const std::string s_UINT32_TYPENAME;
    CORE_API static const std::string s_UINT64_TYPENAME;

    CORE_API static const std::string s_FLOAT_TYPENAME;
    CORE_API static const std::string s_DOUBLE_TYPENAME;

    typedef std::int8_t Int8Type;
    typedef std::int16_t Int16Type;
    typedef std::int32_t Int32Type;
    typedef std::int64_t Int64Type;

    typedef std::uint8_t UInt8Type;
    typedef std::uint16_t UInt16Type;
    typedef std::uint32_t UInt32Type;
    typedef std::uint64_t UInt64Type;

    typedef float FloatType;
    typedef double DoubleType;

};

//-----------------------------------------------------------------------------

template< typename T >
std::string Type::Tool<T>::toString(std::any value) const
{
    return core::tools::getString( std::any_cast<const T> (value));
}

//-----------------------------------------------------------------------------

template< typename T >
std::string Type::Tool<T>::toString(const void* value) const
{
    const T& v = *(static_cast< const T* > (value));
    return core::tools::getString( v );
}
//-----------------------------------------------------------------------------

template< typename T >
Type::Tool<T>::Tool() :
    Type::ToolBase(typeid(T))
{
}

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
    m_name = Type::traitsToString< sizeof(T), std::is_signed<T>::value, std::is_integral<T>::value >();

    m_sizeof           = sizeof(T);
    m_isSigned         = std::is_signed<T>::value;
    m_isFixedPrecision = std::is_integral<T>::value;

    m_tool = SPTR(ToolBase)(new Type::Tool<T>());

    T min = static_cast< T >( std::numeric_limits< T >::lowest() );
    T max = static_cast< T >( std::numeric_limits< T >::max() );

    m_min = min;
    m_max = max;
}

//-----------------------------------------------------------------------------

template <>
CORE_API void Type::setType< void >();

//-----------------------------------------------------------------------------

template <>
CORE_API void Type::setType< char >();

//-----------------------------------------------------------------------------

#ifdef linux

template <>
CORE_API void Type::setType< std::int64_t >();

//-----------------------------------------------------------------------------

template <>
CORE_API void Type::setType< std::uint64_t >();

#endif // linux

//-----------------------------------------------------------------------------

template <int SIZEOF, bool SIGNED, bool ISINTEGRAL>
const std::string& Type::traitsToString()
{
    SIGHT_ERROR("unknown " << (SIGNED ? "signed" : "unsigned")
                           << " " << (ISINTEGRAL ? "integral" : "floating")
                           << " type with size : " << SIZEOF);
    return Type::s_UNSPECIFIED_TYPENAME;
}

//-----------------------------------------------------------------------------

template <typename T>
const std::pair<T, T> Type::minMax() const
{
    return std::pair<T, T>( std::any_cast< T >(m_min), std::any_cast< T >(m_max) );
}

//-----------------------------------------------------------------------------

template<> CORE_API const std::string& Type::traitsToString< 1, true, true > ();
template<> CORE_API const std::string& Type::traitsToString< 2, true, true > ();
template<> CORE_API const std::string& Type::traitsToString< 4, true, true > ();
template<> CORE_API const std::string& Type::traitsToString< 8, true, true > ();

template<> CORE_API const std::string& Type::traitsToString< 1, false, true > ();
template<> CORE_API const std::string& Type::traitsToString< 2, false, true > ();
template<> CORE_API const std::string& Type::traitsToString< 4, false, true > ();
template<> CORE_API const std::string& Type::traitsToString< 8, false, true > ();

template<> CORE_API const std::string& Type::traitsToString< 4, true, false > ();
template<> CORE_API const std::string& Type::traitsToString< 8, true, false > ();

} //end namespace sight::core::tools

namespace std
{
CORE_API std::ostream& operator<< (std::ostream& os, const ::sight::core::tools::Type& type);
} // namespace std
