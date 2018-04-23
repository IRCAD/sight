/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderOgre/Text.hpp"

#include "fwRenderOgre/helper/Font.hpp"

#include <OGRE/OgreGpuProgramParams.h>
#include <OGRE/OgreMaterial.h>
#include <OGRE/OgreNode.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreTechnique.h>
#include <OGRE/Overlay/OgreOverlayManager.h>

namespace fwRenderOgre
{

//------------------------------------------------------------------------------

Text* Text::New(const std::string& _id, ::Ogre::SceneManager* _sm, ::Ogre::OverlayContainer* _parent,
                ::Ogre::FontPtr _font, ::Ogre::Camera* _cam)
{
    const auto& factoryName = ::fwRenderOgre::factory::Text::FACTORY_TYPE_NAME;
    Text* instance          = static_cast< ::fwRenderOgre::Text* >(_sm->createMovableObject(_id, factoryName));

    instance->m_parentContainer = _parent;
    instance->m_camera          = _cam;

    instance->m_overlayText->setDimensions(1.0f, 1.0f);
    instance->m_overlayText->setMetricsMode(Ogre::GMM_RELATIVE);
    instance->m_overlayText->setCharHeight(0.03f);
    instance->m_overlayText->setPosition(0.5, 0.5);

    instance->m_overlayText->setFontName(_font->getName());

    // Clone the font material, thereby allowing each text object to have its own color.
    const ::Ogre::MaterialPtr& fontMtl = ::fwRenderOgre::helper::Font::getFontMtl(_font->getName());
    const ::Ogre::MaterialPtr& textMtl = fontMtl->clone(_id + "_TextMaterial");
    instance->m_overlayText->setMaterial(textMtl);

    _parent->addChild(instance->m_overlayText);

    return instance;
}

//------------------------------------------------------------------------------

Text::Text(const std::string& _id) :
    ::Ogre::MovableObject(_id)
{
    auto& overlayManager = ::Ogre::OverlayManager::getSingleton();
    m_overlayText = dynamic_cast< ::Ogre::TextAreaOverlayElement* >(
        overlayManager.createOverlayElement("TextArea", _id + "TextArea"));
}

//------------------------------------------------------------------------------

Text::~Text()
{
    const ::Ogre::String& overlayTextName = m_overlayText->getName();

    m_parentContainer->removeChild(overlayTextName);
    ::Ogre::OverlayManager::getSingleton().destroyOverlayElement(overlayTextName);
}

//------------------------------------------------------------------------------

void Text::setText(const std::string& _text)
{
    m_overlayText->setCaption(_text);
}

//------------------------------------------------------------------------------

void Text::setPosition(float _x, float _y)
{
    m_overlayText->setPosition(_x, _y);
}

//------------------------------------------------------------------------------

void Text::setCharHeight(float _height)
{
    m_overlayText->setCharHeight(_height);
}

//------------------------------------------------------------------------------

void Text::setTextColor(Ogre::ColourValue _color)
{
    ::Ogre::Pass* fontRenderPass                     = m_overlayText->getMaterial()->getTechnique(0)->getPass(0);
    ::Ogre::GpuProgramParametersSharedPtr passParams = fontRenderPass->getFragmentProgramParameters();

    passParams->setNamedConstant("u_textColor", ::Ogre::Vector3(_color.r, _color.g, _color.b));
}

//------------------------------------------------------------------------------

const Ogre::String& Text::getMovableType() const
{
    return factory::Text::FACTORY_TYPE_NAME;
}

//------------------------------------------------------------------------------

const Ogre::AxisAlignedBox& Text::getBoundingBox() const
{
    return m_bb;
}

//------------------------------------------------------------------------------

Ogre::Real Text::getBoundingRadius() const
{
    return 0.f;
}

//------------------------------------------------------------------------------

void Text::_updateRenderQueue(Ogre::RenderQueue*)
{
    ::Ogre::Node* parentNode = this->getParentNode();

    if(!mVisible)
    {
        m_overlayText->hide();
        return;
    }

    if(parentNode != nullptr)
    {
        const ::Ogre::Vector3& pos = parentNode->_getDerivedPosition();

        const auto viewProjMx = m_camera->getProjectionMatrix() * m_camera->getViewMatrix();
        const auto projPos    = viewProjMx * pos;

        const ::Ogre::Vector2 screenPos(0.5f + projPos.x/2.f, 0.5f - projPos.y/2.f);
        m_overlayText->setPosition(screenPos.x, screenPos.y);

//        std::cerr << "POS " << screenPos << " CAPTION " << m_overlayText->getCaption() << std::endl;

        // Clipping test.
        if(projPos.z < 0.f || projPos.z > 1.f)
        {
            m_overlayText->hide();
        }
        else
        {
            m_overlayText->show();
        }
    }
}

//------------------------------------------------------------------------------

void Text::visitRenderables(Ogre::Renderable::Visitor*, bool)
{
}

//------------------------------------------------------------------------------

} // namespace fwRenderOgre
