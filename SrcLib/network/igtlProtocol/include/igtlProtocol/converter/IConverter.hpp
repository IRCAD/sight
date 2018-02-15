/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IGTLPROTOCOL_CONVERTER_ICONVERTER_HPP__
#define __IGTLPROTOCOL_CONVERTER_ICONVERTER_HPP__

#include "igtlProtocol/config.hpp"
#include "igtlProtocol/exception/Conversion.hpp"

#include <fwCore/macros.hpp>

#include <fwData/Object.hpp>

#include <fwTools/Failed.hpp>

#include <igtl/igtlMessageBase.h>

namespace igtlProtocol
{
namespace converter
{
/**
 * @interface IConverter
 *
 * @brief interface for all converter between igtl::MessageBase
 */
class IGTLPROTOCOL_CLASS_API IConverter
{
public:

    typedef SPTR (IConverter) sptr;

    ///destructor
    virtual ~IConverter()
    {
    }

    /**
     * @brief method to implement conversion igtl message to fwData object
     *
     * @return a fwData::Object
     */
    IGTLPROTOCOL_API virtual ::fwData::Object::sptr fromIgtlMessage(const ::igtl::MessageBase::Pointer src) const = 0;

    /**
     * @brief method to implement conversion fwData object to igtl message
     *
     * @return a igtl::MessageBase
     */
    IGTLPROTOCOL_API virtual ::igtl::MessageBase::Pointer fromFwDataObject (::fwData::Object::csptr src) const = 0;

    /**
     * @brief get the igtlType supported for conversion
     *
     * @return the igtlType supported for conversion
     */
    IGTLPROTOCOL_API virtual std::string const& getIgtlType() const = 0;

    /**
     * @brief get the fwData object type supported for conversion
     *
     * @return the fwData Object type supported for conversion
     */
    IGTLPROTOCOL_API virtual std::string const& getFwDataObjectType() const = 0;
};

} // namespace converter
} // namespace igtlProtocol

#endif // __IGTLPROTOCOL_CONVERTER_ICONVERTER_HPP__
