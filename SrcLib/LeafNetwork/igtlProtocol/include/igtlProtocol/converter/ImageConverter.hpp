/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IGTLPROTOCOL_CONVERTER_IMAGECONVERTER_HPP__
#define __IGTLPROTOCOL_CONVERTER_IMAGECONVERTER_HPP__

#include "igtlProtocol/converter/IConverter.hpp"
#include "igtlProtocol/exception/Conversion.hpp"

namespace igtlProtocol
{
namespace converter
{
/**
 *
 * @brief class to manage conversion between fwData::Image and igtl::ImageMessage
 */
class IGTLPROTOCOL_CLASS_API ImageConverter :
    public IConverter
{
public:
    /// Constructor
    IGTLPROTOCOL_API ImageConverter();

    /// Destructor
    IGTLPROTOCOL_API ~ImageConverter();

    /**
     * @brief convert a igtl::ImageMessage to a ::fwData::Object
     *
     * @return an fwData::Image converted from an igtl::ImageMessage
     */
    IGTLPROTOCOL_API void fromIgtlMessage (::igtl::MessageBase::Pointer const src, ::fwData::Object::sptr& dest) const;

    /**
     * @brief convert a ::fwData::Image to an igtl::MessageBase
     *
     * @return an  igtl::ImageMessage converted from an fwData::Image
     */
    IGTLPROTOCOL_API ::igtl::MessageBase::Pointer fromFwDataObject (::fwData::Object::csptr src) const;

    /**
     * @brief create a new ImageConverter smart pointer
     *
     * @return a smart pointer to an ImageConverter
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

#endif // __IGTLPROTOCOL_CONVERTER_IMAGECONVERTER_HPP__
