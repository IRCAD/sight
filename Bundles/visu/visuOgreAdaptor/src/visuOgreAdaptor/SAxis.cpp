/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuOgreAdaptor/SAxis.hpp"

#include <fwRenderOgre/helper/Scene.hpp>

namespace visuOgreAdaptor
{

const ::fwCom::Slots::SlotKeyType SAxis::s_UPDATE_VISIBILITY_SLOT = "updateVisibility";

fwServicesRegisterMacro(::fwRenderOgre::IAdaptor, ::visuOgreAdaptor::SAxis);

//-----------------------------------------------------------------------------

SAxis::SAxis() noexcept :
    m_entity(nullptr),
    m_materialAdaptor(nullptr),
    m_material(nullptr),
    m_length(50.f)
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
    m_xLineNode->setVisible(isVisible);
    m_yLineNode->setVisible(isVisible);
    m_zLineNode->setVisible(isVisible);

    this->requestRender();
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

    ::Ogre::ManualObject* xLine = sceneMgr->createManualObject("xline");
    m_xLineNode                 = sceneMgr->getRootSceneNode()->createChildSceneNode("xline_node");

    ::Ogre::ManualObject* yLine = sceneMgr->createManualObject("yline");
    m_yLineNode                 = sceneMgr->getRootSceneNode()->createChildSceneNode("yline_node");

    ::Ogre::ManualObject* zLine = sceneMgr->createManualObject("zline");
    m_zLineNode                 = sceneMgr->getRootSceneNode()->createChildSceneNode("zline_node");

    // NOTE: The second parameter to the create method is the resource group the material will be added to.
    // If the group you name does not exist (in your resources.cfg file) the library will assert() and your program will
    // crash

    m_material = ::fwData::Material::New();

    m_materialAdaptor = this->registerService< ::visuOgreAdaptor::SMaterial >("::visuOgreAdaptor::SMaterial");
    m_materialAdaptor->registerInOut(m_material, "material", true);

    m_materialAdaptor->setID(this->getID() + "_" + m_materialAdaptor->getID());
    m_materialAdaptor->setRenderService( this->getRenderService() );
    m_materialAdaptor->setLayerID(m_layerID);
    m_materialAdaptor->setShadingMode("ambient");
    m_materialAdaptor->start();

    xLine->begin(m_materialAdaptor->getMaterialName(), Ogre::RenderOperation::OT_LINE_LIST);
    xLine->position(0, 0, 0);
    xLine->colour(1.0f, 0, 0);
    xLine->normal(1, 0, 0);

    xLine->position(m_length, 0, 0);
    xLine->colour(1.0f, 0, 0);
    xLine->normal(1, 0, 0);

    xLine->end();

    m_xLineNode->attachObject(xLine);

    this->attachNode(xLine);

    yLine->begin(m_materialAdaptor->getMaterialName(), Ogre::RenderOperation::OT_LINE_LIST);

    yLine->position(0, 0, 0);
    yLine->colour(0, 1.0f, 0);
    yLine->normal(1, 0, 0);

    yLine->position(0, m_length, 0);
    yLine->colour(0, 1.0f, 0);
    yLine->normal(1, 0, 0);

    yLine->end();

    m_yLineNode->attachObject(yLine);

    this->attachNode(yLine);

    zLine->begin(m_materialAdaptor->getMaterialName(), Ogre::RenderOperation::OT_LINE_LIST);
    zLine->position(0, 0, 0);
    zLine->colour(0, 0, 1.0f);
    zLine->normal(1, 0, 0);
    zLine->position(0, 0, m_length);
    zLine->colour(0, 0, 1.0f);
    zLine->normal(1, 0, 0);
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

