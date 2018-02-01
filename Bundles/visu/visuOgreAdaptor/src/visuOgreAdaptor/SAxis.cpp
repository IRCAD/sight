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

const std::string SAxis::s_CONFIG_LENGTH = "length";

//-----------------------------------------------------------------------------

SAxis::SAxis() noexcept :
    m_material(nullptr),
    m_length(0.f),
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
    connections.push( ::visuOgreAdaptor::STransform::s_CONFIG_TRANSFORM, ::fwData::Object::s_MODIFIED_SIG,
                      s_UPDATE_SLOT );
    return connections;
}

//-----------------------------------------------------------------------------

void SAxis::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfigTree().get_child("config.<xmlattr>");

    this->setTransformId(config.get<std::string>(::visuOgreAdaptor::STransform::s_CONFIG_TRANSFORM, this->getID()));
    m_length = config.get<float>(s_CONFIG_LENGTH, 50.f);
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
    m_material = ::fwData::Material::New();

    ::visuOgreAdaptor::SMaterial::sptr materialAdaptor = this->registerService< ::visuOgreAdaptor::SMaterial >(
        "::visuOgreAdaptor::SMaterial");
    materialAdaptor->registerInOut(m_material, ::visuOgreAdaptor::SMaterial::s_INOUT_MATERIAL, true);
    materialAdaptor->setID(this->getID() + materialAdaptor->getID());
    materialAdaptor->setMaterialName(this->getID() + materialAdaptor->getID());
    materialAdaptor->setRenderService( this->getRenderService() );
    materialAdaptor->setLayerID(m_layerID);
    materialAdaptor->setShadingMode("ambient");
    materialAdaptor->setMaterialTemplateName(::fwRenderOgre::Material::DEFAULT_MATERIAL_TEMPLATE_NAME);
    materialAdaptor->start();

    materialAdaptor->getMaterialFw()->setHasVertexColor(true);
    materialAdaptor->update();

    // Draw
    xLine->begin(materialAdaptor->getMaterialName(), Ogre::RenderOperation::OT_LINE_LIST);
    xLine->position(0, 0, 0);
    xLine->colour(1.0f, 0, 0);
    xLine->position(m_length, 0, 0);
    xLine->colour(1.0f, 0, 0);
    xLine->end();

    this->attachNode(xLine);

    yLine->begin(materialAdaptor->getMaterialName(), Ogre::RenderOperation::OT_LINE_LIST);
    yLine->position(0, 0, 0);
    yLine->colour(0, 1.0f, 0);
    yLine->position(0, m_length, 0);
    yLine->colour(0, 1.0f, 0);
    yLine->end();

    this->attachNode(yLine);

    zLine->begin(materialAdaptor->getMaterialName(), Ogre::RenderOperation::OT_LINE_LIST);
    zLine->position(0, 0, 0);
    zLine->colour(0, 0, 1.0f);
    zLine->position(0, 0, m_length);
    zLine->colour(0, 0, 1.0f);
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
    m_material = nullptr;
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
