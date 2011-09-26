/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _SCENE2D_DATA_AXIS_HPP_
#define _SCENE2D_DATA_AXIS_HPP_

#include <fwData/Object.hpp>

#include "scene2D/config.hpp"

namespace scene2D
{
namespace data
{



class SCENE2D_CLASS_API Axis : public ::fwData::Object
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (Axis)(::fwData::Object), (()), ::fwTools::Factory::New< Axis >) ;

    typedef enum
   {
      LINEAR,
      LOG
   }  ScaleType;

    /// Constructor, set origin to 0, scale to 1 and type to LINEAR.
    SCENE2D_API Axis() throw();

    /// Basic destructor, do nothing.
    SCENE2D_API virtual ~Axis() throw();

    /// Get m_origin attribute.
    SCENE2D_API float getOrigin();
    
    /// Set m_origin attribute.
    SCENE2D_API void setOrigin (float _origin);
    
    /// Get m_scale attribute.
    SCENE2D_API float getScale();
    
    /// Set m_scale attribute.
    SCENE2D_API void setScale (float _scale);

    /// Get m_scaleType attribute.
    SCENE2D_API std::string getScaleType();
    
    /// Set m_scaleType attribute.
    SCENE2D_API void setScaleType (std::string _scaleType);

private:
    /// The origin and scale.
    float m_origin, m_scale;

    /// The scaleType (LINEAR/LOG).
    ScaleType m_scaleType;

};


} // namespace data
} // namespace scene2D


#endif // _SCENE2D_DATA_AXIS_HPP_

