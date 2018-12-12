/************************************************************************
 *
 * Copyright (C) 2018 IRCAD France
 * Copyright (C) 2018 IHU Strasbourg
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

#include "visuOgreAdaptor/SFrustum.hpp"

#include "visuOgreAdaptor/SCamera.hpp"
#include "visuOgreAdaptor/SMaterial.hpp"

#include <arData/Camera.hpp>

#include <fwCom/Slots.hxx>

#include <fwData/Material.hpp>

#include <fwRenderOgre/helper/Camera.hpp>
#include <fwRenderOgre/helper/Scene.hpp>

#include <fwServices/macros.hpp>

#include <OgreCamera.h>
#include <OgreEntity.h>
#include <OgreMaterial.h>
#include <OgreMeshManager.h>

namespace visuOgreAdaptor
{

fwServicesRegisterMacro(::fwRenderOgre::IAdaptor, ::visuOgreAdaptor::SFrustum);

const ::fwCom::Slots::SlotKeyType SFrustum::s_UPDATE_VISIBILITY_SLOT = "updateVisibility";
const ::fwCom::Slots::SlotKeyType SFrustum::s_TOGGLE_VISIBILITY_SLOT = "toggleVisibility";

const std::string SFrustum::s_CAMERA_INPUT = "camera";
const std::string SFrustum::s_NEAR_CONFIG  = "near";
const std::string SFrustum::s_FAR_CONFIG   = "far";
const std::string SFrustum::s_COLOR_CONFIG = "color";

//-----------------------------------------------------------------------------

SFrustum::SFrustum() noexcept :
    m_ogreCamera(nullptr),
    m_visibility(true),
    m_near(0.f),
    m_far(0.f),
    m_color("#ff0000ff")
{
    newSlot(s_UPDATE_VISIBILITY_SLOT, &SFrustum::updateVisibility, this);
    newSlot(s_TOGGLE_VISIBILITY_SLOT, &SFrustum::toggleVisibility, this);
}

//-----------------------------------------------------------------------------

SFrustum::~SFrustum() noexcept
{
}

//-----------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SFrustum::getAutoConnections() const
{
    ::fwServices::IService::KeyConnectionsMap connections;
    connections.push( s_CAMERA_INPUT, ::fwData::Object::s_MODIFIED_SIG, s_UPDATE_SLOT );
    connections.push( s_CAMERA_INPUT, ::arData::Camera::s_INTRINSIC_CALIBRATED_SIG, s_UPDATE_SLOT );

    return connections;
}

//------------------------------------------------------------------------------

void SFrustum::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfigTree().get_child("config.<xmlattr>");

    this->setTransformId(config.get<std::string>( ::fwRenderOgre::ITransformable::s_TRANSFORM_CONFIG,
                                                  this->getID() + "_transform"));

    m_near  = config.get<float>(s_NEAR_CONFIG, m_near);
    m_far   = config.get<float>(s_FAR_CONFIG, m_far);
    m_color = config.get< std::string >(s_COLOR_CONFIG, m_color);
}

//-----------------------------------------------------------------------------

void SFrustum::starting()
{
    this->initialize();

    // Create material
    m_material = ::fwData::Material::New();
    m_material->diffuse()->setRGBA(m_color);

    ::visuOgreAdaptor::SMaterial::sptr materialAdaptor =
        this->registerService< ::visuOgreAdaptor::SMaterial >("::visuOgreAdaptor::SMaterial");
    materialAdaptor->registerInOut(m_material, ::visuOgreAdaptor::SMaterial::s_MATERIAL_INOUT, true);
    materialAdaptor->setID(this->getID() + materialAdaptor->getID());
    materialAdaptor->setMaterialName(this->getID() + materialAdaptor->getID());
    materialAdaptor->setRenderService( this->getRenderService() );
    materialAdaptor->setLayerID(this->m_layerID);
    materialAdaptor->setShadingMode("ambient");
    materialAdaptor->setMaterialTemplateName(::fwRenderOgre::Material::DEFAULT_MATERIAL_TEMPLATE_NAME);
    materialAdaptor->start();
    materialAdaptor->update();

    // Create camera
    m_ogreCamera = this->getSceneManager()->createCamera(::Ogre::String(this->getID() + s_CAMERA_INPUT));
    m_ogreCamera->setPosition(::Ogre::Vector3(0, 0, 0));
    m_ogreCamera->setMaterial(materialAdaptor->getMaterial());
    m_ogreCamera->setDirection(::Ogre::Vector3(::Ogre::Real(0), ::Ogre::Real(0), ::Ogre::Real(1)));
    m_ogreCamera->setDebugDisplayEnabled(true);

    // Clipping
    if(m_near != 0.f)
    {
        m_ogreCamera->setNearClipDistance(m_near);
    }
    if(m_far != 0.f)
    {
        m_ogreCamera->setFarClipDistance(m_far);
    }

    // Set data to camera
    this->setOgreCamFromData();

    // Add camera to ogre scene
    ::Ogre::SceneNode* rootSceneNode = this->getSceneManager()->getRootSceneNode();
    ::Ogre::SceneNode* transNode     = this->getTransformNode(rootSceneNode);
    transNode->attachObject(m_ogreCamera);

    this->requestRender();
}

//-----------------------------------------------------------------------------

void SFrustum::updating()
{
    this->setOgreCamFromData();
    this->requestRender();
}

//-----------------------------------------------------------------------------

void SFrustum::stopping()
{
    this->unregisterServices();

    m_ogreCamera->detachFromParent();
    this->getSceneManager()->destroyCamera(m_ogreCamera);

    m_ogreCamera = nullptr;
    m_material   = nullptr;
}

//-----------------------------------------------------------------------------

void SFrustum::setOgreCamFromData()
{
    auto camera = this->getInput< ::arData::Camera >(s_CAMERA_INPUT);
    if(camera != nullptr && camera->getIsCalibrated())
    {

        const float width  = static_cast< float >(camera->getWidth());
        const float height = static_cast< float >(camera->getHeight());

        ::Ogre::Matrix4 m =
            ::fwRenderOgre::helper::Camera::computeProjectionMatrix(*camera, width, height, m_near, m_far);

        m_ogreCamera->setCustomProjectionMatrix(true, m);
        m_ogreCamera->setVisible(true);
    }
    else
    {
        SLM_WARN("the input '" + s_CAMERA_INPUT + "' is not set");
    }
}

//-----------------------------------------------------------------------------

void SFrustum::updateVisibility(bool isVisible)
{
    m_visibility = isVisible;
    this->updating();
}

//-----------------------------------------------------------------------------

void SFrustum::toggleVisibility()
{
    m_visibility = !m_visibility;
    this->updating();
}

//-----------------------------------------------------------------------------

} //visuOgreAdaptor
