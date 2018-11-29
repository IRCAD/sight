/************************************************************************
 *
 * Copyright (C) 2014-2018 IRCAD France
 * Copyright (C) 2014-2018 IHU Strasbourg
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

#include "fwRenderOgre/ITransformable.hpp"

#include "fwRenderOgre/helper/Scene.hpp"

namespace fwRenderOgre
{

const std::string ITransformable::s_TRANSFORM_CONFIG = "transform";

//------------------------------------------------------------------------------

void ITransformable::setTransformId(::fwRenderOgre::SRender::OgreObjectIdType _id)
{
    m_transformId = _id;
}

//------------------------------------------------------------------------------

::fwRenderOgre::SRender::OgreObjectIdType ITransformable::getTransformId() const
{
    return m_transformId;
}

//------------------------------------------------------------------------------

::Ogre::SceneNode* ITransformable::getTransformNode(::Ogre::SceneNode* _rootNode)
{
    SLM_ASSERT("Transform id is not set, please call setTransformId before.", !m_transformId.empty());
    m_transformNode = ::fwRenderOgre::helper::Scene::getNodeById(m_transformId, _rootNode);
    if (m_transformNode == nullptr)
    {
        m_transformNode = _rootNode->createChildSceneNode(m_transformId);
    }
    return m_transformNode;
}

//-----------------------------------------------------------------------------

} // namespace fwRenderOgre
