/************************************************************************
 *
 * Copyright (C) 2018-2022 IRCAD France
 * Copyright (C) 2018-2021 IHU Strasbourg
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

#pragma once

#include "viz/scene3d/config.hpp"

#include <OGRE/OgreMaterial.h>
#include <OGRE/OgreTexture.h>
#include <OGRE/Overlay/OgreFont.h>

#include <string>

namespace sight::viz::scene3d
{

namespace helper
{

/**
 * @brief Helper functions to generate fonts and materials for text rendering.
 */
class VIZ_SCENE3D_CLASS_API Font
{
public:

    /**
     * @brief Creates or retrieves the ogre font generated from truetype font with a fixed size
     * @param _trueTypeFileName truetype font file name, must be in a resource folder.
     * @param _size the font's character size in points.
     * @param _trueTypeResolution font map resolution in dots per inch.
     * @return the font generated from the file with the given size.
     */
    static VIZ_SCENE3D_API Ogre::FontPtr getFont(
        const std::string& _trueTypeFileName,
        const size_t _size,
        const std::uint32_t _trueTypeResolution = 220
    );

    /// Creates or retrieves the material used to render the specified font.
    static VIZ_SCENE3D_API Ogre::MaterialPtr getFontMtl(const std::string& _fontName);

private:

    /// Returns the font's texture map.
    static VIZ_SCENE3D_API Ogre::TexturePtr getFontMap(const std::string& _fontName);
};

} // namespace helper

} // namespace sight::viz::scene3d
