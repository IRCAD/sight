/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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
#include "data/object.hpp"

#include <array>

namespace sight::data
{

/**
 * @brief   This class defines color object.
 */

class DATA_CLASS_API color final : public object
{
public:

    using color_t       = float;
    using color_array_t = std::array<color_t, 4>;

    SIGHT_DECLARE_CLASS(color, object);

    DATA_API color();
    DATA_API color(color_t _red, color_t _green = 1.F, color_t _blue = 1.F, color_t _alpha = 1.F);

    /// Destructor
    DATA_API ~color() noexcept override = default;

    /** Get/Set the array of color values (red, green, blue, alpha).
     *  @name color_array_t accessor
     *  @{  */
    color_array_t& rgba();
    const color_array_t& rgba() const;
    void set_rgba(const color_array_t& _v_rgba);
    /** @} */

    ///@{
    /// Set RGBA color
    DATA_API void set_rgba(
        color_t _red,
        color_t _green,
        color_t _blue,
        color_t _alpha = 1.0
    );

    ///@brief set RGBA from hexadecimal format (\#ffffff)
    ///@param[in] hexaColor c hexadecimal format (\#ffffff)
    DATA_API void set_rgba(const std::string& _hexa_color);
    ///@}

    /** @name color attributes accessor
     *  @brief Get editable reference to color attributes
     *  @{
     */
    DATA_API color_t& red();
    DATA_API color_t& green();
    DATA_API color_t& blue();
    DATA_API color_t& alpha();
    /// @}

    /**@brief Get read-only reference to color attributes
     * @{
     */
    DATA_API const color_t& red() const;
    DATA_API const color_t& green() const;
    DATA_API const color_t& blue() const;
    DATA_API const color_t& alpha() const;
    //@}

    /// Equality comparison operators
    /// @{
    DATA_API bool operator==(const color& _other) const noexcept;
    DATA_API bool operator!=(const color& _other) const noexcept;
    /// @}

    /// Defines shallow copy
    /// @throws data::exception if an errors occurs during copy
    /// @param[in] source the source object to copy
    DATA_API void shallow_copy(const object::csptr& _source) override;

    /// Defines deep copy
    /// @throws data::exception if an errors occurs during copy
    /// @param source source object to copy
    /// @param cache cache used to deduplicate pointers
    DATA_API void deep_copy(
        const object::csptr& _source,
        const std::unique_ptr<deep_copy_cache_t>& _cache = std::make_unique<deep_copy_cache_t>()
    ) override;

protected:

    //! RGBA of the image (in terms of points)
    color_array_t m_v_rgba {};
}; // end class color

//-----------------------------------------------------------------------------

inline color::color_array_t& color::rgba()
{
    return this->m_v_rgba;
}

//-----------------------------------------------------------------------------

inline const color::color_array_t& color::rgba() const
{
    return this->m_v_rgba;
}

//-----------------------------------------------------------------------------

inline void color::set_rgba(const color::color_array_t& _v_rgba)
{
    this->m_v_rgba = _v_rgba;
}

//-----------------------------------------------------------------------------

} // end namespace sight::data
