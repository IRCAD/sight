/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>
#include <fwData/registry/macros.hpp>

#include "scene2D/data/Viewport.hpp"

namespace scene2D
{
namespace data
{

fwDataRegisterMacro( ::scene2D::data::Viewport );

Viewport::Viewport(::fwData::Object::Key key)
: m_x (200),
  m_y (200),
  m_width (400),
  m_height (400)
{}

//-----------------------------------------------------------------------------

Viewport::~Viewport()
{
}

//-----------------------------------------------------------------------------

float Viewport::getX()
{
    return m_x;
}

//-----------------------------------------------------------------------------

void Viewport::setX (float _x)
{
    m_x = _x;
}

//-----------------------------------------------------------------------------

float Viewport::getY()
{
    return m_y;
}

//-----------------------------------------------------------------------------

void Viewport::setY (float _y)
{
    m_y = _y;
}

//-----------------------------------------------------------------------------

float Viewport::getWidth()
{
    return m_width;
}

//-----------------------------------------------------------------------------

void Viewport::setWidth (float _width)
{
    m_width = _width;
}

//-----------------------------------------------------------------------------

float Viewport::getHeight()
{
    return m_height;
}

//-----------------------------------------------------------------------------

void Viewport::setHeight (float _height)
{
    m_height = _height;
}

//-----------------------------------------------------------------------------

void Viewport::cachedDeepCopy(const ::fwData::Object::csptr &source, ::fwData::Object::DeepCopyCacheType &cache)
{
    SLM_FATAL("The Viewport::cachedDeepCopy(...) has not been implemented yet.");
}

//-----------------------------------------------------------------------------

} // namespace data
} // namespace scene2D



