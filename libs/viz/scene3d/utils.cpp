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

// cspell:ignore NOLINT

#include "viz/scene3d/utils.hpp"

#include "viz/scene3d/compositor/material_mgr_listener.hpp"
#include "viz/scene3d/detail/collision_tools.hpp"
#include "viz/scene3d/factory/r2vb_renderable.hpp"
#include "viz/scene3d/helper/camera.hpp"
#include "viz/scene3d/ogre.hpp"
#include "viz/scene3d/vr/grid_proxy_geometry.hpp"

#include <core/os/temp_path.hpp>
#include <core/runtime/path.hpp>
#include <core/spy_log.hpp>
#include <core/tools/os.hpp>

#include <data/helper/medical_image.hpp>

#include <OGRE/OgreMaterialManager.h>

#include <OgreConfigFile.h>
#include <OgreException.h>
#include <OgreHardwarePixelBuffer.h>
#include <OgreResourceGroupManager.h>
#include <OgreTextureManager.h>

#include <algorithm>
#include <cctype> // Needed for isspace()
#include <filesystem>

#if _WIN32
constexpr const char* PLUGIN_PATH = "plugins_win32.cfg";
#else
constexpr const char* PLUGIN_PATH = "plugins.cfg";
#endif

namespace sight::viz::scene3d
{

static std::list<std::string> s_module_with_resources_names;

static std::set<std::string> s_ogre_plugins;

viz::scene3d::factory::r2vb_renderable* utils::s_r2_vb_renderable_factory           = nullptr;
viz::scene3d::vr::grid_proxy_geometry_factory* utils::s_grid_proxy_geometry_factory = nullptr;
viz::scene3d::compositor::material_mgr_listener* utils::s_oit_material_listener     = nullptr;

//------------------------------------------------------------------------------

void utils::add_plugins(const std::vector<std::string>& _plugins)
{
    for(const auto& plugin : _plugins)
    {
        s_ogre_plugins.insert(plugin);
    }
}

//------------------------------------------------------------------------------

void utils::load_resources()
{
    Ogre::ConfigFile cf;
    Ogre::String resource_group_name;
    Ogre::String type_name;
    Ogre::String arch_name;

    // Ensure we always load the resources of this library first, since other may reuse our programs or shaders
    std::list<std::string> module_with_resources_names;
    std::copy(
        s_module_with_resources_names.begin(),
        s_module_with_resources_names.end(),
        std::back_inserter(module_with_resources_names)
    );
    module_with_resources_names.emplace_front("sight::viz::scene3d");

    for(const auto& module_name : module_with_resources_names)
    {
        try
        {
            // Check file existence
            const auto path = core::runtime::get_resource_file_path(module_name + "/resources.cfg");

            if(!std::filesystem::exists(path))
            {
                SIGHT_FATAL("File '" + path.string() + "' doesn't exist. Ogre needs it to load resources");
            }

            core::os::temp_file tmp_file;
            std::ofstream new_resource_file(tmp_file);

            if(!std::filesystem::exists(tmp_file))
            {
                SIGHT_FATAL("Can't create the file '" + tmp_file.string() + "'");
            }

            // Copy the resource file and make paths absolute.
            std::ifstream resource_file(path);

            make_paths_absolute("FileSystem", resource_file, new_resource_file, path.parent_path());
            resource_file.close();
            new_resource_file.close();
            cf.load(tmp_file.string());

            const Ogre::ConfigFile::SettingsBySection_ settings_by_section = cf.getSettingsBySection();

            for(const auto& s : settings_by_section)
            {
                resource_group_name = s.first;
                Ogre::ConfigFile::SettingsMultiMap settings = s.second;
                Ogre::ConfigFile::SettingsMultiMap::iterator i;
                for(i = settings.begin() ; i != settings.end() ; ++i)
                {
                    type_name = i->first;
                    arch_name = i->second;
                    Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
                        arch_name,
                        type_name,
                        resource_group_name
                    );
                }
            }
        }
        catch(Ogre::FileNotFoundException&)
        {
            SIGHT_ERROR("Unable to find Ogre resources path : " + module_name);
        }
        catch(...)
        {
            SIGHT_ERROR("Unable to load resource from " + module_name);
        }
    }
}

//------------------------------------------------------------------------------

