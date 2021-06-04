/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include <data/Color.hpp>
#include <data/Image.hpp>
#include <data/Matrix4.hpp>

#include <OGRE/OgreColourValue.h>
#include <OGRE/OgreImage.h>
#include <OGRE/OgrePixelFormat.h>
#include <OGRE/OgreRoot.h>
#include <OGRE/OgreTexture.h>
#include <OGRE/Overlay/OgreOverlaySystem.h>

#include <string>

namespace sight::viz::scene3d
{

namespace factory
{

class R2VBRenderable;
class Text;

}
namespace vr
{

class GridProxyGeometryFactory;

}
namespace compositor
{

class MaterialMgrListener;

}

}

namespace sight::viz::scene3d
{

/**
 * @brief Provide some Ogre general functions for Sight
 */
class VIZ_SCENE3D_CLASS_API Utils
{
public:

    /**
     * @brief Utils::addRessourcesPath
     *        Add a path to load Ogre ressources from a specific path.
     *        You must edit the ressource.cfg file for each module containing specific resources
     *        In this case, you can call this method in the Plugin.cpp file of this module
     * @param path Relative path to the ressource.cfg file from a specific module
     */
    VIZ_SCENE3D_API static void addResourcesPath(const std::filesystem::path& path);

    /**
     * @brief getOgreRoot
     * @return OgreRoot, if it doesn't exist initialise Ogre Root and default Ogre behavior
     */
    VIZ_SCENE3D_API static ::Ogre::Root* getOgreRoot();

    /**
     * @brief destroy the OgreRoot
     */
    VIZ_SCENE3D_API static void destroyOgreRoot();

    /**
     * @brief Convert an Sight image data into an ogre image
     * @param imageFw The Sight Image to convert
     * @return Ogre image
     */
    VIZ_SCENE3D_API static ::Ogre::Image convertToOgreImage(const data::Image::csptr imageFw);

    /**
     * @brief Convert an Ogre texture into a Sight image data
     */
    VIZ_SCENE3D_API static void convertFromOgreTexture(
        ::Ogre::TexturePtr _texture,
        const data::Image::sptr _imageFw,
        bool flip = false
    );

    /**
     * @brief getPixelFormatOgre
     * @param imageFw The Sight Image
     * @return Pixel format of a data::Image
     */
    VIZ_SCENE3D_API static ::Ogre::PixelFormat getPixelFormatOgre(data::Image::csptr imageFw);

    /**
     * @brief set the pixel format of an image from an Ogre pixel format
     * @param _image The Sight Image
     * @param _format Pixel format of Ogre
     */
    VIZ_SCENE3D_API static void setPixelFormatFromOgre(data::Image::sptr _image, ::Ogre::PixelFormat _format);

    /**
     * @brief loadOgreTexture
     * @param _image The Sight Image to convert
     * @param _texture The target texture
     * @param _texType Type of the texture (::Ogre::TEX_TYPE_2D, ::Ogre::TEX_TYPE_3D, ...)
     * @param _dynamic
     */
    VIZ_SCENE3D_API static void loadOgreTexture(
        const data::Image::csptr& _image,
        ::Ogre::TexturePtr _texture,
        ::Ogre::TextureType _texType,
        bool _dynamic
    );

    /**
     * @brief convertImageForNegato
     * @param[out] _textureThe target texture
     * @param[in] _image The Sight Image to convert
     * @return
     */
    VIZ_SCENE3D_API static void convertImageForNegato(::Ogre::Texture* _texture, const data::Image::sptr& _image);

    /**
     * @brief allocateTexture
     *
     * @param[out] _texture The target texture
     * @param[in] _width width of the texture
     * @param[in] _height height of the texture
     * @param[in] _depth depth of the texture
     * @param[in] _format pixel format
     * @param[in] _texType texture type (::OGRE::TEX_TYPE_2D, TEX_TYPE_3D, ...)
     * @param[in] _dynamic true if the image has to be modified frequently
     * @return
     */
    VIZ_SCENE3D_API static void allocateTexture(
        ::Ogre::Texture* _texture,
        size_t _width,
        size_t _height,
        size_t _depth,
        ::Ogre::PixelFormat _format,
        ::Ogre::TextureType _texType,
        bool _dynamic
    );

    VIZ_SCENE3D_API static ::Ogre::OverlaySystem* getOverlaySystem();

    /**
     * @brief convertOgreColorToFwColor
     * @param[in] _ogreColor Color to convert into a Sight color
     * @return The converted Sight color
     */
    VIZ_SCENE3D_API static data::Color::sptr convertOgreColorToFwColor(const Ogre::ColourValue& _ogreColor);

    /// Creates an ogre matrix from a Sight matrix.
    VIZ_SCENE3D_API static ::Ogre::Matrix4 convertTM3DToOgreMx(const data::Matrix4::csptr& _tm3d);

    /// Copies an ogre matrix to a Sight matrix.
    VIZ_SCENE3D_API static void copyOgreMxToTM3D(
        const ::Ogre::Matrix4& _mx,
        const data::Matrix4::sptr& _tm3d
    );

    /// Copies the image's spacing and origin into Ogre vectors.
    VIZ_SCENE3D_API static std::pair< ::Ogre::Vector3, ::Ogre::Vector3> convertSpacingAndOrigin(
        const data::Image::csptr& _img
    );

private:

    /// Parses all resources.cfg files and adds resource locations to the resource group manager.
    static void loadResources();

    /**
     * @brief Copies an ogre config stream and turns paths absolute.
     *
     * @param[in] key Path parameter name whose value needs to be absolute.
     * @param[in] input Input stream, must be from an ogre configuration file.
     * @param[out] output Output stream, where the new config is copied to.
     * @return whether or not the key was found in the input.
     */
    static bool makePathsAbsolute(const std::string& key, std::istream& input, std::ostream& output);

    static ::Ogre::OverlaySystem* s_overlaySystem;

    static viz::scene3d::factory::R2VBRenderable* s_R2VBRenderableFactory;
    static viz::scene3d::factory::Text* s_textFactory;
    static viz::scene3d::vr::GridProxyGeometryFactory* s_gridProxyGeometryFactory;
    static viz::scene3d::compositor::MaterialMgrListener* s_oitMaterialListener;
};

} // namespace sight::viz::scene3d
