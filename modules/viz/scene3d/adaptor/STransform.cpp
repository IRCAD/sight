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

#include "modules/viz/scene3d/adaptor/STransform.hpp"

#include <core/com/Signal.hxx>
#include <core/com/Slots.hxx>

#include <service/macros.hpp>

#include <viz/scene3d/helper/Scene.hpp>

namespace sight::module::viz::scene3d::adaptor
{

//-----------------------------------------------------------------------------

service::IService::KeyConnectionsMap STransform::getAutoConnections() const
{
    service::IService::KeyConnectionsMap connections;
    connections.push(s_TRANSFORM_INOUT, data::Object::s_MODIFIED_SIG, IService::slots::s_UPDATE);
    return connections;
}

//------------------------------------------------------------------------------

void STransform::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfiguration();

    this->setTransformId(config.get<std::string>(s_TRANSFORM_CONFIG));

    m_parentTransformId = config.get<std::string>(s_CONFIG + "parent", m_parentTransformId);
}

//------------------------------------------------------------------------------

void STransform::starting()
{
    this->initialize();
    Ogre::SceneManager* const sceneManager = this->getSceneManager();

    Ogre::SceneNode* const rootSceneNode = sceneManager->getRootSceneNode();
    SIGHT_ASSERT("Root scene node not found", rootSceneNode);

    if(!m_parentTransformId.empty())
    {
        m_parentTransformNode = sight::viz::scene3d::helper::Scene::getNodeById(m_parentTransformId, rootSceneNode);
        if(m_parentTransformNode == nullptr)
        {
            m_parentTransformNode = rootSceneNode->createChildSceneNode(m_parentTransformId);
        }
    }
    else
    {
        m_parentTransformNode = rootSceneNode;
    }

    m_transformNode = this->getOrCreateTransformNode(m_parentTransformNode);

    this->updating();
}

//------------------------------------------------------------------------------

void STransform::updating()
{
    {
        const auto transform = m_matrix.lock();
        m_ogreTransform = Ogre::Affine3(sight::viz::scene3d::Utils::convertTM3DToOgreMx(transform.get_shared()));
    }

    if(m_ogreTransform == Ogre::Affine3::ZERO)
    {
        m_parentTransformNode->removeChild(m_transformNode);
    }
    else
    {
        if(!m_transformNode->isInSceneGraph())
        {
            m_parentTransformNode->addChild(m_transformNode);
        }

        // Decompose the matrix
        Ogre::Vector3 position;
        Ogre::Vector3 scale;
        Ogre::Quaternion orientation;
        m_ogreTransform.decomposition(position, scale, orientation);

        m_transformNode->setOrientation(orientation);
        m_transformNode->setPosition(position);
        m_transformNode->setScale(scale);
    }

    this->requestRender();
}

//------------------------------------------------------------------------------

void STransform::stopping()
{
}

//-----------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.
