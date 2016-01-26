/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuOgreAdaptor/STransform.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/TransformationMatrix3D.hpp>
#include <fwData/mt/ObjectReadLock.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>
#include <fwRenderOgre/SRender.hpp>
#include <fwRenderOgre/IAdaptor.hpp>
#include <fwServices/macros.hpp>

#include <stack>

fwServicesRegisterMacro(::fwRenderOgre::IAdaptor, ::visuOgreAdaptor::STransform,
                        ::fwData::TransformationMatrix3D);

namespace visuOgreAdaptor
{

//-----------------------------------------------------------------------------

static const ::fwCom::Slots::SlotKeyType s_UPDATE_TF = "updateTransformation";

//------------------------------------------------------------------------------

STransform::STransform() throw() :
    m_ogreTransformNode(nullptr),
    m_parentTransformNode(nullptr)
{
    newSlot(s_UPDATE_TF, &STransform::update, this);
}

//------------------------------------------------------------------------------

STransform::~STransform() throw()
{
}

//------------------------------------------------------------------------------

void STransform::doConfigure() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();

    // Base class configuration
    SLM_ASSERT("Not a \"config\" configuration", m_configuration->getName() == "config");
    this->setTransformUID( m_configuration->getAttributeValue("transform") );

    if ( m_configuration->hasAttribute( "parent" ) )
    {
        m_parentTransformUID = m_configuration->getAttributeValue("parent");

        if(m_parentTransformUID.empty())
        {
            OSLM_ERROR(
                "visuOgreAdaptor->Transform: Can't find parent: '"<< m_parentTransformUID <<
                "'. Check XML configuration"
                );
        }
    }
}

//------------------------------------------------------------------------------

void STransform::doStart() throw(fwTools::Failed)
{
    if(!this->getTransformUID().empty())
    {
        m_ogreTransformNode = this->getNodeById(this->getTransformUID());
    }

    if(m_ogreTransformNode)
    {
        this->doUpdate();
        return;
    }

    if (!m_parentTransformUID.empty())
    {
        m_parentTransformNode = getNodeById(m_parentTransformUID);
    }

    ::Ogre::SceneManager* sceneManager = this->getSceneManager();

    if (!m_parentTransformNode)
    {
        if (!m_parentTransformUID.empty())
        {
            m_parentTransformNode = sceneManager->getRootSceneNode()->createChildSceneNode(m_parentTransformUID);
        }
        else
        {
            m_parentTransformNode = sceneManager->getRootSceneNode();
        }
    }

    m_ogreTransformNode = m_parentTransformNode->createChildSceneNode(this->getTransformUID());
    this->doUpdate();
}

//------------------------------------------------------------------------------

::Ogre::SceneNode* STransform::getNodeById(::fwRenderOgre::SRender::OgreObjectIdType transformId)
{
    ::Ogre::SceneNode* sceneNode                 = nullptr;
    ::Ogre::SceneManager* sceneManager           = this->getSceneManager();
    ::Ogre::SceneNode::ChildNodeIterator rootMap = sceneManager->getRootSceneNode()->getChildIterator();

    std::stack< ::Ogre::SceneNode::ChildNodeIterator > stack;
    stack.push(rootMap);

    // Recursive search in the graph
    do
    {
        ::Ogre::SceneNode::ChildNodeIterator map = stack.top();
        stack.pop();
        for(auto it : map)
        {
            if (it.first == transformId)
            {
                sceneNode = static_cast< ::Ogre::SceneNode* >(it.second);
                break;
            }
            stack.push(it.second->getChildIterator());
        }
    }
    while( !stack.empty() );

    return sceneNode;
}

//------------------------------------------------------------------------------

void STransform::updateFromOgre()
{
    ::fwData::TransformationMatrix3D::sptr trf = this->getObject< ::fwData::TransformationMatrix3D >();

    ::fwData::mt::ObjectWriteLock lock(trf);
    for(size_t lt = 0; lt<4; lt++)
    {
        for (size_t lt = 0; lt < 4; lt++)
        {
            for (size_t ct = 0; ct < 4; ct++)
            {
                trf->setCoefficient(lt, ct, m_transform[ct][lt]);
            }
        }
    }

    auto sig = trf->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
    {
        ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        sig->asyncEmit();
    }
}

//------------------------------------------------------------------------------

void STransform::doUpdate() throw(fwTools::Failed)
{
    this->getRenderService()->makeCurrent();

    ::fwData::TransformationMatrix3D::sptr trf = this->getObject< ::fwData::TransformationMatrix3D >();

    ::fwData::mt::ObjectReadLock lock(trf);
    for(size_t lt = 0; lt<4; lt++)
    {
        for(size_t ct = 0; ct<4; ct++)
        {
            m_transform[ct][lt] = static_cast< ::Ogre::Real >(trf->getCoefficient(ct, lt));
        }
    }

    ::Ogre::Vector3 position;
    ::Ogre::Vector3 scale;
    ::Ogre::Quaternion orientation;
    m_transform.decomposition(position, scale, orientation);

    m_ogreTransformNode->setOrientation(orientation);
    m_ogreTransformNode->setPosition(position);
    m_ogreTransformNode->setScale(scale);

    this->requestRender();
}

//------------------------------------------------------------------------------

void STransform::setTransform(const ::Ogre::Matrix4& t)
{
    m_transform = t;
    this->updateFromOgre();
}

//------------------------------------------------------------------------------

const ::Ogre::Matrix4& STransform::getTransform() const
{
    return m_transform;
}

//------------------------------------------------------------------------------

::Ogre::SceneNode* STransform::getSceneNode()
{
    return m_ogreTransformNode;
}

//------------------------------------------------------------------------------

void STransform::doSwap() throw(fwTools::Failed)
{
    this->doUpdate();
}

//------------------------------------------------------------------------------

void STransform::doStop() throw(fwTools::Failed)
{
    this->unregisterServices();
}

//-----------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsType visuOgreAdaptor::STransform::getObjSrvConnections() const
{
    ::fwServices::IService::KeyConnectionsType connections;
    connections.push_back( std::make_pair( ::fwData::Object::s_MODIFIED_SIG, s_UPDATE_TF ) );
    return connections;
}

} //namespace visuOgreAdaptor
