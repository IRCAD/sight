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
 * @brief class to manage conversion between data::Line and igtl::PositionMessage
 */
class IO_IGTL_CLASS_API LineConverter :
    public base
{
public:

    /// Constructor
    IO_IGTL_API LineConverter();

    /// Destructor
    IO_IGTL_API ~LineConverter() override;

    /**
     * @brief convert a igtl::PositionMessage to a data::Line
     *
     * @return an data::Image converted from an ::igtl::ImageMessage
     */
    [[nodiscard]] IO_IGTL_API data::Object::sptr fromIgtlMessage(::igtl::MessageBase::Pointer src) const override;

    /**
     * @brief convert a data::Line to an igtl::PositionMessage
     *
     * @return an  ::igtl::ImageMessage converted from an data::Image
     */
    [[nodiscard]] IO_IGTL_API ::igtl::MessageBase::Pointer fromFwDataObject(data::Object::csptr src) const override;

    /**
     * @brief create a new LineConverter smart pointer
     *
     * @return a smart pointer to an ImageConverter
     */
    IO_IGTL_API static base::sptr New();

    /**
     * @brief get the igtlType supported for conversion
     *
     * @return the igtlType supported for conversion
     */
    [[nodiscard]] IO_IGTL_API std::string const& getIgtlType() const override;

    /**
     * @brief get the fwData object type supported for conversion
     *
     * @return the fwData Object type supported for conversion
     */
    [[nodiscard]] IO_IGTL_API std::string const& getFwDataObjectType() const override;

private:

    /// igtl type supported for conversion
    static std::string const s_IGTL_TYPE;

    /// fwData type supported for conversion
    static std::string const s_FWDATA_OBJECT_TYPE;
};

} // namespace sight::io::igtl::detail::converter
