/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "data/config.hpp"
#include "data/factory/new.hpp"
#include "data/generic_field_base.hpp"

#include <core/base.hpp>

#include <boost/lexical_cast.hpp>

#include <string>

namespace sight::data
{

/**
 * @brief   Generic "field" object template.
 *
 * A generic object contains a value.
 *
 * @see     data::real data::real, data::boolean, data::integer
 */
template<typename T>
class DATA_CLASS_API generic_field : public generic_field_base
{
public:

    SIGHT_DECLARE_CLASS(generic_field<T>, generic_field_base);

    using value_t = T;

    /// @brief Get the value (mutable version).
    T& value() noexcept
    {
        return m_value;
    }

    /// @brief Get the value (constant version).
    const T& value() const noexcept
    {
        return m_value;
    }

    /// @brief set the value
    void set_value(const T& _new_value) noexcept
    {
        m_value = _new_value;
    }

    /// @brief get the value
    T get_value() const noexcept
    {
        return m_value;
    }

    /// @brief Conversion to a scalar type.
    operator T() noexcept {
        return m_value;
    }

    //------------------------------------------------------------------------------

    inline bool operator==(const generic_field& _other) const noexcept
    {
        try
        {
            // Try to cast in the same type
            const auto& other_field = dynamic_cast<const generic_field<T>&>(_other);

            // If the type is a floating point type
            if constexpr(std::is_floating_point_v<T>)
            {
                // Use our handcrafted comparison (which manage scaled epsilon and NaN / Inf)
                if(!core::tools::is_equal(this->m_value, other_field.m_value))
                {
                    return false;
                }
            }
            else if(!(this->m_value == other_field.m_value))
            {
                return false;
            }

            // Do not forget to call superclass == operator
            // base_class_t have a pure virtual == operator, so we call the base class from base_class_t
            using base = typename base_class_t::base_class_t;
            return base::operator==(_other);
        }
        catch([[maybe_unused]] const std::bad_cast& exp)
        {
            SIGHT_ASSERT("generic_field must have same value_t: " << exp.what(), false);
        }

        return false;
    }

    //------------------------------------------------------------------------------

    inline bool operator<(const generic_field& _other) const noexcept
    {
        try
        {
            // Try to cast in the same type
            const auto& other_field = dynamic_cast<const generic_field<T>&>(_other);

            // If the type is a floating point type
            if constexpr(std::is_floating_point_v<T>)
            {
                // Use our handcrafted comparison (which manage scaled epsilon and NaN / Inf)
                if(!core::tools::is_less(this->m_value, other_field.m_value))
                {
                    return false;
                }
            }
            else if(!(this->m_value < other_field.m_value))
            {
                return false;
            }

            return true;
        }
        catch([[maybe_unused]] const std::bad_cast& exp)
        {
            SIGHT_ASSERT("generic_field must have same value_t: " << exp.what(), false);
        }

        return false;
    }

    //------------------------------------------------------------------------------

    inline bool operator>(const generic_field& _other) const noexcept
    {
        try
        {
            // Try to cast in the same type
            const auto& other_field = dynamic_cast<const generic_field<T>&>(_other);

            // If the type is a floating point type
            if constexpr(std::is_floating_point_v<T>)
            {
                // Use our handcrafted comparison (which manage scaled epsilon and NaN / Inf)
                if(!core::tools::is_greater(this->m_value, other_field.m_value))
                {
                    return false;
                }
            }
            else if(!(this->m_value > other_field.m_value))
            {
                return false;
            }

            return true;
        }
        catch([[maybe_unused]] const std::bad_cast& exp)
        {
            SIGHT_ASSERT("generic_field must have same value_t: " << exp.what(), false);
        }

        return false;
    }

    //------------------------------------------------------------------------------

    inline bool operator<=(const generic_field& _other) const noexcept
    {
        return !(*this > _other);
    }

    //------------------------------------------------------------------------------

    inline bool operator>=(const generic_field& _other) const noexcept
    {
        return !(*this < _other);
    }

    //------------------------------------------------------------------------------

    std::string to_string() const override
    {
        return boost::lexical_cast<std::string>(this->m_value);
    }

    //------------------------------------------------------------------------------

    void from_string(const std::string& _value) override
    {
        this->m_value = boost::lexical_cast<T>(_value);
    }

protected:

    template<typename GT>
    static typename GT::sptr generic_field_factory(typename GT::value_t _value);

    static sptr generic_field_factory(T _value);

    /**
     * @brief Constructor.
     * @param[in] value The initial value.
     */
    generic_field(const T _value = T()) noexcept :
        m_value(_value)
    {
    }

    /**
     * @brief Destructor.
     */
    ~generic_field() noexcept override =
        default;

    //------------------------------------------------------------------------------

    std::ostream& to_o_stream(std::ostream& _os) const override
    {
        return _os << this->value();
    }

    /// @brief The stored value.
    T m_value;
};

//------------------------------------------------------------------------------

template<typename T>
template<typename GT>
typename GT::sptr generic_field<T>::generic_field_factory(const typename GT::value_t _value)
{
    typename GT::sptr field;
    field          = std::make_shared<GT>();
    field->value() = _value;
    return field;
}

//------------------------------------------------------------------------------

template<typename T>
typename generic_field<T>::sptr generic_field<T>::generic_field_factory(const T _value)
{
    typename generic_field<T>::sptr field;
    field          = GenericFieldFactory<generic_field<T> >(_value);
    field->value() = _value;
    return field;
}

} // namespace sight::data
