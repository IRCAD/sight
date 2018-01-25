/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuOgreAdaptor/SAxis.hpp"

#include "visuOgreAdaptor/STransform.hpp"

#include <fwCom/Slots.hxx>

#include <fwRenderOgre/helper/Scene.hpp>

namespace visuOgreAdaptor
{

const ::fwCom::Slots::SlotKeyType SAxis::s_UPDATE_VISIBILITY_SLOT = "updateVisibility";

fwServicesRegisterMacro(::fwRenderOgre::IAdaptor, ::visuOgreAdaptor::SAxis);

//-----------------------------------------------------------------------------

SAxis::SAxis() noexcept :
    m_materialAdaptor(nullptr),
    m_material(nullptr),
    m_xLineNode(nullptr),
    m_yLineNode(nullptr),
    m_zLineNode(nullptr),
    m_length(50.f),
    m_isVisible(true)
{

    newSlot(s_UPDATE_VISIBILITY_SLOT, &SAxis::updateVisibility, this);
}

//-----------------------------------------------------------------------------

SAxis::~SAxis() noexcept
{
}

//-----------------------------------------------------------------------------

void SAxis::updateVisibility(bool isVisible)
{
    m_isVisible = isVisible;
    m_xLineNode->setVisible(m_isVisible);
    m_yLineNode->setVisible(m_isVisible);
    m_zLineNode->setVisible(m_isVisible);

    this->requestRender();
}

//-----------------------------------------------------------------------------

bool SAxis::getVisibility() const
{
    return m_isVisible;
}

//-----------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap visuOgreAdaptor::SAxis::getAutoConnections() const
{
    ::fwServices::IService::KeyConnectionsMap connections;
    connections.push( "transform", ::fwData::Object::s_MODIFIED_SIG, s_UPDATE_SLOT );
    return connections;
}

//-----------------------------------------------------------------------------

void SAxis::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfigTree().get_child("config.<xmlattr>");

    if(config.count("transform"))
    {
        this->setTransformId(config.get<std::string>("transform"));
    }

    if(config.count("length"))
    {
        m_length = config.get<float>("length");
    }
}

//-----------------------------------------------------------------------------

void SAxis::starting()
{
    this->initialize();

    ::Ogre::SceneManager* sceneMgr = this->getSceneManager();

    ::Ogre::ManualObject* xLine = sceneMgr->createManualObject(this->getID() + "_xline");
    m_xLineNode                 = sceneMgr->getRootSceneNode()->createChildSceneNode(this->getID() + "_xline_node");

    ::Ogre::ManualObject* yLine = sceneMgr->createManualObject(this->getID() + "_yline");
    m_yLineNode                 = sceneMgr->getRootSceneNode()->createChildSceneNode(this->getID() + "_yline_node");

    ::Ogre::ManualObject* zLine = sceneMgr->createManualObject(this->getID() + "_zline");
    m_zLineNode                 = sceneMgr->getRootSceneNode()->createChildSceneNode(this->getID() + "_zline_node");

    // set the material
    m_material = ::fwData::Material::New();

    m_materialAdaptor = this->registerService< ::visuOgreAdaptor::SMaterial >("::visuOgreAdaptor::SMaterial");
    m_materialAdaptor->registerInOut(m_material, "material", true);

    m_materialAdaptor->setID(this->getID() + "_" + m_materialAdaptor->getID());
    m_materialAdaptor->setRenderService( this->getRenderService() );
    m_materialAdaptor->setLayerID(m_layerID);
    m_materialAdaptor->setShadingMode("ambient");
    m_materialAdaptor->start();
    m_materialAdaptor->update();

    // Draw
    xLine->begin(m_materialAdaptor->getMaterialName(), Ogre::RenderOperation::OT_LINE_LIST);
    xLine->position(0, 0, 0);
    xLine->position(m_length, 0, 0);
    xLine->end();

    m_xLineNode->attachObject(xLine);

    this->attachNode(xLine);

    yLine->begin(m_materialAdaptor->getMaterialName(), Ogre::RenderOperation::OT_LINE_LIST);
    yLine->position(0, 0, 0);
    yLine->position(0, m_length, 0);
    yLine->end();

    m_yLineNode->attachObject(yLine);

    this->attachNode(yLine);

    zLine->begin(m_materialAdaptor->getMaterialName(), Ogre::RenderOperation::OT_LINE_LIST);
    zLine->position(0, 0, 0);
    zLine->position(0, 0, m_length);
    zLine->end();

    m_zLineNode->attachObject(zLine);

    this->attachNode(zLine);
}

//-----------------------------------------------------------------------------

void SAxis::updating()
{
    this->requestRender();
}

//-----------------------------------------------------------------------------

void SAxis::stopping()
{
    this->unregisterServices();
    m_materialAdaptor.reset();
    m_material = nullptr;
}

//-----------------------------------------------------------------------------

void SAxis::attachNode(::Ogre::MovableObject* _node)
{
    auto transformService = ::visuOgreAdaptor::STransform::dynamicCast(m_transformService.lock());

    ::Ogre::SceneNode* rootSceneNode = this->getSceneManager()->getRootSceneNode();
    ::Ogre::SceneNode* transNode     =
        ::fwRenderOgre::helper::Scene::getNodeById(this->getTransformId(), rootSceneNode);

    if (transNode == nullptr)
    {
        transNode = rootSceneNode->createChildSceneNode(this->getTransformId());
    }
    ::Ogre::SceneNode* node = _node->getParentSceneNode();
    if ((node != transNode) && transNode)
    {
        _node->detachFromParent();
        transNode->attachObject(_node);
    }
}

//-----------------------------------------------------------------------------

} //visuOgreAdaptor
