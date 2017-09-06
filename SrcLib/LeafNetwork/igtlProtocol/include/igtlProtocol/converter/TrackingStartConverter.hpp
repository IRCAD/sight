/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IGTLPROTOCOL_CONVERTER_TRACKINGSTARTCONVERTER_HPP__
#define __IGTLPROTOCOL_CONVERTER_TRACKINGSTARTCONVERTER_HPP__

#include "igtlProtocol/converter/IConverter.hpp"
#include "igtlProtocol/exception/Conversion.hpp"

namespace igtlProtocol
{
namespace converter
{
/**
 *
 * @brief class to manage conversion between fwData::Composite and STT_TDATA in igtl::TrackingDataMessage
 */
class IGTLPROTOCOL_CLASS_API TrackingStartConverter :  public IConverter
{
public:
    /// Constructor
    IGTLPROTOCOL_API TrackingStartConverter();

    /// Destructor
    IGTLPROTOCOL_API ~TrackingStartConverter();

    /// convert a igtl::MessageBase to a ::fwData::Object
    IGTLPROTOCOL_API void fromIgtlMessage (::igtl::MessageBase::Pointer const src, ::fwData::Object::sptr& dest) const;

    /**
     * @brief convert a fwData::Composite to a igtl::TrackingDataMessage
     * @return an igtl::TrackingDataMessage converted from an fwData::Composite
     */
    IGTLPROTOCOL_API ::igtl::MessageBase::Pointer fromFwDataObject (::fwData::Object::csptr src) const;

    /**
     * @brief create new TrackingStartConverter
     * @return a smart pointer to a TrackingStartConverter
     */
    IGTLPROTOCOL_API static IConverter::sptr New();

    /**
     * @brief get the igtlType supported for conversion
     * @return the igtlType supported for conversion
     */
    IGTLPROTOCOL_API std::string const& getIgtlType() const;

    /**
     * @brief get the fwData object type supported for conversion
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

#endif // __IGTLPROTOCOL_CONVERTER_TRACKINGSTARTCONVERTER_HPP__
