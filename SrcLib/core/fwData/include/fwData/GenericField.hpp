/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_GENERICFIELD_HPP_
#define _FWDATA_GENERICFIELD_HPP_

#include <string>

#include <fwCore/base.hpp>

#include <fwTools/Factory.hpp>
#include "fwData/config.hpp"
#include "fwData/Object.hpp"

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
class FWDATA_CLASS_API GenericField : public Object
{
public:
    fwCoreClassDefinitionsWithNFactoriesMacro( (GenericField<T>)(::fwData::Object::Baseclass),
       ((::fwTools::Factory::New< GenericField<T> > ,() ))
       //((GenericFieldFactory<T>  ,((T)) ))
       );


    typedef T ValueType;

    template< typename GT >
    static typename GT::sptr GenericFieldFactory(const typename GT::ValueType value)
    {
        typename GT::sptr field;
        field = ::fwTools::Factory::New< GT >();
        field->value() = value;
        return field;
    }

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

    /// @brief Get the value (mutable version).
    T& value() throw() { return m_value; }

    /// @brief Get the value (constant version).
    const T& value() const throw() { return m_value; }

    /// @brief Conversion to a scalar type.
    operator T() throw() { return m_value; }

protected:
    /// @brief The stored value.
    T m_value;
};



} // namespace fwData

//#include "fwData/GenericField.hxx"

#endif // _FWDATA_GENERICFIELD_HPP_
