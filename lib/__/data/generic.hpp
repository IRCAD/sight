/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include <sight/data/config.hpp>

#include "data/exception.hpp"
#include "data/factory/new.hpp"
#include "data/string_serializable.hpp"

#include <boost/lexical_cast.hpp>

#include <string>

namespace sight::data
{

/* *INDENT-OFF* */
namespace
{

template <typename T>
concept supports_subscript = requires(T a, size_t i) {
    { a[i] };
};
}
/* *INDENT-ON* */

/**
 * @brief   Generic "field" object template.
 *
 * A generic object contains a value.
 *
 * @see     data::real data::real, data::boolean, data::integer, data::vec
 */
template<typename T>
class SIGHT_DATA_CLASS_API generic : public string_serializable
{
public:

    SIGHT_DECLARE_CLASS(generic<T>, string_serializable);

    using value_t = T;

    //------------------------------------------------------------------------------

    generic() noexcept :
        m_value(T())
    {
    }

    generic(const T& _value) noexcept :
        m_value(_value)
    {
    }

    explicit generic(const generic<T>& _other) :
        m_value(_other.value())
    {
    }

    explicit generic(generic<T>&& _other) :
        m_value(_other.value())
    {
    }

    //------------------------------------------------------------------------------

    virtual ~generic() noexcept = default;

    //------------------------------------------------------------------------------

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

    const T& default_value() const
    {
        return m_default_value;
    }

    //------------------------------------------------------------------------------

    void set_default_value() final
    {
        m_default_value = m_value;
    }

    /// Reset to the default value
    void reset() final
    {
        m_value = m_default_value;
    }

    //------------------------------------------------------------------------------

    /// Defines shallow copy
    /// @throws data::exception if an errors occurs during copy
    /// @param[in] _source the source object to copy
    void shallow_copy(const object::csptr& _source) override;

    /// Defines deep copy
    /// @throws data::exception if an errors occurs during copy
    /// @param _source source object to copy
    /// @param _cache cache used to deduplicate pointers
    void deep_copy(
        const object::csptr& _source,
        const std::unique_ptr<deep_copy_cache_t>& _cache = std::make_unique<deep_copy_cache_t>()
    ) override;

    //------------------------------------------------------------------------------

    generic<T>& operator=(const generic<T>& _other);
    generic<T>& operator=(generic<T>&& _other);
    generic<T>& operator=(const T& _other);
    generic<T>& operator=(T&& _other);

    //------------------------------------------------------------------------------

    constexpr auto& operator[](const std::size_t _index) const noexcept requires supports_subscript<T>;
    constexpr auto& operator[](const std::size_t _index) noexcept requires supports_subscript<T>;

    //------------------------------------------------------------------------------

    bool operator==(const generic& _other) const noexcept;
    bool operator<(const generic& _other) const noexcept;
    bool operator>(const generic& _other) const noexcept;

    //------------------------------------------------------------------------------

    inline bool operator<=(const generic& _other) const noexcept
    {
        return !(*this > _other);
    }

    //------------------------------------------------------------------------------

    inline bool operator>=(const generic& _other) const noexcept
    {
        return !(*this < _other);
    }

protected:

    //------------------------------------------------------------------------------

    std::ostream& to_ostream(std::ostream& _os) const override
    {
        return _os << this->to_string();
    }

    /// The stored value.
    T m_value {};

    /// The default value.
    T m_default_value {};
};

//------------------------------------------------------------------------------

template<class T>
constexpr auto& generic<T>::operator[](const std::size_t _index) const noexcept requires supports_subscript<T>
{
    return this->value()[_index];
}

//------------------------------------------------------------------------------

template<class T>
constexpr auto& generic<T>::operator[](const std::size_t _index) noexcept requires supports_subscript<T>
{
    return this->value()[_index];
}

//------------------------------------------------------------------------------

template<typename T>
void generic<T>::shallow_copy(const object::csptr& _source)
{
    const auto& other = std::dynamic_pointer_cast<const generic<T> >(_source);

    SIGHT_THROW_EXCEPTION_IF(
        exception("Unable to copy " + (_source ? _source->classname() : std::string("<NULL>")) + " to " + classname()),
        other == nullptr
    );

    m_value = other->m_value;

    base_class_t::shallow_copy(other);
}

//------------------------------------------------------------------------------

template<typename T>
void generic<T>::deep_copy(const object::csptr& _source, const std::unique_ptr<deep_copy_cache_t>& _cache)
{
    const auto& other = std::dynamic_pointer_cast<const generic<T> >(_source);

    SIGHT_THROW_EXCEPTION_IF(
        exception("Unable to copy " + (_source ? _source->classname() : std::string("<NULL>")) + " to " + classname()),
        other == nullptr
    );

    m_value = other->m_value;

    base_class_t::deep_copy(other, _cache);
}

//------------------------------------------------------------------------------

template<class T>
generic<T>& generic<T>::operator=(const generic<T>& _other)
{
    this->value() = _other.value();
    return *this;
}

//------------------------------------------------------------------------------

template<class T>
generic<T>& generic<T>::operator=(generic<T>&& _other)
{
    this->value() = _other.value();
    return *this;
}

//------------------------------------------------------------------------------

template<class T>
generic<T>& generic<T>::operator=(const T& _other)
{
    this->value() = _other;
    return *this;
}

//------------------------------------------------------------------------------

template<class T>
generic<T>& generic<T>::operator=(T&& _other)
{
    this->value() = _other;
    return *this;
}

//------------------------------------------------------------------------------

template<typename T>
bool generic<T>::operator==(const generic& _other) const noexcept
{
    try
    {
        // Try to cast in the same type
        const auto& other_field = dynamic_cast<const generic<T>&>(_other);

        // If the type is a floating point type
        if constexpr(std::is_floating_point_v<T>)
        {
            // Use our handcrafted comparison (which manage scaled epsilon and NaN / Inf)
            if(!core::is_equal(this->m_value, other_field.m_value))
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
        SIGHT_ASSERT("generic must have same value_t: " << exp.what(), false);
    }

    return false;
}

//------------------------------------------------------------------------------

template<typename T>
bool generic<T>::operator<(const generic& _other) const noexcept
{
    try
    {
        // Try to cast in the same type
        const auto& other_field = dynamic_cast<const generic<T>&>(_other);

        // If the type is a floating point type
        if constexpr(std::is_floating_point_v<T>)
        {
            // Use our handcrafted comparison (which manage scaled epsilon and NaN / Inf)
            if(!core::is_less(this->m_value, other_field.m_value))
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
        SIGHT_ASSERT("generic must have same value_t: " << exp.what(), false);
    }

    return false;
}

//------------------------------------------------------------------------------

template<typename T>
bool generic<T>::operator>(const generic& _other) const noexcept
{
    try
    {
        // Try to cast in the same type
        const auto& other_field = dynamic_cast<const generic<T>&>(_other);

        // If the type is a floating point type
        if constexpr(std::is_floating_point_v<T>)
        {
            // Use our handcrafted comparison (which manage scaled epsilon and NaN / Inf)
            if(!core::is_greater(this->m_value, other_field.m_value))
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
        SIGHT_ASSERT("generic must have same value_t: " << exp.what(), false);
    }

    return false;
}

} // namespace sight::data
