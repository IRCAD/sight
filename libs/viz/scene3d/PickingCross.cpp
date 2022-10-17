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

#include "viz/scene3d/PickingCross.hpp"

#include <viz/scene3d/ogre.hpp>
#include <viz/scene3d/Utils.hpp>

#include <OGRE/OgreMaterialManager.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreSceneNode.h>

namespace sight::viz::scene3d
{

//------------------------------------------------------------------------------

PickingCross::PickingCross(const std::string& _name, Ogre::SceneManager& _sceneManager, Ogre::SceneNode& _parent) :
    m_sceneManager(_sceneManager)
{
    m_cross = _sceneManager.createManualObject(_name +  +"_PickingCross");
    const auto basicAmbientMat = Ogre::MaterialManager::getSingleton().getByName(
        "BasicAmbient",
        sight::viz::scene3d::RESOURCE_GROUP
    );
    auto crossMat = basicAmbientMat->clone(_name + "_CrossMaterial");
    const Ogre::ColourValue yellow(1.F, .75F, 0.F);
    crossMat->setAmbient(yellow);
    crossMat->setDiffuse(yellow);
    crossMat->setDepthCheckEnabled(false);
    m_cross->estimateVertexCount(4);
    m_cross->begin(crossMat, Ogre::RenderOperation::OT_LINE_LIST);
    for(std::uint8_t i = 0 ; i < 4 ; ++i)
    {
        m_cross->position(Ogre::Vector3::ZERO);
    }

    m_cross->end();
    m_cross->setVisible(false);
    // Always render the widget after the plane it is displayed on.
    m_cross->setRenderQueueGroupAndPriority(rq::s_NEGATO_WIDGET_ID, 1);

    _parent.attachObject(m_cross);
}

//------------------------------------------------------------------------------

PickingCross::~PickingCross()
{
    auto crossMtl = m_cross->getSections()[0]->getMaterial();
    Ogre::MaterialManager::getSingleton().remove(crossMtl);

    m_sceneManager.destroyManualObject(m_cross);
}

//------------------------------------------------------------------------------

void PickingCross::update(
    const Ogre::Vector3& _a,
    const Ogre::Vector3& _b,
    const Ogre::Vector3& _c,
    const Ogre::Vector3& _d
)
{
    m_cross->beginUpdate(0);
    m_cross->position(_a);
    m_cross->position(_b);
    m_cross->position(_c);
    m_cross->position(_d);
    m_cross->end();
    m_cross->setVisible(true);
}

} // namespace sight::viz::scene3d
