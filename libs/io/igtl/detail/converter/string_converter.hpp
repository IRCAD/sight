/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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
#include "io/igtl/detail/exception/conversion.hpp"

namespace sight::io::igtl::detail::converter
{

/**
 *
 * @brief class to manage conversion between data::string and igtl::StringMessage
 */
class SIGHT_IO_IGTL_CLASS_API string_converter :
    public base
{
public:

    /// Constructor
    SIGHT_IO_IGTL_API string_converter();

    /// Destructor
    SIGHT_IO_IGTL_API ~string_converter() override;

    /**
     * @brief convert a igtl::StringMessage to a data::string
     *
     * @return an data::string converted from an igtl::StringMessage
     */
    [[nodiscard]] SIGHT_IO_IGTL_API data::object::sptr from_igtl_message(::igtl::MessageBase::Pointer _src) const
    override;

    /**
     * @brief convert a data::string to a igtl::StringMessage
     *
     * @return an data::string converted from an igtl::StringMessage
     */
    [[nodiscard]] SIGHT_IO_IGTL_API ::igtl::MessageBase::Pointer from_fw_data_object(data::object::csptr _src) const
    override;

    /**
     * @brief create new StringConverter
     *
     * @return a smart pointer to a StringConverter
     */
    SIGHT_IO_IGTL_API static base::sptr New();

    /**
     * @brief get the igtlType supported for conversion
     *
     * @return the igtlType supported for conversion
     */
    [[nodiscard]] SIGHT_IO_IGTL_API std::string const& get_igtl_type() const override;

    /**
     * @brief get the fwData object type supported for conversion
     *
     * @return the fwData Object type supported for conversion
     */
    [[nodiscard]] SIGHT_IO_IGTL_API std::string const& get_fw_data_object_type() const override;

private:

    /// igtl type supported for conversion
    static const std::string IGTL_TYPE;

    /// fwData type supported for conversion
    static const std::string FWDATA_OBJECT_TYPE;
};

} // namespace sight::io::igtl::detail::converter
