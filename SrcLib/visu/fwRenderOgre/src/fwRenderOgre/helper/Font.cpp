/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderOgre/helper/Font.hpp"

#include <OGRE/OgreMaterialManager.h>
#include <OGRE/OgrePass.h>
#include <OGRE/OgreTechnique.h>
#include <OGRE/OgreTextureManager.h>
#include <OGRE/OgreTextureUnitState.h>
#include <OGRE/Overlay/OgreFontManager.h>

namespace fwRenderOgre
{

namespace helper
{

//------------------------------------------------------------------------------

Ogre::FontPtr Font::getFont(const std::string& _trueTypeFileName, const size_t _size)
{
    // Search for ttf extension in the file name.
    const size_t extPos         = _trueTypeFileName.rfind(".ttf");
    const size_t fileNameLength = _trueTypeFileName.size() - 4;

    SLM_ASSERT(_trueTypeFileName + "doesn't seem to be a truetype font (*.ttf) file.",
               extPos == fileNameLength && _trueTypeFileName.size() > 4);

    ::Ogre::FontManager& fontManager = ::Ogre::FontManager::getSingleton();

    const std::string fontName = _trueTypeFileName.substr(0, fileNameLength) + std::to_string(_size);
    ::Ogre::FontPtr font = fontManager.getByName(fontName);

    if(!font)
    {
        font = fontManager.create(fontName, ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        font->setType(::Ogre::FontType::FT_TRUETYPE);
        font->setTrueTypeSize(_size);
        // TODO: set this according to the screen's DPI.
        font->setTrueTypeResolution(220);
        font->setAntialiasColour(false);
        font->setSource(_trueTypeFileName);
        font->load(false);
    }

    return font;
}

//------------------------------------------------------------------------------

Ogre::TexturePtr Font::getFontMap(const std::string& _fontName)
{
    ::Ogre::TexturePtr fontMap = ::Ogre::TextureManager::getSingleton().getByName(_fontName + "Texture");

    SLM_ASSERT("Could not find a font map for " + _fontName + ". Please make sure that the font is loaded.", fontMap);

    return fontMap;
}

//------------------------------------------------------------------------------

Ogre::MaterialPtr Font::getFontMtl(const std::string& _fontName)
{
    const std::string mtlName = _fontName + "TextMtl";
    ::Ogre::MaterialManager& mm = ::Ogre::MaterialManager::getSingleton();
    ::Ogre::MaterialPtr fontMtl = mm.getByName(mtlName);

    if(!fontMtl)
    {
        const auto& baseTextMtl = mm.getByName("Text");
        SLM_ASSERT("'Text' material not found, please make that the resource exists and has been loaded.", baseTextMtl);

        fontMtl = baseTextMtl->clone(mtlName);
        fontMtl->load(false);

        ::Ogre::TexturePtr fontMap = getFontMap(_fontName);

        ::Ogre::Technique* fontRenderTechnique = fontMtl->getTechnique(0);
        SLM_ASSERT("This font's material has no technique.", fontRenderTechnique);
        ::Ogre::Pass* fontRenderPass = fontRenderTechnique->getPass(0);
        SLM_ASSERT("This font's material has no pass.", fontRenderPass);
        ::Ogre::TextureUnitState* fontMapTextUnit = fontRenderPass->getTextureUnitState("fontMap");
        SLM_ASSERT("This font's pass has no texture unit named 'fontMap'.", fontRenderPass);
        fontMapTextUnit->setTexture(fontMap);
    }

    return fontMtl;
}

//------------------------------------------------------------------------------

} // namespace helper

} // namespace fwRenderOgre
