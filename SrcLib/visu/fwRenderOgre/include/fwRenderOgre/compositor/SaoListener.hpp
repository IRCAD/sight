/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