void utils::add_resources_path(const std::string& _module_name)
{
    SIGHT_ASSERT("Empty resource path", !_module_name.empty());

    s_module_with_resources_names.push_front(_module_name);
}

//------------------------------------------------------------------------------

Ogre::Root* utils::get_ogre_root()
{
    using namespace std::literals::string_literals;

    Ogre::Root* root = Ogre::Root::getSingletonPtr();

    if(root == nullptr)
    {
        const auto& conf_path = core::runtime::get_library_resource_file_path("viz_scene3d/"s + PLUGIN_PATH);

        // Check file existence
        if(!std::filesystem::exists(conf_path))
        {
            SIGHT_FATAL("File '" + conf_path.string() + "' doesn't exist. Ogre needs it to be configured");
        }

        core::os::temp_file tmp_plugin_cfg;

        // Set the actual plugin path in the plugin config file and add application plugins.
        {
            std::ofstream new_plugin(tmp_plugin_cfg.string());

            SIGHT_FATAL_IF(
                "Can't create temporary config file'" + tmp_plugin_cfg.string() + "'",
                !std::filesystem::exists(tmp_plugin_cfg)
            );
            SIGHT_FATAL_IF("Failed to open new plugin file", !new_plugin.is_open());

            /*
             * Intermediate lambda which parses the configuration file and plugins enabled from setOgrePlugins.
             * @param confPath: path to the initial Ogre configuration file.
             * @returns std::string holding the actual configuration given to Ogre::Root constructor.
             */
            constexpr auto generate_ogre_config =
                [](const std::filesystem::path& _conf_path) -> std::string
                {
                    std::ifstream in(_conf_path.string());
                    std::stringstream plugins;
                    std::string pluginfolder;

#if defined(WIN32) && defined(DEBUG)
                    constexpr auto lib_name = "OgreMain_d";
#else
                    constexpr auto lib_name = "OgreMain";
#endif

                    constexpr std::string_view plugin_folder_token = "PluginFolder=";
                    constexpr std::string_view plugin_token        = "Plugin=";

#if defined(WIN32)
                    const std::string module = std::filesystem::weakly_canonical(
                        core::tools::os::get_shared_library_path(lib_name).remove_filename() / ".." / "plugins" / "ogre"
                    ).string();
#else
                    const std::string module =
                        core::tools::os::get_shared_library_path(lib_name).remove_filename().string();
#endif

                    // First parse config and looks for required plugin and plugin folder
                    {
                        for(std::string line ; std::getline(in, line) ; )
                        {
                            // Remove all spaces
                            {
                                line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
                            }

                            // Skip comments
                            if(line.empty() || line.front() == '#')
                            {
                                continue;
                            }

                            // Line starts with a plugin name
                            if(line.starts_with(plugin_token))
                            {
                                plugins << line << std::endl;
                                SIGHT_DEBUG("Adding " << line << " to Ogre plugins");
                            }

                            // Line starts with plugin folder path
                            if(line.starts_with(plugin_folder_token))
                            {
                                constexpr std::size_t offset = std::string_view(plugin_folder_token).size();

                                pluginfolder = line;
                                pluginfolder.insert(offset, module); //Insert the module path after "PluginFolder="
                            }
                        }
                    }

                    // Then, add application plugins
                    {
                        for(const auto& plugin : s_ogre_plugins)
                        {
                            plugins << plugin_token << plugin << std::endl;
                        }
                    }

                    SIGHT_FATAL_IF("No 'PluginFolder' folder set in " + _conf_path.string(), pluginfolder.empty());
                    std::stringstream result;
                    result << plugins.str() << std::endl << pluginfolder;

                    return result.str();
                };

            const auto ogre_config = generate_ogre_config(conf_path);

            // Write to the new plugin file
            new_plugin << ogre_config << std::endl;
        }

        root = new Ogre::Root(tmp_plugin_cfg.string());

        const Ogre::RenderSystemList& rs_list = root->getAvailableRenderers();

        Ogre::RenderSystem* rs = nullptr;

        if(!rs_list.empty())
        {
            rs = rs_list.front();
        }

        /*
           This list setup the search order for used render system.
         */
        Ogre::StringVector render_order;

        render_order.emplace_back("OpenGL");

        for(auto& iter : render_order)
        {
            for(auto* it : rs_list)
            {
                if(it->getName().find(iter) != Ogre::String::npos)
                {
                    rs = it;
                    break;
                }
            }

            if(rs != nullptr)
            {
                break;
            }
        }

        SIGHT_ASSERT("Abort render system configuration, no render system found", rs);

        rs->setConfigOption("Full Screen", "false");
        rs->setConfigOption("VSync", "false");
        rs->setConfigOption("Display Frequency", "60");
        rs->setConfigOption("Separate Shader Objects", "No");

        root->setRenderSystem(rs);

        root->initialise(false);

        load_resources();

        // Register factory for R2VB renderables objects
        s_r2_vb_renderable_factory = OGRE_NEW viz::scene3d::factory::r2vb_renderable();
        Ogre::Root::getSingleton().addMovableObjectFactory(s_r2_vb_renderable_factory);

        // Register factory for grid_proxy_geometry objects
        s_grid_proxy_geometry_factory = OGRE_NEW viz::scene3d::vr::grid_proxy_geometry_factory();
        Ogre::Root::getSingleton().addMovableObjectFactory(s_grid_proxy_geometry_factory);

        // Add the material manager listener that allows us to generate OIT techniques
        s_oit_material_listener = new viz::scene3d::compositor::material_mgr_listener();
        Ogre::MaterialManager::getSingleton().addListener(s_oit_material_listener);
    }

    return root;
}

