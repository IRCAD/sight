/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderOgre/ITransformable.hpp"

#include <fwRenderOgre/helper/Scene.hpp>

namespace fwRenderOgre
{

const std::string ITransformable::s_CONFIG_TRANSFORM = "transform";

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

::Ogre::SceneNode* ITransformable::getTransformNode(::Ogre::SceneNode* _rootNode) const
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
