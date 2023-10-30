/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include "modules/viz/scene3d/adaptor/transform.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

#include <service/macros.hpp>

#include <viz/scene3d/helper/scene.hpp>

namespace sight::module::viz::scene3d::adaptor
{

//-----------------------------------------------------------------------------

service::connections_t transform::auto_connections() const
{
    service::connections_t connections;
    connections.push(TRANSFORM_INOUT, data::object::MODIFIED_SIG, service::slots::UPDATE);
    return connections;
}

//------------------------------------------------------------------------------

void transform::configuring()
{
    this->configure_params();

    const config_t config = this->get_config();

    this->set_transform_id(config.get<std::string>(TRANSFORM_CONFIG));

    m_parent_transform_id = config.get<std::string>(CONFIG + "parent", m_parent_transform_id);
}

//------------------------------------------------------------------------------

void transform::starting()
{
    this->initialize();
    Ogre::SceneManager* const scene_manager = this->get_scene_manager();

    Ogre::SceneNode* const root_scene_node = scene_manager->getRootSceneNode();
    SIGHT_ASSERT("Root scene node not found", root_scene_node);

    if(!m_parent_transform_id.empty())
    {
        m_parent_transform_node = sight::viz::scene3d::helper::scene::get_node_by_id(
            m_parent_transform_id,
            root_scene_node
        );
        if(m_parent_transform_node == nullptr)
        {
            m_parent_transform_node = root_scene_node->createChildSceneNode(m_parent_transform_id);
        }
    }
    else
    {
        m_parent_transform_node = root_scene_node;
    }

    m_transform_node = this->get_or_create_transform_node(m_parent_transform_node);

    this->updating();
}

//------------------------------------------------------------------------------

void transform::updating()
{
    {
        const auto transform = m_matrix.lock();
        m_ogre_transform = Ogre::Affine3(sight::viz::scene3d::utils::to_ogre_matrix(transform.get_shared()));
    }

    if(m_ogre_transform == Ogre::Affine3::ZERO)
    {
        m_parent_transform_node->removeChild(m_transform_node);
    }
    else
    {
        if(!m_transform_node->isInSceneGraph())
        {
            m_parent_transform_node->addChild(m_transform_node);
        }

        // Decompose the matrix
        Ogre::Vector3 position;
        Ogre::Vector3 scale;
        Ogre::Quaternion orientation;
        m_ogre_transform.decomposition(position, scale, orientation);

        m_transform_node->setOrientation(orientation);
        m_transform_node->setPosition(position);
        m_transform_node->setScale(scale);
    }

    this->request_render();
}

//------------------------------------------------------------------------------

void transform::stopping()
{
}

//-----------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.
