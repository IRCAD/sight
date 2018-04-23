/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderOgre/factory/Text.hpp"
#include "fwRenderOgre/Text.hpp"

namespace fwRenderOgre
{

namespace factory
{

//-----------------------------------------------------------------------------

const ::Ogre::String Text::FACTORY_TYPE_NAME = "Text";

//-----------------------------------------------------------------------------

const ::Ogre::String& Text::getType(void) const
{
    return FACTORY_TYPE_NAME;
}

//-----------------------------------------------------------------------------

::Ogre::MovableObject* Text::createInstanceImpl( const ::Ogre::String& name, const ::Ogre::NameValuePairList* )
{
    return new ::fwRenderOgre::Text(name);
}

//-----------------------------------------------------------------------------

void Text::destroyInstance( ::Ogre::MovableObject* obj)
{
    delete obj;
}

} // namespace factory

} // namespace fwRenderOgre
