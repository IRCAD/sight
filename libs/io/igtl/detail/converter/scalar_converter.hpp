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

// cspell:ignore NOLINT

#pragma once

#include "io/igtl/detail/converter/base.hpp"
#include "io/igtl/detail/exception/conversion.hpp"
#include "io/igtl/detail/helper/scalar_to_bytes.hpp"
#include "io/igtl/detail/raw_message.hpp"

#include <data/integer.hpp>
#include <data/real.hpp>

#include <boost/type_traits/is_base_of.hpp>

#include <utility>

namespace sight::io::igtl::detail::converter
{

/**
 *
 * @brief class to manage conversion between data::integer of Float and igtl::RawMessage
 */
template<typename ScalarType, typename fw_data_object_t>
class SIGHT_IO_IGTL_CLASS_API scalar_converter : public base
{
public:

    /// Constructor
    scalar_converter()
    {
        static_assert(std::is_base_of<data::object, fw_data_object_t>::value);
    }

    scalar_converter(std::string _a) :
        m_igtl_type(std::move(_a))
    {
        static_assert(std::is_base_of<data::object, fw_data_object_t>::value);
    }

    /// Destructor
    ~scalar_converter() override
    = default;

    /**
     * @brief convert a igtl::RawMessage(which contain scalar) to a fw_data_object_t
     *
     * @return an data::integer converted from an igtl::RawMessage
     */
    [[nodiscard]] data::object::sptr from_igtl_message(const ::igtl::MessageBase::Pointer _src) const override
    {
        auto obj = std::make_shared<fw_data_object_t>();

        raw_message::Pointer msg = raw_message::Pointer(dynamic_cast<raw_message*>(_src.GetPointer()));
        const ScalarType scalar  = helper::scalar_to_bytes<ScalarType>::from_bytes(msg->get_message().data());
        obj->set_value(scalar);

        return obj;
    }

    /**
     * @brief convert a data::Scalar to an igtl::RawMessage
     *
     * @return an  igtl::RawMessage converted from an data::integer
     */
    [[nodiscard]] ::igtl::MessageBase::Pointer from_fw_data_object(data::object::csptr _src) const override
    {
        raw_message::Pointer msg;
        typename fw_data_object_t::csptr obj = std::dynamic_pointer_cast<const fw_data_object_t>(_src);

        msg = raw_message::New(m_igtl_type);
        raw_message::raw_data_t content = helper::scalar_to_bytes<ScalarType>::to_bytes(ScalarType(obj->get_value()));
        msg->append(content);
        return ::igtl::MessageBase::Pointer(msg); // NOLINT(modernize-return-braced-init-list)
    }

    /**
     * @brief create a new ScalarConverter smart pointer
     *
     * @return a smart pointer to an ScalarConverter
     */
    static base::sptr New(std::string const& _igtl_scalar_type)
    {
        return std::make_shared<scalar_converter<ScalarType, fw_data_object_t> >(_igtl_scalar_type);
    }

    /**
     * @brief get the igtlType supported for conversion
     *
     * @return the igtlType supported for conversion
     */
    [[nodiscard]] const std::string& get_igtl_type() const override
    {
        return m_igtl_type;
    }

    /**
     * @brief get the fwData object type supported for conversion
     *
     * @return the fwData Object type supported for conversion
     */
    [[nodiscard]] const std::string& get_fw_data_object_type() const override
    {
        return fw_data_object_t::classname();
    }

protected:

    /// igtl type supported for conversion
    const std::string m_igtl_type;
};

class SIGHT_IO_IGTL_CLASS_API int_converter : public scalar_converter<int,
                                                                      data::integer>
{
public:

    using superclass = scalar_converter<int, data::integer>;
    /// Constructor
    int_converter() :
        superclass("INTEGER")
    {
    }

    /// Destructor
    ~int_converter() override
    = default;

    /**
     * @brief create a new IntConverter smart pointer
     *
     * @return a smart pointer to an IntConverter
     */
    static base::sptr New()
    {
        return std::make_shared<int_converter>();
    }
};

class SIGHT_IO_IGTL_CLASS_API float_converter : public scalar_converter<float,
                                                                        data::real>
{
public:

    using superclass = scalar_converter<float, data::real>;
    /// Constructor
    float_converter() :
        superclass("FLOAT")
    {
    }

    /// Destructor
    ~float_converter() override
    = default;

    /**
     * @brief create a new FloatConverter smart pointer
     *
     * @return a smart pointer to an FloatConverter
     */
    static base::sptr New()
    {
        return std::make_shared<float_converter>();
    }
};

} // namespace sight::io::igtl::detail::converter
