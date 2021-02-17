/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
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

#pragma once

#include "viz/scene3d/config.hpp"

// used to view the content of the Ogre Compositor Chain
#include <OGRE/OgreCompositorInstance.h>
#include <OGRE/OgrePrerequisites.h>

namespace Ogre
{
class Viewport;
}

namespace sight::viz::scene3d
{
namespace compositor
{

// Here we add the class derivated from the Ogre Listener
class VIZ_SCENE3D_CLASS_API SaoListener : public ::Ogre::CompositorInstance::Listener
{

public:

    VIZ_SCENE3D_API SaoListener() = delete;

    VIZ_SCENE3D_API SaoListener(::Ogre::Viewport* vp) :
        m_viewport(vp)
    {
    }

    /// Method called before a render_target operation involving a material to set dynamically the material parameters.
    VIZ_SCENE3D_API void notifyMaterialRender(::Ogre::uint32 pass_id, ::Ogre::MaterialPtr& mat);

private:

    ::Ogre::Viewport* m_viewport;

};

} // namespace compositor.

} // namespace sight::viz::scene3d.