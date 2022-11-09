/************************************************************************
 *
 * Copyright (C) 2018-2022 IRCAD France
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

#include "modules/viz/scene3d/adaptor/SFrustumList.hpp"

#include <core/com/Slots.hxx>

#include <service/macros.hpp>

#include <viz/scene3d/helper/Camera.hpp>
#include <viz/scene3d/helper/ManualObject.hpp>
#include <viz/scene3d/helper/Scene.hpp>

#include <OgreCamera.h>
#include <OgreSceneNode.h>

namespace sight::module::viz::scene3d::adaptor
{

static const core::com::Slots::SlotKeyType s_CLEAR_SLOT       = "clear";
static const core::com::Slots::SlotKeyType s_ADD_FRUSTUM_SLOT = "addFrustum";

static const std::string s_NEAR_CONFIG   = "near";
static const std::string s_FAR_CONFIG    = "far";
static const std::string s_COLOR_CONFIG  = "color";
static const std::string s_NB_MAX_CONFIG = "nbMax";

//-----------------------------------------------------------------------------

SFrustumList::SFrustumList() noexcept
{
    newSlot(s_CLEAR_SLOT, &SFrustumList::clear, this);
    newSlot(s_ADD_FRUSTUM_SLOT, &SFrustumList::addFrustum, this);
}

//-----------------------------------------------------------------------------

SFrustumList::~SFrustumList() noexcept =
    default;

//-----------------------------------------------------------------------------

void SFrustumList::configuring()
{
    this->configureParams();

    const ConfigType configType = this->getConfiguration();
    const ConfigType config     = configType.get_child("config.<xmlattr>");

    this->setTransformId(
        config.get<std::string>(
            sight::viz::scene3d::ITransformable::s_TRANSFORM_CONFIG,
            this->getID() + "_transform"
        )
    );

    m_near     = config.get<float>(s_NEAR_CONFIG, m_near);
    m_far      = config.get<float>(s_FAR_CONFIG, m_far);
    m_color    = config.get<std::string>(s_COLOR_CONFIG, m_color);
    m_capacity = config.get<unsigned int>(s_NB_MAX_CONFIG, m_capacity);
}

//-----------------------------------------------------------------------------

void SFrustumList::starting()
{
    this->initialize();

    m_frustumList.set_capacity(m_capacity);

    // Create material
    m_material = data::Material::New();
    m_material->diffuse()->setRGBA(m_color);

    m_materialAdaptor = this->registerService<module::viz::scene3d::adaptor::SMaterial>(
        "sight::module::viz::scene3d::adaptor::SMaterial"
    );
    m_materialAdaptor->setInOut(m_material, module::viz::scene3d::adaptor::SMaterial::s_MATERIAL_INOUT, true);
    m_materialAdaptor->setID(this->getID() + "_" + m_materialAdaptor->getID());
    m_materialAdaptor->setMaterialName(this->getID() + "_" + m_materialAdaptor->getID());
    m_materialAdaptor->setRenderService(this->getRenderService());
    m_materialAdaptor->setLayerID(this->m_layerID);
    m_materialAdaptor->setShadingMode("ambient");
    m_materialAdaptor->setMaterialTemplateName(sight::viz::scene3d::Material::DEFAULT_MATERIAL_TEMPLATE_NAME);
    m_materialAdaptor->start();
    m_materialAdaptor->update();
}

//-----------------------------------------------------------------------------

service::IService::KeyConnectionsMap SFrustumList::getAutoConnections() const
{
    service::IService::KeyConnectionsMap connections;
    connections.push(s_TRANSFORM_INPUT, data::Matrix4::s_MODIFIED_SIG, s_ADD_FRUSTUM_SLOT);
    return connections;
}

//-----------------------------------------------------------------------------

void SFrustumList::updating()
{
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
    Ogre::SceneNode* rootSceneNode = this->getSceneManager()->getRootSceneNode();
    Ogre::SceneNode* transNode     = this->getOrCreateTransformNode(rootSceneNode);

    transNode->setVisible(_visible);
}

//-----------------------------------------------------------------------------

void SFrustumList::addFrustum()
{
    //Get camera parameters
    const auto cameraData    = m_camera.lock();
    Ogre::Camera* ogreCamera =
        this->getSceneManager()->createCamera(
            Ogre::String(
                this->getID() + "_camera" + std::to_string(
                    m_currentCamIndex
                )
            )
        );

    Ogre::SceneNode* rootSceneNode = this->getSceneManager()->getRootSceneNode();
    Ogre::SceneNode* transNode     = this->getOrCreateTransformNode(rootSceneNode);
    transNode->attachObject(ogreCamera);

    // Clipping
    if(m_near != 0.F)
    {
        ogreCamera->setNearClipDistance(m_near);
    }

    if(m_far != 0.F)
    {
        ogreCamera->setFarClipDistance(m_far);
    }

    if(cameraData->getIsCalibrated())
    {
        // Set data to camera
        const auto width  = static_cast<float>(cameraData->getWidth());
        const auto height = static_cast<float>(cameraData->getHeight());
        Ogre::Matrix4 m   =
            sight::viz::scene3d::helper::Camera::computeProjectionMatrix(*cameraData, width, height, m_near, m_far);
        ogreCamera->setCustomProjectionMatrix(true, m);

        if(m_frustumList.full())
        {
            //Remove the oldest one
            auto f = m_frustumList.back();

            f.first->detachFromParent();
            this->getSceneManager()->destroyManualObject(f.first);
        }

        auto* const frustum = this->getSceneManager()->createManualObject(
            this->getID() + "_frustum" + std::to_string(
                m_currentCamIndex
            )
        );
        auto* const frustumNode = rootSceneNode->createChildSceneNode("Node_" + std::to_string(m_currentCamIndex));

        sight::viz::scene3d::helper::ManualObject::createFrustum(
            frustum,
            m_materialAdaptor->getMaterialName(),
            *ogreCamera
        );

        this->setTransfromToNode(frustumNode);
        frustumNode->attachObject(frustum);

        //Add the new one
        m_frustumList.push_front({frustum, frustumNode});

        m_currentCamIndex++;

        this->updating();
    }
    else
    {
        SIGHT_ERROR("Camera isn't calibrated");
    }
}

//-----------------------------------------------------------------------------

void SFrustumList::setTransfromToNode(Ogre::SceneNode* _node)
{
    const auto transform = m_transform.lock();
    Ogre::Affine3 ogreMat;

    for(std::size_t lt = 0 ; lt < 4 ; lt++)
    {
        for(std::size_t ct = 0 ; ct < 4 ; ct++)
        {
            ogreMat[ct][lt] = static_cast<Ogre::Real>((*transform)(ct, lt));
        }
    }

    // Decompose the matrix
    Ogre::Vector3 position;
    Ogre::Vector3 scale;
    Ogre::Quaternion orientation;
    ogreMat.decomposition(position, scale, orientation);

    const Ogre::Quaternion rotateX(Ogre::Degree(180), Ogre::Vector3(1, 0, 0));
    const Ogre::Quaternion rotateZ(Ogre::Degree(180), Ogre::Vector3(0, 0, 1));
    orientation = orientation * rotateZ * rotateX;

    _node->setOrientation(orientation);
    _node->setPosition(position);
}

//-----------------------------------------------------------------------------

void SFrustumList::clear()
{
    for(const auto& f : m_frustumList)
    {
        f.first->detachFromParent();
        this->getSceneManager()->destroyManualObject(f.first);
    }

    m_frustumList.clear();
}

//-----------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.
