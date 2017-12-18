/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuOgreAdaptor/SAxis.hpp"

namespace visuOgreAdaptor
{

const ::fwCom::Slots::SlotKeyType SAxis::s_UPDATE_VISIBILITY_SLOT = "updateVisibility";

fwServicesRegisterMacro(::fwRenderOgre::IAdaptor, ::visuOgreAdaptor::SAxis);

//-----------------------------------------------------------------------------

SAxis::SAxis() noexcept :
    m_entity(nullptr),
    m_materialAdaptor(nullptr),
    m_material(nullptr)
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
}

//-----------------------------------------------------------------------------

void SAxis::starting()
{
    this->initialize();

    ::Ogre::SceneManager* sceneMgr = this->getSceneManager();

    ::Ogre::ManualObject* xLine  = sceneMgr->createManualObject("xline");
    ::Ogre::SceneNode* xLineNode = sceneMgr->getRootSceneNode()->createChildSceneNode("xline_node");

    ::Ogre::ManualObject* yLine  = sceneMgr->createManualObject("yline");
    ::Ogre::SceneNode* yLineNode = sceneMgr->getRootSceneNode()->createChildSceneNode("yline_node");

    ::Ogre::ManualObject* zLine  = sceneMgr->createManualObject("zline");
    ::Ogre::SceneNode* zLineNode = sceneMgr->getRootSceneNode()->createChildSceneNode("zline_node");

    // NOTE: The second parameter to the create method is the resource group the material will be added to.
    // If the group you name does not exist (in your resources.cfg file) the library will assert() and your program will
    // crash

    m_material = ::fwData::Material::New();

    m_materialAdaptor = this->registerService< ::visuOgreAdaptor::SMaterial >("::visuOgreAdaptor::SMaterial");
    m_materialAdaptor->registerInOut(m_material, "material", true);

    m_materialAdaptor->setID(this->getID() + "_" + m_materialAdaptor->getID());
    m_materialAdaptor->setRenderService( this->getRenderService() );
    m_materialAdaptor->setLayerID(m_layerID);
    //m_materialAdaptor->setHasVertexColor(true);
    m_materialAdaptor->setShadingMode("ambient");
    m_materialAdaptor->start();

    xLine->begin(m_materialAdaptor->getMaterialName(), Ogre::RenderOperation::OT_LINE_LIST);
    xLine->position(0, 0, 0);
    xLine->colour(1.0f, 0, 0);
    xLine->normal(1, 0, 0);

    xLine->position(50, 0, 0);
    xLine->colour(1.0f, 0, 0);
    xLine->normal(1, 0, 0);

    xLine->end();

    xLineNode->attachObject(xLine);

    yLine->begin(m_materialAdaptor->getMaterialName(), Ogre::RenderOperation::OT_LINE_LIST);

    yLine->position(0, 0, 0);
    yLine->colour(0, 1.0f, 0);
    yLine->normal(1, 0, 0);

    yLine->position(0, 50, 0);
    yLine->colour(0, 1.0f, 0);
    yLine->normal(1, 0, 0);

    yLine->end();

    yLineNode->attachObject(yLine);

    zLine->begin(m_materialAdaptor->getMaterialName(), Ogre::RenderOperation::OT_LINE_LIST);
    zLine->position(0, 0, 0);
    zLine->colour(0, 0, 1.0f);
    zLine->normal(1, 0, 0);
    zLine->position(0, 0, 50);
    zLine->colour(0, 0, 1.0f);
    zLine->normal(1, 0, 0);
    zLine->end();

    zLineNode->attachObject(zLine);

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

    m_materialAdaptor->stop();
    m_material = nullptr;

}

//-----------------------------------------------------------------------------

} //visuOgreAdaptor

