/************************************************************************
 *
 * Copyright (C) 2018-2019 IRCAD France
 * Copyright (C) 2018-2019 IHU Strasbourg
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

#include "visuOgreAdaptor/SFrustumList.hpp"

#include <arData/Camera.hpp>

#include <fwCom/Slots.hxx>

#include <fwData/mt/ObjectReadLock.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <fwRenderOgre/helper/Camera.hpp>
#include <fwRenderOgre/helper/Scene.hpp>

#include <fwServices/macros.hpp>

#include <OgreCamera.h>
#include <OgreSceneNode.h>

namespace visuOgreAdaptor
{

fwServicesRegisterMacro(::fwRenderOgre::IAdaptor, ::visuOgreAdaptor::SFrustumList);

const ::fwCom::Slots::SlotKeyType SFrustumList::s_CLEAR_SLOT             = "clear";
const ::fwCom::Slots::SlotKeyType SFrustumList::s_UPDATE_VISIBILITY_SLOT = "updateVisibility";
const ::fwCom::Slots::SlotKeyType SFrustumList::s_TOGGLE_VISIBILITY_SLOT = "toggleVisibility";
const ::fwCom::Slots::SlotKeyType SFrustumList::s_ADD_FRUSTUM_SLOT       = "addFrustum";

const std::string s_CAMERA_NAME_INPUT = "camera";
const std::string s_NEAR_CONFIG       = "near";
const std::string s_FAR_CONFIG        = "far";
const std::string s_COLOR_CONFIG      = "color";
const std::string s_TRANSFORM_INPUT   = "transform";
const std::string s_NB_MAX_CONFIG     = "nbMax";

//-----------------------------------------------------------------------------

SFrustumList::SFrustumList() noexcept
{
    newSlot(s_UPDATE_VISIBILITY_SLOT, &SFrustumList::updateVisibility, this);
    newSlot(s_TOGGLE_VISIBILITY_SLOT, &SFrustumList::toggleVisibility, this);
    newSlot(s_CLEAR_SLOT, &SFrustumList::clear, this);
    newSlot(s_ADD_FRUSTUM_SLOT, &SFrustumList::addFrustum, this);
}

//-----------------------------------------------------------------------------

SFrustumList::~SFrustumList() noexcept
{
}

//-----------------------------------------------------------------------------

fwServices::IService::KeyConnectionsMap SFrustumList::getAutoConnections() const
{
    ::fwServices::IService::KeyConnectionsMap connections;
    connections.push(s_TRANSFORM_INPUT, ::fwData::TransformationMatrix3D::s_MODIFIED_SIG, s_ADD_FRUSTUM_SLOT );
    return connections;
}

//-----------------------------------------------------------------------------

void SFrustumList::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfigTree().get_child("config.<xmlattr>");

    this->setTransformId(config.get<std::string>( ::fwRenderOgre::ITransformable::s_TRANSFORM_CONFIG,
                                                  this->getID() + "_transform"));

    m_near     = config.get< float >(s_NEAR_CONFIG, m_near);
    m_far      = config.get< float >(s_FAR_CONFIG, m_far);
    m_color    = config.get< std::string >(s_COLOR_CONFIG, m_color);
    m_capacity = config.get< unsigned int >(s_NB_MAX_CONFIG, m_capacity);
}

//-----------------------------------------------------------------------------

void SFrustumList::starting()
{
    this->initialize();

    m_frustumList.set_capacity(m_capacity);

    // Create material
    m_material = ::fwData::Material::New();
    m_material->diffuse()->setRGBA(m_color);

    m_materialAdaptor = this->registerService< ::visuOgreAdaptor::SMaterial >("::visuOgreAdaptor::SMaterial");
    m_materialAdaptor->registerInOut(m_material, ::visuOgreAdaptor::SMaterial::s_MATERIAL_INOUT, true);
    m_materialAdaptor->setID(this->getID() + "_" + m_materialAdaptor->getID());
    m_materialAdaptor->setMaterialName(this->getID() + "_" + m_materialAdaptor->getID());
    m_materialAdaptor->setRenderService( this->getRenderService() );
    m_materialAdaptor->setLayerID(this->m_layerID);
    m_materialAdaptor->setShadingMode("ambient");
    m_materialAdaptor->setMaterialTemplateName(::fwRenderOgre::Material::DEFAULT_MATERIAL_TEMPLATE_NAME);
    m_materialAdaptor->start();
    m_materialAdaptor->update();

}

//-----------------------------------------------------------------------------

void SFrustumList::updateVisibility(bool isVisible)
{
    m_visibility = isVisible;
    this->updateAllVisibility();
}

//-----------------------------------------------------------------------------

void SFrustumList::toggleVisibility()
{
    m_visibility = !m_visibility;
    this->updateAllVisibility();
}

//-----------------------------------------------------------------------------

void SFrustumList::addFrustum()
{
    //Get camera parameters
    const std::shared_ptr< const ::arData::Camera > fwCamera = this->getInput< ::arData::Camera >(s_CAMERA_NAME_INPUT);
    SLM_ASSERT("Required input '" + s_CAMERA_NAME_INPUT + "' is not set", fwCamera);

    ::Ogre::Camera* camera;
    camera = this->getSceneManager()->createCamera(::Ogre::String(this->getID()+"_camera"
                                                                  + std::to_string(m_currentCamIndex)));
    camera->setMaterial(m_materialAdaptor->getMaterial());
    camera->setVisible(m_visibility);
    camera->setDebugDisplayEnabled(m_visibility);

    // Clipping
    if(m_near != 0.f)
    {
        camera->setNearClipDistance(m_near);
    }
    if(m_far != 0.f)
    {
        camera->setFarClipDistance(m_far);
    }

    // Set data to camera
    const float width  = static_cast< float >(fwCamera->getWidth());
    const float height = static_cast< float >(fwCamera->getHeight());
    ::Ogre::Matrix4 m =
        ::fwRenderOgre::helper::Camera::computeProjectionMatrix(*fwCamera, width, height, m_near, m_far);
    camera->setCustomProjectionMatrix(true, m);

    // Set position
    ::Ogre::Affine3 ogreMat;
    const auto fwTransform = this->getInput< ::fwData::TransformationMatrix3D >(s_TRANSFORM_INPUT);

    // Multithreaded lock
    {
        ::fwData::mt::ObjectReadLock lock(fwTransform);

        for(size_t lt = 0; lt < 4; lt++)
        {
            for(size_t ct = 0; ct < 4; ct++)
            {
                ogreMat[ct][lt] = static_cast< ::Ogre::Real >(fwTransform->getCoefficient(ct, lt));
            }
        }
    }

    // Decompose the matrix
    ::Ogre::Vector3 position;
    ::Ogre::Vector3 scale;
    ::Ogre::Quaternion orientation;
    ogreMat.decomposition(position, scale, orientation);

    const ::Ogre::Quaternion rotateX(::Ogre::Degree(180), ::Ogre::Vector3(1, 0, 0));
    const ::Ogre::Quaternion rotateZ(::Ogre::Degree(180), ::Ogre::Vector3(0, 0, 1));
    orientation = orientation * rotateZ * rotateX;

    camera->setOrientation(orientation);
    camera->setPosition(position);

    if(m_frustumList.full())
    {
        //Remove the oldest one
        this->getSceneManager()->destroyCamera(m_frustumList.back());
    }
    //Add the new one
    m_frustumList.push_front(camera);

    m_currentCamIndex++;

    this->updating();
}

//-----------------------------------------------------------------------------

void SFrustumList::updateAllVisibility()
{
    for(auto camera : m_frustumList)
    {
        camera->setDebugDisplayEnabled(m_visibility);
    }
}

//-----------------------------------------------------------------------------

void SFrustumList::clear()
{
    for(auto camera : m_frustumList)
    {
        this->getSceneManager()->destroyCamera(camera);
    }

    m_frustumList.clear();
}

//-----------------------------------------------------------------------------

void SFrustumList::updating()
{
    ::Ogre::SceneNode* rootSceneNode = this->getSceneManager()->getRootSceneNode();
    m_sceneNode                      = this->getTransformNode(rootSceneNode);
    m_sceneNode->attachObject(m_frustumList.front());

    this->requestRender();
}

//-----------------------------------------------------------------------------

void SFrustumList::stopping()
{
    this->unregisterServices();
    this->clear();
    m_materialAdaptor.reset();
    m_materialAdaptor = nullptr;
    m_material        = nullptr;
}

//-----------------------------------------------------------------------------

}
