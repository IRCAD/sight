/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#pragma once

#include "igtlProtocol/config.hpp"
#include "igtlProtocol/exception/Conversion.hpp"

#include <fwCore/macros.hpp>

#include <fwData/Object.hpp>

#include <fwTools/Failed.hpp>

#include <igtlMessageBase.h>

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
