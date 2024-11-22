/************************************************************************
 *
 * Copyright (C) 2014-2024 IRCAD France
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

#include <sight/viz/scene3d/config.hpp>

#include <data/color.hpp>
#include <data/image.hpp>
#include <data/matrix4.hpp>

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

class r2vb_renderable;
class Text;

} // namespace factory
namespace vr
{

class grid_proxy_geometry_factory;

} // namespace vr
namespace compositor
{

class material_mgr_listener;

} // namespace compositor

} // namespace sight::viz::scene3d

namespace sight::viz::scene3d
{

/**
 * @brief Provide some Ogre general functions for Sight
 */
class SIGHT_VIZ_SCENE3D_CLASS_API utils
{
public:

    /**
     *  @brief Registers Ogre Plugins to load upon root creation.
     */
    SIGHT_VIZ_SCENE3D_API static void add_plugins(const std::vector<std::string>& _plugins);

    /**
     * @brief utils::add_resources_path
     *        Add a path to load Ogre resources from a specific module.
     *        You must edit the resource.cfg file for each module containing specific resources
     *        In this case, you can call this method in the plugin.cpp file of this module
     * @param _path Relative path to the resource.cfg file from a specific module
     */
    SIGHT_VIZ_SCENE3D_API static void add_resources_path(const std::string& _path);

    /**
     * @brief getOgreRoot
     * @return OgreRoot, if it doesn't exist initialise Ogre Root and default Ogre behavior
     */
    SIGHT_VIZ_SCENE3D_API static Ogre::Root* get_ogre_root();

    /**
     * @brief destroy the OgreRoot
     */
    SIGHT_VIZ_SCENE3D_API static void destroy_ogre_root();

    /**
     * @brief Convert an Ogre texture into a Sight image data
     */
    SIGHT_VIZ_SCENE3D_API static void convert_from_ogre_texture(
        Ogre::TexturePtr _texture,
        const data::image::sptr _image_fw,
        bool _flip = false
    );

    /**
     * @brief getPixelFormatOgre
     * @param _image_fw The Sight image
     * @return Pixel format of a data::image
     */
    SIGHT_VIZ_SCENE3D_API static Ogre::PixelFormat get_pixel_format_ogre(const data::image& _image_fw);

    /**
     * @brief get the image pixel format from an Ogre pixel format
     * @param _format Pixel format of Ogre
     */
    SIGHT_VIZ_SCENE3D_API static std::pair<core::type, enum data::image::pixel_format_t> get_pixel_format_from_ogre(
        Ogre::PixelFormat _format
    );
    /**
     * @brief get the texture window that should be used to shift the pixel values in the GLSL
     * @param _format Pixel format
     * @return The window that should be used to scale pixel values
     */
    SIGHT_VIZ_SCENE3D_API static Ogre::Vector2 get_texture_window(core::type _format);

    /**
     * @brief allocateTexture
     *
     * @param[out] _texture The target texture
     * @param[in] _width width of the texture
     * @param[in] _height height of the texture
     * @param[in] _depth depth of the texture
     * @param[in] _format pixel format
     * @param[in] _tex_type texture type (Ogre::TEX_TYPE_2D, TEX_TYPE_3D, ...)
     * @param[in] _dynamic true if the image has to be modified frequently
     * @return
     */
    SIGHT_VIZ_SCENE3D_API static void allocate_texture(
        Ogre::Texture* _texture,
        std::size_t _width,
        std::size_t _height,
        std::size_t _depth,
        Ogre::PixelFormat _format,
        Ogre::TextureType _tex_type,
        bool _dynamic
    );

    /**
     * @brief convert an OgreColor into a sight::data::color
     * @param[in] _ogre_color Color to convert into a Sight color
     * @return The converted Sight color
     */
    SIGHT_VIZ_SCENE3D_API static data::color::sptr from_ogre_color(const Ogre::ColourValue& _ogre_color);

    /// Creates an ogre matrix from a Sight matrix.
    SIGHT_VIZ_SCENE3D_API static Ogre::Matrix4 to_ogre_matrix(const data::matrix4::csptr& _tm3d);

    /// Copies an ogre matrix to a Sight matrix.
    SIGHT_VIZ_SCENE3D_API static void from_ogre_matrix(const Ogre::Matrix4& _mx, const data::matrix4::sptr& _tm3d);

    /// Copies the image's spacing and origin into Ogre vectors.
    SIGHT_VIZ_SCENE3D_API static std::pair<Ogre::Vector3, Ogre::Vector3> convert_spacing_and_origin(
        const data::image::csptr& _img
    );

    /// Copies the image's spacing and origin into Ogre vectors.
    /// Version with const reference of image
    SIGHT_VIZ_SCENE3D_API static std::pair<Ogre::Vector3, Ogre::Vector3> convert_spacing_and_origin(
        const data::image& _img
    );

    /// Converts world coordinates to slices indexes of _image if possible, thrown an exception if not.
    SIGHT_VIZ_SCENE3D_API static Ogre::Vector3i world_to_slices(const data::image& _image, const Ogre::Vector3& _world);

    /**
     * @brief Pick an object from a screen-space position.
     * @param _x screen width position.
     * @param _y screen height position.
     * @param _query_mask Mask used to query entities with matching flags.
     * @param _layer scene manager where to pick objects from.
     * @return Object and intersection.
     */
    SIGHT_VIZ_SCENE3D_API static std::optional<std::pair<Ogre::MovableObject*, Ogre::Vector3> > pick_object(
        int _x,
        int _y,
        std::uint32_t _query_mask,
        Ogre::SceneManager& _layer,
        bool _shift_toward_camera = false
    );

    /**
     * @brief Pick a voxel in a 3D image at a world-space position.
     * @param _image source image.
     * @param _position 3D world-space position.
     * @param _origin image origin.
     * @param _spacing image spacing.
     * @return True if an object has been selected.
     */
    SIGHT_VIZ_SCENE3D_API static std::string pick_image(
        const data::image& _image,
        const Ogre::Vector3& _position,
        const Ogre::Vector3& _origin,
        const Ogre::Vector3& _spacing
    );

private:

    /// Parses all resources.cfg files and adds resource locations to the resource group manager.
    static void load_resources();

    /**
     * @brief Copies an ogre config stream and turns paths absolute.
     *
     * @param[in] _key Path parameter name whose value needs to be absolute.
     * @param[in] _input Input stream, must be from an ogre configuration file.
     * @param[out] _output Output stream, where the new config is copied to.
     * @return whether or not the key was found in the input.
     */
    static bool make_paths_absolute(
        const std::string& _key,
        std::istream& _input,
        std::ostream& _output,
        const std::filesystem::path& _module_path
    );

    static viz::scene3d::factory::r2vb_renderable* s_r2_vb_renderable_factory;
    static viz::scene3d::factory::Text* s_text_factory;
    static viz::scene3d::vr::grid_proxy_geometry_factory* s_grid_proxy_geometry_factory;
    static viz::scene3d::compositor::material_mgr_listener* s_oit_material_listener;
};

} // namespace sight::viz::scene3d
