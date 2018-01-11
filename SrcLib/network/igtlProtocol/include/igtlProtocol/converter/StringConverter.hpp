/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IGTLPROTOCOL_CONVERTER_STRINGCONVERTER_HPP__
#define __IGTLPROTOCOL_CONVERTER_STRINGCONVERTER_HPP__

#include "igtlProtocol/converter/IConverter.hpp"
#include "igtlProtocol/exception/Conversion.hpp"

namespace igtlProtocol
{
namespace converter
{
/**
 *
 * @brief class to manage conversion between fwData::String and igtl::StringMessage
 */
class IGTLPROTOCOL_CLASS_API StringConverter :
    public IConverter
{
public:
    /// Constructor
    IGTLPROTOCOL_API StringConverter();

    /// Destructor
    IGTLPROTOCOL_API ~StringConverter();

    /**
     * @brief convert a igtl::StringMessage to a ::fwData::String
     *
     * @return an fwData::String converted from an igtl::StringMessage
     */
    IGTLPROTOCOL_API ::fwData::Object::sptr fromIgtlMessage(const ::igtl::MessageBase::Pointer src) const;

    /**
     * @brief convert a fwData::String to a igtl::StringMessage
     *
     * @return an fwData::String converted from an igtl::StringMessage
     */
    IGTLPROTOCOL_API ::igtl::MessageBase::Pointer fromFwDataObject (::fwData::Object::csptr src) const;

    /**
     * @brief create new StringConverter
     *
     * @return a smart pointer to a StringConverter
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

#endif // __IGTLPROTOCOL_CONVERTER_STRINGCONVERTER_HPP__
