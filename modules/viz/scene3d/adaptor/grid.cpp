/************************************************************************
 *
 * Copyright (C) 2023 IRCAD France
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

#include "modules/viz/scene3d/adaptor/grid.hpp"

#include "modules/viz/scene3d/adaptor/transform.hpp"

#include <core/com/slots.hxx>

#include <data/tools/color.hpp>

#include <viz/scene3d/helper/manual_object.hpp>
#include <viz/scene3d/ogre.hpp>

#include <Ogre.h>
#include <OgreAxisAlignedBox.h>

namespace sight::module::viz::scene3d::adaptor
{

static const core::com::slots::key_t s_UPDATE_LENGTH_SLOT = "updateSize";

//-----------------------------------------------------------------------------

grid::grid() noexcept
{
    new_slot(s_UPDATE_LENGTH_SLOT, &grid::updateSize, this);
}

//-----------------------------------------------------------------------------

void grid::configuring()
{
    this->configureParams();

    const config_t config = this->get_config();

    // parsing transform or create an "empty" one
    this->setTransformId(
        config.get<std::string>(
            sight::viz::scene3d::transformable::s_TRANSFORM_CONFIG,
            this->get_id() + "_transform"
        )
    );

    m_size = config.get<float>(s_CONFIG + "size", m_size);

    const std::string color = config.get(s_CONFIG + "color", "#FFFFFF");
    std::array<std::uint8_t, 4> rgba {};
    data::tools::color::hexaStringToRGBA(color, rgba);
    m_color.r = static_cast<float>(rgba[0]) / 255.F;
    m_color.g = static_cast<float>(rgba[1]) / 255.F;
    m_color.b = static_cast<float>(rgba[2]) / 255.F;
    m_color.a = static_cast<float>(rgba[3]) / 255.F;

    m_dashLength = config.get(s_CONFIG + "dashLength", m_dashLength);
    m_step       = config.get(s_CONFIG + "step", m_step);
    m_elevation  = config.get(s_CONFIG + "elevation", m_elevation);
}

//-----------------------------------------------------------------------------

void grid::starting()
{
    this->initialize();
    this->getRenderService()->makeCurrent();

    Ogre::SceneManager* scene_mgr = this->getSceneManager();

    m_line = scene_mgr->createManualObject(this->get_id() + "_grid");
    // Set the line as dynamic, so we can update it later on, when the length changes
    m_line->setDynamic(true);

    // Set the material
    m_material = std::make_shared<data::material>();

    m_materialAdaptor = this->registerService<module::viz::scene3d::adaptor::material>(
        "sight::module::viz::scene3d::adaptor::material"
    );
    m_materialAdaptor->set_inout(m_material, module::viz::scene3d::adaptor::material::s_MATERIAL_INOUT, true);
    m_materialAdaptor->configure(
        this->get_id() + m_materialAdaptor->get_id(),
        this->get_id() + m_materialAdaptor->get_id(),
        this->getRenderService(),
        m_layerID,
        "ambient"
    );
    m_materialAdaptor->start();

    m_materialAdaptor->getMaterialFw()->setHasVertexColor(true);
    m_materialAdaptor->update();

    // Draw the line
    this->drawGrid(false);

    this->attachNode(m_line);

    this->setVisible(m_isVisible);
}

//-----------------------------------------------------------------------------

void grid::updating()
{
    if(m_isVisible)
    {
        this->getRenderService()->makeCurrent();
        // Draw
        this->drawGrid(true);
    }

    this->requestRender();
}

//-----------------------------------------------------------------------------

void grid::stopping()
{
    this->getRenderService()->makeCurrent();
    this->unregisterServices();
    m_material = nullptr;
    if(m_line != nullptr)
    {
        m_line->detachFromParent();
        this->getSceneManager()->destroyManualObject(m_line);
        m_line = nullptr;
    }
}

//-----------------------------------------------------------------------------

void grid::attachNode(Ogre::MovableObject* _object)
{
    Ogre::SceneNode* root_scene_node = this->getSceneManager()->getRootSceneNode();
    Ogre::SceneNode* trans_node      = this->getOrCreateTransformNode(root_scene_node);
    SIGHT_ASSERT("Transform node shouldn't be null", trans_node);

    trans_node->setVisible(m_isVisible);
    trans_node->attachObject(_object);
}

//-----------------------------------------------------------------------------

void grid::drawGrid(bool _existing_line)
{
    if(!_existing_line)
    {
        m_line->begin(
            m_materialAdaptor->getMaterialName(),
            Ogre::RenderOperation::OT_LINE_LIST,
            sight::viz::scene3d::RESOURCE_GROUP
        );
    }
    else
    {
        m_line->beginUpdate(0);
    }

    const auto half_size = m_size * 0.5F;
    const auto step_size = m_size / static_cast<float>(m_step);

    float x = -half_size;
    for(std::size_t i = 0 ; i <= m_step ; ++i)
    {
        sight::viz::scene3d::helper::manual_object::drawDashedLine(
            m_line,
            Ogre::Vector3(x, m_elevation, -half_size),
            Ogre::Vector3(x, m_elevation, half_size),
            m_dashLength,
            m_dashLength,
            m_color
        );
        x += step_size;
    }

    float y = -half_size;
    for(std::size_t i = 0 ; i <= m_step ; ++i)
    {
        sight::viz::scene3d::helper::manual_object::drawDashedLine(
            m_line,
            Ogre::Vector3(-half_size, m_elevation, y),
            Ogre::Vector3(half_size, m_elevation, y),
            m_dashLength,
            m_dashLength,
            m_color
        );
        y += step_size;
    }

    m_line->end();

    // Set the bounding box
    Ogre::Vector3 bb_min(-half_size, 0.F, -half_size);
    Ogre::Vector3 bb_max(half_size, 0.F, half_size);
    Ogre::AxisAlignedBox box(bb_min, bb_max);
    m_line->setBoundingBox(box);
}

//-----------------------------------------------------------------------------

void grid::setVisible(bool /*_visible*/)
{
    Ogre::SceneNode* root_scene_node = this->getSceneManager()->getRootSceneNode();
    Ogre::SceneNode* trans_node      = this->getOrCreateTransformNode(root_scene_node);
    trans_node->setVisible(m_isVisible);
    this->updating();
}

//-----------------------------------------------------------------------------

void grid::updateSize(float _size)
{
    m_size = _size;
    this->updating();
}

//-----------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.
