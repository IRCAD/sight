/************************************************************************
 *
 * Copyright (C) 2018-2019 IRCAD France
 * Copyright (C) 2018-2019 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "fwRenderOgre/Text.hpp"

#include "fwRenderOgre/helper/Font.hpp"

#include <fwCore/spyLog.hpp>

#include <OGRE/OgreGpuProgramParams.h>
#include <OGRE/OgreMaterial.h>
#include <OGRE/OgreMaterialManager.h>
#include <OGRE/OgreNode.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreTechnique.h>
#include <OGRE/OgreViewport.h>
#include <OGRE/Overlay/OgreOverlayManager.h>

namespace fwRenderOgre
{

class ResizeListener : public ::Ogre::Viewport::Listener
{

public:
    ResizeListener(Text& _text) :
        m_text(_text)
    {

    }

    virtual ~ResizeListener()
    {

    }

    //------------------------------------------------------------------------------

    virtual void viewportDimensionsChanged(::Ogre::Viewport*)
    {
        m_text.resize();
    }

private:

    Text& m_text;
};

//------------------------------------------------------------------------------

Text* Text::New(const std::string& _id, ::Ogre::SceneManager* _sm, ::Ogre::OverlayContainer* _parent,
                const std::string& _fontSource, size_t _fontSize, float _fontResolution, ::Ogre::Camera* _cam)
{
    const auto& factoryName = ::fwRenderOgre::factory::Text::FACTORY_TYPE_NAME;
    Text* instance          = static_cast< ::fwRenderOgre::Text* >(_sm->createMovableObject(_id, factoryName));

    instance->m_parentContainer = _parent;
    instance->m_camera          = _cam;

    const std::uint32_t fontMapResolution = static_cast<std::uint32_t>(std::round(_fontResolution));
    auto font                             = helper::Font::getFont(_fontSource, _fontSize, fontMapResolution);
    instance->setFont(font);

    instance->m_overlayText->setDimensions(1.0f, 1.0f);
    instance->m_overlayText->setMetricsMode(Ogre::GMM_RELATIVE);
    instance->setDotsPerInch(_fontResolution);

    instance->m_listener = new ResizeListener(*instance);
    _cam->getViewport()->addListener(instance->m_listener);

    _parent->addChild(instance->m_overlayText);

    return instance;
}

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
    instance->m_overlayText->setPosition(0.5, 0.5);

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
    auto material = m_overlayText->getMaterial();
    m_parentContainer->removeChild(m_overlayText->getName());
    ::Ogre::OverlayManager::getSingleton().destroyOverlayElement(m_overlayText);

    ::Ogre::MaterialManager::getSingleton().remove(material);

    if(m_listener)
    {
        m_camera->getViewport()->removeListener(m_listener);
        delete m_listener;
    }
}

//------------------------------------------------------------------------------

void Text::setText(const std::string& _text)
{
    m_overlayText->setCaption(_text);
}

//------------------------------------------------------------------------------

void Text::setPosition(float _x, float _y)
{
    m_position = ::Ogre::Vector2(_x, _y);

    // Ogre doesn't handle vertical alignment for text elements so we have to do it ourselves ..
    const float textHeight = this->getTextHeight();
    const auto vAlign      = m_overlayText->getVerticalAlignment();
    const float alignedY   = vAlign == ::Ogre::GVA_BOTTOM ? _y - textHeight :
                             vAlign == ::Ogre::GVA_CENTER ? _y - textHeight * 0.5f : _y;

    m_overlayText->setPosition(_x, alignedY);
}

//------------------------------------------------------------------------------

float Text::getTextHeight() const
{
    auto& caption = m_overlayText->getCaption();

#ifdef WIN32
#pragma warning(push)
#pragma warning( disable : 4996 ) // Ogre still uses the old iterator API deprecated in c++17.
#endif
    const auto nbLineEnds = std::count(caption.begin(), caption.end(), '\n') + 1;
#ifdef WIN32
#pragma warning(pop)
#endif

    return static_cast<float>(nbLineEnds) * m_overlayText->getCharHeight();
}

//------------------------------------------------------------------------------

void Text::setDotsPerInch(float _dpi)
{
    constexpr float pointsPerInch = 72.f; // a standard DTP points is equal to 1/72th of an inch (US).
    const float fontSize          = m_font->getTrueTypeSize(); // in dots

    _dpi = std::round(_dpi);

    m_heightInPixels = (fontSize / pointsPerInch) * _dpi;

    this->resize();

    std::uint32_t fontRes = static_cast<std::uint32_t>(_dpi);
    if(m_font->getTrueTypeResolution() != fontRes)
    {
        auto newFont = helper::Font::getFont(m_font->getSource(),
                                             static_cast<size_t>(std::round(fontSize)),
                                             fontRes);
        this->setFont(newFont);
    }
}

//------------------------------------------------------------------------------

void Text::setCharHeight(float _height)
{
    m_overlayText->setCharHeight(_height);
}

//------------------------------------------------------------------------------

void Text::setTextColor(const Ogre::ColourValue& _color)
{
    m_textColor = _color;

    auto material = m_overlayText->getMaterial();
    SLM_ASSERT("No material set for this Text.", material);
    ::Ogre::Technique* fontRenderTechnique = material->getTechnique(0);
    SLM_ASSERT("This Text's material has no technique.", fontRenderTechnique);
    ::Ogre::Pass* fontRenderPass = fontRenderTechnique->getPass(0);
    SLM_ASSERT("This Text's material has no pass.", fontRenderPass);

    ::Ogre::GpuProgramParametersSharedPtr passParams = fontRenderPass->getFragmentProgramParameters();
    SLM_ASSERT("The Text's material pass has no fragment shader attached.", passParams);

    passParams->setNamedConstant("u_textColor", m_textColor);
}

//------------------------------------------------------------------------------

void Text::setVisible(bool _visible)
{
    if(_visible)
    {
        m_overlayText->show();
    }
    else
    {
        m_overlayText->hide();
    }
}

//------------------------------------------------------------------------------

void Text::setTextAlignment(const ::Ogre::TextAreaOverlayElement::Alignment _hAlignement,
                            const ::Ogre::GuiVerticalAlignment _vAlignement)
{
    m_overlayText->setVerticalAlignment(_vAlignement);
    m_overlayText->setAlignment(_hAlignement);
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

    if(parentNode != nullptr)
    {
        const ::Ogre::Vector3& pos = parentNode->_getDerivedPosition();

        const auto viewProjMx = m_camera->getProjectionMatrixWithRSDepth() * m_camera->getViewMatrix();
        const auto projPos    = viewProjMx * pos;

        const ::Ogre::Vector2 screenPos(0.5f + projPos.x/2.f, 0.5f - projPos.y/2.f);
        this->setPosition(screenPos.x, screenPos.y);

        // Clipping test.
        if(projPos.z < -1.f || projPos.z > 1.f)
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

void Text::setFont(::Ogre::FontPtr _font)
{
    m_font = _font;
    m_overlayText->setFontName(_font->getName());

    auto oldMtl = m_overlayText->getMaterial();
    if(oldMtl)
    {
        ::Ogre::MaterialManager::getSingleton().remove(oldMtl);
    }

    // Clone the font material, thereby allowing each text object to have its own color.
    const ::Ogre::MaterialPtr& fontMtl = ::fwRenderOgre::helper::Font::getFontMtl(_font->getName());

    const auto ttfRes       = m_font->getTrueTypeResolution();
    const auto& textMtlName = this->getName() + "_TextMaterial_dpi" + std::to_string(ttfRes);
    ::Ogre::MaterialPtr textMtl = ::Ogre::MaterialManager::getSingleton().getByName(textMtlName);
    if(!textMtl)
    {
        textMtl = fontMtl->clone(textMtlName);
    }

    m_overlayText->setMaterial(textMtl);
    this->setTextColor(m_textColor);
}

//------------------------------------------------------------------------------

void Text::resize()
{
    const int vpHeight  = m_camera->getViewport()->getActualHeight();
    const float newSize = m_heightInPixels / static_cast<float>(vpHeight);

    m_overlayText->setCharHeight(newSize);
    this->setPosition(m_position.x, m_position.y);
}

} // namespace fwRenderOgre
