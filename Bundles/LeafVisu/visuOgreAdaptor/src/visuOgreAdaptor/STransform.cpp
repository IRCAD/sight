/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
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
#include <fwRenderOgre/helper/Scene.hpp>

#include <fwServices/macros.hpp>

fwServicesRegisterMacro(::fwRenderOgre::IAdaptor, ::visuOgreAdaptor::STransform, ::fwData::TransformationMatrix3D);

namespace visuOgreAdaptor
{

//-----------------------------------------------------------------------------

static const ::fwCom::Slots::SlotKeyType s_UPDATE_TF = "updateTransformation";

//------------------------------------------------------------------------------

STransform::STransform() throw() :
    m_transformNode(nullptr),
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
    ::Ogre::SceneNode* rootSceneNode = this->getSceneManager()->getRootSceneNode();
    if(!this->getTransformUID().empty())
    {
        m_transformNode = ::fwRenderOgre::helper::Scene::getNodeById(this->getTransformUID(), rootSceneNode);
    }

    if(m_transformNode)
    {
        this->doUpdate();
        return;
    }

    if (!m_parentTransformUID.empty())
    {
        m_parentTransformNode = ::fwRenderOgre::helper::Scene::getNodeById(m_parentTransformUID, rootSceneNode);
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

    m_transformNode = m_parentTransformNode->createChildSceneNode(this->getTransformUID());
    this->doUpdate();
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

    m_transformNode->setOrientation(orientation);
    m_transformNode->setPosition(position);
    m_transformNode->setScale(scale);

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
    return m_transformNode;
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
