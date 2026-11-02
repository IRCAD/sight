/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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

#include "module/viz/scene3d/adaptor/fly_camera.hpp"

#include "viz/scene3d/interactor/trackball.hpp"
#include "viz/scene3d/layer.hpp"
#include "viz/scene3d/utils.hpp"

#include <OgreCamera.h>
#include <OgreMatrix4.h>
#include <OgreSceneNode.h>

namespace sight::module::viz::scene3d::adaptor
{

namespace interactor_3d = sight::viz::scene3d::interactor;

//-----------------------------------------------------------------------------

fly_camera::fly_camera() noexcept
{
    new_slot(slots::SET_INTERACTION_ENABLED, &fly_camera::set_interaction_enabled, this);
    new_signal<signals::interaction_disabled_t>(signals::INTERACTION_DISABLED);
}

//-----------------------------------------------------------------------------

void fly_camera::set_interaction_enabled(bool _enabled)
{
    {
        const std::scoped_lock lock(m_rotation_mutex);
        if(m_interaction_enabled == _enabled)
        {
            return;
        }

        this->render_service()->make_current();
        auto* node = this->layer()->get_default_camera()->getParentSceneNode();
        if(_enabled)
        {
            m_normal_position    = node->getPosition();
            m_normal_orientation = node->getOrientation();
            m_user_rotation      = Ogre::Quaternion::IDENTITY;
        }
        else
        {
            node->setPosition(m_normal_position);
            node->setOrientation(m_normal_orientation);
        }

        m_has_pose            = false;
        m_interaction_enabled = _enabled;
    }

    if(_enabled)
    {
        this->updating();
    }
    else
    {
        this->async_emit(fly_camera::signals::INTERACTION_DISABLED);
    }

    this->request_render();
}

//-----------------------------------------------------------------------------

void fly_camera::configuring()
{
    this->configure_params();
    m_priority = this->get_config().get<int>(CONFIG + "priority", m_priority);
}

//-----------------------------------------------------------------------------

void fly_camera::starting()
{
    sight::viz::scene3d::adaptor::init();

    const auto layer = this->layer();
    auto interactor  = std::dynamic_pointer_cast<sight::viz::scene3d::interactor::base>(this->get_sptr());
    layer->add_interactor(interactor, m_priority);
}

//-----------------------------------------------------------------------------

void fly_camera::updating()
{
    const std::scoped_lock lock(m_rotation_mutex);
    if(!m_interaction_enabled)
    {
        return;
    }

    const auto transform = m_transform_in.const_lock();
    if(!transform)
    {
        return;
    }

    this->render_service()->make_current();
    const Ogre::Matrix4 pose = sight::viz::scene3d::utils::to_ogre_matrix(transform.get_shared());
    const Ogre::Quaternion orientation(pose.linear());

    const Ogre::Quaternion camera_flip(Ogre::Degree(180), Ogre::Vector3::UNIT_X);
    auto* node = this->layer()->get_default_camera()->getParentSceneNode();
    {
        node->setPosition(pose.getTrans());
        // Compose the path pose with the rotation added by mouse or keyboard interaction.
        m_base_orientation = orientation * camera_flip;
        node->setOrientation(m_base_orientation * m_user_rotation);
    }
    m_has_pose = true;
    this->request_render();
}

//-----------------------------------------------------------------------------

service::connections_t fly_camera::auto_connections() const
{
    auto connections = adaptor::auto_connections();
    connections.push("data.transform_in", data::signals::MODIFIED, service::slots::UPDATE);
    return connections;
}

//-----------------------------------------------------------------------------

void fly_camera::stopping()
{
    this->set_interaction_enabled(false);
    m_has_pose = false;
    if(const auto layer = this->layer(); layer)
    {
        auto interactor = std::dynamic_pointer_cast<sight::viz::scene3d::interactor::base>(this->get_sptr());
        layer->remove_interactor(interactor);
    }

    sight::viz::scene3d::adaptor::deinit();
}

//-----------------------------------------------------------------------------

void fly_camera::mouse_move_event(
    mouse_button _button,
    modifier /*_mods*/,
    int /*_x*/,
    int /*_y*/,
    int _dx,
    int _dy
)
{
    if(!m_interaction_enabled || !m_has_pose || _button != left)
    {
        return;
    }

    if(_dx != 0 || _dy != 0)
    {
        camera_rotate(-_dx, _dy);
        this->layer()->request_render();
    }

    // The trackball handles left-drag too; it must not apply a second rotation.
    this->layer()->cancel_further_interaction();
}

//-----------------------------------------------------------------------------

void fly_camera::camera_rotate(int _dx, int _dy)
{
    const std::scoped_lock lock(m_rotation_mutex);
    if(!m_interaction_enabled || !m_has_pose)
    {
        return;
    }

    Ogre::Camera* const camera = this->layer()->get_default_camera();
    interactor_3d::camera_rotate(camera, _dx, _dy, 0.F, interactor_3d::DEFAULT_VIEW_UP);

    // Keep the rotation relative to the path so the next path pose does not reset it.
    m_user_rotation = m_base_orientation.Inverse() * camera->getParentSceneNode()->getOrientation();
    m_user_rotation.normalise();
}

} // namespace sight::module::viz::scene3d::adaptor
