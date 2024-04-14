/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
 * Copyright (C) 2014-2021 IHU Strasbourg
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

#include "viz/scene3d/transformable.hpp"

#include "viz/scene3d/adaptor.hpp"
#include "viz/scene3d/helper/scene.hpp"

namespace sight::viz::scene3d
{

const std::string transformable::TRANSFORM_CONFIG = "config.<xmlattr>.transform";

//------------------------------------------------------------------------------

void transformable::set_transform_id(viz::scene3d::render::ogre_object_id_t _id)
{
    m_transform_id = _id;
}

//------------------------------------------------------------------------------

viz::scene3d::render::ogre_object_id_t transformable::get_transform_id() const
{
    return m_transform_id;
}

//------------------------------------------------------------------------------

Ogre::SceneNode* transformable::get_transform_node() const
{
    return m_transform_node;
}

//------------------------------------------------------------------------------

Ogre::SceneNode* transformable::get_or_create_transform_node(
    Ogre::SceneNode* const _parent_node
)
{
    SIGHT_ASSERT("Transform id is not set.", !m_transform_id.empty());
    auto* transform_node = viz::scene3d::helper::scene::get_node_by_id(m_transform_id, _parent_node);
    if(transform_node == nullptr)
    {
        transform_node = _parent_node->createChildSceneNode(m_transform_id);
    }

    // assign m_transform to the retrieved/created node.
    m_transform_node = transform_node;

    return m_transform_node;
}

//-----------------------------------------------------------------------------

} // namespace sight::viz::scene3d