//------------------------------------------------------------------------------

void utils::destroy_ogre_root()
{
    Ogre::MaterialManager::getSingleton().removeListener(s_oit_material_listener);
    delete s_oit_material_listener;

    Ogre::Root::getSingleton().removeMovableObjectFactory(s_grid_proxy_geometry_factory);
    delete s_grid_proxy_geometry_factory;

    Ogre::Root::getSingleton().removeMovableObjectFactory(s_r2_vb_renderable_factory);
    delete s_r2_vb_renderable_factory;

    Ogre::Root* root = viz::scene3d::utils::get_ogre_root();
    Ogre::ResourceGroupManager::getSingleton().shutdownAll();

    // Processes all dirty and pending deletion passes, needs to be done before the root deletion.
    //
    // Actually in some case, the deletion of the Ogre root may cause a crash when material resources are deleted.
    // This function deletes items in the graveyard and allows to delete the root properly.
    Ogre::Pass::processPendingPassUpdates();

    delete root;
}

//------------------------------------------------------------------------------

void utils::convert_from_ogre_texture(Ogre::TexturePtr _texture, const data::image::sptr _image, bool _flip)
{
    SIGHT_ASSERT("texture is null", _texture);
    SIGHT_ASSERT("image is null", _image);

    data::image::size_t image_size = {_texture->getWidth(), 0, 0};

    if(_texture->getHeight() > 1)
    {
        image_size[1] = _texture->getHeight();

        if(_texture->getDepth() > 1)
        {
            image_size[2] = _texture->getDepth();
        }
    }

    auto [type, format] = utils::get_pixel_format_from_ogre(_texture->getFormat());
    data::image::spacing_t spacing = {1., 1., 1.};
    data::image::origin_t origin   = {0., 0., 0.};

    _image->set_spacing(spacing);
    _image->set_origin(origin);
    _image->resize(image_size, type, format);

    // Get the pixel buffer
    Ogre::HardwarePixelBufferSharedPtr pixel_buffer = _texture->getBuffer();

    // Lock the pixel buffer and copy it
    {
        const auto dump_lock = _image->dump_lock();

        auto* __restrict dst_buffer = reinterpret_cast<std::uint8_t*>(_image->buffer());

        pixel_buffer->lock(Ogre::HardwareBuffer::HBL_READ_ONLY);
        const Ogre::PixelBox& pixel_box           = pixel_buffer->getCurrentLock();
        const std::size_t pitch                   = pixel_box.rowPitch * _image->num_components();
        const std::uint8_t* __restrict src_buffer =
            reinterpret_cast<const std::uint8_t*>(pixel_box.data) + (_flip ? pixel_box.getConsecutiveSize() : 0);

        // Copy and flip the image
        const auto num_rows = pixel_box.getConsecutiveSize() / pitch;
        if(_flip)
        {
            for(std::size_t i = 0 ; i < num_rows ; ++i)
            {
                src_buffer -= pitch;
                std::memcpy(dst_buffer, src_buffer, pitch);
                dst_buffer += pitch;
            }
        }
        else
        {
            for(std::size_t i = 0 ; i < num_rows ; ++i)
            {
                std::memcpy(dst_buffer, src_buffer, pitch);
                dst_buffer += pitch;
                src_buffer += pitch;
            }
        }

        // Unlock the pixel buffer
        pixel_buffer->unlock();
    }
}

