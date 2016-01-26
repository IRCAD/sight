/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderOgre/ITransformable.hpp"

namespace fwRenderOgre
{

::fwRenderOgre::SRender::OgreObjectIdType ITransformable::getTransformUID() const
{
    return m_transformUID;
}

//------------------------------------------------------------------------------

void ITransformable::setTransformUID(::fwRenderOgre::SRender::OgreObjectIdType newUID)
{
    m_transformUID = newUID;
}

//------------------------------------------------------------------------------

::fwRenderOgre::SRender::OgreObjectIdType ITransformable::getParentTransformUID() const
{
    return m_parentTransformUID;
}

//------------------------------------------------------------------------------

void ITransformable::setParentTransformUID(::fwRenderOgre::SRender::OgreObjectIdType newUID)
{
    m_parentTransformUID = newUID;
}

//-----------------------------------------------------------------------------

} // namespace fwRenderOgre
