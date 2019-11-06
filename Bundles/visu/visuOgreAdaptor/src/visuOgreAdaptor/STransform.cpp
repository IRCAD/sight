/************************************************************************
 *
 * Copyright (C) 2014-2019 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#include "visuOgreAdaptor/STransform.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/mt/ObjectReadLock.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>

#include <fwServices/macros.hpp>

namespace visuOgreAdaptor
{

static const ::fwServices::IService::KeyType s_TRANSFORM_INOUT = "transform";

//------------------------------------------------------------------------------

STransform::STransform() noexcept
{
}

//------------------------------------------------------------------------------

STransform::~STransform() noexcept
{
}

//-----------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap STransform::getAutoConnections() const
{
    ::fwServices::IService::KeyConnectionsMap connections;
    connections.push( s_TRANSFORM_INOUT, ::fwData::Object::s_MODIFIED_SIG, s_UPDATE_SLOT );
    return connections;
}

//------------------------------------------------------------------------------

void STransform::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfigTree().get_child("config.<xmlattr>");

    this->setTransformId( config.get<std::string>("transform") );

    m_parentTransformId = config.get<std::string>("parent", m_parentTransformId);
}

//------------------------------------------------------------------------------

void STransform::starting()
{
    this->initialize();
    ::Ogre::SceneManager* const sceneManager = this->getSceneManager();

    ::Ogre::SceneNode* const rootSceneNode = sceneManager->getRootSceneNode();
    SLM_ASSERT("Root scene node not found", rootSceneNode);

    if (!m_parentTransformId.empty())
    {
        m_parentTransformNode = this->getTransformNode(m_parentTransformId, rootSceneNode);
    }
    else
    {
        m_parentTransformNode = rootSceneNode;
    }

    m_transformNode = this->getTransformNode(m_parentTransformNode);

    this->updating();
}

//------------------------------------------------------------------------------

void STransform::updating()
{
    const auto fwTransform = this->getInOut< ::fwData::TransformationMatrix3D >(s_TRANSFORM_INOUT);
    SLM_ASSERT("'" + s_TRANSFORM_INOUT + "' does not exist.", fwTransform);

    {
        const ::fwData::mt::ObjectReadLock lock(fwTransform);

        for(size_t lt = 0; lt < 4; ++lt)
        {
            for(size_t ct = 0; ct < 4; ++ct)
            {
                m_ogreTransform[ct][lt] = static_cast< ::Ogre::Real >(fwTransform->getCoefficient(ct, lt));
            }
        }
    }

    if(m_ogreTransform == ::Ogre::Affine3::ZERO)
    {
        m_transformNode->setVisible(false);
    }
    else
    {
        m_transformNode->setVisible(true);

        // Decompose the matrix
        ::Ogre::Vector3 position;
        ::Ogre::Vector3 scale;
        ::Ogre::Quaternion orientation;
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

} //namespace visuOgreAdaptor
