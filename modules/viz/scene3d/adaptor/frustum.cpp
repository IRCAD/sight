/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
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

#include "modules/viz/scene3d/adaptor/frustum.hpp"

#include "modules/viz/scene3d/adaptor/camera.hpp"
#include "modules/viz/scene3d/adaptor/material.hpp"

#include <core/com/slots.hxx>

#include <service/macros.hpp>

#include <viz/scene3d/helper/camera.hpp>
#include <viz/scene3d/helper/ManualObject.hpp>
#include <viz/scene3d/helper/Scene.hpp>

#include <OgreCamera.h>
#include <OgreEntity.h>
#include <OgreMaterial.h>
#include <OgreMeshManager.h>

namespace sight::module::viz::scene3d::adaptor
{

//-----------------------------------------------------------------------------

frustum::frustum() noexcept =
    default;

//-----------------------------------------------------------------------------

frustum::~frustum() noexcept =
    default;

//------------------------------------------------------------------------------

void frustum::configuring()
{
    this->configureParams();

    const config_t config = this->get_config();

    this->setTransformId(
        config.get<std::string>(
            sight::viz::scene3d::transformable::s_TRANSFORM_CONFIG,
            this->get_id() + "_transform"
        )
    );

    static const std::string s_NEAR_CONFIG  = s_CONFIG + "near";
    static const std::string s_FAR_CONFIG   = s_CONFIG + "far";
    static const std::string s_COLOR_CONFIG = s_CONFIG + "color";

    m_near  = config.get<float>(s_NEAR_CONFIG, m_near);
    m_far   = config.get<float>(s_FAR_CONFIG, m_far);
    m_color = config.get<std::string>(s_COLOR_CONFIG, m_color);
}

//-----------------------------------------------------------------------------

void frustum::starting()
{
    this->initialize();

    // Create camera
    m_ogreCamera = this->getSceneManager()->createCamera(Ogre::String(this->get_id() + std::string(s_CAMERA_INPUT)));
    m_ogreCamera->setVisible(m_isVisible);

    // Clipping
    if(m_near != 0.F)
    {
        m_ogreCamera->setNearClipDistance(m_near);
    }

    if(m_far != 0.F)
    {
        m_ogreCamera->setFarClipDistance(m_far);
    }

    // Set data to camera
    this->setOgreCamFromData();

    // Add camera to ogre scene
    Ogre::SceneNode* rootSceneNode = this->getSceneManager()->getRootSceneNode();
    Ogre::SceneNode* transNode     = this->getOrCreateTransformNode(rootSceneNode);
    transNode->attachObject(m_ogreCamera);

    // Set position
    transNode->setPosition(Ogre::Vector3(0, 0, 0));
    transNode->setDirection(Ogre::Vector3(Ogre::Real(0), Ogre::Real(0), Ogre::Real(1)));

    // Create material for the frustum
    m_material = std::make_shared<data::material>();
    m_material->diffuse()->setRGBA(m_color);

    module::viz::scene3d::adaptor::material::sptr materialAdaptor =
        this->registerService<module::viz::scene3d::adaptor::material>(
            "sight::module::viz::scene3d::adaptor::material"
        );
    materialAdaptor->set_inout(m_material, module::viz::scene3d::adaptor::material::s_MATERIAL_INOUT, true);
    materialAdaptor->configure(
        this->get_id() + materialAdaptor->get_id(),
        this->get_id() + materialAdaptor->get_id(),
        this->getRenderService(),
        m_layerID,
        "ambient"
    );
    materialAdaptor->start();
    materialAdaptor->update();

    m_frustum = this->getSceneManager()->createManualObject(this->get_id() + "_frustum");
    sight::viz::scene3d::helper::ManualObject::createFrustum(
        m_frustum,
        materialAdaptor->getMaterialName(),
        *m_ogreCamera
    );
    transNode->attachObject(m_frustum);

    this->requestRender();
}

//-----------------------------------------------------------------------------

service::connections_t frustum::auto_connections() const
{
    service::connections_t connections;
    connections.push(s_CAMERA_INPUT, data::camera::MODIFIED_SIG, service::slots::UPDATE);
    connections.push(s_CAMERA_INPUT, data::camera::INTRINSIC_CALIBRATED_SIG, service::slots::UPDATE);

    return connections;
}

//-----------------------------------------------------------------------------

void frustum::updating()
{
    this->setOgreCamFromData();
    m_frustum->setVisible(m_isVisible);
    this->requestRender();
}

//-----------------------------------------------------------------------------

void frustum::stopping()
{
    this->unregisterServices();

    m_frustum->detachFromParent();
    m_ogreCamera->detachFromParent();
    this->getSceneManager()->destroyManualObject(m_frustum);
    this->getSceneManager()->destroyCamera(m_ogreCamera);

    m_ogreCamera = nullptr;
    m_material   = nullptr;
}

//-----------------------------------------------------------------------------

void frustum::setOgreCamFromData()
{
    const auto camera = m_camera.lock();

    if(camera->getIsCalibrated())
    {
        const auto width  = static_cast<float>(camera->getWidth());
        const auto height = static_cast<float>(camera->getHeight());

        Ogre::Matrix4 m =
            sight::viz::scene3d::helper::camera::computeProjectionMatrix(*camera, width, height, m_near, m_far);

        m_ogreCamera->setCustomProjectionMatrix(true, m);
    }
    else
    {
        SIGHT_WARN("The camera '" + std::string(s_CAMERA_INPUT) + "' is not calibrated");
    }
}

//-----------------------------------------------------------------------------

void frustum::setVisible(bool _isVisible)
{
    m_frustum->setVisible(_isVisible);
    this->requestRender();
}

//-----------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.