//------------------------------------------------------------------------------

Ogre::PixelFormat utils::get_pixel_format_ogre(const data::image& _image)
{
    const core::type pixel_type           = _image.type();
    const std::size_t number_of_component = _image.num_components();

    if(number_of_component == 1)
    {
        if(pixel_type == core::type::INT8)
        {
            // int8
            return Ogre::PF_L8;
        }

        if(pixel_type == core::type::UINT8)
        {
            // uint8
            return Ogre::PF_L8;
        }

        if(pixel_type == core::type::INT16)
        {
            // int16
            return Ogre::PF_L16;
        }

        if(pixel_type == core::type::UINT16)
        {
            // uint16
            return Ogre::PF_L16;
        }

        if(pixel_type == core::type::FLOAT)
        {
            // float
            return Ogre::PF_FLOAT32_R;
        }

        if(pixel_type == core::type::INT32)
        {
            // 32 bits are not well handled in our TF approach. However, most 32bits images fits in 16 bits.
            return Ogre::PF_L16;
        }

        if(pixel_type == core::type::UINT32)
        {
            // 32 bits are not well handled in our TF approach. However, most 32bits images fits in 16 bits.
            return Ogre::PF_L16;
        }

        SIGHT_THROW("Format '" + pixel_type.name() + "' not handled");
    }

    if(number_of_component == 2)
    {
        if(pixel_type == core::type::UINT8)
        {
            // uint8
            return Ogre::PF_RG8;
        }

        if(pixel_type == core::type::INT8)
        {
            // int16
            return Ogre::PF_R8G8_SNORM;
        }

        if(pixel_type == core::type::FLOAT)
        {
            // float
            return Ogre::PF_FLOAT32_GR;
        }

        SIGHT_THROW("Format '" + pixel_type.name() + "' not handled");
    }

    // PixelFormat in little endian
    if(pixel_type == core::type::UINT8)
    {
        // uint8
        return number_of_component == 3 ? Ogre::PF_BYTE_RGB : Ogre::PF_BYTE_RGBA;
    }

    if(pixel_type == core::type::UINT16)
    {
        // uint16
        return number_of_component == 3 ? Ogre::PF_R16G16B16_UINT : Ogre::PF_R16G16B16A16_UINT;
    }

    if(pixel_type == core::type::UINT32)
    {
        // uint32
        return number_of_component == 3 ? Ogre::PF_R32G32B32_UINT : Ogre::PF_R32G32B32A32_UINT;
    }

    if(pixel_type == core::type::INT8)
    {
        // int8
        return number_of_component == 3 ? Ogre::PF_R8G8B8_SINT : Ogre::PF_R8G8B8A8_SINT;
    }

    if(pixel_type == core::type::INT16)
    {
        // int16
        return number_of_component == 3 ? Ogre::PF_R16G16B16_SINT : Ogre::PF_R16G16B16A16_SINT;
    }

    if(pixel_type == core::type::INT32)
    {
        // int32
        return number_of_component == 3 ? Ogre::PF_R32G32B32_SINT : Ogre::PF_R32G32B32A32_SINT;
    }

    if(pixel_type == core::type::FLOAT)
    {
        return number_of_component == 3 ? Ogre::PF_FLOAT32_RGB : Ogre::PF_FLOAT32_RGBA;
    }

    if(pixel_type == core::type::DOUBLE)
    {
        SIGHT_FATAL("Pixel format not handled.");
    }

    SIGHT_WARN("Pixel format not found, trying with the default 8-bits RGBA.");
    return Ogre::PF_BYTE_RGBA;
}

//------------------------------------------------------------------------------

