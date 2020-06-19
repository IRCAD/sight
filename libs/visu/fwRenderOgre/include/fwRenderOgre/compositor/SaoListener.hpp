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

#ifndef __FWRENDEROGRE_COMPOSITOR_SAOLISTENER_HPP__
#define __FWRENDEROGRE_COMPOSITOR_SAOLISTENER_HPP__

#include "fwRenderOgre/config.hpp"

// used to view the content of the Ogre Compositor Chain
#include <OGRE/OgreCompositorInstance.h>
#include <OGRE/OgrePrerequisites.h>

namespace Ogre
{
class Viewport;
}

namespace fwRenderOgre
{
namespace compositor
{

// Here we add the class derivated from the Ogre Listener
class FWRENDEROGRE_CLASS_API SaoListener : public ::Ogre::CompositorInstance::Listener
{
public:
    SaoListener() = delete;
    SaoListener(::Ogre::Viewport* vp) :
        m_viewport(vp)
    {
    }

    // method called before a render_target operation involving a material to set dynamically the material parameters
    void notifyMaterialRender(::Ogre::uint32 pass_id, ::Ogre::MaterialPtr& mat);

private:
    ::Ogre::Viewport* m_viewport;
};

} // namespace compositor

}// namespace fwRenderOgre

#endif // __FWRENDEROGRE_COMPOSITOR_SAOLISTENER_HPP__
