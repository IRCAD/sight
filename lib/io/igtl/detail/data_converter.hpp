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

#include <sight/io/igtl/config.hpp>

#include "io/igtl/detail/converter/base.hpp"

#include <core/tools/failed.hpp>

namespace sight::io::igtl::detail
{

///converterRegisterMacro need to be called by every converter
#define CONVERTER_REGISTER_MACRO(ClassName) \
        static io::igtl::detail::data_converter::registry<ClassName> BOOST_PP_CAT(s__factory__record__, __LINE__);

/**
 *
 * @brief Utility class for conversion between igtl data and fwData
 * Every converter should register to this class, due to this pattern DataConverter is a singleton
 * and need to be called by DataConverter::getInstance()
 */
class SIGHT_IO_IGTL_CLASS_API data_converter
{
public:

    /// Constructor
    SIGHT_IO_IGTL_API data_converter();

    ///Typedef to a DataConverter::sptr
    using sptr = std::shared_ptr<data_converter>;

    ///Destructor
    SIGHT_IO_IGTL_API ~data_converter();

    ///Static method to access a instance of DataConverter (singleton)
    SIGHT_IO_IGTL_API static data_converter::sptr get_instance();

    ///Static method called by the registry class to register a new converter
    SIGHT_IO_IGTL_API static void register_converter(converter::base::sptr _c);

    ///Class Registry used by the macro to register new converter in the m_converters
    template<typename T>
    class registry
    {
    public:

        registry()
        {
            data_converter::register_converter(T::New());
        }
    };

    /**
     * @brief convert a data::object to a ::igtl::MessageBase
     * @return a ::igtl::MessageBase smart pointer
     */
    [[nodiscard]] SIGHT_IO_IGTL_API ::igtl::MessageBase::Pointer from_fw_object(data::object::csptr _src) const;

    /**
     * @brief convert a ::igtl::MessageBase to a data::object
     */
    [[nodiscard]] SIGHT_IO_IGTL_API data::object::sptr from_igtl_message(const ::igtl::MessageBase::Pointer _src) const;

    /**
     * @brief get status message
     *
     * @param[in] _igtl_code see openigtlink documentation
     * @param[in] _igtl_sub_code see openigtlink documentation
     * @param[in] _err_msg error msg
     *
     * @return igtl message smart pointer
     */
    static SIGHT_IO_IGTL_API ::igtl::MessageBase::Pointer get_status_message(
        int _igtl_code,
        int _igtl_sub_code,
        const std::string& _err_msg
    );

    /**
     * @brief get capability message
     *
     * @return igtl message smart pointer contain igtl::CapabilityMessage with all supported types
     */
    [[nodiscard]] SIGHT_IO_IGTL_API ::igtl::MessageBase::Pointer get_capabilities_message() const;

private:

    /// a vector of base smart pointer
    std::vector<converter::base::sptr> m_converters;
};

} //namespace sight::io::igtl::detail
