/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "fwData/config.hpp"
#include "fwData/factory/new.hpp"
#include "fwData/Object.hpp"

#include <array>

fwCampAutoDeclareDataMacro((fwData)(Color), FWDATA_API);

namespace fwData
{
/**
 * @brief   This class defines color object.
 */

class FWDATA_CLASS_API Color : public Object
{

public:

    typedef float ColorType;
    typedef std::array<ColorType, 4> ColorArray;

    fwCoreClassMacro(Color, ::fwData::Object, ::fwData::factory::New< Color >);

    fwCampMakeFriendDataMacro((fwData)(Color));

    FWDATA_API static sptr New(ColorType red, ColorType green = 1.f, ColorType blue = 1.f, ColorType alpha = 1.f);

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWDATA_API Color( ::fwData::Object::Key key );

    /// Destructor
    FWDATA_API virtual ~Color();

    /// Defines shallow copy
    FWDATA_API void shallowCopy( const Object::csptr& _source ) override;

    /// Defines deep copy
    FWDATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache) override;

    /** Get/Set the array of color values (red, green, blue, alpha).
     *  @name ColorArray accessor
     *  @{  */
    ColorArray& getRGBA();
    const ColorArray& getRGBA() const;
    void setRGBA(const ColorArray&  _vRGBA);
    /** @} */

    ///@{
    /// Set RGBA color
    FWDATA_API void setRGBA( const ColorType red, const ColorType green, const ColorType blue,
                             const ColorType alpha = 1.0 );

    ///@brief set RGBA from hexadecimal format (\#ffffff)
    ///@param[in] hexaColor c hexadecimal format (\#ffffff)
    FWDATA_API void setRGBA(const std::string& hexaColor );
    ///@}

    /** @name color attributes accessor
     *  @brief Get editable reference to color attributes
     *  @{
     */
    FWDATA_API ColorType& red();
    FWDATA_API ColorType& green();
    FWDATA_API ColorType& blue();
    FWDATA_API ColorType& alpha();
    /// @}

    /**@brief Get read-only reference to color attributes
     * @{
     */
    FWDATA_API const ColorType& red()   const;
    FWDATA_API const ColorType& green() const;
    FWDATA_API const ColorType& blue()  const;
    FWDATA_API const ColorType& alpha() const;
    //@}

protected:

    //! RGBA of the image (in terms of points)
    ColorArray m_vRGBA;

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

inline void Color::setRGBA(const Color::ColorArray&  _vRGBA)
{
    this->m_vRGBA = _vRGBA;
}

//-----------------------------------------------------------------------------

} // end namespace fwData
