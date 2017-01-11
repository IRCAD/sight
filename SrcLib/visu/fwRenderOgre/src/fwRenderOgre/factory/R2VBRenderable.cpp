/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderOgre/factory/R2VBRenderable.hpp"
#include "fwRenderOgre/R2VBRenderable.hpp"

namespace fwRenderOgre
{

namespace factory
{

//-----------------------------------------------------------------------------

::Ogre::String R2VBRenderable::FACTORY_TYPE_NAME = "R2VBRenderable";

//-----------------------------------------------------------------------------

const ::Ogre::String& R2VBRenderable::getType(void) const
{
    return FACTORY_TYPE_NAME;
}

//-----------------------------------------------------------------------------

::Ogre::MovableObject* R2VBRenderable::createInstanceImpl(
    const ::Ogre::String& name, const ::Ogre::NameValuePairList* params)
{
    return new ::fwRenderOgre::R2VBRenderable(name);
}

//-----------------------------------------------------------------------------

void R2VBRenderable::destroyInstance( ::Ogre::MovableObject* obj)
{
    delete obj;
}

} // namespace factory

} // namespace fwRenderOgre
