/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATA_COLOR_HPP__
#define __FWDATA_COLOR_HPP__

#include "fwData/config.hpp"
#include "fwData/Object.hpp"
#include "fwData/factory/new.hpp"

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
    typedef std::array<ColorType,4> ColorArray;

    fwCoreClassDefinitionsWithNFactoriesMacro( (Color)(::fwData::Object),
                                               ((::fwData::factory::New< Color >,() ))
                                                   ((ColorFactory,
                                                     ((ColorType))((ColorType)(1.0)) ((ColorType) (1.0)) ((ColorType)(
                                                                                                              1.0)) ))
                                               );

    fwCampMakeFriendDataMacro((fwData)(Color));

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWDATA_API Color( ::fwData::Object::Key key );

    /// Destructor
    FWDATA_API virtual ~Color();

    /// Defines shallow copy
    FWDATA_API void shallowCopy( const Object::csptr& _source );

    /// Defines deep copy
    FWDATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType &cache);

    /** Get/Set the array of color values (red, green, blue, alpha).
     *  @name ColorArray accessor
     *  @{  */
    const ColorArray getRGBA() const;
    ColorArray& getRefRGBA();
    const ColorArray& getCRefRGBA() const;
    void setRGBA(const ColorArray&  _vRGBA);
    /** @} */

    ///@{
    /// Set RGBA color
    FWDATA_API void setRGBA( const ColorType red, const ColorType green, const ColorType blue,
                             const ColorType alpha = 1.0 );

    ///@brief set RGBA from hexadecimal format (\#ffffff)
    ///@param[in] hexaColor c hexadecimal format (\#ffffff)
    FWDATA_API void setRGBA( std::string hexaColor );
    ///@}

    /** @name color attributes accessor
     *  @brief Get editable reference to color attributes
     *  @{
     */
    FWDATA_API ColorType  &red();
    FWDATA_API ColorType  &green();
    FWDATA_API ColorType  &blue();
    FWDATA_API ColorType  &alpha();
    /// @}

    /**@brief Get read-only reference to color attributes
     * @{
     */
    FWDATA_API const ColorType  &red()   const;
    FWDATA_API const ColorType  &green() const;
    FWDATA_API const ColorType  &blue()  const;
    FWDATA_API const ColorType  &alpha() const;
    //@}

protected:


    FWDATA_API static sptr ColorFactory(ColorType red, ColorType green, ColorType blue, ColorType alpha);

    //! RGBA of the image (in terms of points)
    ColorArray m_vRGBA;

}; // end class Color

//-----------------------------------------------------------------------------

inline const Color::ColorArray Color::getRGBA() const
{
    return m_vRGBA;
}

//-----------------------------------------------------------------------------

inline Color::ColorArray& Color::getRefRGBA()
{
    return this->m_vRGBA;
}

//-----------------------------------------------------------------------------

inline const Color::ColorArray& Color::getCRefRGBA() const
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


#endif // __FWDATA_COLOR_HPP__
