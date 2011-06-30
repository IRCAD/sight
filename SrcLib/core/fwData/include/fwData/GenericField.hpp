/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_GENERICFIELD_HPP_
#define _FWDATA_GENERICFIELD_HPP_

#include <string>

#include <boost/lexical_cast.hpp>

#include <fwCore/base.hpp>

#include <fwTools/Factory.hpp>
#include "fwData/config.hpp"
#include "fwData/GenericFieldBase.hpp"

namespace fwData
{

/**
 * @class   GenericField
 * @brief   Generic "field" object template.
 *
 * A generic object contains a value.
 *
 * @see     ::fwData::Float, ::fwData::Boolean, ::fwData::Integer
 *
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 */
template< typename T >
class FWDATA_CLASS_API GenericField : public GenericFieldBase
{

public:
    fwCoreClassDefinitionsWithNFactoriesMacro( (GenericField<T>)(::fwData::Object),
       ((::fwTools::Factory::New< GenericField<T> > ,() ))
       ((GenericFieldFactory  ,((const  T)) ))
       );


    typedef T ValueType;

    /// @brief Get the value (mutable version).
    T& value() throw() { return m_value; }

    /// @brief Get the value (constant version).
    const T& value() const throw() { return m_value; }

    /// @brief set the value
    void setValue(const T &newValue) throw() { m_value = newValue; }

    /// @brief get the value
    T getValue() const throw() { return m_value; }

    /// @brief Conversion to a scalar type.
    operator T() throw() { return m_value; }

    FWDATA_API bool operator== (const GenericFieldBase &lf)
    {
        const ::fwData::GenericField<T> &gField =  dynamic_cast< const ::fwData::GenericField<T> & >(lf);
        SLM_ASSERT("GenericField must have same ValueType.", &gField );
        return ( this->m_value == gField.value() );
    }
    FWDATA_API bool operator!= (const GenericFieldBase &lf )
    {
        const ::fwData::GenericField<T> &gField =  dynamic_cast< const ::fwData::GenericField<T> & >(lf);
        SLM_ASSERT("GenericField must have same ValueType.", &gField);
        return ( this->m_value != gField.value() );
    }
    FWDATA_API bool operator< (const GenericFieldBase &lf )
    {
        const ::fwData::GenericField<T> &gField =  dynamic_cast< const ::fwData::GenericField<T> & >(lf);
        SLM_ASSERT("GenericField must have same ValueType.", &gField);
        return ( this->m_value < gField.value() );
    }
    FWDATA_API bool operator> (const GenericFieldBase &lf)
    {
        const ::fwData::GenericField<T> &gField =  dynamic_cast< const ::fwData::GenericField<T> & >(lf);
        SLM_ASSERT("GenericField must have same ValueType.", &gField);
        return ( this->m_value > gField.value() );
    }
    FWDATA_API bool operator<= (const GenericFieldBase &lf)
    {
        const ::fwData::GenericField<T> &gField =  dynamic_cast< const ::fwData::GenericField<T> & >(lf);
        SLM_ASSERT("GenericField must have same ValueType.", &gField);
        return ( this->m_value <= gField.value() );
    }
    FWDATA_API bool operator>= (const GenericFieldBase &lf )
    {
        const ::fwData::GenericField<T> &gField =  dynamic_cast< const ::fwData::GenericField<T> & >(lf);
        SLM_ASSERT("GenericField must have same ValueType.", &gField);
        return ( this->m_value >= gField.value() );
    }

    FWDATA_API virtual ::std::string toString() const
    {
       return ::boost::lexical_cast< ::std::string >(this->m_value);
    }

    FWDATA_API virtual void fromString(const ::std::string &_value)
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
    FWDATA_API GenericField( const T value = T( ) ) throw()
    :   m_value( value )
    {}

    /**
     * @brief Destructor.
     */
    FWDATA_API virtual ~GenericField() throw() {}

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
    field = ::fwTools::Factory::New< GT >();
    field->value() = value;
    return field;
}

template< typename T >
typename GenericField<T>::sptr GenericField<T>::GenericFieldFactory(const T value)
{
    typename GenericField<T>::sptr field;
    field = GenericFieldFactory< GenericField<T> >(value);
    field->value() = value;
    return field;
}


} // namespace fwData

#endif // _FWDATA_GENERICFIELD_HPP_
