/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRENDEROGRE_UTILS_HPP__
#define __FWRENDEROGRE_UTILS_HPP__

#include <fwData/Image.hpp>

#include <OGRE/OgreRoot.h>
#include <OGRE/OgreImage.h>
#include <OGRE/OgrePixelFormat.h>
#include <OGRE/OgreTexture.h>
#include <OGRE/Overlay/OgreOverlaySystem.h>

#include <string>

#include "fwRenderOgre/config.hpp"

namespace fwRenderOgre
{
/**
 * @class Utils
 * @brief Provide some Ogre general functions for FW4SPL
 */
class FWRENDEROGRE_CLASS_API Utils
{
public:

    /**
     * @brief Utils::addRessourcesPath
     *        Add a path to load Ogre ressources from a specific path.
     *        You must edit the ressource.cfg file for each Bundle containing specific resources
     *        In this case, you can call this method in the Plugin.cpp file of this Bundle
     * @param path Relative path to the ressource.cfg file from a specific Bundle
     */
    FWRENDEROGRE_API static void addResourcesPath(const std::string& path);

    /**
     * @brief getOgreRoot
     * @return OgreRoot, if it doesn't exist initialise Ogre Root and default Ogre behavior
     */
    FWRENDEROGRE_API static ::Ogre::Root* getOgreRoot();

    /**
     * @brief destroy the OgreRoot
     */
    FWRENDEROGRE_API static void destroyOgreRoot();

    /**
     * @brief convertFwDataImageToOgreImage
     * @param imageFw The FW4SPL Image to convert
     * @return
     */
    FWRENDEROGRE_API static ::Ogre::Image convertFwDataImageToOgreImage( const ::fwData::Image::sptr imageFw);

    /**
     * @brief convertImageForNegato
     * @param _texture The target texture
     * @param _image The FW4SPL Image to convert
     * @return
     */
    FWRENDEROGRE_API static void convertImageForNegato( ::Ogre::Texture* _texture,
                                                        const ::fwData::Image::sptr& _image );

    /**
     * @brief getPixelFormatOgre
     * @param imageFw The FW4SPL Image
     * @return Pixel format of a fwData::Image
     */
    FWRENDEROGRE_API static ::Ogre::PixelFormat getPixelFormatOgre( ::fwData::Image::sptr imageFw );

    FWRENDEROGRE_API static void loadOgreTexture(::fwData::Image::sptr image, ::Ogre::TexturePtr texture,
                                                 ::Ogre::TextureType texType, ::Ogre::PixelFormat pxFormat);

    FWRENDEROGRE_API static ::Ogre::OverlaySystem* getOverlaySystem();

private:
    static ::Ogre::OverlaySystem* s_overlaySystem;
};

} // namespace fwRenderOgre

#endif // __FWRENDEROGRE_UTILS_HPP__
