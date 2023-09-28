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
#include "data/Object.hpp"

#include <array>

namespace sight::data
{

/**
 * @brief   This class defines color object.
 */

class DATA_CLASS_API Color final : public Object
{
public:

    typedef float ColorType;
    typedef std::array<ColorType, 4> ColorArray;

    SIGHT_DECLARE_CLASS(Color, Object);

    DATA_API Color();
    DATA_API Color(ColorType red, ColorType green = 1.F, ColorType blue = 1.F, ColorType alpha = 1.F);

    /// Destructor
    DATA_API ~Color() noexcept override = default;

    /** Get/Set the array of color values (red, green, blue, alpha).
     *  @name ColorArray accessor
     *  @{  */
    ColorArray& getRGBA();
    const ColorArray& getRGBA() const;
    void setRGBA(const ColorArray& _vRGBA);
    /** @} */

    ///@{
    /// Set RGBA color
    DATA_API void setRGBA(
        ColorType red,
        ColorType green,
        ColorType blue,
        ColorType alpha = 1.0
    );

    ///@brief set RGBA from hexadecimal format (\#ffffff)
    ///@param[in] hexaColor c hexadecimal format (\#ffffff)
    DATA_API void setRGBA(const std::string& hexaColor);
    ///@}

    /** @name color attributes accessor
     *  @brief Get editable reference to color attributes
     *  @{
     */
    DATA_API ColorType& red();
    DATA_API ColorType& green();
    DATA_API ColorType& blue();
    DATA_API ColorType& alpha();
    /// @}

    /**@brief Get read-only reference to color attributes
     * @{
     */
    DATA_API const ColorType& red() const;
    DATA_API const ColorType& green() const;
    DATA_API const ColorType& blue() const;
    DATA_API const ColorType& alpha() const;
    //@}

    /// Equality comparison operators
    /// @{
    DATA_API bool operator==(const Color& other) const noexcept;
    DATA_API bool operator!=(const Color& other) const noexcept;
    /// @}

    /// Defines shallow copy
    /// @throws data::Exception if an errors occurs during copy
    /// @param[in] source the source object to copy
    DATA_API void shallow_copy(const Object::csptr& source) override;

    /// Defines deep copy
    /// @throws data::Exception if an errors occurs during copy
    /// @param source source object to copy
    /// @param cache cache used to deduplicate pointers
    DATA_API void deep_copy(
        const Object::csptr& source,
        const std::unique_ptr<DeepCopyCacheType>& cache = std::make_unique<DeepCopyCacheType>()
    ) override;

protected:

    //! RGBA of the image (in terms of points)
    ColorArray m_vRGBA {};
}; // end class Color

//-----------------------------------------------------------------------------

inline Color::ColorArray& Color::getRGBA()
{
    return this->m_vRGBA;
}

//-----------------------------------------------------------------------------

inline const Color::ColorArray& Color::getRGBA() const
{
    return this->m_vRGBA;
}

//-----------------------------------------------------------------------------

inline void Color::setRGBA(const Color::ColorArray& _vRGBA)
{
    this->m_vRGBA = _vRGBA;
}

//-----------------------------------------------------------------------------

} // end namespace sight::data
