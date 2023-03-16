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

//-----------------------------------------------------------------------------

SAxis::SAxis() noexcept
{
    newSlot("updateOriginColor", &SAxis::updateOriginColor, this);
}

//-----------------------------------------------------------------------------

void SAxis::updateOriginColor(sight::data::Color::sptr _newColor)
{
    if(m_origin == nullptr)
    {
        return;
    }

    m_originMaterial->setAmbient(_newColor);

    const auto sig = m_originMaterial->signal<core::com::Signal<void()> >(data::Material::s_MODIFIED_SIG);
    sig->asyncEmit();
}

//-----------------------------------------------------------------------------

void SAxis::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfiguration();

    const std::string transformId = config.get<std::string>(
        module::viz::scene3d::adaptor::STransform::s_TRANSFORM_CONFIG,
        this->getID() + "_transform"
    );

    this->setTransformId(transformId);

    static const std::string s_LENGTH_CONFIG       = s_CONFIG + "length";
    static const std::string s_LABEL_CONFIG        = s_CONFIG + "label";
    static const std::string s_FONT_SIZE_CONFIG    = s_CONFIG + "fontSize";
    static const std::string s_FONT_SOURCE_CONFIG  = s_CONFIG + "fontSource";
    static const std::string s_ORIGIN_CONFIG       = s_CONFIG + "origin";
    static const std::string s_AXIS_CONFIG         = s_CONFIG + "axis";
    static const std::string s_ORIGIN_COLOR_CONFIG = s_CONFIG + "originColor";
    static const std::string s_AXIS_NAME           = s_CONFIG + "name";

    m_length           = config.get<float>(s_LENGTH_CONFIG, m_length);
    m_enableLabel      = config.get<bool>(s_LABEL_CONFIG, m_enableLabel);
    m_fontSize         = config.get<std::size_t>(s_FONT_SIZE_CONFIG, m_fontSize);
    m_fontSource       = config.get(s_FONT_SOURCE_CONFIG, m_fontSource);
    m_originVisibility = config.get<bool>(s_ORIGIN_CONFIG, m_originVisibility);
    m_axisVisibility   = config.get<bool>(s_AXIS_CONFIG, m_axisVisibility);
    m_axisName         = config.get<std::string>(s_AXIS_NAME, m_axisName);

    m_originColor = config.get<std::string>(s_ORIGIN_COLOR_CONFIG, m_originColor);
    SIGHT_ASSERT(
        "Color string should start with '#' and followed by 6 or 8 "
        "hexadecimal digits. Given color: " << m_originColor,
        m_originColor[0] == '#'
        && (m_originColor.length() == 7 || m_originColor.length() == 9)
    );

    // Force disable label if axisVisibility is false.
    m_enableLabel = m_axisVisibility ? m_enableLabel : false;
}

//-----------------------------------------------------------------------------

