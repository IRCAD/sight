/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#ifndef __IGTLPROTOCOL_CONVERTER_SCALARCONVERTER_HPP__
#define __IGTLPROTOCOL_CONVERTER_SCALARCONVERTER_HPP__

#include "igtlProtocol/converter/IConverter.hpp"
#include "igtlProtocol/exception/Conversion.hpp"
#include "igtlProtocol/helper/ScalarToBytes.hpp"
#include "igtlProtocol/RawMessage.hpp"

#include <fwData/Float.hpp>
#include <fwData/Integer.hpp>

#include <boost/lexical_cast.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits/is_base_of.hpp>

namespace igtlProtocol
{
namespace converter
{
/**
 *
 * @brief class to manage conversion between fwData::Integer of Float and igtl::RawMessage
 */
template<typename ScalarType, typename FwDataObjectType>
class IGTLPROTOCOL_CLASS_API ScalarConverter : public IConverter
{
public:

    /// Constructor
    ScalarConverter()
    {
        BOOST_STATIC_ASSERT( ::boost::is_base_of< ::fwData::Object, FwDataObjectType>::value);
    }

    ScalarConverter(const std::string& a) :
        m_igtlType(a)
    {
        BOOST_STATIC_ASSERT( ::boost::is_base_of< ::fwData::Object, FwDataObjectType>::value);
    }

    /// Destructor
    virtual ~ScalarConverter()
    {
    }

    /**
     * @brief convert a igtl::RawMessage(which contain scalar) to a FwDataObjectType
     *
     * @return an fwData::Integer converted from an igtl::RawMessage
     */
    ::fwData::Object::sptr fromIgtlMessage (const ::igtl::MessageBase::Pointer src) const
    {
        typename FwDataObjectType::sptr obj = FwDataObjectType::New();

        RawMessage::Pointer msg = RawMessage::Pointer(dynamic_cast< RawMessage*>(src.GetPointer()));
        const ScalarType scalar = helper::ScalarToBytes<ScalarType>::fromBytes(msg->getMessage().data());
        obj->setValue(scalar);

        return obj;
    }

    /**
     * @brief convert a ::fwData::Scalar to an igtl::RawMessage
     *
     * @return an  igtl::RawMessage converted from an fwData::Integer
     */
    ::igtl::MessageBase::Pointer fromFwDataObject (::fwData::Object::csptr src) const
    {
        RawMessage::Pointer msg;
        typename FwDataObjectType::csptr obj = FwDataObjectType::dynamicConstCast(src);

        msg = RawMessage::New(m_igtlType);
        RawMessage::RawDataType content = helper::ScalarToBytes<ScalarType>::toBytes(obj->getValue());
        msg->append(content);
        return ::igtl::MessageBase::Pointer(msg);
    }

    /**
     * @brief create a new ScalarConverter smart pointer
     *
     * @return a smart pointer to an ScalarConverter
     */
    static IConverter::sptr New(std::string const& igtlScalarType)
    {
        return std::make_shared<ScalarConverter<ScalarType, FwDataObjectType> >(igtlScalarType);
    }

    /**
     * @brief get the igtlType supported for conversion
     *
     * @return the igtlType supported for conversion
     */
    const std::string& getIgtlType() const
    {
        return m_igtlType;
    }

    /**
     * @brief get the fwData object type supported for conversion
     *
     * @return the fwData Object type supported for conversion
     */
    const std::string& getFwDataObjectType() const
    {
        return FwDataObjectType::classname();
    }

protected:

    /// igtl type supported for conversion
    const std::string m_igtlType;

};

class IGTLPROTOCOL_CLASS_API IntConverter : public ScalarConverter< int, ::fwData::Integer>
{
public:
    typedef ScalarConverter< int, ::fwData::Integer>  Superclass;
    /// Constructor
    IntConverter() :
        Superclass("INTEGER")
    {
    }

    /// Destructor
    virtual ~IntConverter()
    {
    }

    /**
     * @brief create a new IntConverter smart pointer
     *
     * @return a smart pointer to an IntConverter
     */
    static IConverter::sptr New()
    {
        return std::make_shared< IntConverter >();
    }
};

class IGTLPROTOCOL_CLASS_API FloatConverter : public ScalarConverter< float, ::fwData::Float>
{
public:
    typedef ScalarConverter< float, ::fwData::Float>  Superclass;
    /// Constructor
    FloatConverter() :
        Superclass("FLOAT")
    {
    }

    /// Destructor
    virtual ~FloatConverter()
    {
    }

    /**
     * @brief create a new FloatConverter smart pointer
     *
     * @return a smart pointer to an FloatConverter
     */
    static IConverter::sptr New()
    {
        return std::make_shared< FloatConverter >();
    }
};

} // namespace converter
} // namespace igtlProtocol

#endif // __IGTLPROTOCOL_CONVERTER_SCALARCONVERTER_HPP__
