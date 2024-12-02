/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include <sight/data/config.hpp>

#include "data/vec.hpp"

namespace sight::data
{

/**
 * @brief   This class defines color object.
 */

class SIGHT_DATA_CLASS_API color final : public sight::data::vec<float,
                                                                 4>
{
public:

    using color_t = float;

    SIGHT_DECLARE_CLASS(color, data::string_serializable);

    SIGHT_DATA_API color();
    SIGHT_DATA_API color(color_t _red, color_t _green = 1.F, color_t _blue = 1.F, color_t _alpha = 1.F);
    SIGHT_DATA_API color(const std::string& _string_color);

    /// Destructor
    SIGHT_DATA_API ~color() noexcept override = default;

    using sight::data::vec<float, 4>::vec;
    using sight::data::vec<float, 4>::operator=;

    /** Get/Set the array of color values (red, green, blue, alpha).
     *  @name array_t accessor
     *  @{  */
    array_t& rgba();
    const array_t& rgba() const;
    void set_rgba(const array_t& _v_rgba);
    /** @} */

    ///@{
    /// Set RGBA color
    SIGHT_DATA_API void set_rgba(
        color_t _red,
        color_t _green,
        color_t _blue,
        color_t _alpha = 1.0
    );
    SIGHT_DATA_API void set_rgba(
        std::uint8_t _red,
        std::uint8_t _green,
        std::uint8_t _blue,
        std::uint8_t _alpha = 255
    );

    //------------------------------------------------------------------------------
    SIGHT_DATA_API std::string to_string() const               final;
    SIGHT_DATA_API void from_string(const std::string& _value) final;

    ///@brief set RGBA from hexadecimal format (\#ffffff)
    ///@param[in] _hexa_color c hexadecimal format (\#ffffff)
    [[deprecated("Use from_string() instead")]] SIGHT_DATA_API void set_rgba(const std::string& _hexa_color);
    ///@}

    /** @name color attributes accessor
     *  @brief Get editable reference to color attributes
     *  @{
     */
    SIGHT_DATA_API color_t& red();
    SIGHT_DATA_API color_t& green();
    SIGHT_DATA_API color_t& blue();
    SIGHT_DATA_API color_t& alpha();
    /// @}

    /**@brief Get read-only reference to color attributes
     * @{
     */
    SIGHT_DATA_API const color_t& red() const;
    SIGHT_DATA_API const color_t& green() const;
    SIGHT_DATA_API const color_t& blue() const;
    SIGHT_DATA_API const color_t& alpha() const;
    //@}
}; // end class color

//-----------------------------------------------------------------------------

inline color::array_t& color::rgba()
{
    return this->value();
}

//-----------------------------------------------------------------------------

inline const color::array_t& color::rgba() const
{
    return this->value();
}

//-----------------------------------------------------------------------------

inline void color::set_rgba(const color::array_t& _v_rgba)
{
    this->value() = _v_rgba;
}

//-----------------------------------------------------------------------------

} // end namespace sight::data
