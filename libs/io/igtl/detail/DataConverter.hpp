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

#include "io/igtl/config.hpp"
#include "io/igtl/detail/converter/base.hpp"

#include <core/tools/failed.hpp>

namespace sight::io::igtl::detail
{

///converterRegisterMacro need to be called by every converter
#define CONVERTER_REGISTER_MACRO(ClassName) \
    static io::igtl::detail::DataConverter::Registry<ClassName> BOOST_PP_CAT(s__factory__record__, __LINE__);

/**
 *
 * @brief Utility class for conversion between igtl data and fwData
 * Every converter should register to this class, due to this pattern DataConverter is a singleton
 * and need to be called by DataConverter::getInstance()
 */
class IO_IGTL_CLASS_API DataConverter
{
public:

    /// Constructor
    IO_IGTL_API DataConverter();

    ///Typedef to a DataConverter::sptr
    typedef SPTR(DataConverter) sptr;

    ///Destructor
    IO_IGTL_API ~DataConverter();

    ///Static method to access a instance of DataConverter (singleton)
    IO_IGTL_API static DataConverter::sptr getInstance();

    ///Static method called by the registry class to register a new converter
    IO_IGTL_API static void registerConverter(converter::base::sptr _c);

    ///Class Registry used by the macro to register new converter in the m_converters
    template<typename T>
    class Registry
    {
    public:

        Registry()
        {
            DataConverter::registerConverter(T::New());
        }
    };

    /**
     * @brief convert a data::object to a ::igtl::MessageBase
     * @return a ::igtl::MessageBase smart pointer
     */
    [[nodiscard]] IO_IGTL_API ::igtl::MessageBase::Pointer fromFwObject(data::object::csptr _src) const;

    /**
     * @brief convert a ::igtl::MessageBase to a data::object
     */
    [[nodiscard]] IO_IGTL_API data::object::sptr fromIgtlMessage(const ::igtl::MessageBase::Pointer _src) const;

    /**
     * @brief get status message
     *
     * @param[in] igtlCode see openigtlink documentation
     * @param[in] igtlSubCode see openigtlink documentation
     * @param[in] errMsg error msg
     *
     * @return igtl message smart pointer
     */
    static IO_IGTL_API ::igtl::MessageBase::Pointer getStatusMessage(
        int _igtl_code,
        int _igtl_sub_code,
        const std::string& _err_msg
    );

    /**
     * @brief get capability message
     *
     * @return igtl message smart pointer contain igtl::CapabilityMessage with all supported types
     */
    [[nodiscard]] IO_IGTL_API ::igtl::MessageBase::Pointer getCapabilitiesMessage() const;

private:

    /// a vector of base smart pointer
    std::vector<converter::base::sptr> m_converters;
};

} //namespace sight::io::igtl::detail
