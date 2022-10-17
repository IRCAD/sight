/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "io/igtl/config.hpp"
#include "io/igtl/detail/exception/Conversion.hpp"

#include <core/macros.hpp>
#include <core/tools/Failed.hpp>

#include <data/Object.hpp>

#include <igtlMessageBase.h>

namespace sight::io::igtl::detail::converter
{

/**
 * @interface IConverter
 *
 * @brief interface for all converter between ::igtl::MessageBase
 */
class IO_IGTL_CLASS_API IConverter
{
public:

    typedef SPTR(IConverter) sptr;

    ///destructor
    virtual ~IConverter()
    = default;

    /**
     * @brief method to implement conversion igtl message to fwData object
     *
     * @return a data::Object
     */
    [[nodiscard]] IO_IGTL_API virtual data::Object::sptr fromIgtlMessage(::igtl::MessageBase::Pointer src) const =
    0;

    /**
     * @brief method to implement conversion fwData object to igtl message
     *
     * @return a ::igtl::MessageBase
     */
    [[nodiscard]] IO_IGTL_API virtual ::igtl::MessageBase::Pointer fromFwDataObject(data::Object::csptr src) const = 0;

    /**
     * @brief get the igtlType supported for conversion
     *
     * @return the igtlType supported for conversion
     */
    [[nodiscard]] IO_IGTL_API virtual std::string const& getIgtlType() const = 0;

    /**
     * @brief get the fwData object type supported for conversion
     *
     * @return the fwData Object type supported for conversion
     */
    [[nodiscard]] IO_IGTL_API virtual std::string const& getFwDataObjectType() const = 0;
};

} // namespace sight::io::igtl::detail::converter
