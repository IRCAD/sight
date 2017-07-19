/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATA_GENERICFIELD_HPP__
#define __FWDATA_GENERICFIELD_HPP__

#include "fwData/factory/new.hpp"
#include "fwData/config.hpp"
#include "fwData/GenericFieldBase.hpp"

#include <fwCore/base.hpp>

#include <boost/lexical_cast.hpp>

#include <string>

namespace fwData
{

/**
 * @brief   Generic "field" object template.
 *
 * A generic object contains a value.
 *
 * @see     ::fwData::Float, ::fwData::Boolean, ::fwData::Integer
 */
template< typename T >
class FWDATA_CLASS_API GenericField : public GenericFieldBase
{

public:
    fwCoreNonInstanciableClassDefinitionsMacro( (GenericField<T>)(::fwData::Object) );

    typedef T ValueType;

    /// @brief Get the value (mutable version).
    T& value() noexcept
    {
        return m_value;
    }

    /// @brief Get the value (constant version).
    const T& value() const noexcept
    {
        return m_value;
    }

    /// @brief set the value
    void setValue(const T &newValue) noexcept
    {
        m_value = newValue;
    }

    /// @brief get the value
    T getValue() const noexcept
    {
        return m_value;
    }

    /// @brief Conversion to a scalar type.
    operator T() noexcept { return m_value; }

    bool operator== (const GenericFieldBase &lf)
    {
        const ::fwData::GenericField<T> &gField = dynamic_cast< const ::fwData::GenericField<T> & >(lf);
        SLM_ASSERT("GenericField must have same ValueType.", &gField );
        return ( this->m_value == gField.value() );
    }

    bool operator!= (const GenericFieldBase &lf )
    {
        const ::fwData::GenericField<T> &gField = dynamic_cast< const ::fwData::GenericField<T> & >(lf);
        SLM_ASSERT("GenericField must have same ValueType.", &gField);
        return ( this->m_value != gField.value() );
    }

    bool operator< (const GenericFieldBase &lf )
    {
        const ::fwData::GenericField<T> &gField = dynamic_cast< const ::fwData::GenericField<T> & >(lf);
        SLM_ASSERT("GenericField must have same ValueType.", &gField);
        return ( this->m_value < gField.value() );
    }

    bool operator> (const GenericFieldBase &lf)
    {
        const ::fwData::GenericField<T> &gField = dynamic_cast< const ::fwData::GenericField<T> & >(lf);
        SLM_ASSERT("GenericField must have same ValueType.", &gField);
        return ( this->m_value > gField.value() );
    }

    bool operator<= (const GenericFieldBase &lf)
    {
        const ::fwData::GenericField<T> &gField = dynamic_cast< const ::fwData::GenericField<T> & >(lf);
        SLM_ASSERT("GenericField must have same ValueType.", &gField);
        return ( this->m_value <= gField.value() );
    }

    bool operator>= (const GenericFieldBase &lf )
    {
        const ::fwData::GenericField<T> &gField = dynamic_cast< const ::fwData::GenericField<T> & >(lf);
        SLM_ASSERT("GenericField must have same ValueType.", &gField);
        return ( this->m_value >= gField.value() );
    }

    virtual ::std::string toString() const
    {
        return ::boost::lexical_cast< ::std::string >(this->m_value);
    }

    virtual void fromString(const ::std::string &_value)
    {
        this->m_value = ::boost::lexical_cast< T >(_value);
    }

protected:

    template< typename GT >
    static typename GT::sptr GenericFieldFactory(const typename GT::ValueType value);

    static sptr GenericFieldFactory(const T value);


    /**
     * @brief Constructor.
     * @param[in] value The initial value.
     */
    GenericField( const T value = T( ) ) noexcept :
        m_value( value )
    {
    }

    /**
     * @brief Destructor.
     */
    virtual ~GenericField() noexcept
    {
    }

    virtual std::ostream & toOStream( std::ostream &_os ) const
    {
        return _os << this->value();
    }

    /// @brief The stored value.
    T m_value;
};


template< typename T >
template< typename GT >
typename GT::sptr GenericField<T>::GenericFieldFactory(const typename GT::ValueType value)
{
    typename GT::sptr field;
    field          = ::fwData::factory::New< GT >();
    field->value() = value;
    return field;
}

template< typename T >
typename GenericField<T>::sptr GenericField<T>::GenericFieldFactory(const T value)
{
    typename GenericField<T>::sptr field;
    field          = GenericFieldFactory< GenericField<T> >(value);
    field->value() = value;
    return field;
}


} // namespace fwData

#endif // __FWDATA_GENERICFIELD_HPP__

