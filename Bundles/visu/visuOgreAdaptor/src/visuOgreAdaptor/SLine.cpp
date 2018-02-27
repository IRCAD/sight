/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuOgreAdaptor/SLine.hpp"

#include "visuOgreAdaptor/STransform.hpp"

#include <fwCom/Slots.hxx>

#include <fwRenderOgre/helper/Scene.hpp>

#include <fwServices/macros.hpp>

#include <OgreAxisAlignedBox.h>
#include <OgreVector3.h>

namespace visuOgreAdaptor
{

const ::fwCom::Slots::SlotKeyType SLine::s_UPDATE_VISIBILITY_SLOT = "updateVisibility";
const ::fwCom::Slots::SlotKeyType SLine::s_TOGGLE_VISIBILITY_SLOT = "toggleVisibility";

fwServicesRegisterMacro(::fwRenderOgre::IAdaptor, ::visuOgreAdaptor::SLine);

static const std::string s_LENGTH_CONFIG = "length";

//-----------------------------------------------------------------------------

SLine::SLine() noexcept :
    m_material(nullptr),
    m_length(0.f),
    m_isVisible(true)
{
    newSlot(s_UPDATE_VISIBILITY_SLOT, &SLine::updateVisibility, this);
    newSlot(s_TOGGLE_VISIBILITY_SLOT, &SLine::toggleVisibility, this);
}

//-----------------------------------------------------------------------------

SLine::~SLine() noexcept
{
}

//-----------------------------------------------------------------------------

void SLine::updateVisibility(bool isVisible)
{
    m_isVisible = isVisible;
    this->updating();
}

//------------------------------------------------------------------------------

void SLine::toggleVisibility()
{
    m_isVisible = !m_isVisible;
    this->updating();
}

//-----------------------------------------------------------------------------

bool SLine::getVisibility() const
{
    return m_isVisible;
}

//-----------------------------------------------------------------------------

void SLine::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfigTree().get_child("config.<xmlattr>");

    // parsing transform or create an "empty" one
    const std::string transformId = config.get<std::string>(::visuOgreAdaptor::STransform::s_CONFIG_TRANSFORM,
                                                            this->getID() + "_transform");

    this->setTransformId(transformId);
    m_length = config.get<float>(s_LENGTH_CONFIG, 50.f);

    const std::string color = config.get("color", "#FFFFFF");

    m_color = ::fwData::Color::New();
    m_color->setRGBA(color);
}

//-----------------------------------------------------------------------------

void SLine::starting()
{
    this->initialize();

    ::Ogre::SceneManager* sceneMgr = this->getSceneManager();

    ::Ogre::ManualObject* line = sceneMgr->createManualObject(this->getID() + "_line");

    // set the material
    m_material = ::fwData::Material::New();

    ::visuOgreAdaptor::SMaterial::sptr materialAdaptor = this->registerService< ::visuOgreAdaptor::SMaterial >(
        "::visuOgreAdaptor::SMaterial");
    materialAdaptor->registerInOut(m_material, ::visuOgreAdaptor::SMaterial::s_MATERIAL_INOUT, true);
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
    line->begin(materialAdaptor->getMaterialName(), ::Ogre::RenderOperation::OT_LINE_LIST);
    line->position(0, 0, 0);
    line->colour(m_color->red(), m_color->green(), m_color->blue());
    line->position(0, 0, m_length);
    line->colour(m_color->red(), m_color->green(), m_color->blue() );
    line->end();

    // Set the bounding box of your Manual Object
    ::Ogre::Vector3 bbMin(-0.1f, -0.1f, 0.f);
    ::Ogre::Vector3 bbMax(0.1f, 0.1f, m_length);
    ::Ogre::AxisAlignedBox box(bbMin, bbMax);
    line->setBoundingBox(box);
    this->attachNode(line);

    this->requestRender();
}

//-----------------------------------------------------------------------------

void SLine::updating()
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

void SLine::stopping()
{
    this->unregisterServices();
    m_material = nullptr;
}

//-----------------------------------------------------------------------------

void SLine::attachNode(::Ogre::MovableObject* object)
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
