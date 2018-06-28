/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuOgreAdaptor/SLine.hpp"

#include "visuOgreAdaptor/STransform.hpp"

#include <fwCom/Slots.hxx>

#include <fwDataTools/Color.hpp>

#include <fwRenderOgre/helper/Scene.hpp>

#include <fwServices/macros.hpp>

#include <OgreAxisAlignedBox.h>
#include <OgreVector3.h>

namespace visuOgreAdaptor
{

const ::fwCom::Slots::SlotKeyType SLine::s_UPDATE_LENGTH_SLOT     = "updateLength";
const ::fwCom::Slots::SlotKeyType SLine::s_UPDATE_VISIBILITY_SLOT = "updateVisibility";
const ::fwCom::Slots::SlotKeyType SLine::s_TOGGLE_VISIBILITY_SLOT = "toggleVisibility";

fwServicesRegisterMacro(::fwRenderOgre::IAdaptor, ::visuOgreAdaptor::SLine);

static const std::string s_LENGTH_CONFIG = "length";

//-----------------------------------------------------------------------------

SLine::SLine() noexcept :
    m_material(nullptr),
    m_line(nullptr),
    m_length(0.f),
    m_isVisible(true)
{
    newSlot(s_UPDATE_VISIBILITY_SLOT, &SLine::updateVisibility, this);
    newSlot(s_TOGGLE_VISIBILITY_SLOT, &SLine::toggleVisibility, this);
    newSlot(s_UPDATE_LENGTH_SLOT, &SLine::updateLength, this);
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
    this->setTransformId(config.get<std::string>( ::fwRenderOgre::ITransformable::s_CONFIG_TRANSFORM,
                                                  this->getID() + "_transform"));

    m_length = config.get<float>(s_LENGTH_CONFIG, 50.f);

    const std::string color = config.get("color", "#FFFFFF");

    std::uint8_t rgba[4];
    ::fwDataTools::Color::hexaStringToRGBA(color, rgba);

    m_color.r = static_cast<float>(rgba[0]) / 255.f;
    m_color.g = static_cast<float>(rgba[1]) / 255.f;
    m_color.b = static_cast<float>(rgba[2]) / 255.f;
    m_color.a = static_cast<float>(rgba[3]) / 255.f;
}

//-----------------------------------------------------------------------------

void SLine::starting()
{
    this->initialize();

    ::Ogre::SceneManager* sceneMgr = this->getSceneManager();

    m_line = sceneMgr->createManualObject(this->getID() + "_line");
    // Set the line as dynamic, so we can update it later on, when the length changes
    m_line->setDynamic(true);

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
    m_line->begin(materialAdaptor->getMaterialName(), ::Ogre::RenderOperation::OT_LINE_LIST);
    m_line->position(0, 0, 0);
    m_line->colour(m_color);
    m_line->position(0, 0, m_length);
    m_line->colour(m_color);
    m_line->end();

    // Set the bounding box of your Manual Object
    ::Ogre::Vector3 bbMin(-0.1f, -0.1f, 0.f);
    ::Ogre::Vector3 bbMax(0.1f, 0.1f, m_length);
    ::Ogre::AxisAlignedBox box(bbMin, bbMax);
    m_line->setBoundingBox(box);
    this->attachNode(m_line);

    this->requestRender();
}

//-----------------------------------------------------------------------------

void SLine::updating()
{
    ::Ogre::SceneNode* rootSceneNode = this->getSceneManager()->getRootSceneNode();
    ::Ogre::SceneNode* transNode     = this->getTransformNode(rootSceneNode);
    transNode->setVisible(m_isVisible);
    this->requestRender();
}

//-----------------------------------------------------------------------------

void SLine::stopping()
{
    this->unregisterServices();
    m_material = nullptr;
    if(m_line)
    {
        m_line->detachFromParent();
        this->getSceneManager()->destroyManualObject(m_line);
        m_line = nullptr;
    }
}

//-----------------------------------------------------------------------------

void SLine::attachNode(::Ogre::MovableObject* object)
{
    ::Ogre::SceneNode* rootSceneNode = this->getSceneManager()->getRootSceneNode();
    ::Ogre::SceneNode* transNode     = this->getTransformNode(rootSceneNode);
    transNode->setVisible(m_isVisible);
    transNode->attachObject(object);
}

//-----------------------------------------------------------------------------

void SLine::updateLength(float length)
{
    m_length = length;

    // Update the line length with the new length
    m_line->beginUpdate(0);
    m_line->position(0, 0, 0);
    m_line->colour(m_color);
    m_line->position(0, 0, m_length);
    m_line->colour(m_color);
    m_line->end();

    // Update the bouding box
    ::Ogre::Vector3 bbMin(-0.1f, -0.1f, 0.f);
    ::Ogre::Vector3 bbMax(0.1f, 0.1f, m_length);
    ::Ogre::AxisAlignedBox box(bbMin, bbMax);
    m_line->setBoundingBox(box);

    this->requestRender();
}

} //visuOgreAdaptor
