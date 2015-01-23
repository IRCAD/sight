/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _SCENE2D_DATA_VIEWPORT_HPP_
#define _SCENE2D_DATA_VIEWPORT_HPP_

#include <fwData/Object.hpp>

#include "scene2D/config.hpp"

namespace scene2D
{
namespace data
{



/**
* @brief Manage the current viewport of the scene2D.
*/
class SCENE2D_CLASS_API Viewport : public ::fwData::Object
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (Viewport)(::fwData::Object), (()), ::fwData::factory::New< Viewport >) ;

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    SCENE2D_API Viewport( ::fwData::Object::Key key );
    SCENE2D_API virtual ~Viewport();

    /// Defines shallow copy
    SCENE2D_API void shallowCopy( const ::fwData::Object::csptr& _source );

    /// Defines deep copy
    SCENE2D_API void cachedDeepCopy( const ::fwData::Object::csptr& _source, DeepCopyCacheType &cache );

    SCENE2D_API float getX();
    SCENE2D_API void setX (float _x);
    SCENE2D_API float getY();
    SCENE2D_API void setY (float _y);
    SCENE2D_API float getWidth();
    SCENE2D_API void setWidth (float _width);
    SCENE2D_API float getHeight();
    SCENE2D_API void setHeight (float _height);

private:
    float m_x, m_y, m_width, m_height;

};


} // namespace data
} // namespace scene2D


#endif // _SCENE2D_DATA_VIEWPORT_HPP_

