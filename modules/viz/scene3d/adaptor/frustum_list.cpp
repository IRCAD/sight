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

#include "modules/viz/scene3d/adaptor/frustum_list.hpp"

#include <core/com/slots.hxx>

#include <service/macros.hpp>

#include <viz/scene3d/helper/camera.hpp>
#include <viz/scene3d/helper/manual_object.hpp>
#include <viz/scene3d/helper/scene.hpp>

#include <OgreCamera.h>
#include <OgreSceneNode.h>

namespace sight::module::viz::scene3d::adaptor
{

static const core::com::slots::key_t CLEAR_SLOT       = "clear";
static const core::com::slots::key_t ADD_FRUSTUM_SLOT = "addFrustum";

//-----------------------------------------------------------------------------

frustum_list::frustum_list() noexcept
{
    new_slot(CLEAR_SLOT, &frustum_list::clear, this);
    new_slot(ADD_FRUSTUM_SLOT, &frustum_list::addFrustum, this);
}

//-----------------------------------------------------------------------------

frustum_list::~frustum_list() noexcept =
    default;

//-----------------------------------------------------------------------------

void frustum_list::configuring()
{
    this->configureParams();

    const config_t config = this->get_config();

    this->setTransformId(
        config.get<std::string>(
            sight::viz::scene3d::transformable::s_TRANSFORM_CONFIG,
            this->get_id() + "_transform"
        )
    );

    static const std::string s_NEAR_CONFIG   = s_CONFIG + "near";
    static const std::string s_FAR_CONFIG    = s_CONFIG + "far";
    static const std::string s_COLOR_CONFIG  = s_CONFIG + "color";
    static const std::string s_NB_MAX_CONFIG = s_CONFIG + "nbMax";

    m_near     = config.get<float>(s_NEAR_CONFIG, m_near);
    m_far      = config.get<float>(s_FAR_CONFIG, m_far);
    m_color    = config.get<std::string>(s_COLOR_CONFIG, m_color);
    m_capacity = config.get<unsigned int>(s_NB_MAX_CONFIG, m_capacity);
}

//-----------------------------------------------------------------------------

void frustum_list::starting()
{
    this->initialize();

    m_frustumList.set_capacity(m_capacity);

    // Create material
    m_material = std::make_shared<data::material>();
    m_material->diffuse()->setRGBA(m_color);

    m_materialAdaptor = this->registerService<module::viz::scene3d::adaptor::material>(
        "sight::module::viz::scene3d::adaptor::material"
    );
    m_materialAdaptor->set_inout(m_material, module::viz::scene3d::adaptor::material::s_MATERIAL_INOUT, true);
    m_materialAdaptor->configure(
        this->get_id() + "_" + m_materialAdaptor->get_id(),
        this->get_id() + "_" + m_materialAdaptor->get_id(),
        this->getRenderService(),
        m_layerID,
        "ambient"
    );
    m_materialAdaptor->start();
    m_materialAdaptor->update();
}

//-----------------------------------------------------------------------------

service::connections_t frustum_list::auto_connections() const
{
    service::connections_t connections;
    connections.push(s_TRANSFORM_INPUT, data::matrix4::MODIFIED_SIG, ADD_FRUSTUM_SLOT);
    return connections;
}

//-----------------------------------------------------------------------------

void frustum_list::updating()
{
    this->requestRender();
}

//-----------------------------------------------------------------------------

void frustum_list::stopping()
{
    this->unregisterServices();
    this->clear();
    m_materialAdaptor.reset();
    m_materialAdaptor = nullptr;
    m_material        = nullptr;
}

//-----------------------------------------------------------------------------

void frustum_list::setVisible(bool _visible)
{
    Ogre::SceneNode* root_scene_node = this->getSceneManager()->getRootSceneNode();
    Ogre::SceneNode* trans_node      = this->getOrCreateTransformNode(root_scene_node);

    trans_node->setVisible(_visible);
}

//-----------------------------------------------------------------------------

void frustum_list::addFrustum()
{
    //Get camera parameters
    const auto camera_data    = m_camera.lock();
    Ogre::Camera* ogre_camera =
        this->getSceneManager()->createCamera(
            Ogre::String(
                this->get_id() + "_camera" + std::to_string(
                    m_currentCamIndex
                )
            )
        );

    Ogre::SceneNode* root_scene_node = this->getSceneManager()->getRootSceneNode();
    Ogre::SceneNode* trans_node      = this->getOrCreateTransformNode(root_scene_node);
    trans_node->attachObject(ogre_camera);

    // Clipping
    if(m_near != 0.F)
    {
        ogre_camera->setNearClipDistance(m_near);
    }

    if(m_far != 0.F)
    {
        ogre_camera->setFarClipDistance(m_far);
    }

    if(camera_data->getIsCalibrated())
    {
        // Set data to camera
        const auto width  = static_cast<float>(camera_data->getWidth());
        const auto height = static_cast<float>(camera_data->getHeight());
        Ogre::Matrix4 m   =
            sight::viz::scene3d::helper::camera::computeProjectionMatrix(*camera_data, width, height, m_near, m_far);
        ogre_camera->setCustomProjectionMatrix(true, m);

        if(m_frustumList.full())
        {
            //Remove the oldest one
            auto f = m_frustumList.back();

            f.first->detachFromParent();
            this->getSceneManager()->destroyManualObject(f.first);
        }

        auto* const frustum = this->getSceneManager()->createManualObject(
            this->get_id() + "_frustum" + std::to_string(
                m_currentCamIndex
            )
        );
        auto* const frustum_node = root_scene_node->createChildSceneNode("Node_" + std::to_string(m_currentCamIndex));

        sight::viz::scene3d::helper::manual_object::createFrustum(
            frustum,
            m_materialAdaptor->getMaterialName(),
            *ogre_camera
        );

        this->setTransfromToNode(frustum_node);
        frustum_node->attachObject(frustum);

        //Add the new one
        m_frustumList.push_front({frustum, frustum_node});

        m_currentCamIndex++;

        this->updating();
    }
    else
    {
        SIGHT_ERROR("Camera isn't calibrated");
    }
}

//-----------------------------------------------------------------------------

void frustum_list::setTransfromToNode(Ogre::SceneNode* _node)
{
    const auto transform = m_transform.lock();
    Ogre::Affine3 ogre_mat;

    for(std::size_t lt = 0 ; lt < 4 ; lt++)
    {
        for(std::size_t ct = 0 ; ct < 4 ; ct++)
        {
            ogre_mat[ct][lt] = static_cast<Ogre::Real>((*transform)(ct, lt));
        }
    }

    // Decompose the matrix
    Ogre::Vector3 position;
    Ogre::Vector3 scale;
    Ogre::Quaternion orientation;
    ogre_mat.decomposition(position, scale, orientation);

    const Ogre::Quaternion rotate_x(Ogre::Degree(180), Ogre::Vector3(1, 0, 0));
    const Ogre::Quaternion rotate_z(Ogre::Degree(180), Ogre::Vector3(0, 0, 1));
    orientation = orientation * rotate_z * rotate_x;

    _node->setOrientation(orientation);
    _node->setPosition(position);
}

//-----------------------------------------------------------------------------

void frustum_list::clear()
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
