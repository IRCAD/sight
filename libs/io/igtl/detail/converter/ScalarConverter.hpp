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

// cspell:ignore NOLINT

#pragma once

#include "io/igtl/detail/converter/base.hpp"
#include "io/igtl/detail/exception/Conversion.hpp"
#include "io/igtl/detail/helper/ScalarToBytes.hpp"
#include "io/igtl/detail/RawMessage.hpp"

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
class IO_IGTL_CLASS_API ScalarConverter : public base
{
public:

    /// Constructor
    ScalarConverter()
    {
        static_assert(std::is_base_of<data::object, fw_data_object_t>::value);
    }

    ScalarConverter(std::string _a) :
        M_IGTL_TYPE(std::move(_a))
    {
        static_assert(std::is_base_of<data::object, fw_data_object_t>::value);
    }

    /// Destructor
    ~ScalarConverter() override
    = default;

    /**
     * @brief convert a igtl::RawMessage(which contain scalar) to a fw_data_object_t
     *
     * @return an data::integer converted from an igtl::RawMessage
     */
    [[nodiscard]] data::object::sptr fromIgtlMessage(const ::igtl::MessageBase::Pointer _src) const override
    {
        auto obj = std::make_shared<fw_data_object_t>();

        RawMessage::Pointer msg = RawMessage::Pointer(dynamic_cast<RawMessage*>(_src.GetPointer()));
        const ScalarType scalar = helper::ScalarToBytes<ScalarType>::fromBytes(msg->getMessage().data());
        obj->setValue(scalar);

        return obj;
    }

    /**
     * @brief convert a data::Scalar to an igtl::RawMessage
     *
     * @return an  igtl::RawMessage converted from an data::integer
     */
    [[nodiscard]] ::igtl::MessageBase::Pointer fromFwDataObject(data::object::csptr _src) const override
    {
        RawMessage::Pointer msg;
        typename fw_data_object_t::csptr obj = std::dynamic_pointer_cast<const fw_data_object_t>(_src);

        msg = RawMessage::New(M_IGTL_TYPE);
        RawMessage::raw_data_t content = helper::ScalarToBytes<ScalarType>::toBytes(ScalarType(obj->getValue()));
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
        return std::make_shared<ScalarConverter<ScalarType, fw_data_object_t> >(_igtl_scalar_type);
    }

    /**
     * @brief get the igtlType supported for conversion
     *
     * @return the igtlType supported for conversion
     */
    [[nodiscard]] const std::string& get_igtl_type() const override
    {
        return M_IGTL_TYPE;
    }

    /**
     * @brief get the fwData object type supported for conversion
     *
     * @return the fwData Object type supported for conversion
     */
    [[nodiscard]] const std::string& getFwDataObjectType() const override
    {
        return fw_data_object_t::classname();
    }

protected:

    /// igtl type supported for conversion
    const std::string M_IGTL_TYPE;
};

class IO_IGTL_CLASS_API IntConverter : public ScalarConverter<int,
                                                              data::integer>
{
public:

    typedef ScalarConverter<int, data::integer> Superclass;
    /// Constructor
    IntConverter() :
        Superclass("INTEGER")
    {
    }

    /// Destructor
    ~IntConverter() override
    = default;

    /**
     * @brief create a new IntConverter smart pointer
     *
     * @return a smart pointer to an IntConverter
     */
    static base::sptr New()
    {
        return std::make_shared<IntConverter>();
    }
};

class IO_IGTL_CLASS_API FloatConverter : public ScalarConverter<float,
                                                                data::real>
{
public:

    typedef ScalarConverter<float, data::real> Superclass;
    /// Constructor
    FloatConverter() :
        Superclass("FLOAT")
    {
    }

    /// Destructor
    ~FloatConverter() override
    = default;

    /**
     * @brief create a new FloatConverter smart pointer
     *
     * @return a smart pointer to an FloatConverter
     */
    static base::sptr New()
    {
        return std::make_shared<FloatConverter>();
    }
};

} // namespace sight::io::igtl::detail::converter
