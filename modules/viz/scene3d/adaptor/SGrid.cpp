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

#include "modules/viz/scene3d/adaptor/SGrid.hpp"

#include "modules/viz/scene3d/adaptor/STransform.hpp"

#include <core/com/slots.hxx>

#include <data/tools/Color.hpp>

#include <viz/scene3d/helper/ManualObject.hpp>
#include <viz/scene3d/ogre.hpp>

#include <Ogre.h>
#include <OgreAxisAlignedBox.h>

namespace sight::module::viz::scene3d::adaptor
{

static const core::com::slots::key_t s_UPDATE_LENGTH_SLOT = "updateSize";

//-----------------------------------------------------------------------------

SGrid::SGrid() noexcept
{
    new_slot(s_UPDATE_LENGTH_SLOT, &SGrid::updateSize, this);
}

//-----------------------------------------------------------------------------

void SGrid::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfiguration();

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
    data::tools::Color::hexaStringToRGBA(color, rgba);
    m_color.r = static_cast<float>(rgba[0]) / 255.F;
    m_color.g = static_cast<float>(rgba[1]) / 255.F;
    m_color.b = static_cast<float>(rgba[2]) / 255.F;
    m_color.a = static_cast<float>(rgba[3]) / 255.F;

    m_dashLength = config.get(s_CONFIG + "dashLength", m_dashLength);
    m_step       = config.get(s_CONFIG + "step", m_step);
    m_elevation  = config.get(s_CONFIG + "elevation", m_elevation);
}

//-----------------------------------------------------------------------------

void SGrid::starting()
{
    this->initialize();
    this->getRenderService()->makeCurrent();

    Ogre::SceneManager* sceneMgr = this->getSceneManager();

    m_line = sceneMgr->createManualObject(this->get_id() + "_grid");
    // Set the line as dynamic, so we can update it later on, when the length changes
    m_line->setDynamic(true);

    // Set the material
    m_material = std::make_shared<data::Material>();

    m_materialAdaptor = this->registerService<module::viz::scene3d::adaptor::SMaterial>(
        "sight::module::viz::scene3d::adaptor::SMaterial"
    );
    m_materialAdaptor->setInOut(m_material, module::viz::scene3d::adaptor::SMaterial::s_MATERIAL_INOUT, true);
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

void SGrid::updating()
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

void SGrid::stopping()
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

void SGrid::attachNode(Ogre::MovableObject* object)
{
    Ogre::SceneNode* rootSceneNode = this->getSceneManager()->getRootSceneNode();
    Ogre::SceneNode* transNode     = this->getOrCreateTransformNode(rootSceneNode);
    SIGHT_ASSERT("Transform node shouldn't be null", transNode);

    transNode->setVisible(m_isVisible);
    transNode->attachObject(object);
}

//-----------------------------------------------------------------------------

void SGrid::drawGrid(bool _existingLine)
{
    if(!_existingLine)
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

    const auto halfSize = m_size * 0.5F;
    const auto stepSize = m_size / static_cast<float>(m_step);

    float x = -halfSize;
    for(std::size_t i = 0 ; i <= m_step ; ++i)
    {
        sight::viz::scene3d::helper::ManualObject::drawDashedLine(
            m_line,
            Ogre::Vector3(x, m_elevation, -halfSize),
            Ogre::Vector3(x, m_elevation, halfSize),
            m_dashLength,
            m_dashLength,
            m_color
        );
        x += stepSize;
    }

    float y = -halfSize;
    for(std::size_t i = 0 ; i <= m_step ; ++i)
    {
        sight::viz::scene3d::helper::ManualObject::drawDashedLine(
            m_line,
            Ogre::Vector3(-halfSize, m_elevation, y),
            Ogre::Vector3(halfSize, m_elevation, y),
            m_dashLength,
            m_dashLength,
            m_color
        );
        y += stepSize;
    }

    m_line->end();

    // Set the bounding box
    Ogre::Vector3 bbMin(-halfSize, 0.F, -halfSize);
    Ogre::Vector3 bbMax(halfSize, 0.F, halfSize);
    Ogre::AxisAlignedBox box(bbMin, bbMax);
    m_line->setBoundingBox(box);
}

//-----------------------------------------------------------------------------

void SGrid::setVisible(bool /*_visible*/)
{
    Ogre::SceneNode* rootSceneNode = this->getSceneManager()->getRootSceneNode();
    Ogre::SceneNode* transNode     = this->getOrCreateTransformNode(rootSceneNode);
    transNode->setVisible(m_isVisible);
    this->updating();
}

//-----------------------------------------------------------------------------

void SGrid::updateSize(float _size)
{
    m_size = _size;
    this->updating();
}

//-----------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.
