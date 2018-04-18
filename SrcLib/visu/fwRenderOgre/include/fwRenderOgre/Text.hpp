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
#include <OGRE/Overlay/OgreOverlayContainer.h>
#include <OGRE/Overlay/OgreTextAreaOverlayElement.h>

namespace fwRenderOgre
{

class FWRENDEROGRE_CLASS_API Text
{
public:

    FWRENDEROGRE_API Text(const std::string& id, ::Ogre::OverlayContainer* _parent, ::Ogre::FontPtr _font);

    FWRENDEROGRE_API ~Text();

    FWRENDEROGRE_API void setText(const std::string& text);

    FWRENDEROGRE_API void setPosition(float x, float y);

private:

    ::Ogre::TextAreaOverlayElement* m_overlayText;

};

} // namespace fwRenderOgre
