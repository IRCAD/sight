/************************************************************************
 *
 * Copyright (C) 2019 IRCAD France
 * Copyright (C) 2019 IHU Strasbourg
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

#include "visuOgreAdaptor/SVector.hpp"

#include "visuOgreAdaptor/STransform.hpp"

#include <fwCom/Slots.hxx>

#include <fwDataTools/Color.hpp>

#include <fwRenderOgre/helper/Font.hpp>
#include <fwRenderOgre/helper/ManualObject.hpp>
#include <fwRenderOgre/helper/Scene.hpp>
#include <fwRenderOgre/SRender.hpp>

#include <fwServices/macros.hpp>

#include <OgreNode.h>

namespace visuOgreAdaptor
{

const ::fwCom::Slots::SlotKeyType SVector::s_UPDATE_LENGTH_SLOT     = "updateLength";
const ::fwCom::Slots::SlotKeyType SVector::s_UPDATE_VISIBILITY_SLOT = "updateVisibility";
const ::fwCom::Slots::SlotKeyType SVector::s_TOGGLE_VISIBILITY_SLOT = "toggleVisibility";

static const std::string s_COLOR_CONFIG  = "color";
static const std::string s_LENGTH_CONFIG = "length";

fwServicesRegisterMacro(::fwRenderOgre::IAdaptor, ::visuOgreAdaptor::SVector);

//-----------------------------------------------------------------------------

SVector::SVector() noexcept
{
    newSlot(s_UPDATE_VISIBILITY_SLOT, &SVector::updateVisibility, this);
    newSlot(s_TOGGLE_VISIBILITY_SLOT, &SVector::toggleVisibility, this);
    newSlot(s_UPDATE_LENGTH_SLOT, &SVector::updateLength, this);
}

//-----------------------------------------------------------------------------

SVector::~SVector() noexcept
{
}

//-----------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap visuOgreAdaptor::SVector::getAutoConnections() const
{
    ::fwServices::IService::KeyConnectionsMap connections;
    return connections;
}

//-----------------------------------------------------------------------------

void SVector::configuring()
{
    this->configureParams();

    const ConfigType configType = this->getConfigTree();
    const ConfigType config     = configType.get_child("config.<xmlattr>");

    const std::string transformId = config.get<std::string>(::visuOgreAdaptor::STransform::s_TRANSFORM_CONFIG,
                                                            this->getID() + "_transform");

    this->setTransformId(transformId);
    m_length = config.get<float>(s_LENGTH_CONFIG, m_length);
    m_color  = config.get<std::string>(s_COLOR_CONFIG, m_color);
    OSLM_ASSERT(
        "Color string should start with '#' and followed by 6 ou 8 "
        "hexadecimal digits. Given color : " << m_color,
            m_color[0] == '#'
            && ( m_color.length() == 7 || m_color.length() == 9)
        );
}

//-----------------------------------------------------------------------------

void SVector::starting()
{
    this->initialize();

    this->getRenderService()->makeCurrent();

    ::Ogre::SceneNode* rootSceneNode = this->getSceneManager()->getRootSceneNode();
    ::Ogre::SceneNode* transformNode = this->getTransformNode(rootSceneNode);
    m_sceneNode                      = transformNode->createChildSceneNode(this->getID() + "_mainNode");

    // set the material
    m_material = ::fwData::Material::New();

    m_materialAdaptor = this->registerService< ::visuOgreAdaptor::SMaterial >(
        "::visuOgreAdaptor::SMaterial");
    m_materialAdaptor->registerInOut(m_material, ::visuOgreAdaptor::SMaterial::s_MATERIAL_INOUT, true);
    m_materialAdaptor->setID(this->getID() + m_materialAdaptor->getID());
    m_materialAdaptor->setMaterialName(this->getID() + m_materialAdaptor->getID());
    m_materialAdaptor->setRenderService( this->getRenderService() );
    m_materialAdaptor->setLayerID(m_layerID);
    m_materialAdaptor->setMaterialTemplateName(::fwRenderOgre::Material::DEFAULT_MATERIAL_TEMPLATE_NAME);
    m_materialAdaptor->start();

    m_materialAdaptor->getMaterialFw()->setHasVertexColor(true);
    m_materialAdaptor->update();

    this->createVector();

    this->updateVisibility(m_isVisible);

    this->requestRender();
}

//-----------------------------------------------------------------------------

void SVector::updating()
{
    this->getRenderService()->makeCurrent();

    deleteVector();
    createVector();

    this->requestRender();
}

//-----------------------------------------------------------------------------

void SVector::stopping()
{
    this->getRenderService()->makeCurrent();

    this->deleteVector();

    ::Ogre::SceneManager* sceneMgr   = this->getSceneManager();
    ::Ogre::SceneNode* rootSceneNode = sceneMgr->getRootSceneNode();
    ::Ogre::SceneNode* transformNode = this->getTransformNode(rootSceneNode);
    transformNode->removeAndDestroyChild(this->getID() + "_mainNode");

    this->unregisterServices();
    m_material.reset();
}

//-----------------------------------------------------------------------------

void SVector::createVector()
{
    // Size, these value allow to display a vector with good enough ratio.
    const float cylinderLength = m_length - m_length/10;
    const float cylinderRadius = m_length/80;
    const float coneLength     = m_length - cylinderLength;
    const float coneRadius     = cylinderRadius*2;
    const unsigned sample      = 64;

    // Color
    std::uint8_t color[4];
    ::fwDataTools::Color::hexaStringToRGBA(m_color, color);
    ::Ogre::ColourValue ogreColor(color[0] / 255.f, color[1] / 255.f, color[2] / 255.f);

    // Draw
    ::Ogre::SceneManager* sceneMgr = this->getSceneManager();
    m_line                         = sceneMgr->createManualObject(this->getID() + "_line");
    m_cone                         = sceneMgr->createManualObject(this->getID() + "_cone");

    // Line
    ::fwRenderOgre::helper::ManualObject::createCylinder(m_line, m_materialAdaptor->getMaterialName(),
                                                         ogreColor,
                                                         cylinderRadius,
                                                         cylinderLength,
                                                         sample);
    ::Ogre::SceneNode* lineNode = m_sceneNode->createChildSceneNode(this->getID() + "_lineNode");
    m_line->setBoundingBox(::Ogre::AxisAlignedBox(::Ogre::Vector3(0.f, -coneRadius, -coneRadius),
                                                  ::Ogre::Vector3(m_length, coneRadius, coneRadius)));
    lineNode->attachObject(m_line);
    // Rotate around y axis to create the cylinder on z Axis (consistent with SLine adaptor)
    lineNode->yaw(::Ogre::Degree(-90));

    // Cone
    ::fwRenderOgre::helper::ManualObject::createCone(m_cone, m_materialAdaptor->getMaterialName(),
                                                     ogreColor,
                                                     coneRadius,
                                                     coneLength,
                                                     sample);
    ::Ogre::SceneNode* coneNode = m_sceneNode->createChildSceneNode(this->getID() + "_coneNode");

    coneNode->attachObject(m_cone);
    coneNode->translate(0.f, 0.f, cylinderLength);
    coneNode->yaw(::Ogre::Degree(-90));

}

//-----------------------------------------------------------------------------

void SVector::deleteVector()
{
    if(m_sceneNode != nullptr)
    {
        m_sceneNode->removeAndDestroyChild(this->getID() + "_lineNode");
        m_sceneNode->removeAndDestroyChild(this->getID() + "_coneNode");
    }

    ::Ogre::SceneManager* sceneMgr = this->getSceneManager();

    sceneMgr->destroyManualObject(m_line);
    sceneMgr->destroyManualObject(m_cone);
}

//-----------------------------------------------------------------------------

void SVector::updateVisibility(bool _isVisible)
{
    m_isVisible = _isVisible;

    if(m_sceneNode)
    {
        m_sceneNode->setVisible(m_isVisible);
    }

    this->updating();
}

//------------------------------------------------------------------------------

void SVector::toggleVisibility()
{
    this->updateVisibility(!m_isVisible);
}

//-----------------------------------------------------------------------------

void SVector::updateLength(float _length)
{
    m_length = _length;

    this->update();
}

//-----------------------------------------------------------------------------

} //visuOgreAdaptor