Ogre::Vector2 utils::get_texture_window(core::type _format)
{
    static const std::map<core::type, Ogre::Vector2> s_TEXTURE_WINDOW = {
        {core::type::INT8, {std::numeric_limits<std::int8_t>::min(), std::numeric_limits<std::uint8_t>::max()}},
        {core::type::UINT8, {std::numeric_limits<std::uint8_t>::min(), std::numeric_limits<std::uint8_t>::max()}},
        {core::type::INT16, {std::numeric_limits<std::int16_t>::min(), std::numeric_limits<std::uint16_t>::max()}},
        {core::type::UINT16, {std::numeric_limits<std::uint16_t>::min(), std::numeric_limits<std::uint16_t>::max()}},
        // 32 bits are not well handled in our TF approach. However, most 32bits images fits in 16 bits.
        {core::type::INT32, {std::numeric_limits<std::int16_t>::min(), std::numeric_limits<std::uint16_t>::max()}},
        {core::type::UINT32, {std::numeric_limits<std::uint16_t>::min(), std::numeric_limits<std::uint16_t>::max()}},
    };

    SIGHT_ERROR_IF(
        "texture windowing not supported for 64 bits integers formats",
        _format == core::type::INT64 || _format == core::type::UINT64
    );

    auto it = s_TEXTURE_WINDOW.find(_format);
    if(it != s_TEXTURE_WINDOW.end())
    {
        return it->second;
    }

    return {0., 1.};
}

//------------------------------------------------------------------------------

std::pair<core::type, enum data::image::pixel_format_t> utils::get_pixel_format_from_ogre(Ogre::PixelFormat _format)
{
    enum data::image::pixel_format_t pixel_format = data::image::pixel_format_t::undefined;

    switch(_format)
    {
        case Ogre::PF_L8:
        case Ogre::PF_L16:
        case Ogre::PF_R16_UINT:
        case Ogre::PF_R32_SINT:
        case Ogre::PF_FLOAT32_R:
            pixel_format = data::image::pixel_format_t::gray_scale;
            break;

        case Ogre::PF_RG8:
        case Ogre::PF_R8G8_SNORM:
            SIGHT_FATAL("Pixel format " << _format << " not found.");
            break;

        case Ogre::PF_BYTE_RGB:
        case Ogre::PF_R8G8B8:
        case Ogre::PF_R16G16B16_UINT:
        case Ogre::PF_R32G32B32_UINT:
        case Ogre::PF_R8G8B8_SINT:
        case Ogre::PF_R16G16B16_SINT:
        case Ogre::PF_R32G32B32_SINT:
        case Ogre::PF_SHORT_RGB:
        case Ogre::PF_FLOAT32_RGB:
            pixel_format = data::image::pixel_format_t::rgb;
            break;

        case Ogre::PF_BYTE_RGBA:
        case Ogre::PF_A8R8G8B8:
        case Ogre::PF_B8G8R8A8:
        case Ogre::PF_R8G8B8A8:
        case Ogre::PF_X8R8G8B8:
        case Ogre::PF_X8B8G8R8:
        case Ogre::PF_R16G16B16A16_UINT:
        case Ogre::PF_R32G32B32A32_UINT:
        case Ogre::PF_R8G8B8A8_SINT:
        case Ogre::PF_R16G16B16A16_SINT:
        case Ogre::PF_R32G32B32A32_SINT:
        case Ogre::PF_SHORT_RGBA:
        case Ogre::PF_FLOAT32_RGBA:
            pixel_format = data::image::pixel_format_t::rgba;
            break;

        default:
            SIGHT_FATAL("Pixel format " << _format << " not found.");
    }

    core::type pixel_type;
    switch(_format)
    {
        case Ogre::PF_L8:
        case Ogre::PF_RG8:
        case Ogre::PF_R8G8B8:
        case Ogre::PF_A8R8G8B8:
        case Ogre::PF_B8G8R8A8:
        case Ogre::PF_R8G8B8A8:
        case Ogre::PF_X8R8G8B8:
        case Ogre::PF_X8B8G8R8:
        case Ogre::PF_BYTE_RGB:
        case Ogre::PF_BYTE_RGBA:
            pixel_type = core::type::UINT8;
            break;

        case Ogre::PF_R8G8_SNORM:
        case Ogre::PF_R8G8B8_SINT:
        case Ogre::PF_R8G8B8A8_SINT:
            pixel_type = core::type::INT8;
            break;

        case Ogre::PF_L16:
        case Ogre::PF_R16G16B16_UINT:
        case Ogre::PF_R16_UINT:
        case Ogre::PF_R16G16B16A16_UINT:
            pixel_type = core::type::UINT16;
            break;

        case Ogre::PF_SHORT_RGB:
        case Ogre::PF_SHORT_RGBA:
        case Ogre::PF_R16G16B16_SINT:
        case Ogre::PF_R16G16B16A16_SINT:
            pixel_type = core::type::INT16;
            break;

        case Ogre::PF_R32G32B32_UINT:
        case Ogre::PF_R32G32B32A32_UINT:
            pixel_type = core::type::UINT32;
            break;

        case Ogre::PF_R32G32B32_SINT:
        case Ogre::PF_R32_SINT:
        case Ogre::PF_R32G32B32A32_SINT:
            pixel_type = core::type::INT32;
            break;

        case Ogre::PF_FLOAT32_R:
        case Ogre::PF_FLOAT32_RGB:
        case Ogre::PF_FLOAT32_RGBA:
            pixel_type = core::type::FLOAT;
            break;

        default:
            SIGHT_ERROR("Pixel format " << _format << " not found, defaults to s_UINT8.");
            pixel_type = core::type::UINT8;
    }

    return std::make_pair(pixel_type, pixel_format);
}

