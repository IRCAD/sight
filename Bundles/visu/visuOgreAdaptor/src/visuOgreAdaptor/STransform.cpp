/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuOgreAdaptor/STransform.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/mt/ObjectReadLock.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>

#include <fwRenderOgre/helper/Scene.hpp>
#include <fwRenderOgre/IAdaptor.hpp>
#include <fwRenderOgre/SRender.hpp>

#include <fwServices/macros.hpp>

fwServicesRegisterMacro(::fwRenderOgre::IAdaptor, ::visuOgreAdaptor::STransform, ::fwData::TransformationMatrix3D);

namespace visuOgreAdaptor
{

//------------------------------------------------------------------------------

STransform::STransform() noexcept :
    m_transformNode(nullptr),
    m_parentTransformNode(nullptr)
{
}

//------------------------------------------------------------------------------

STransform::~STransform() noexcept
{
}

//------------------------------------------------------------------------------

void STransform::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfigTree().get_child("config.<xmlattr>");

    this->setTransformId( config.get<std::string>("transform") );

    if ( config.count( "parent" ) )
    {
        m_parentTransformId = config.get<std::string>("parent");
    }
}

//------------------------------------------------------------------------------

void STransform::starting()
{
    this->initialize();

    ::Ogre::SceneNode* rootSceneNode = this->getSceneManager()->getRootSceneNode();
    if(!this->getTransformId().empty())
    {
        m_transformNode = ::fwRenderOgre::helper::Scene::getNodeById(this->getTransformId(), rootSceneNode);
    }

    if (!m_parentTransformId.empty())
    {
        m_parentTransformNode = ::fwRenderOgre::helper::Scene::getNodeById(m_parentTransformId, rootSceneNode);
    }

    ::Ogre::SceneManager* sceneManager = this->getSceneManager();

    if (!m_parentTransformNode)
    {
        if (!m_parentTransformId.empty())
        {
            m_parentTransformNode = sceneManager->getRootSceneNode()->createChildSceneNode(m_parentTransformId);
        }
        else
        {
            m_parentTransformNode = sceneManager->getRootSceneNode();
        }
    }

    if (!m_transformNode)
    {
        m_transformNode = m_parentTransformNode->createChildSceneNode(this->getTransformId());
    }
    else
    {
        m_transformNode->getParent()->removeChild(m_transformNode);
        m_parentTransformNode->addChild(m_transformNode);
    }
    this->updating();
}

//------------------------------------------------------------------------------

void STransform::updateFromOgre()
{
    auto fwTransform = this->getObject< ::fwData::TransformationMatrix3D >();
    ::fwData::mt::ObjectWriteLock lock(fwTransform);
    for(size_t lt = 0; lt < 4; lt++)
    {
        for (size_t ct = 0; ct < 4; ct++)
        {
            fwTransform->setCoefficient(ct, lt, static_cast<double>(m_ogreTransform[ct][lt]));
        }
    }

    auto sig = fwTransform->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
    {
        ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        sig->asyncEmit();
    }
}

//------------------------------------------------------------------------------

void STransform::updating()
{
    auto fwTransform = this->getObject< ::fwData::TransformationMatrix3D >();

    // Multithreaded lock
    {
        ::fwData::mt::ObjectReadLock lock(fwTransform);

        for(size_t lt = 0; lt < 4; lt++)
        {
            for(size_t ct = 0; ct < 4; ct++)
            {
                m_ogreTransform[ct][lt] = static_cast< ::Ogre::Real >(fwTransform->getCoefficient(ct, lt));
            }
        }
    }

    // Decompose the matrix
    ::Ogre::Vector3 position;
    ::Ogre::Vector3 scale;
    ::Ogre::Quaternion orientation;
    m_ogreTransform.decomposition(position, scale, orientation);

    m_transformNode->setOrientation(orientation);
    m_transformNode->setPosition(position);
    m_transformNode->setScale(scale);

    this->requestRender();
}

//------------------------------------------------------------------------------

void STransform::setTransform(const ::Ogre::Matrix4& t)
{
    m_ogreTransform = t;
    this->updateFromOgre();
}

//------------------------------------------------------------------------------

const ::Ogre::Matrix4& STransform::getTransform() const
{
    return m_ogreTransform;
}

//------------------------------------------------------------------------------

::Ogre::SceneNode* STransform::getSceneNode() const
{
    return m_transformNode;
}

//------------------------------------------------------------------------------

void STransform::stopping()
{
    this->getSceneManager()->destroySceneNode(m_transformNode);
    m_transformNode = nullptr;
}

//-----------------------------------------------------------------------------

} //namespace visuOgreAdaptor
