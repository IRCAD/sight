/************************************************************************
 *
 * Copyright (C) 2014-2017 IRCAD France
 * Copyright (C) 2014-2017 IHU Strasbourg
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
    const ::Ogre::String& name, const ::Ogre::NameValuePairList* /*params*/)
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
