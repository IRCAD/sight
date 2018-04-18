/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderOgre/Text.hpp"

#include "fwRenderOgre/helper/Font.hpp"

#include <OGRE/Overlay/OgreOverlayManager.h>

namespace fwRenderOgre
{

//------------------------------------------------------------------------------

Text::Text(const std::string& id, ::Ogre::OverlayContainer* _parent, ::Ogre::FontPtr _font)
{
    auto& overlayManager = ::Ogre::OverlayManager::getSingleton();

    m_overlayText = dynamic_cast< ::Ogre::TextAreaOverlayElement*>(overlayManager.createOverlayElement("TextArea", id));

    m_overlayText->setDimensions(1.0f, 1.0f);
    m_overlayText->setMetricsMode(Ogre::GMM_RELATIVE);
    m_overlayText->setCharHeight(0.03f);
    m_overlayText->setPosition(0.5, 0.5);

    m_overlayText->setFontName(_font->getName());
    m_overlayText->setMaterial(::fwRenderOgre::helper::Font::getFontMtl(_font->getName()));

    _parent->addChild(m_overlayText);
}

//------------------------------------------------------------------------------

Text::~Text()
{

}

//------------------------------------------------------------------------------

void Text::setText(const std::string& text)
{
    m_overlayText->setCaption(text);
}

//------------------------------------------------------------------------------

} // namespace fwRenderOgre
