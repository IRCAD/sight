/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#pragma once

#include "io/igtl/detail/converter/base.hpp"
#include "io/igtl/detail/exception/Conversion.hpp"

namespace sight::io::igtl::detail::converter
{

/**
 *
 * @brief class to manage conversion between data::composite and igtl::TrackingDataMessage
 */
class IO_IGTL_CLASS_API CompositeConverter : public base
{
public:

    /// Constructor
    IO_IGTL_API CompositeConverter();

    /// Destructor
    IO_IGTL_API ~CompositeConverter() override;

    /// convert a ::igtl::MessageBase to a data::object
    [[nodiscard]] IO_IGTL_API data::object::sptr fromIgtlMessage(::igtl::MessageBase::Pointer _src) const override;

    /**
     * @brief convert a data::composite to a igtl::TrackingDataMessage
     * @return an igtl::TrackingDataMessage converted from an data::composite
     */
    [[nodiscard]] IO_IGTL_API ::igtl::MessageBase::Pointer fromFwDataObject(data::object::csptr _src) const override;

    /**
     * @brief create new CompositeConverter
     * @return a smart pointer to a CompositeConverter
     */
    IO_IGTL_API static base::sptr New();

    /**
     * @brief get the igtlType supported for conversion
     * @return the igtlType supported for conversion
     */
    [[nodiscard]] IO_IGTL_API std::string const& get_igtl_type() const override;

    /**
     * @brief get the fwData object type supported for conversion
     * @return the fwData Object type supported for conversion
     */
    [[nodiscard]] IO_IGTL_API std::string const& getFwDataObjectType() const override;

private:

    /// igtl type supported for conversion
    static const std::string s_IGTL_TYPE;

    /// fwData type supported for conversion
    static const std::string s_FWDATA_OBJECT_TYPE;
};

} // namespace sight::io::igtl::detail::converter