//------------------------------------------------------------------------------

void utils::allocate_texture(
    Ogre::Texture* _texture,
    std::size_t _width,
    std::size_t _height,
    std::size_t _depth,
    Ogre::PixelFormat _format,
    Ogre::TextureType _tex_type,
    bool _dynamic
)
{
    auto usage = _dynamic ? Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE : Ogre::TU_STATIC_WRITE_ONLY;

    _texture->unload();

    _texture->setWidth(static_cast<Ogre::uint32>(_width));
    _texture->setHeight(static_cast<Ogre::uint32>(_height));
    _texture->setDepth(static_cast<Ogre::uint32>(_depth));
    _texture->setTextureType(_tex_type);
    _texture->setNumMipmaps(0);

    _texture->setFormat(_format);
    _texture->setUsage(usage);

    _texture->createInternalResources();
}

//------------------------------------------------------------------------------

data::color::sptr utils::from_ogre_color(const Ogre::ColourValue& _ogre_color)
{
    data::color::sptr fw_color = std::make_shared<data::color>();
    fw_color->set_rgba(_ogre_color.r, _ogre_color.g, _ogre_color.b, _ogre_color.a);

    return fw_color;
}

//------------------------------------------------------------------------------

Ogre::Matrix4 utils::to_ogre_matrix(const data::matrix4::csptr& _tm3d)
{
    std::array<Ogre::Real, 16> float_data {};
    std::transform(_tm3d->begin(), _tm3d->end(), float_data.begin(), boost::numeric_cast<float, double>);

    return Ogre::Matrix4(float_data.data());
}

//------------------------------------------------------------------------------

void utils::from_ogre_matrix(const Ogre::Matrix4& _mx, const data::matrix4::sptr& _tm3d)
{
    for(std::uint8_t l = 0 ; l < 4 ; ++l)
    {
        for(std::uint8_t c = 0 ; c < 4 ; ++c)
        {
            (*_tm3d)(l, c) = _mx[l][c];
        }
    }
}

//------------------------------------------------------------------------------

Ogre::Vector3 utils::get_ogre_spacing(const data::image& _image)
{
    const auto& spacing = _image.spacing();

    return {
        static_cast<Ogre::Real>(spacing[0]),
        static_cast<Ogre::Real>(spacing[1]),
        static_cast<Ogre::Real>(spacing[2])
    };
}

//------------------------------------------------------------------------------

Ogre::Vector3 utils::get_ogre_origin(const data::image& _image)
{
    const auto& origin = _image.origin();

    return {
        static_cast<Ogre::Real>(origin[0]),
        static_cast<Ogre::Real>(origin[1]),
        static_cast<Ogre::Real>(origin[2])
    };
}

//------------------------------------------------------------------------------

Ogre::Quaternion utils::get_ogre_orientation(const data::image& _image)
{
    const auto& orientation = _image.orientation();

    return Ogre::Matrix3 {
        static_cast<Ogre::Real>(orientation[0]),
        static_cast<Ogre::Real>(orientation[1]),
        static_cast<Ogre::Real>(orientation[2]),
        static_cast<Ogre::Real>(orientation[3]),
        static_cast<Ogre::Real>(orientation[4]),
        static_cast<Ogre::Real>(orientation[5]),
        static_cast<Ogre::Real>(orientation[6]),
        static_cast<Ogre::Real>(orientation[7]),
        static_cast<Ogre::Real>(orientation[8])
    };
}

