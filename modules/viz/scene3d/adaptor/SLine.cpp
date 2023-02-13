/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
 * Copyright (C) 2017-2020 IHU Strasbourg
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

#include "modules/viz/scene3d/adaptor/SLine.hpp"

#include "modules/viz/scene3d/adaptor/STransform.hpp"

#include <core/com/Slots.hxx>

#include <data/tools/Color.hpp>

#include <service/macros.hpp>

#include <viz/scene3d/helper/Scene.hpp>
#include <viz/scene3d/ogre.hpp>

#include <Ogre.h>
#include <OgreAxisAlignedBox.h>

namespace sight::module::viz::scene3d::adaptor
{

static const core::com::Slots::SlotKeyType s_UPDATE_LENGTH_SLOT = "updateLength";

//-----------------------------------------------------------------------------

SLine::SLine() noexcept
{
    newSlot(s_UPDATE_LENGTH_SLOT, &SLine::updateLength, this);
}

//-----------------------------------------------------------------------------

SLine::~SLine() noexcept =
    default;

//-----------------------------------------------------------------------------

void SLine::setVisible(bool /*_visible*/)
{
    this->updating();
}

//-----------------------------------------------------------------------------

void SLine::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfiguration();

    // parsing transform or create an "empty" one
    this->setTransformId(
        config.get<std::string>(
            sight::viz::scene3d::ITransformable::s_TRANSFORM_CONFIG,
            this->getID() + "_transform"
        )
    );

    static const std::string s_LENGTH_CONFIG     = s_CONFIG + "length";
    static const std::string s_DASHED_CONFIG     = s_CONFIG + "dashed";
    static const std::string s_DASHLENGTH_CONFIG = s_CONFIG + "dashLength";
    static const std::string s_COLOR_CONFIG      = s_CONFIG + "color";

    m_length = config.get<float>(s_LENGTH_CONFIG, m_length);

    const std::string color = config.get(s_COLOR_CONFIG, "#FFFFFF");
    std::array<std::uint8_t, 4> rgba {};
    data::tools::Color::hexaStringToRGBA(color, rgba);
    m_color.r = static_cast<float>(rgba[0]) / 255.F;
    m_color.g = static_cast<float>(rgba[1]) / 255.F;
    m_color.b = static_cast<float>(rgba[2]) / 255.F;
    m_color.a = static_cast<float>(rgba[3]) / 255.F;

    m_dashed     = config.get(s_DASHED_CONFIG, m_dashed);
    m_dashLength = config.get(s_DASHLENGTH_CONFIG, m_dashLength);
}

//-----------------------------------------------------------------------------

void SLine::starting()
{
    this->initialize();
    this->getRenderService()->makeCurrent();

    Ogre::SceneManager* sceneMgr = this->getSceneManager();

    m_line = sceneMgr->createManualObject(this->getID() + "_line");
    // Set the line as dynamic, so we can update it later on, when the length changes
    m_line->setDynamic(true);

    // Set the material
    m_material = data::Material::New();

    m_materialAdaptor = this->registerService<module::viz::scene3d::adaptor::SMaterial>(
        "sight::module::viz::scene3d::adaptor::SMaterial"
    );
    m_materialAdaptor->setInOut(m_material, module::viz::scene3d::adaptor::SMaterial::s_MATERIAL_INOUT, true);
    m_materialAdaptor->configure(
        this->getID() + m_materialAdaptor->getID(),
        this->getID() + m_materialAdaptor->getID(),
        this->getRenderService(),
        m_layerID,
        "ambient"
    );
    m_materialAdaptor->start();

    m_materialAdaptor->getMaterialFw()->setHasVertexColor(true);
    m_materialAdaptor->update();

    // Draw the line
    this->drawLine(false);

    // Set the bounding box of your Manual Object
    Ogre::Vector3 bbMin(-0.1F, -0.1F, 0.F);
    Ogre::Vector3 bbMax(0.1F, 0.1F, m_length);
    Ogre::AxisAlignedBox box(bbMin, bbMax);
    m_line->setBoundingBox(box);

    this->attachNode(m_line);

    this->requestRender();
}

//-----------------------------------------------------------------------------

void SLine::updating()
{
    Ogre::SceneNode* rootSceneNode = this->getSceneManager()->getRootSceneNode();
    Ogre::SceneNode* transNode     = this->getOrCreateTransformNode(rootSceneNode);
    transNode->setVisible(m_isVisible);
    this->requestRender();
}

//-----------------------------------------------------------------------------

void SLine::stopping()
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

void SLine::attachNode(Ogre::MovableObject* object)
{
    Ogre::SceneNode* rootSceneNode = this->getSceneManager()->getRootSceneNode();
    Ogre::SceneNode* transNode     = this->getOrCreateTransformNode(rootSceneNode);
    SIGHT_ASSERT("Transform node shouldn't be null", transNode);

    transNode->setVisible(m_isVisible);
    transNode->attachObject(object);
}

//-----------------------------------------------------------------------------

void SLine::drawLine(bool _existingLine)
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

    m_line->colour(m_color);

    if(m_dashed)
    {
        float f = 0.F;
        for(std::size_t i = 0 ; i <= static_cast<std::size_t>(m_length / (m_dashLength * 2)) ; i++)
        {
            m_line->position(0, 0, f);
            m_line->position(0, 0, f + m_dashLength);

            f += m_dashLength * 2;
        }
    }
    else
    {
        m_line->position(0, 0, 0);
        m_line->position(0, 0, m_length);
    }

    m_line->end();
}

//-----------------------------------------------------------------------------

void SLine::updateLength(float _length)
{
    this->getRenderService()->makeCurrent();

    m_length = _length;

    // Draw
    this->drawLine(true);

    // Set the bounding box of your Manual Object
    Ogre::Vector3 bbMin(-0.1F, -0.1F, 0.F);
    Ogre::Vector3 bbMax(0.1F, 0.1F, m_length);
    Ogre::AxisAlignedBox box(bbMin, bbMax);
    m_line->setBoundingBox(box);

    this->requestRender();
}

} // namespace sight::module::viz::scene3d::adaptor.
