/************************************************************************
 *
 * Copyright (C) 2017-2019 IRCAD France
 * Copyright (C) 2017-2019 IHU Strasbourg
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

#include "visuOgreAdaptor/SAxis.hpp"

#include "visuOgreAdaptor/STransform.hpp"

#include <fwCom/Slots.hxx>

#include <fwData/Color.hpp>

#include <fwRenderOgre/helper/Font.hpp>
#include <fwRenderOgre/helper/ManualObject.hpp>
#include <fwRenderOgre/helper/Scene.hpp>
#include <fwRenderOgre/SRender.hpp>

#include <fwServices/macros.hpp>

#include <OgreNode.h>

namespace visuOgreAdaptor
{

const ::fwCom::Slots::SlotKeyType s_UPDATE_VISIBILITY_SLOT = "updateVisibility";
const ::fwCom::Slots::SlotKeyType s_TOGGLE_VISIBILITY_SLOT = "toggleVisibility";

fwServicesRegisterMacro(::fwRenderOgre::IAdaptor, ::visuOgreAdaptor::SAxis)

//-----------------------------------------------------------------------------

SAxis::SAxis() noexcept
{
    newSlot(s_UPDATE_VISIBILITY_SLOT, &SAxis::updateVisibility, this);
    newSlot(s_TOGGLE_VISIBILITY_SLOT, &SAxis::toggleVisibility, this);
}

//-----------------------------------------------------------------------------

SAxis::~SAxis() noexcept
{
}

//-----------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap visuOgreAdaptor::SAxis::getAutoConnections() const
{
    ::fwServices::IService::KeyConnectionsMap connections;
    connections.push( ::visuOgreAdaptor::STransform::s_TRANSFORM_CONFIG, ::fwData::Object::s_MODIFIED_SIG,
                      s_UPDATE_SLOT );
    return connections;
}

//-----------------------------------------------------------------------------

void SAxis::configuring()
{
    this->configureParams();

    const ConfigType configType = this->getConfigTree();
    const ConfigType config     = configType.get_child("config.<xmlattr>");

    const std::string transformId = config.get<std::string>(::visuOgreAdaptor::STransform::s_TRANSFORM_CONFIG,
                                                            this->getID() + "_transform");

    this->setTransformId(transformId);

    m_isVisible        = config.get<bool>("visible", m_isVisible);
    m_length           = config.get<float>("length", m_length);
    m_enableLabel      = config.get<bool>("label", m_enableLabel);
    m_fontSize         = config.get<size_t>("fontSize", m_fontSize);
    m_markerVisibility = config.get<bool>("marker", m_markerVisibility);

    m_markerColor = config.get<std::string>("markerColor", m_markerColor);
    OSLM_ASSERT(
        "Color string should start with '#' and followed by 6 ou 8 "
        "hexadecimal digits. Given color : " << m_markerColor,
            m_markerColor[0] == '#'
            && ( m_markerColor.length() == 7 || m_markerColor.length() == 9)
        );
}

//-----------------------------------------------------------------------------

void SAxis::starting()
{
    this->initialize();

    this->getRenderService()->makeCurrent();

    ::Ogre::SceneNode* rootSceneNode = this->getSceneManager()->getRootSceneNode();
    ::Ogre::SceneNode* transformNode = this->getTransformNode(rootSceneNode);
    m_sceneNode                      = transformNode->createChildSceneNode(this->getID() + "_mainNode");

    ::Ogre::SceneManager* sceneMgr = this->getSceneManager();

    if(m_markerVisibility)
    {
        m_marker = sceneMgr->createManualObject(this->getID() + "_marker");
    }

    m_xLine = sceneMgr->createManualObject(this->getID() + "_xline");
    m_yLine = sceneMgr->createManualObject(this->getID() + "_yline");
    m_zLine = sceneMgr->createManualObject(this->getID() + "_zline");

    m_xCone = sceneMgr->createManualObject(this->getID() + "_xCone");
    m_yCone = sceneMgr->createManualObject(this->getID() + "_yCone");
    m_zCone = sceneMgr->createManualObject(this->getID() + "_zCone");

    // set the material
    m_material = ::fwData::Material::New();

    ::visuOgreAdaptor::SMaterial::sptr materialAdaptor = this->registerService< ::visuOgreAdaptor::SMaterial >(
        "::visuOgreAdaptor::SMaterial");
    materialAdaptor->registerInOut(m_material, ::visuOgreAdaptor::SMaterial::s_MATERIAL_INOUT, true);
    materialAdaptor->setID(this->getID() + materialAdaptor->getID());
    materialAdaptor->setMaterialName(this->getID() + materialAdaptor->getID());
    materialAdaptor->setRenderService( this->getRenderService() );
    materialAdaptor->setLayerID(m_layerID);
    materialAdaptor->setMaterialTemplateName(::fwRenderOgre::Material::DEFAULT_MATERIAL_TEMPLATE_NAME);
    materialAdaptor->start();

    materialAdaptor->getMaterialFw()->setHasVertexColor(true);
    materialAdaptor->update();

    const float dpi       = this->getRenderService()->getInteractorManager()->getLogicalDotsPerInch();
    const auto fontSource = "DejaVuSans.ttf";

    // Sizes
    const float markerRadius   = m_length * 0.1f;
    const float cylinderLength = m_length * 0.8f;
    const float cylinderRadius = m_length * 0.025f;
    const float coneLength     = m_length - cylinderLength;
    const float coneRadius     = cylinderRadius*3;
    const unsigned sample      = 64;

    // Draw

    // Marker
    if(m_markerVisibility)
    {
        ::fwData::Color::sptr markerColor = ::fwData::Color::New();
        markerColor->setRGBA(m_markerColor);
        ::fwRenderOgre::helper::ManualObject::createSphere(m_marker, materialAdaptor->getMaterialName(),
                                                           ::Ogre::ColourValue(markerColor->red(), markerColor->green(),
                                                                               markerColor->blue(),
                                                                               markerColor->alpha()),
                                                           markerRadius,
                                                           sample);
        m_sceneNode->attachObject(m_marker);
    }

    // X axis
    ::fwRenderOgre::helper::ManualObject::createCylinder(m_xLine, materialAdaptor->getMaterialName(),
                                                         ::Ogre::ColourValue(::Ogre::ColourValue::Red),
                                                         cylinderRadius,
                                                         cylinderLength,
                                                         sample);
    ::Ogre::SceneNode* xLineNode = m_sceneNode->createChildSceneNode(this->getID() + "_xLine");
    m_xLine->setBoundingBox(::Ogre::AxisAlignedBox(::Ogre::Vector3(0.f, -coneRadius, -coneRadius),
                                                   ::Ogre::Vector3(m_length, coneRadius, coneRadius)));
    xLineNode->attachObject(m_xLine);
    xLineNode->pitch(::Ogre::Degree(90));

    // Y axis
    ::fwRenderOgre::helper::ManualObject::createCylinder(m_yLine,
                                                         materialAdaptor->getMaterialName(),
                                                         ::Ogre::ColourValue(::Ogre::ColourValue::Green),
                                                         cylinderRadius,
                                                         cylinderLength,
                                                         sample);
    ::Ogre::SceneNode* yLineNode = m_sceneNode->createChildSceneNode(this->getID() + "_yLine");
    m_yLine->setBoundingBox(::Ogre::AxisAlignedBox(::Ogre::Vector3(0.f, -coneRadius, -coneRadius),
                                                   ::Ogre::Vector3(m_length, coneRadius, coneRadius)));
    yLineNode->attachObject(m_yLine);
    yLineNode->roll(::Ogre::Degree(90));

    // Z axis
    ::fwRenderOgre::helper::ManualObject::createCylinder(m_zLine,
                                                         materialAdaptor->getMaterialName(),
                                                         ::Ogre::ColourValue(::Ogre::ColourValue::Blue),
                                                         cylinderRadius,
                                                         cylinderLength,
                                                         sample);
    ::Ogre::SceneNode* zLineNode = m_sceneNode->createChildSceneNode(this->getID() + "_zLine");
    m_zLine->setBoundingBox(::Ogre::AxisAlignedBox(::Ogre::Vector3(0.f, -coneRadius, -coneRadius),
                                                   ::Ogre::Vector3(m_length, coneRadius, coneRadius)));
    zLineNode->attachObject(m_zLine);
    zLineNode->yaw(::Ogre::Degree(-90));

    ::Ogre::OverlayContainer* textContainer = this->getLayer()->getOverlayTextPanel();
    ::Ogre::Camera* cam                     = this->getLayer()->getDefaultCamera();

    // X cone
    ::fwRenderOgre::helper::ManualObject::createCone(m_xCone, materialAdaptor->getMaterialName(),
                                                     ::Ogre::ColourValue(::Ogre::ColourValue::Red),
                                                     coneRadius,
                                                     coneLength,
                                                     sample);
    ::Ogre::SceneNode* xConeNode = m_sceneNode->createChildSceneNode(this->getID() + "_xCone");

    if(m_enableLabel)
    {
        m_axisLabels[0] = ::fwRenderOgre::Text::New(
            this->getID() + "_xAxisLabel", sceneMgr, textContainer, fontSource, m_fontSize, dpi, cam);
        m_axisLabels[0]->setText("X");
        xConeNode->attachObject(m_axisLabels[0]);
    }

    xConeNode->attachObject(m_xCone);
    xConeNode->translate(cylinderLength, 0.f, 0.f);

    // Y cone
    ::fwRenderOgre::helper::ManualObject::createCone(m_yCone, materialAdaptor->getMaterialName(),
                                                     ::Ogre::ColourValue(::Ogre::ColourValue::Green),
                                                     coneRadius,
                                                     coneLength,
                                                     sample);
    ::Ogre::SceneNode* yConeNode = m_sceneNode->createChildSceneNode(this->getID() + "_yCone");
    yConeNode->attachObject(m_yCone);

    if(m_enableLabel)
    {
        m_axisLabels[1] = ::fwRenderOgre::Text::New(
            this->getID() + "_yAxisLabel", sceneMgr, textContainer, fontSource, m_fontSize, dpi, cam);
        m_axisLabels[1]->setText("Y");
        yConeNode->attachObject(m_axisLabels[1]);
    }

    yConeNode->translate(0.f, cylinderLength, 0.f);
    yConeNode->roll(::Ogre::Degree(90));

    // Z cone
    ::fwRenderOgre::helper::ManualObject::createCone(m_zCone, materialAdaptor->getMaterialName(),
                                                     ::Ogre::ColourValue(::Ogre::ColourValue::Blue),
                                                     coneRadius,
                                                     coneLength,
                                                     sample);
    ::Ogre::SceneNode* zConeNode = m_sceneNode->createChildSceneNode(this->getID() + "_zCone");
    zConeNode->attachObject(m_zCone);

    if(m_enableLabel)
    {
        m_axisLabels[2] = ::fwRenderOgre::Text::New(
            this->getID() + "_zAxisLabel", sceneMgr, textContainer, fontSource, m_fontSize, dpi, cam);
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

    ::Ogre::SceneManager* sceneMgr = this->getSceneManager();

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
        for(auto& label : m_axisLabels)
        {
            SLM_ASSERT("label should not be null", label);
            label->detachFromParent();
            sceneMgr->destroyMovableObject(label);
            label = nullptr;
        }
    }

    if(m_markerVisibility)
    {
        sceneMgr->destroyManualObject(m_marker);
    }

    sceneMgr->destroyManualObject(m_xLine);
    sceneMgr->destroyManualObject(m_yLine);
    sceneMgr->destroyManualObject(m_zLine);

    sceneMgr->destroyManualObject(m_xCone);
    sceneMgr->destroyManualObject(m_yCone);
    sceneMgr->destroyManualObject(m_zCone);

    ::Ogre::SceneNode* rootSceneNode = sceneMgr->getRootSceneNode();
    ::Ogre::SceneNode* transformNode = this->getTransformNode(rootSceneNode);
    transformNode->removeAndDestroyChild(this->getID() + "_mainNode");

    this->unregisterServices();
    m_material.reset();
}

//-----------------------------------------------------------------------------

void SAxis::updateVisibility(bool isVisible)
{
    m_isVisible = isVisible;

    if(m_sceneNode)
    {
        m_sceneNode->setVisible(m_isVisible);
        if(m_enableLabel)
        {
            for(auto& label : m_axisLabels)
            {
                SLM_ASSERT("label should not be null", label);
                label->setVisible(isVisible);
            }
        }
    }

    this->updating();
}

//------------------------------------------------------------------------------

void SAxis::toggleVisibility()
{
    this->updateVisibility(!m_isVisible);
}

//-----------------------------------------------------------------------------

} //visuOgreAdaptor