//------------------------------------------------------------------------------

Ogre::Vector3i utils::world_to_slices(const data::image& _image, const Ogre::Vector3& _world)
{
    // Avoid 0 division
    const auto& spacing = _image.spacing();

    SIGHT_THROW_EXCEPTION_IF(
        core::exception("image spacing cannot be '0'"),
        core::is_equal(0.0, spacing[0])
        || core::is_equal(0.0, spacing[1])
        || core::is_equal(0.0, spacing[2])
    );

    const auto voxel  = _image.world_to_image(_world, true);
    const auto& sizes = _image.size();

    for(std::size_t i = 0 ; i < sizes.size() ; ++i)
    {
        if(voxel[int(i)] < 0 || voxel[int(i)] >= int(sizes[i]))
        {
            SIGHT_THROW_EXCEPTION(core::exception("Point is outside image boundaries"));
        }
    }

    return {
        static_cast<int>(voxel[0]),
        static_cast<int>(voxel[1]),
        static_cast<int>(voxel[2])
    };
}

//------------------------------------------------------------------------------

std::optional<std::pair<Ogre::MovableObject*, Ogre::Vector3> > utils::pick_object(
    int _x,
    int _y,
    std::uint32_t _query_mask,
    Ogre::SceneManager& _layer,
    bool _shift_toward_camera
)
{
    const auto* const camera = _layer.getCamera(viz::scene3d::layer::DEFAULT_CAMERA_NAME);
    const auto vp_pos        = viz::scene3d::helper::camera::convert_from_window_to_viewport_space(*camera, _x, _y);
    const Ogre::Ray vp_ray   = camera->getCameraToViewportRay(vp_pos.x, vp_pos.y);

    viz::scene3d::detail::collision_tools tool(_layer, _query_mask);

    auto&& [entityFound, rayIntersect, selectedObject, _] = tool.raycast(vp_ray, _query_mask);

    if(entityFound)
    {
        SIGHT_DEBUG("Entity find and intersect at " << rayIntersect << "(WS)");

        if(_shift_toward_camera)
        {
            rayIntersect -= vp_ray.getDirection() * 0.01F;
        }
    }
    else
    {
        return std::nullopt;
    }

    return std::make_optional(std::make_pair(selectedObject, rayIntersect));
}

//------------------------------------------------------------------------------

std::string utils::pick_image(
    const data::image& _image,
    const Ogre::Vector3& _position
)
{
    const auto picked_voxel = _image.world_to_image(_position, true, true);
    const auto intensity    = _image.get_pixel_as_string(
        data::image::index_t(picked_voxel[0]),
        data::image::index_t(picked_voxel[1]),
        data::image::index_t(picked_voxel[2])
    );

    return "(" + std::to_string(picked_voxel[0]) + ", "
           + std::to_string(picked_voxel[1]) + ", "
           + std::to_string(picked_voxel[2]) + "): "
           + intensity;
}

//------------------------------------------------------------------------------

bool utils::make_paths_absolute(
    const std::string& _key,
    std::istream& _input,
    std::ostream& _output,
    const std::filesystem::path& _module_path
)
{
    bool key_found = false;

    const std::size_t key_size = _key.size();

    for(std::string line ; std::getline(_input, line) ; )
    {
        // Remove all whitespace from the line.
        line.erase(std::remove_if(line.begin(), line.end(), isspace), line.end());

        // Skip comments, go to the next line.
        if(line[0] != '#')
        {
            if(line.substr(0, key_size) == _key)
            {
                SIGHT_FATAL_IF("Key '" + _key + "' has no value bound to it.", line.size() < key_size + 1);

                const auto current_path = std::filesystem::path(line.substr(key_size + 1));

                if(!current_path.is_absolute())
                {
                    const auto abs_path = _module_path / current_path;
                    _output << _key << "=" << abs_path.string() << std::endl;
                }
                else
                {
                    _output << line << std::endl;
                }

                key_found = true;
            }
            else
            {
                _output << line << std::endl;
            }
        }
    }

    return key_found;
}

//------------------------------------------------------------------------------

} // namespace sight::viz::scene3d
