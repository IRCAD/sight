/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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
#include <core/tools/failed.hpp>

#include <data/object.hpp>

#include <igtlMessageBase.h>

namespace sight::io::igtl::detail::converter
{

/**
 * @interface base
 *
 * @brief interface for all converter between ::igtl::MessageBase
 */
class IO_IGTL_CLASS_API base
{
public:

    using sptr = std::shared_ptr<base>;

    ///destructor
    virtual ~base()
    = default;

    /**
     * @brief method to implement conversion igtl message to fwData object
     *
     * @return a data::object
     */
    [[nodiscard]] IO_IGTL_API virtual data::object::sptr from_igtl_message(::igtl::MessageBase::Pointer _src) const =
    0;

    /**
     * @brief method to implement conversion fwData object to igtl message
     *
     * @return a ::igtl::MessageBase
     */
    [[nodiscard]] IO_IGTL_API virtual ::igtl::MessageBase::Pointer from_fw_data_object(data::object::csptr _src) const =
    0;

    /**
     * @brief get the igtlType supported for conversion
     *
     * @return the igtlType supported for conversion
     */
    [[nodiscard]] IO_IGTL_API virtual std::string const& get_igtl_type() const = 0;

    /**
     * @brief get the fwData object type supported for conversion
     *
     * @return the fwData Object type supported for conversion
     */
    [[nodiscard]] IO_IGTL_API virtual std::string const& get_fw_data_object_type() const = 0;
};

} // namespace sight::io::igtl::detail::converter
