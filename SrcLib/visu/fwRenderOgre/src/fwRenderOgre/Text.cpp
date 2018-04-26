/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderOgre/Text.hpp"

#include "fwRenderOgre/helper/Font.hpp"
#include "fwRenderOgre/Utils.hpp"

#include <OGRE/OgreGpuProgramParams.h>
#include <OGRE/OgreMaterial.h>
#include <OGRE/OgreNode.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreTechnique.h>
#include <OGRE/Overlay/OgreOverlayManager.h>

namespace fwRenderOgre
{

//------------------------------------------------------------------------------

Text* Text::New(const std::string& _id, ::Ogre::SceneManager* _sm, ::Ogre::FontPtr _font, ::Ogre::Camera* _cam)
{
    const auto& factoryName = ::fwRenderOgre::factory::Text::FACTORY_TYPE_NAME;
    Text* instance          = static_cast< ::fwRenderOgre::Text* >(_sm->createMovableObject(_id, factoryName));

    instance->m_camera = _cam;

    instance->m_overlayText->setDimensions(1.0f, 1.0f);
    instance->m_overlayText->setMetricsMode(Ogre::GMM_RELATIVE);
    instance->m_overlayText->setCharHeight(0.03f);
    instance->m_overlayText->setPosition(0.5, 0.5);
    instance->m_overlayText->setFontName(_font->getName());

    // Clone the font material, thereby allowing each text object to have its own color.
    const ::Ogre::MaterialPtr& fontMtl = ::fwRenderOgre::helper::Font::getFontMtl(_font->getName());
    const ::Ogre::MaterialPtr& textMtl = fontMtl->clone(_id + "_TextMaterial");
    instance->m_overlayText->setMaterial(textMtl);
    instance->m_overlayText->initialise();
    instance->m_overlayText->_updateFromParent();
    instance->m_overlayText->show();
    instance->setTransform(::Ogre::Matrix4::IDENTITY);
    instance->setMaterial(textMtl);

    instance->setRenderQueueGroup(::Ogre::RENDER_QUEUE_OVERLAY);

    return instance;
}

//------------------------------------------------------------------------------

Text::Text(const std::string& _id) :
    ::Ogre::SimpleRenderable(_id)
{
    auto& overlayManager = ::Ogre::OverlayManager::getSingleton();
    m_overlayText = dynamic_cast< ::Ogre::TextAreaOverlayElement* >(
        overlayManager.createOverlayElement("TextArea", _id + "TextArea"));
}

//------------------------------------------------------------------------------

Text::~Text()
{
    const ::Ogre::String& overlayTextName = m_overlayText->getName();

    ::Ogre::OverlayManager::getSingleton().destroyOverlayElement(overlayTextName);
}

//------------------------------------------------------------------------------

void Text::setText(const std::string& _text)
{
    m_overlayText->setCaption(_text);
    m_dirty = true;
}

//------------------------------------------------------------------------------

void Text::setPosition(float _x, float _y)
{
    m_overlayText->setPosition(_x, _y);
    m_dirty = true;
}

//------------------------------------------------------------------------------

void Text::setCharHeight(float _height)
{
    m_overlayText->setCharHeight(_height);
    m_dirty = true;
}

//------------------------------------------------------------------------------

void Text::setTextColor(Ogre::ColourValue _color)
{
    SLM_ASSERT("No material set for this Text.", mMaterial);
    ::Ogre::Technique* fontRenderTechnique = mMaterial->getTechnique(0);
    SLM_ASSERT("This Text's material has no technique.", fontRenderTechnique);
    ::Ogre::Pass* fontRenderPass = fontRenderTechnique->getPass(0);
    SLM_ASSERT("This Text's material has no pass.", fontRenderPass);

    ::Ogre::GpuProgramParametersSharedPtr passParams = fontRenderPass->getFragmentProgramParameters();
    SLM_ASSERT("The Text's material pass has no fragment shader attached.", passParams);

    passParams->setNamedConstant("u_textColor", _color);
}

//------------------------------------------------------------------------------

const Ogre::String& Text::getMovableType() const
{
    return factory::Text::FACTORY_TYPE_NAME;
}

//------------------------------------------------------------------------------

Ogre::Real Text::getBoundingRadius() const
{
    return 0.f;
}

//------------------------------------------------------------------------------

void Text::_updateRenderQueue(Ogre::RenderQueue* rq)
{
    ::Ogre::Node* parentNode = this->getParentNode();

    if(!mVisible)
    {
        return;
    }

    if(parentNode != nullptr)
    {
        const ::Ogre::Vector3& pos = parentNode->_getDerivedPosition();

        const auto viewProjMx = m_camera->getProjectionMatrix() * m_camera->getViewMatrix();
        const auto projPos    = viewProjMx * pos;

        const ::Ogre::Vector2 screenPos(0.5f + projPos.x/2.f, 0.5f - projPos.y/2.f);
        this->setPosition(screenPos.x, screenPos.y);

        // Clipping test.
        if(projPos.z < 0.f || projPos.z > 1.f)
        {
            m_overlayText->hide();
        }
        else
        {
            m_overlayText->show();
            rq->addRenderable(this);
        }
    }
}

//------------------------------------------------------------------------------

void Text::getRenderOperation(Ogre::RenderOperation& _op)
{
    if(m_dirty)
    {
        this->updateTextGeometry();
    }

    m_overlayText->getRenderOperation(mRenderOp);
    _op = mRenderOp;
}

//------------------------------------------------------------------------------

Ogre::Real Text::getSquaredViewDepth(const Ogre::Camera* _cam) const
{
    if(this->getParentNode() != nullptr)
    {
        return this->getParentNode()->getSquaredViewDepth(_cam);
    }
    else
    {
        return 1.f;
    }
}

//------------------------------------------------------------------------------

void Text::getWorldTransforms(Ogre::Matrix4* _xform) const
{
    *_xform = ::Ogre::Matrix4::IDENTITY;

    // Invert y axis to transform the geometry from the overlay's world (top-left origin)
    // to render system's viewport world (bottom-left origin).
    _xform->setScale(::Ogre::Vector3(1.f, -1.f, 1.f));
}

//------------------------------------------------------------------------------

void Text::updateTextGeometry()
{
    // HACK: updates the current viewport size in the overlay manager.
    // This way we can handle multiple viewports. It needs to be done because the overlay system
    // was designed for a single viewport only.
    // The viewport's size needs to be the right one when updating the geometry because the text's height is defined
    // as a fraction of the viewport's height.
    ::Ogre::OverlayManager::getSingleton()._queueOverlaysForRendering(m_camera, nullptr, m_camera->getViewport());

    m_overlayText->_updateFromParent();
    m_overlayText->_update();
}

} // namespace fwRenderOgre
