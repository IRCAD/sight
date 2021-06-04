/************************************************************************
 *
 * Copyright (C) 2017-2021 IRCAD France
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

#include "modules/viz/scene3d/adaptor/SAxis.hpp"

#include "modules/viz/scene3d/adaptor/STransform.hpp"

#include <core/com/Slots.hxx>

#include <data/Color.hpp>

#include <service/macros.hpp>

#include <viz/scene3d/helper/Font.hpp>
#include <viz/scene3d/helper/ManualObject.hpp>
#include <viz/scene3d/helper/Scene.hpp>
#include <viz/scene3d/SRender.hpp>

#include <OgreNode.h>

namespace sight::module::viz::scene3d::adaptor
{

static const std::string s_LENGTH_CONFIG       = "length";
static const std::string s_LABEL_CONFIG        = "label";
static const std::string s_FONT_SIZE_CONFIG    = "fontSize";
static const std::string s_FONT_SOURCE_CONFIG  = "fontSource";
static const std::string s_ORIGIN_CONFIG       = "origin";
static const std::string s_ORIGIN_COLOR_CONFIG = "originColor";

//-----------------------------------------------------------------------------

SAxis::SAxis() noexcept
{
}

//-----------------------------------------------------------------------------

SAxis::~SAxis() noexcept
{
}

//-----------------------------------------------------------------------------

void SAxis::configuring()
{
    this->configureParams();

    const ConfigType configType = this->getConfigTree();
    const ConfigType config     = configType.get_child("config.<xmlattr>");

    const std::string transformId = config.get<std::string>(
        module::viz::scene3d::adaptor::STransform::s_TRANSFORM_CONFIG,
        this->getID() + "_transform"
    );

    this->setTransformId(transformId);

    m_length           = config.get<float>(s_LENGTH_CONFIG, m_length);
    m_enableLabel      = config.get<bool>(s_LABEL_CONFIG, m_enableLabel);
    m_fontSize         = config.get<size_t>(s_FONT_SIZE_CONFIG, m_fontSize);
    m_fontSource       = config.get(s_FONT_SOURCE_CONFIG, m_fontSource);
    m_originVisibility = config.get<bool>(s_ORIGIN_CONFIG, m_originVisibility);

    m_originColor = config.get<std::string>(s_ORIGIN_COLOR_CONFIG, m_originColor);
    SIGHT_ASSERT(
        "Color string should start with '#' and followed by 6 or 8 "
        "hexadecimal digits. Given color: " << m_originColor,
        m_originColor[0] == '#'
        && (m_originColor.length() == 7 || m_originColor.length() == 9)
    );
}

//-----------------------------------------------------------------------------

void SAxis::starting()
{
    this->initialize();

    this->getRenderService()->makeCurrent();

    ::Ogre::SceneNode* const rootSceneNode = this->getSceneManager()->getRootSceneNode();
    ::Ogre::SceneNode* const transformNode = this->getTransformNode(rootSceneNode);
    m_sceneNode = transformNode->createChildSceneNode(this->getID() + "_mainNode");

    ::Ogre::SceneManager* const sceneMgr = this->getSceneManager();

    if(m_originVisibility)
    {
        m_origin = sceneMgr->createManualObject(this->getID() + "_origin");
    }

    m_xLine = sceneMgr->createManualObject(this->getID() + "_xline");
    m_yLine = sceneMgr->createManualObject(this->getID() + "_yline");
    m_zLine = sceneMgr->createManualObject(this->getID() + "_zline");

    m_xCone = sceneMgr->createManualObject(this->getID() + "_xCone");
    m_yCone = sceneMgr->createManualObject(this->getID() + "_yCone");
    m_zCone = sceneMgr->createManualObject(this->getID() + "_zCone");

    // set the material
    m_material = data::Material::New();

    const module::viz::scene3d::adaptor::SMaterial::sptr materialAdaptor =
        this->registerService<module::viz::scene3d::adaptor::SMaterial>(
            "::sight::module::viz::scene3d::adaptor::SMaterial"
        );
    materialAdaptor->registerInOut(m_material, module::viz::scene3d::adaptor::SMaterial::s_MATERIAL_INOUT, true);
    materialAdaptor->setID(this->getID() + materialAdaptor->getID());
    materialAdaptor->setMaterialName(this->getID() + materialAdaptor->getID());
    materialAdaptor->setRenderService(this->getRenderService());
    materialAdaptor->setLayerID(m_layerID);
    materialAdaptor->setMaterialTemplateName(sight::viz::scene3d::Material::DEFAULT_MATERIAL_TEMPLATE_NAME);
    materialAdaptor->start();

    materialAdaptor->getMaterialFw()->setHasVertexColor(true);
    materialAdaptor->update();

    const float dpi = this->getRenderService()->getInteractorManager()->getLogicalDotsPerInch();

    // Sizes
    const float originRadius   = m_length * 0.1f;
    const float cylinderLength = m_length * 0.85f;
    const float cylinderRadius = m_length * 0.025f;
    const float coneLength     = m_length - cylinderLength;
    const float coneRadius     = cylinderRadius * 2.f;
    const unsigned sample      = 64;

    // Draw

    // origin
    if(m_originVisibility)
    {
        const data::Color::sptr originColor = data::Color::New();
        originColor->setRGBA(m_originColor);
        sight::viz::scene3d::helper::ManualObject::createSphere(
            m_origin,
            materialAdaptor->getMaterialName(),
            ::Ogre::ColourValue(
                originColor->red(),
                originColor->green(),
                originColor->blue(),
                originColor->alpha()
            ),
            originRadius,
            sample
        );
        m_sceneNode->attachObject(m_origin);
    }

    // X axis
    sight::viz::scene3d::helper::ManualObject::createCylinder(
        m_xLine,
        materialAdaptor->getMaterialName(),
        ::Ogre::ColourValue(::Ogre::ColourValue::Red),
        cylinderRadius,
        cylinderLength,
        sample
    );
    ::Ogre::SceneNode* const xLineNode = m_sceneNode->createChildSceneNode(this->getID() + "_xLine");
    xLineNode->attachObject(m_xLine);
    xLineNode->pitch(::Ogre::Degree(90));

    // Y axis
    sight::viz::scene3d::helper::ManualObject::createCylinder(
        m_yLine,
        materialAdaptor->getMaterialName(),
        ::Ogre::ColourValue(::Ogre::ColourValue::Green),
        cylinderRadius,
        cylinderLength,
        sample
    );
    ::Ogre::SceneNode* const yLineNode = m_sceneNode->createChildSceneNode(this->getID() + "_yLine");
    yLineNode->attachObject(m_yLine);
    yLineNode->roll(::Ogre::Degree(90));

    // Z axis
    sight::viz::scene3d::helper::ManualObject::createCylinder(
        m_zLine,
        materialAdaptor->getMaterialName(),
        ::Ogre::ColourValue(::Ogre::ColourValue::Blue),
        cylinderRadius,
        cylinderLength,
        sample
    );
    ::Ogre::SceneNode* const zLineNode = m_sceneNode->createChildSceneNode(this->getID() + "_zLine");
    zLineNode->attachObject(m_zLine);
    zLineNode->yaw(::Ogre::Degree(-90));

    ::Ogre::OverlayContainer* const textContainer = this->getLayer()->getOverlayTextPanel();
    ::Ogre::Camera* const cam                     = this->getLayer()->getDefaultCamera();

    // X cone
    sight::viz::scene3d::helper::ManualObject::createCone(
        m_xCone,
        materialAdaptor->getMaterialName(),
        ::Ogre::ColourValue(::Ogre::ColourValue::Red),
        coneRadius,
        coneLength,
        sample
    );
    ::Ogre::SceneNode* const xConeNode = m_sceneNode->createChildSceneNode(this->getID() + "_xCone");

    if(m_enableLabel)
    {
        m_axisLabels[0] = sight::viz::scene3d::Text::New(
            this->getID() + "_xAxisLabel",
            sceneMgr,
            textContainer,
            m_fontSource,
            m_fontSize,
            dpi,
            cam
        );
        m_axisLabels[0]->setText("X");
        xConeNode->attachObject(m_axisLabels[0]);
    }

    xConeNode->attachObject(m_xCone);
    xConeNode->translate(cylinderLength, 0.f, 0.f);

    // Y cone
    sight::viz::scene3d::helper::ManualObject::createCone(
        m_yCone,
        materialAdaptor->getMaterialName(),
        ::Ogre::ColourValue(::Ogre::ColourValue::Green),
        coneRadius,
        coneLength,
        sample
    );
    ::Ogre::SceneNode* const yConeNode = m_sceneNode->createChildSceneNode(this->getID() + "_yCone");
    yConeNode->attachObject(m_yCone);

    if(m_enableLabel)
    {
        m_axisLabels[1] = sight::viz::scene3d::Text::New(
            this->getID() + "_yAxisLabel",
            sceneMgr,
            textContainer,
            m_fontSource,
            m_fontSize,
            dpi,
            cam
        );
        m_axisLabels[1]->setText("Y");
        yConeNode->attachObject(m_axisLabels[1]);
    }

    yConeNode->translate(0.f, cylinderLength, 0.f);
    yConeNode->roll(::Ogre::Degree(90));

    // Z cone
    sight::viz::scene3d::helper::ManualObject::createCone(
        m_zCone,
        materialAdaptor->getMaterialName(),
        ::Ogre::ColourValue(::Ogre::ColourValue::Blue),
        coneRadius,
        coneLength,
        sample
    );
    ::Ogre::SceneNode* const zConeNode = m_sceneNode->createChildSceneNode(this->getID() + "_zCone");
    zConeNode->attachObject(m_zCone);

    if(m_enableLabel)
    {
        m_axisLabels[2] = sight::viz::scene3d::Text::New(
            this->getID() + "_zAxisLabel",
            sceneMgr,
            textContainer,
            m_fontSource,
            m_fontSize,
            dpi,
            cam
        );
        m_axisLabels[2]->setText("Z");
        zConeNode->attachObject(m_axisLabels[2]);
    }

    zConeNode->translate(0.f, 0.f, cylinderLength);
    zConeNode->yaw(::Ogre::Degree(-90));

    this->updateVisibility(m_isVisible);

    this->requestRender();
}

//-----------------------------------------------------------------------------

void SAxis::updating()
{
    this->requestRender();
}

//-----------------------------------------------------------------------------

void SAxis::stopping()
{
    this->getRenderService()->makeCurrent();

    ::Ogre::SceneManager* const sceneMgr = this->getSceneManager();

    if(m_sceneNode != nullptr)
    {
        m_sceneNode->removeAndDestroyChild(this->getID() + "_xLine");
        m_sceneNode->removeAndDestroyChild(this->getID() + "_yLine");
        m_sceneNode->removeAndDestroyChild(this->getID() + "_zLine");
        m_sceneNode->removeAndDestroyChild(this->getID() + "_xCone");
        m_sceneNode->removeAndDestroyChild(this->getID() + "_yCone");
        m_sceneNode->removeAndDestroyChild(this->getID() + "_zCone");
    }

    if(m_enableLabel)
    {
        for(sight::viz::scene3d::Text* label : m_axisLabels)
        {
            SIGHT_ASSERT("label should not be null", label);
            label->detachFromParent();
            sceneMgr->destroyMovableObject(label);
            label = nullptr;
        }
    }

    if(m_originVisibility)
    {
        sceneMgr->destroyManualObject(m_origin);
    }

    sceneMgr->destroyManualObject(m_xLine);
    sceneMgr->destroyManualObject(m_yLine);
    sceneMgr->destroyManualObject(m_zLine);

    sceneMgr->destroyManualObject(m_xCone);
    sceneMgr->destroyManualObject(m_yCone);
    sceneMgr->destroyManualObject(m_zCone);

    ::Ogre::SceneNode* const rootSceneNode = sceneMgr->getRootSceneNode();
    ::Ogre::SceneNode* const transformNode = this->getTransformNode(rootSceneNode);
    transformNode->removeAndDestroyChild(this->getID() + "_mainNode");

    this->unregisterServices();
    m_material.reset();
}

//-----------------------------------------------------------------------------

void SAxis::setVisible(bool _visible)
{
    if(m_sceneNode)
    {
        m_sceneNode->setVisible(_visible);
        if(m_enableLabel)
        {
            for(sight::viz::scene3d::Text* const label : m_axisLabels)
            {
                SIGHT_ASSERT("label should not be null", label);
                label->setVisible(_visible);
            }
        }
    }

    this->updating();
}

//-----------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.
