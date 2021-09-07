/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
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

#include "viz/scene3d/ITransformable.hpp"

#include "viz/scene3d/helper/Scene.hpp"

namespace sight::viz::scene3d
{

const std::string ITransformable::s_TRANSFORM_CONFIG = "transform";

//------------------------------------------------------------------------------

void ITransformable::setTransformId(viz::scene3d::SRender::OgreObjectIdType _id)
{
    m_transformId = _id;
}

//------------------------------------------------------------------------------

viz::scene3d::SRender::OgreObjectIdType ITransformable::getTransformId() const
{
    return m_transformId;
}

//------------------------------------------------------------------------------

::Ogre::SceneNode* ITransformable::getTransformNode(::Ogre::SceneNode* const _parentNode)
{
    SIGHT_ASSERT("Transform id is not set, please call setTransformId before.", !m_transformId.empty());
    m_transformNode = this->getTransformNode(m_transformId, _parentNode);
    return m_transformNode;
}

//------------------------------------------------------------------------------

::Ogre::SceneNode* ITransformable::getTransformNode(
    const std::string& _name,
    ::Ogre::SceneNode* const _parentNode
)
{
    SIGHT_ASSERT("Transform name is not set.", !_name.empty());
    auto transformNode = viz::scene3d::helper::Scene::getNodeById(_name, _parentNode);
    if(transformNode == nullptr)
    {
        transformNode = _parentNode->createChildSceneNode(_name);
    }

    return transformNode;
}

//-----------------------------------------------------------------------------

} // namespace sight::viz::scene3d
