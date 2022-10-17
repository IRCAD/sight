/************************************************************************
 *
 * Copyright (C) 2014-2022 IRCAD France
 * Copyright (C) 2014-2021 IHU Strasbourg
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

#include "viz/scene3d/factory/R2VBRenderable.hpp"
#include "viz/scene3d/R2VBRenderable.hpp"

namespace sight::viz::scene3d::factory
{

//-----------------------------------------------------------------------------

Ogre::String R2VBRenderable::FACTORY_TYPE_NAME = "R2VBRenderable";

//-----------------------------------------------------------------------------

const Ogre::String& R2VBRenderable::getType() const
{
    return FACTORY_TYPE_NAME;
}

//-----------------------------------------------------------------------------

Ogre::MovableObject* R2VBRenderable::createInstanceImpl(
    const Ogre::String& name,
    const Ogre::NameValuePairList* /*params*/
)
{
    return new viz::scene3d::R2VBRenderable(name);
}

#if OGRE_VERSION_PATCH < 9
//------------------------------------------------------------------------------

void R2VBRenderable::destroyInstance(Ogre::MovableObject* obj)
{
    delete obj;
}
#endif

} // namespace sight::viz::scene3d::factory
