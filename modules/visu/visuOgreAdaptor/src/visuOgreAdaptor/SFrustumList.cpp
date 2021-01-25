/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
 * Copyright (C) 2018-2020 IHU Strasbourg
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

#include <core/com/Slots.hxx>

#include <data/Camera.hpp>
#include <data/TransformationMatrix3D.hpp>

#include <fwRenderOgre/helper/Camera.hpp>
#include <fwRenderOgre/helper/Scene.hpp>

#include <fwServices/macros.hpp>

#include <OgreCamera.h>
#include <OgreSceneNode.h>

namespace visuOgreAdaptor
{

static const core::com::Slots::SlotKeyType s_CLEAR_SLOT       = "clear";
static const core::com::Slots::SlotKeyType s_ADD_FRUSTUM_SLOT = "addFrustum";

static const std::string s_CAMERA_INPUT    = "camera";
static const std::string s_NEAR_CONFIG     = "near";
static const std::string s_FAR_CONFIG      = "far";
static const std::string s_COLOR_CONFIG    = "color";
static const std::string s_TRANSFORM_INPUT = "transform";
static const std::string s_NB_MAX_CONFIG   = "nbMax";

//-----------------------------------------------------------------------------

SFrustumList::SFrustumList() noexcept
{
    newSlot(s_CLEAR_SLOT, &SFrustumList::clear, this);
    newSlot(s_ADD_FRUSTUM_SLOT, &SFrustumList::addFrustum, this);
}

//-----------------------------------------------------------------------------

SFrustumList::~SFrustumList() noexcept
{
}

//-----------------------------------------------------------------------------

void SFrustumList::configuring()
{
    this->configureParams();

    const ConfigType configType = this->getConfigTree();
    const ConfigType config     = configType.get_child("config.<xmlattr>");

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
    m_material = data::Material::New();
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

fwServices::IService::KeyConnectionsMap SFrustumList::getAutoConnections() const
{
    ::fwServices::IService::KeyConnectionsMap connections;
    connections.push(s_TRANSFORM_INPUT, data::TransformationMatrix3D::s_MODIFIED_SIG, s_ADD_FRUSTUM_SLOT);
    return connections;
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

void SFrustumList::setVisible(bool _visible)
{
    for(auto camera : m_frustumList)
    {
        camera->setDebugDisplayEnabled(_visible);
    }
}

//-----------------------------------------------------------------------------

void SFrustumList::addFrustum()
{
    //Get camera parameters
    const auto cameraData = this->getLockedInput< data::Camera >(s_CAMERA_INPUT);

    ::Ogre::Camera* ogreCamera =
        this->getSceneManager()->createCamera(::Ogre::String(this->getID()+"_camera"
                                                             + std::to_string(m_currentCamIndex)));
    ogreCamera->setMaterial(m_materialAdaptor->getMaterial());
    ogreCamera->setVisible(m_isVisible);
    ogreCamera->setDebugDisplayEnabled(m_isVisible);

    // Clipping
    if(m_near != 0.f)
    {
        ogreCamera->setNearClipDistance(m_near);
    }
    if(m_far != 0.f)
    {
        ogreCamera->setFarClipDistance(m_far);
    }

    // Set data to camera
    const float width  = static_cast< float >(cameraData->getWidth());
    const float height = static_cast< float >(cameraData->getHeight());
    ::Ogre::Matrix4 m =
        ::fwRenderOgre::helper::Camera::computeProjectionMatrix(*cameraData, width, height, m_near, m_far);
    ogreCamera->setCustomProjectionMatrix(true, m);

    // Set position
    ::Ogre::Affine3 ogreMat;
    {
        const auto transform = this->getLockedInput< data::TransformationMatrix3D >(s_TRANSFORM_INPUT);

        for(size_t lt = 0; lt < 4; lt++)
        {
            for(size_t ct = 0; ct < 4; ct++)
            {
                ogreMat[ct][lt] = static_cast< ::Ogre::Real >(transform->getCoefficient(ct, lt));
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

    ogreCamera->setOrientation(orientation);
    ogreCamera->setPosition(position);

    if(m_frustumList.full())
    {
        //Remove the oldest one
        this->getSceneManager()->destroyCamera(m_frustumList.back());
    }
    //Add the new one
    m_frustumList.push_front(ogreCamera);

    m_currentCamIndex++;

    this->updating();
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

} // namespace visuOgreAdaptor.
