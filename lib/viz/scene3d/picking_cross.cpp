/************************************************************************
 *
 * Copyright (C) 2022-2023 IRCAD France
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

#include "viz/scene3d/picking_cross.hpp"

#include <viz/scene3d/helper/manual_object.hpp>
#include <viz/scene3d/ogre.hpp>
#include <viz/scene3d/utils.hpp>

#include <OGRE/OgreMaterialManager.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreSceneNode.h>

namespace sight::viz::scene3d
{

//------------------------------------------------------------------------------

picking_cross::picking_cross(const std::string& _name, Ogre::SceneManager& _scene_manager, Ogre::SceneNode& _parent) :
    m_scene_manager(_scene_manager)
{
    m_cross = _scene_manager.createManualObject(_name +  +"_picking_cross");
    const auto basic_ambient_mat = Ogre::MaterialManager::getSingleton().getByName(
        "BasicAmbient",
        sight::viz::scene3d::RESOURCE_GROUP
    );
    auto cross_mat = basic_ambient_mat->clone(_name + "_CrossMaterial");
    const Ogre::ColourValue yellow(1.F, .75F, 0.F, 1.F);
    cross_mat->setAmbient(yellow);
    cross_mat->setDiffuse(yellow);
    cross_mat->setDepthCheckEnabled(false);
    m_cross->estimateVertexCount(4);
    m_cross->begin(cross_mat, Ogre::RenderOperation::OT_LINE_LIST);
    for(std::uint8_t i = 0 ; i < 4 ; ++i)
    {
        m_cross->position(Ogre::Vector3::ZERO);
    }

    m_cross->end();
    m_cross->setVisible(false);
    // Always render the widget after the plane it is displayed on.
    m_cross->setRenderQueueGroupAndPriority(rq::NEGATO_WIDGET_ID, 1);

    _parent.attachObject(m_cross);
}

//------------------------------------------------------------------------------

picking_cross::~picking_cross()
{
    auto cross_mtl = m_cross->getSections()[0]->getMaterial();
    Ogre::MaterialManager::getSingleton().remove(cross_mtl);

    m_scene_manager.destroyManualObject(m_cross);
}

//------------------------------------------------------------------------------

void picking_cross::update(
    const Ogre::Vector3& _a,
    const Ogre::Vector3& _b,
    const Ogre::Vector3& _c,
    const Ogre::Vector3& _d
)
{
    using sight::viz::scene3d::helper::manual_object;
    const auto dash_length  = std::max(_a.distance(_b), _c.distance(_d)) / 400.F;
    const auto dash_spacing = std::max(_a.distance(_b), _c.distance(_d)) / 200.F;
    m_cross->beginUpdate(0);
    manual_object::draw_dashed_line(
        m_cross,
        _a,
        _b,
        dash_length,
        dash_spacing
    );
    manual_object::draw_dashed_line(
        m_cross,
        _c,
        _d,
        dash_length,
        dash_spacing
    );
    m_cross->end();
    m_cross->setVisible(true);
}

} // namespace sight::viz::scene3d
