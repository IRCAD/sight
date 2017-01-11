/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IGTLPROTOCOL_CONVERTER_MATRIXCONVERTER_HPP__
#define __IGTLPROTOCOL_CONVERTER_MATRIXCONVERTER_HPP__

#include "igtlProtocol/converter/IConverter.hpp"
#include "igtlProtocol/exception/Conversion.hpp"

namespace igtlProtocol
{
namespace converter
{
/**
 *
 * @brief class to manage conversion between fwData::TransformationMatrix3D and igtl::TransformationMessage
 */
class IGTLPROTOCOL_CLASS_API MatrixConverter :
    public IConverter
{
public:
    /// Constructor
    IGTLPROTOCOL_API MatrixConverter();

    /// Destructor
    IGTLPROTOCOL_API ~MatrixConverter();

    /**
     * @brief convert a igtl::ImageMessage to a ::fwData::Object
     *
     * @return an fwData::Image converted from an igtl::ImageMessage
     */
    IGTLPROTOCOL_API void fromIgtlMessage (::igtl::MessageBase::Pointer const src,
                                           ::fwData::Object::sptr& dest) const
    throw (::igtlProtocol::exception::Conversion);

    /**
     * @brief convert a fwData::Image to a igtl::ImageMessage
     *
     * @return an fwData::Image converted from an igtl::ImageMessage
     */
    IGTLPROTOCOL_API ::igtl::MessageBase::Pointer fromFwDataObject (::fwData::Object::csptr src) const;

    /**
     * @brief create new MatrixConverter
     *
     * @return a smart pointer to a MatrixConverter
     */
    IGTLPROTOCOL_API static IConverter::sptr New();

    /**
     * @brief get the igtlType supported for conversion
     *
     * @return the igtlType supported for conversion
     */
    IGTLPROTOCOL_API std::string const& getIgtlType() const;

    /**
     * @brief get the fwData object type supported for conversion
     *
     * @return the fwData Object type supported for conversion
     */
    IGTLPROTOCOL_API std::string const& getFwDataObjectType() const;
private:
    /// igtl type supported for conversion
    static const std::string s_IGTL_TYPE;

    /// fwData type supported for conversion
    static const std::string s_FWDATA_OBJECT_TYPE;
};
} // namespace converter
} // namespace igtlProtocol

#endif // __IGTLPROTOCOL_CONVERTER_MATRIXCONVERTER_HPP__
