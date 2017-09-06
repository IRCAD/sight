/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IGTLPROTOCOL_CONVERTER_SCALARCONVERTER_HPP__
#define __IGTLPROTOCOL_CONVERTER_SCALARCONVERTER_HPP__

#include "igtlProtocol/RawMessage.hpp"
#include "igtlProtocol/converter/IConverter.hpp"
#include "igtlProtocol/helper/ScalarToBytes.hpp"
#include "igtlProtocol/exception/Conversion.hpp"

#include <boost/static_assert.hpp>
#include <boost/type_traits/is_base_of.hpp>
#include <boost/lexical_cast.hpp>

#include <fwData/Integer.hpp>
#include <fwData/Float.hpp>

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
    void fromIgtlMessage (::igtl::MessageBase::Pointer const src,
                          ::fwData::Object::sptr& dest) const
    {
        RawMessage::Pointer msg;
        typename FwDataObjectType::sptr obj;

        FW_RAISE_EXCEPTION_IF(exception::Conversion("Incompatible destination object type must be a scalar type"),
                              dest->getClassname() != FwDataObjectType::classname());

        obj = FwDataObjectType::dynamicCast(dest);
        msg = RawMessage::Pointer(dynamic_cast< RawMessage*>(src.GetPointer()));
        const ScalarType scalar = helper::ScalarToBytes<ScalarType>::fromBytes(msg->getMessage().data());
        obj->setValue(scalar);
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
    std::string const& getIgtlType() const
    {
        return m_igtlType;
    }

    /**
     * @brief get the fwData object type supported for conversion
     *
     * @return the fwData Object type supported for conversion
     */
    std::string const& getFwDataObjectType() const
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
    IntConverter() : Superclass("INTEGER")
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
    FloatConverter() : Superclass("FLOAT")
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