void SAxis::starting()
{
    this->initialize();

    this->getRenderService()->makeCurrent();

    Ogre::SceneNode* const rootSceneNode = this->getSceneManager()->getRootSceneNode();
    Ogre::SceneNode* const transformNode = this->getOrCreateTransformNode(rootSceneNode);
    m_sceneNode = transformNode->createChildSceneNode(this->getID() + "_mainNode");

    Ogre::SceneManager* const sceneMgr = this->getSceneManager();

    if(m_originVisibility)
    {
        m_origin = sceneMgr->createManualObject(this->getID() + "_origin");
    }

    if(m_axisVisibility)
    {
        m_xLine = sceneMgr->createManualObject(this->getID() + "_xLine");
        m_yLine = sceneMgr->createManualObject(this->getID() + "_yLine");
        m_zLine = sceneMgr->createManualObject(this->getID() + "_zLine");

        m_xCone = sceneMgr->createManualObject(this->getID() + "_xCone");
        m_yCone = sceneMgr->createManualObject(this->getID() + "_yCone");
        m_zCone = sceneMgr->createManualObject(this->getID() + "_zCone");
    }

    // set the material
    m_material       = data::Material::New();
    m_originMaterial = data::Material::New();

    const module::viz::scene3d::adaptor::SMaterial::sptr materialAdaptor =
        this->registerService<module::viz::scene3d::adaptor::SMaterial>(
            "sight::module::viz::scene3d::adaptor::SMaterial"
        );
    materialAdaptor->setInOut(m_material, module::viz::scene3d::adaptor::SMaterial::s_MATERIAL_INOUT, true);
    materialAdaptor->configure(
        this->getID() + materialAdaptor->getID(),
        this->getID() + materialAdaptor->getID(),
        this->getRenderService(),
        m_layerID
    );
    materialAdaptor->start();

    materialAdaptor->getMaterialFw()->setHasVertexColor(true);
    materialAdaptor->update();

    const module::viz::scene3d::adaptor::SMaterial::sptr originMaterialAdaptor =
        this->registerService<module::viz::scene3d::adaptor::SMaterial>(
            "sight::module::viz::scene3d::adaptor::SMaterial",
            this->getID() + "_originMaterialAdp"
        );
    originMaterialAdaptor->setInOut(m_originMaterial, module::viz::scene3d::adaptor::SMaterial::s_MATERIAL_INOUT, true);
    originMaterialAdaptor->configure(
        this->getID() + originMaterialAdaptor->getID(),
        this->getID() + originMaterialAdaptor->getID(),
        this->getRenderService(),
        m_layerID,
        "ambient"
    );
    originMaterialAdaptor->start();
    originMaterialAdaptor->update();

    const float dpi = this->getRenderService()->getInteractorManager()->getLogicalDotsPerInch();

    // Sizes
    const float originRadius   = m_length * 0.1F;
    const float cylinderLength = m_length * 0.85F;
    const float cylinderRadius = m_length * 0.025F;
    const float coneLength     = m_length - cylinderLength;
    const float coneRadius     = cylinderRadius * 2.F;
    const unsigned sample      = 64;

    // Draw

    // origin

    if(m_originVisibility)
    {
        const data::Color::sptr originColor = data::Color::New();
        originColor->setRGBA(m_originColor);
        m_originMaterial->setAmbient(originColor);
        m_originMaterial->setDiffuse(data::Color::New(0.F, 0.F, 0.F, 1.F));

        const auto sig = m_originMaterial->signal<core::com::Signal<void()> >(data::Material::s_MODIFIED_SIG);
        sig->asyncEmit();

        sight::viz::scene3d::helper::ManualObject::createSphere(
            m_origin,
            originMaterialAdaptor->getMaterialName(),
            Ogre::ColourValue(
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

    Ogre::OverlayContainer* const textContainer = this->getLayer()->getOverlayTextPanel();
    Ogre::Camera* const cam                     = this->getLayer()->getDefaultCamera();

    if(m_axisVisibility)
    {
        // X axis
        sight::viz::scene3d::helper::ManualObject::createCylinder(
            m_xLine,
            materialAdaptor->getMaterialName(),
            Ogre::ColourValue(Ogre::ColourValue::Red),
            cylinderRadius,
            cylinderLength,
            sample
        );
        Ogre::SceneNode* const xLineNode = m_sceneNode->createChildSceneNode(this->getID() + "_xLine");
        xLineNode->attachObject(m_xLine);
        xLineNode->pitch(Ogre::Degree(90));

        // Y axis
        sight::viz::scene3d::helper::ManualObject::createCylinder(
            m_yLine,
            materialAdaptor->getMaterialName(),
            Ogre::ColourValue(Ogre::ColourValue::Green),
            cylinderRadius,
            cylinderLength,
            sample
        );
        Ogre::SceneNode* const yLineNode = m_sceneNode->createChildSceneNode(this->getID() + "_yLine");
        yLineNode->attachObject(m_yLine);
        yLineNode->roll(Ogre::Degree(90));

        // Z axis
        sight::viz::scene3d::helper::ManualObject::createCylinder(
            m_zLine,
            materialAdaptor->getMaterialName(),
            Ogre::ColourValue(Ogre::ColourValue::Blue),
            cylinderRadius,
            cylinderLength,
            sample
        );
        Ogre::SceneNode* const zLineNode = m_sceneNode->createChildSceneNode(this->getID() + "_zLine");
        zLineNode->attachObject(m_zLine);
        zLineNode->yaw(Ogre::Degree(-90));

        // X cone
        sight::viz::scene3d::helper::ManualObject::createCone(
            m_xCone,
            materialAdaptor->getMaterialName(),
            Ogre::ColourValue(Ogre::ColourValue::Red),
            coneRadius,
            coneLength,
            sample
        );
        Ogre::SceneNode* const xConeNode = m_sceneNode->createChildSceneNode(this->getID() + "_xCone");

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
        xConeNode->translate(cylinderLength, 0.F, 0.F);

        // Y cone
        sight::viz::scene3d::helper::ManualObject::createCone(
            m_yCone,
            materialAdaptor->getMaterialName(),
            Ogre::ColourValue(Ogre::ColourValue::Green),
            coneRadius,
            coneLength,
            sample
        );
        Ogre::SceneNode* const yConeNode = m_sceneNode->createChildSceneNode(this->getID() + "_yCone");
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

        yConeNode->translate(0.F, cylinderLength, 0.F);
        yConeNode->roll(Ogre::Degree(90));

        // Z cone
        sight::viz::scene3d::helper::ManualObject::createCone(
            m_zCone,
            materialAdaptor->getMaterialName(),
            Ogre::ColourValue(Ogre::ColourValue::Blue),
            coneRadius,
            coneLength,
            sample
        );
        Ogre::SceneNode* const zConeNode = m_sceneNode->createChildSceneNode(this->getID() + "_zCone");
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

        zConeNode->translate(0.F, 0.F, cylinderLength);
        zConeNode->yaw(Ogre::Degree(-90));
    }

    // Display Name if provided.
    if(!m_axisName.empty())
    {
        m_axisNameTxt = sight::viz::scene3d::Text::New(
            this->getID() + "_Name",
            sceneMgr,
            textContainer,
            m_fontSource,
            m_fontSize,
            dpi,
            cam
        );

        const data::Color::sptr txtColor = data::Color::New();
        txtColor->setRGBA(m_originColor);

        m_axisNameTxt->setTextColor(
            Ogre::ColourValue(
                txtColor->red(),
                txtColor->green(),
                txtColor->blue()
            )
        );
        m_axisNameTxt->setText(m_axisName);
        m_sceneNode->attachObject(m_axisNameTxt);
    }

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

    Ogre::SceneManager* const sceneMgr = this->getSceneManager();

    if(m_sceneNode != nullptr)
    {
        if(m_axisVisibility)
        {
            m_sceneNode->removeAndDestroyChild(this->getID() + "_xLine");
            m_sceneNode->removeAndDestroyChild(this->getID() + "_yLine");
            m_sceneNode->removeAndDestroyChild(this->getID() + "_zLine");
            m_sceneNode->removeAndDestroyChild(this->getID() + "_xCone");
            m_sceneNode->removeAndDestroyChild(this->getID() + "_yCone");
            m_sceneNode->removeAndDestroyChild(this->getID() + "_zCone");
        }
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

    if(m_axisNameTxt != nullptr)
    {
        m_axisNameTxt->detachFromParent();
        sceneMgr->destroyMovableObject(m_axisNameTxt);
        m_axisNameTxt = nullptr;
    }

    if(m_originVisibility)
    {
        sceneMgr->destroyManualObject(m_origin);
    }

    if(m_axisVisibility)
    {
        sceneMgr->destroyManualObject(m_xLine);
        sceneMgr->destroyManualObject(m_yLine);
        sceneMgr->destroyManualObject(m_zLine);

        sceneMgr->destroyManualObject(m_xCone);
        sceneMgr->destroyManualObject(m_yCone);
        sceneMgr->destroyManualObject(m_zCone);
    }

    Ogre::SceneNode* const transformNode = this->getTransformNode();
    if(transformNode != nullptr)
    {
        transformNode->removeAndDestroyChild(this->getID() + "_mainNode");
    }

    this->unregisterServices();
    m_material.reset();
}

//-----------------------------------------------------------------------------

void SAxis::setVisible(bool _visible)
{
    if(m_sceneNode != nullptr)
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

        if(m_axisNameTxt != nullptr)
        {
            m_axisNameTxt->setVisible(_visible);
        }
    }

    this->updating();
}

//-----------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.
