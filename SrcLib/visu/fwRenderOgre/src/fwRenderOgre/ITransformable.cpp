/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderOgre/ITransformable.hpp"

namespace fwRenderOgre
{

const std::string ITransformable::s_CONFIG_TRANSFORM = "transform";

//-----------------------------------------------------------------------------

::fwRenderOgre::SRender::OgreObjectIdType ITransformable::getTransformId() const
{
    return m_transformId;
}

//------------------------------------------------------------------------------

void ITransformable::setTransformId(::fwRenderOgre::SRender::OgreObjectIdType newId)
{
    m_transformId = newId;
}

//------------------------------------------------------------------------------

::fwRenderOgre::SRender::OgreObjectIdType ITransformable::getParentTransformId() const
{
    return m_parentTransformId;
}

//------------------------------------------------------------------------------

void ITransformable::setParentTransformId(::fwRenderOgre::SRender::OgreObjectIdType newId)
{
    m_parentTransformId = newId;
}

//-----------------------------------------------------------------------------

} // namespace fwRenderOgre
