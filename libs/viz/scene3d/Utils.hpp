/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
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

#include <optional>
#include <string>

namespace sight::viz::scene3d
{

namespace factory
{

class R2VBRenderable;
class Text;

} // namespace factory
namespace vr
{

class GridProxyGeometryFactory;

} // namespace vr
namespace compositor
{

class MaterialMgrListener;

} // namespace compositor

} // namespace sight::viz::scene3d

namespace sight::viz::scene3d
{

/**
 * @brief Provide some Ogre general functions for Sight
 */
class VIZ_SCENE3D_CLASS_API Utils
{
public:

    /**
     *  @brief Registers Ogre Plugins to load upon root creation.
     */
    VIZ_SCENE3D_API static void addPlugins(const std::vector<std::string>& plugins);

    /**
     * @brief Utils::addRessourcesPath
     *        Add a path to load Ogre ressources from a specific module.
     *        You must edit the ressource.cfg file for each module containing specific resources
     *        In this case, you can call this method in the plugin.cpp file of this module
     * @param path Relative path to the ressource.cfg file from a specific module
     */
    VIZ_SCENE3D_API static void addResourcesPath(const std::string& moduleName);

    /**
     * @brief getOgreRoot
     * @return OgreRoot, if it doesn't exist initialise Ogre Root and default Ogre behavior
     */
    VIZ_SCENE3D_API static Ogre::Root* getOgreRoot();

    /**
     * @brief destroy the OgreRoot
     */
    VIZ_SCENE3D_API static void destroyOgreRoot();

    /**
     * @brief Convert an Ogre texture into a Sight image data
     */
    VIZ_SCENE3D_API static void convertFromOgreTexture(
        Ogre::TexturePtr _texture,
        const data::Image::sptr _imageFw,
        bool flip = false
    );

    /**
     * @brief getPixelFormatOgre
     * @param imageFw The Sight Image
     * @return Pixel format of a data::Image
     */
    VIZ_SCENE3D_API static Ogre::PixelFormat getPixelFormatOgre(const data::Image& imageFw);

    /**
     * @brief set the pixel format of an image from an Ogre pixel format
     * @param _image The Sight Image
     * @param _format Pixel format of Ogre
     */
    VIZ_SCENE3D_API static std::pair<core::type, data::Image::PixelFormat> getPixelFormatFromOgre(
        Ogre::PixelFormat _format
    );
    /**
     * @brief get the texture window that should be used to shift the pixel values in the GLSL
     * @param _format Pixel format
     * @return The window that should be used to scale pixel values
     */
    VIZ_SCENE3D_API static Ogre::Vector2 getTextureWindow(core::type _format);

    /**
     * @brief allocateTexture
     *
     * @param[out] _texture The target texture
     * @param[in] _width width of the texture
     * @param[in] _height height of the texture
     * @param[in] _depth depth of the texture
     * @param[in] _format pixel format
     * @param[in] _texType texture type (Ogre::TEX_TYPE_2D, TEX_TYPE_3D, ...)
     * @param[in] _dynamic true if the image has to be modified frequently
     * @return
     */
    VIZ_SCENE3D_API static void allocateTexture(
        Ogre::Texture* _texture,
        std::size_t _width,
        std::size_t _height,
        std::size_t _depth,
        Ogre::PixelFormat _format,
        Ogre::TextureType _texType,
        bool _dynamic
    );

    /**
     * @brief convertOgreColorToFwColor
     * @param[in] _ogreColor Color to convert into a Sight color
     * @return The converted Sight color
     */
    VIZ_SCENE3D_API static data::Color::sptr convertOgreColorToFwColor(const Ogre::ColourValue& _ogreColor);

    /// Creates an ogre matrix from a Sight matrix.
    VIZ_SCENE3D_API static Ogre::Matrix4 convertTM3DToOgreMx(const data::Matrix4::csptr& _tm3d);

    /// Copies an ogre matrix to a Sight matrix.
    VIZ_SCENE3D_API static void copyOgreMxToTM3D(
        const Ogre::Matrix4& _mx,
        const data::Matrix4::sptr& _tm3d
    );

    /// Copies the image's spacing and origin into Ogre vectors.
    VIZ_SCENE3D_API static std::pair<Ogre::Vector3, Ogre::Vector3> convertSpacingAndOrigin(
        const data::Image::csptr& _img
    );

    /// Copies the image's spacing and origin into Ogre vectors.
    /// Version with const reference of image
    VIZ_SCENE3D_API static std::pair<Ogre::Vector3, Ogre::Vector3> convertSpacingAndOrigin(
        const data::Image& _img
    );

    /// Converts world coordinates to slices indexes of _image if possible, thrown an exception if not.
    VIZ_SCENE3D_API static Ogre::Vector3i worldToSlices(const data::Image& _image, const Ogre::Vector3& _world);

    /**
     * @brief Pick an object from a screen-space position.
     * @param _x screen width position.
     * @param _y screen height position.
     * @param _queryMask Mask used to query entities with matching flags.
     * @param _layer scene manager where to pick objects from.
     * @return Object and intersection.
     */
    VIZ_SCENE3D_API static std::optional<std::pair<Ogre::MovableObject*, Ogre::Vector3> > pickObject(
        int _x,
        int _y,
        std::uint32_t _queryMask,
        Ogre::SceneManager& _layer
    );

    /**
     * @brief Pick a voxel in a 3D image at a world-space position.
     * @param _image source image.
     * @param _position 3D world-space position.
     * @param _origin image origin.
     * @param _spacing image spacing.
     * @return True if an object has been selected.
     */
    VIZ_SCENE3D_API static std::string pickImage(
        const data::Image& _image,
        const Ogre::Vector3& position,
        const Ogre::Vector3& origin,
        const Ogre::Vector3& spacing
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
    static bool makePathsAbsolute(
        const std::string& key,
        std::istream& input,
        std::ostream& output,
        const std::filesystem::path& modulePath
    );

    static viz::scene3d::factory::R2VBRenderable* s_R2VBRenderableFactory;
    static viz::scene3d::factory::Text* s_textFactory;
    static viz::scene3d::vr::GridProxyGeometryFactory* s_gridProxyGeometryFactory;
    static viz::scene3d::compositor::MaterialMgrListener* s_oitMaterialListener;
};

} // namespace sight::viz::scene3d
