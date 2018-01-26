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
const ::fwCom::Slots::SlotKeyType SAxis::s_TOGGLE_VISIBILITY_SLOT = "toggleVisibility";

fwServicesRegisterMacro(::fwRenderOgre::IAdaptor, ::visuOgreAdaptor::SAxis);

//-----------------------------------------------------------------------------

SAxis::SAxis() noexcept :
    m_materialRedAdaptor(nullptr),
    m_materialGreenAdaptor(nullptr),
    m_materialBlueAdaptor(nullptr),
    m_materialRed(nullptr),
    m_materialGreen(nullptr),
    m_materialBlue(nullptr),
    m_length(50.f),
    m_isVisible(true)
{
    newSlot(s_UPDATE_VISIBILITY_SLOT, &SAxis::updateVisibility, this);
    newSlot(s_TOGGLE_VISIBILITY_SLOT, &SAxis::toggleVisibility, this);
}

//-----------------------------------------------------------------------------

SAxis::~SAxis() noexcept
{
}

//-----------------------------------------------------------------------------

void SAxis::updateVisibility(bool isVisible)
{
    m_isVisible = isVisible;
    this->updating();
}

//------------------------------------------------------------------------------

void SAxis::toggleVisibility()
{
    m_isVisible = !m_isVisible;
    this->updating();
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
    ::Ogre::ManualObject* yLine = sceneMgr->createManualObject(this->getID() + "_yline");
    ::Ogre::ManualObject* zLine = sceneMgr->createManualObject(this->getID() + "_zline");

    // set the material
    m_materialRed = ::fwData::Material::New();
    m_materialRed->diffuse()->setRGBA("#ff0000ff");

    m_materialRedAdaptor = this->registerService< ::visuOgreAdaptor::SMaterial >("::visuOgreAdaptor::SMaterial");
    m_materialRedAdaptor->registerInOut(m_materialRed, "material", true);
    m_materialRedAdaptor->setID(this->getID() + "_" + m_materialRedAdaptor->getID());
    m_materialRedAdaptor->setMaterialName(this->getID() + "_" + m_materialRedAdaptor->getID());
    m_materialRedAdaptor->setRenderService( this->getRenderService() );
    m_materialRedAdaptor->setLayerID(m_layerID);
    m_materialRedAdaptor->setShadingMode("ambient");
    m_materialRedAdaptor->setMaterialTemplateName(::fwRenderOgre::Material::DEFAULT_MATERIAL_TEMPLATE_NAME);
    m_materialRedAdaptor->start();
    m_materialRedAdaptor->update();

    m_materialGreen = ::fwData::Material::New();
    m_materialGreen->diffuse()->setRGBA("#00ff00ff");

    m_materialGreenAdaptor = this->registerService< ::visuOgreAdaptor::SMaterial >("::visuOgreAdaptor::SMaterial");
    m_materialGreenAdaptor->registerInOut(m_materialGreen, "material", true);
    m_materialGreenAdaptor->setID(this->getID() + "_" + m_materialGreenAdaptor->getID());
    m_materialGreenAdaptor->setMaterialName(this->getID() + "_" + m_materialGreenAdaptor->getID());
    m_materialGreenAdaptor->setRenderService( this->getRenderService() );
    m_materialGreenAdaptor->setLayerID(m_layerID);
    m_materialGreenAdaptor->setShadingMode("ambient");
    m_materialGreenAdaptor->setMaterialTemplateName(::fwRenderOgre::Material::DEFAULT_MATERIAL_TEMPLATE_NAME);
    m_materialGreenAdaptor->start();
    m_materialGreenAdaptor->update();

    m_materialBlue = ::fwData::Material::New();
    m_materialBlue->diffuse()->setRGBA("#0000ffff");

    m_materialBlueAdaptor = this->registerService< ::visuOgreAdaptor::SMaterial >("::visuOgreAdaptor::SMaterial");
    m_materialBlueAdaptor->registerInOut(m_materialBlue, "material", true);
    m_materialBlueAdaptor->setID(this->getID() + "_" + m_materialBlueAdaptor->getID());
    m_materialBlueAdaptor->setMaterialName(this->getID() + "_" + m_materialBlueAdaptor->getID());
    m_materialBlueAdaptor->setRenderService( this->getRenderService() );
    m_materialBlueAdaptor->setLayerID(m_layerID);
    m_materialBlueAdaptor->setShadingMode("ambient");
    m_materialBlueAdaptor->setMaterialTemplateName(::fwRenderOgre::Material::DEFAULT_MATERIAL_TEMPLATE_NAME);
    m_materialBlueAdaptor->start();
    m_materialBlueAdaptor->update();

    // Draw
    xLine->begin(m_materialRedAdaptor->getMaterialName(), Ogre::RenderOperation::OT_LINE_LIST);
    xLine->position(0, 0, 0);
    xLine->position(m_length, 0, 0);
    xLine->end();

    this->attachNode(xLine);

    yLine->begin(m_materialGreenAdaptor->getMaterialName(), Ogre::RenderOperation::OT_LINE_LIST);
    yLine->position(0, 0, 0);
    yLine->position(0, m_length, 0);
    yLine->end();

    this->attachNode(yLine);

    zLine->begin(m_materialBlueAdaptor->getMaterialName(), Ogre::RenderOperation::OT_LINE_LIST);
    zLine->position(0, 0, 0);
    zLine->position(0, 0, m_length);
    zLine->end();

    this->attachNode(zLine);

    this->requestRender();
}

//-----------------------------------------------------------------------------

void SAxis::updating()
{
    ::Ogre::SceneNode* rootSceneNode = this->getSceneManager()->getRootSceneNode();
    ::Ogre::SceneNode* transNode     =
        ::fwRenderOgre::helper::Scene::getNodeById(this->getTransformId(), rootSceneNode);
    if (transNode != nullptr)
    {
        transNode->setVisible(m_isVisible);
    }
    this->requestRender();
}

//-----------------------------------------------------------------------------

void SAxis::stopping()
{
    this->unregisterServices();
    m_materialRedAdaptor.reset();
    m_materialGreenAdaptor.reset();
    m_materialBlueAdaptor.reset();
    m_materialRed   = nullptr;
    m_materialGreen = nullptr;
    m_materialBlue  = nullptr;
}

//-----------------------------------------------------------------------------

void SAxis::attachNode(::Ogre::MovableObject* object)
{
    ::Ogre::SceneNode* rootSceneNode = this->getSceneManager()->getRootSceneNode();
    ::Ogre::SceneNode* transNode     =
        ::fwRenderOgre::helper::Scene::getNodeById(this->getTransformId(), rootSceneNode);

    if (transNode == nullptr)
    {
        transNode = rootSceneNode->createChildSceneNode(this->getTransformId());
        transNode->setVisible(m_isVisible);
    }
    transNode->attachObject(object);
}

//-----------------------------------------------------------------------------

} //visuOgreAdaptor
