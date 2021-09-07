/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "io/igtl/detail/converter/IConverter.hpp"
#include "io/igtl/detail/exception/Conversion.hpp"

namespace sight::io::igtl::detail
{

namespace converter
{

/**
 *
 * @brief class to manage conversion between data::Composite and STP_TDATA in igtl::TrackingDataMessage
 */
class IO_IGTL_CLASS_API TrackingStopConverter : public IConverter
{
public:

    /// Constructor
    IO_IGTL_API TrackingStopConverter();

    /// Destructor
    IO_IGTL_API ~TrackingStopConverter();

    /// convert a ::igtl::MessageBase to a data::Object
    IO_IGTL_API data::Object::sptr fromIgtlMessage(::igtl::MessageBase::Pointer const src) const;

    /**
     * @brief convert a data::Composite to a igtl::TrackingDataMessage
     * @return an igtl::TrackingDataMessage converted from an data::Composite
     */
    IO_IGTL_API ::igtl::MessageBase::Pointer fromFwDataObject(data::Object::csptr src) const;

    /**
     * @brief create new TrackingStopConverter
     * @return a smart pointer to a TrackingStopConverter
     */
    IO_IGTL_API static IConverter::sptr New();

    /**
     * @brief get the igtlType supported for conversion
     * @return the igtlType supported for conversion
     */
    IO_IGTL_API std::string const& getIgtlType() const;

    /**
     * @brief get the fwData object type supported for conversion
     * @return the fwData Object type supported for conversion
     */
    IO_IGTL_API std::string const& getFwDataObjectType() const;

private:

    /// igtl type supported for conversion
    static const std::string s_IGTL_TYPE;

    /// fwData type supported for conversion
    static const std::string s_FWDATA_OBJECT_TYPE;
};

} // namespace converter

} // namespace sight::io::igtl::detail
