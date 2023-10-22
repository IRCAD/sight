/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
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

#include "viz/scene3d/factory/r2vb_renderable.hpp"
#include "viz/scene3d/r2vb_renderable.hpp"

namespace sight::viz::scene3d::factory
{

//-----------------------------------------------------------------------------

Ogre::String r2vb_renderable::FACTORY_TYPE_NAME = "r2vb_renderable";

//-----------------------------------------------------------------------------

const Ogre::String& r2vb_renderable::getType() const
{
    return FACTORY_TYPE_NAME;
}

//-----------------------------------------------------------------------------

Ogre::MovableObject* r2vb_renderable::createInstanceImpl(
    const Ogre::String& _name,
    const Ogre::NameValuePairList* /*params*/
)
{
    return new viz::scene3d::r2vb_renderable(_name);
}

#if OGRE_VERSION_PATCH < 9
//------------------------------------------------------------------------------

void r2vb_renderable::destroyInstance(Ogre::MovableObject* obj)
{
    delete obj;
}
#endif

} // namespace sight::viz::scene3d::factory
