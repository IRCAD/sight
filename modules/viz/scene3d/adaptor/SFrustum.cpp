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

#include "modules/viz/scene3d/adaptor/SFrustum.hpp"

#include "modules/viz/scene3d/adaptor/SCamera.hpp"
#include "modules/viz/scene3d/adaptor/SMaterial.hpp"

#include <core/com/Slots.hxx>

#include <data/Camera.hpp>
#include <data/Material.hpp>

#include <service/macros.hpp>

#include <viz/scene3d/helper/Camera.hpp>
#include <viz/scene3d/helper/Scene.hpp>

#include <OgreCamera.h>
#include <OgreEntity.h>
#include <OgreMaterial.h>
#include <OgreMeshManager.h>

namespace sight::module::viz::scene3d::adaptor
{

static const std::string s_CAMERA_INPUT = "camera";
static const std::string s_NEAR_CONFIG  = "near";
static const std::string s_FAR_CONFIG   = "far";
static const std::string s_COLOR_CONFIG = "color";

//-----------------------------------------------------------------------------

SFrustum::SFrustum() noexcept
{
}

//-----------------------------------------------------------------------------

SFrustum::~SFrustum() noexcept
{
}

//------------------------------------------------------------------------------

void SFrustum::configuring()
{
    this->configureParams();

    const ConfigType configType = this->getConfigTree();
    const ConfigType config     = configType.get_child("config.<xmlattr>");

    this->setTransformId(
        config.get<std::string>(
            sight::viz::scene3d::ITransformable::s_TRANSFORM_CONFIG,
            this->getID() + "_transform"
        )
    );

    m_near  = config.get<float>(s_NEAR_CONFIG, m_near);
    m_far   = config.get<float>(s_FAR_CONFIG, m_far);
    m_color = config.get<std::string>(s_COLOR_CONFIG, m_color);
}

//-----------------------------------------------------------------------------

void SFrustum::starting()
{
    this->initialize();

    // Create material
    m_material = data::Material::New();
    m_material->diffuse()->setRGBA(m_color);

    module::viz::scene3d::adaptor::SMaterial::sptr materialAdaptor =
        this->registerService<module::viz::scene3d::adaptor::SMaterial>(
            "::sight::module::viz::scene3d::adaptor::SMaterial"
        );
    materialAdaptor->setInOut(m_material, module::viz::scene3d::adaptor::SMaterial::s_MATERIAL_INOUT, true);
    materialAdaptor->setID(this->getID() + materialAdaptor->getID());
    materialAdaptor->setMaterialName(this->getID() + materialAdaptor->getID());
    materialAdaptor->setRenderService(this->getRenderService());
    materialAdaptor->setLayerID(this->m_layerID);
    materialAdaptor->setShadingMode("ambient");
    materialAdaptor->setMaterialTemplateName(sight::viz::scene3d::Material::DEFAULT_MATERIAL_TEMPLATE_NAME);
    materialAdaptor->start();
    materialAdaptor->update();

    // Create camera
    m_ogreCamera = this->getSceneManager()->createCamera(::Ogre::String(this->getID() + s_CAMERA_INPUT));
    m_ogreCamera->setMaterial(materialAdaptor->getMaterial());
    m_ogreCamera->setVisible(m_isVisible);

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

    // Set position
    m_ogreCamera->setPosition(::Ogre::Vector3(0, 0, 0));
    m_ogreCamera->setDirection(::Ogre::Vector3(::Ogre::Real(0), ::Ogre::Real(0), ::Ogre::Real(1)));

    // Add camera to ogre scene
    ::Ogre::SceneNode* rootSceneNode = this->getSceneManager()->getRootSceneNode();
    ::Ogre::SceneNode* transNode     = this->getTransformNode(rootSceneNode);
    transNode->attachObject(m_ogreCamera);

    this->requestRender();
}

//-----------------------------------------------------------------------------

service::IService::KeyConnectionsMap SFrustum::getAutoConnections() const
{
    service::IService::KeyConnectionsMap connections;
    connections.push(s_CAMERA_INPUT, data::Camera::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_CAMERA_INPUT, data::Camera::s_INTRINSIC_CALIBRATED_SIG, s_UPDATE_SLOT);

    return connections;
}

//-----------------------------------------------------------------------------

void SFrustum::updating()
{
    this->setOgreCamFromData();
    m_ogreCamera->setDebugDisplayEnabled(m_isVisible);
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
    const auto camera = this->getLockedInput<data::Camera>(s_CAMERA_INPUT);

    if(camera->getIsCalibrated())
    {
        const float width  = static_cast<float>(camera->getWidth());
        const float height = static_cast<float>(camera->getHeight());

        ::Ogre::Matrix4 m =
            sight::viz::scene3d::helper::Camera::computeProjectionMatrix(*camera, width, height, m_near, m_far);

        m_ogreCamera->setCustomProjectionMatrix(true, m);
    }
    else
    {
        SIGHT_WARN("The camera '" + s_CAMERA_INPUT + "' is not calibrated");
    }
}

//-----------------------------------------------------------------------------

void SFrustum::setVisible(bool _isVisible)
{
    m_ogreCamera->setDebugDisplayEnabled(_isVisible);
    this->requestRender();
}

//-----------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.
