/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "fwRenderOgre/config.hpp"

#include <OGRE/OgreMaterial.h>
#include <OGRE/OgreTexture.h>
#include <OGRE/Overlay/OgreFont.h>

#include <string>

namespace fwRenderOgre
{

namespace helper
{

/**
 * @brief Helper functions to generate fonts and materials for text rendering.
 */
class FWRENDEROGRE_CLASS_API Font
{
public:

    /**
     * @brief Creates or retrieves the ogre font generated from truetype font with a fixed size
     * @param _trueTypeFileName truetype font file name, must be in a resource folder.
     * @param _size the font's character size in points.
     * @return the font generated from the file with the given size.
     */
    static FWRENDEROGRE_API ::Ogre::FontPtr getFont(const std::string& _trueTypeFileName, const size_t _size);

    /// Creates or retrieves the material used to render the specified font.
    static FWRENDEROGRE_API ::Ogre::MaterialPtr getFontMtl(const std::string& _fontName);

private:

    /// Returns the font's texture map.
    static FWRENDEROGRE_API ::Ogre::TexturePtr getFontMap(const std::string& _fontName);

};

} // namespace helper

} // namespace fwRenderOgre
