/************************************************************************
 *
 * Copyright (C) 2022 IRCAD France
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

#include <OGRE/OgreManualObject.h>

#include <OgrePrerequisites.h>

#include <string>
namespace Ogre
{

class SceneNode;
class SceneManager;

} // namespace Ogre

namespace sight::viz::scene3d
{

/**
 * @brief Cross displayed on top of negatoscope planes. It is made of two line segments, normally perpendicular.
 */
class VIZ_SCENE3D_CLASS_API PickingCross
{
public:

    using sptr = std::shared_ptr<PickingCross>;

    /**
     * @brief Creates a across, instantiates its material. The cross is not visible when created, call update() to set
     * the coordinates of its two segments.
     * @param _name unique identifier of the scene node.
     * @param _sceneManager the Ogre scene manager.
     * @param _parent parent node where attach the plane.
     */
    VIZ_SCENE3D_API PickingCross(const std::string& _name, Ogre::SceneManager& _sceneManager, Ogre::SceneNode& _parent);

    /// Cleans ogre resources.
    VIZ_SCENE3D_API ~PickingCross();

    /// Update the coordinates of the cross. The first segment 1 is defined by [a;b] and the second segment by [c;d].
    VIZ_SCENE3D_API void update(
        const Ogre::Vector3& _a,
        const Ogre::Vector3& _b,
        const Ogre::Vector3& _c,
        const Ogre::Vector3& _d
    );

    /**
     * @brief Shows/hides the cross in the scene.
     * @param _visible use true to show the cross.
     */
    void setVisible(bool _visible);

private:

    /// Contains the cross
    Ogre::ManualObject* m_cross;
    /// Parent scene manager
    Ogre::SceneManager& m_sceneManager;
};

//------------------------------------------------------------------------------

inline void PickingCross::setVisible(bool _visible)
{
    m_cross->setVisible(_visible);
}

} // namespace sight::viz::scene3d
