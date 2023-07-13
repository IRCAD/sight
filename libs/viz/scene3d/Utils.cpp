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

// cspell:ignore NOLINT

#include "viz/scene3d/Utils.hpp"

#include "viz/scene3d/compositor/MaterialMgrListener.hpp"
#include "viz/scene3d/detail/CollisionTools.hpp"
#include "viz/scene3d/factory/R2VBRenderable.hpp"
#include "viz/scene3d/helper/Camera.hpp"
#include "viz/scene3d/ogre.hpp"
#include "viz/scene3d/vr/GridProxyGeometry.hpp"

#include <core/os/TempPath.hpp>
#include <core/runtime/path.hpp>
#include <core/spyLog.hpp>
#include <core/tools/Os.hpp>

#include <data/helper/MedicalImage.hpp>

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

static std::list<std::string> s_moduleWithResourcesNames;

static std::set<std::string> s_ogrePlugins;

viz::scene3d::factory::R2VBRenderable* Utils::s_R2VBRenderableFactory         = nullptr;
viz::scene3d::vr::GridProxyGeometryFactory* Utils::s_gridProxyGeometryFactory = nullptr;
viz::scene3d::compositor::MaterialMgrListener* Utils::s_oitMaterialListener   = nullptr;

//------------------------------------------------------------------------------

void Utils::addPlugins(const std::vector<std::string>& plugins)
{
    for(const auto& plugin : plugins)
    {
        s_ogrePlugins.insert(plugin);
    }
}

//------------------------------------------------------------------------------

void Utils::loadResources()
{
    Ogre::ConfigFile cf;
    Ogre::String resourceGroupName;
    Ogre::String typeName;
    Ogre::String archName;

    // Ensure we always load the resources of this library first, since other may reuse our programs or shaders
    std::list<std::string> moduleWithResourcesNames;
    std::copy(
        s_moduleWithResourcesNames.begin(),
        s_moduleWithResourcesNames.end(),
        std::back_inserter(moduleWithResourcesNames)
    );
    moduleWithResourcesNames.push_front("sight::viz::scene3d");

    for(const auto& moduleName : moduleWithResourcesNames)
    {
        try
        {
            // Check file existence
            const auto path = core::runtime::getResourceFilePath(moduleName + "/resources.cfg");

            if(!std::filesystem::exists(path))
            {
                SIGHT_FATAL("File '" + path.string() + "' doesn't exist. Ogre needs it to load resources");
            }

            core::os::TempFile tmpFile;
            std::ofstream newResourceFile(tmpFile);

            if(!std::filesystem::exists(tmpFile))
            {
                SIGHT_FATAL("Can't create the file '" + tmpFile.string() + "'");
            }

            // Copy the resource file and make paths absolute.
            std::ifstream resourceFile(path);

            makePathsAbsolute("FileSystem", resourceFile, newResourceFile, path.parent_path());
            resourceFile.close();
            newResourceFile.close();
            cf.load(tmpFile.string());

            const Ogre::ConfigFile::SettingsBySection_ settings_by_section = cf.getSettingsBySection();

            for(const auto& s : settings_by_section)
            {
                resourceGroupName = s.first;
                Ogre::ConfigFile::SettingsMultiMap settings = s.second;
                Ogre::ConfigFile::SettingsMultiMap::iterator i;
                for(i = settings.begin() ; i != settings.end() ; ++i)
                {
                    typeName = i->first;
                    archName = i->second;
                    Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
                        archName,
                        typeName,
                        resourceGroupName
                    );
                }
            }
        }
        catch(Ogre::FileNotFoundException&)
        {
            SIGHT_ERROR("Unable to find Ogre resources path : " + moduleName);
        }
        catch(...)
        {
            SIGHT_ERROR("Unable to load resource from " + moduleName);
        }
    }
}

//------------------------------------------------------------------------------

void Utils::addResourcesPath(const std::string& moduleName)
{
    SIGHT_ASSERT("Empty resource path", !moduleName.empty());

    s_moduleWithResourcesNames.push_front(moduleName);
}

//------------------------------------------------------------------------------

Ogre::Root* Utils::getOgreRoot()
{
    using namespace std::literals::string_literals;

    Ogre::Root* root = Ogre::Root::getSingletonPtr();

    if(root == nullptr)
    {
        const auto& confPath = core::runtime::getLibraryResourceFilePath("viz_scene3d/"s + PLUGIN_PATH);

        // Check file existence
        if(!std::filesystem::exists(confPath))
        {
            SIGHT_FATAL("File '" + confPath.string() + "' doesn't exist. Ogre needs it to be configured");
        }

        core::os::TempFile tmpPluginCfg;

        // Set the actual plugin path in the plugin config file and add application plugins.
        {
            std::ofstream newPlugin(tmpPluginCfg.string());

            SIGHT_FATAL_IF(
                "Can't create temporary config file'" + tmpPluginCfg.string() + "'",
                !std::filesystem::exists(tmpPluginCfg)
            );
            SIGHT_FATAL_IF("Failed to open new plugin file", !newPlugin.is_open());

            /*
             * Intermediate lambda which parses the configuration file and plugins enabled from setOgrePlugins.
             * @param confPath: path to the initial Ogre configuration file.
             * @returns std::string holding the actual configuration given to Ogre::Root constructor.
             */
            constexpr auto generateOgreConfig =
                [](const std::filesystem::path& confPath) -> std::string
                {
                    std::ifstream in(confPath.string());
                    std::stringstream plugins;
                    std::string pluginfolder;

#if defined(WIN32) && defined(DEBUG)
                    constexpr auto libName = "OgreMain_d";
#else
                    constexpr auto libName = "OgreMain";
#endif

                    constexpr std::string_view pluginFolderToken = "PluginFolder=";
                    constexpr std::string_view pluginToken       = "Plugin=";

                    const std::string module =
                        core::tools::os::getSharedLibraryPath(libName).remove_filename().string();

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
                            if(line.rfind(pluginToken, 0) == 0)
                            {
                                plugins << line << std::endl;
                                SIGHT_DEBUG("Adding " << line << " to Ogre plugins");
                            }

                            // Line starts with plugin folder path
                            if(line.rfind(pluginFolderToken, 0) == 0)
                            {
                                constexpr std::size_t offset = std::string_view(pluginFolderToken).size();

                                pluginfolder = line;
                                pluginfolder.insert(offset, module); //Insert the module path after "PluginFolder="
                            }
                        }
                    }

                    // Then, add application plugins
                    {
                        for(const auto& plugin : s_ogrePlugins)
                        {
                            plugins << pluginToken << plugin << std::endl;
                        }
                    }

                    SIGHT_FATAL_IF("No 'PluginFolder' folder set in " + confPath.string(), pluginfolder.empty());
                    std::stringstream result;
                    result << plugins.str() << std::endl << pluginfolder;

                    return result.str();
                };

            const auto ogreConfig = generateOgreConfig(confPath);

            // Write to the new plugin file
            newPlugin << ogreConfig << std::endl;
        }

        root = new Ogre::Root(tmpPluginCfg.string());

        const Ogre::RenderSystemList& rsList = root->getAvailableRenderers();

        Ogre::RenderSystem* rs = nullptr;

        if(!rsList.empty())
        {
            rs = rsList.front();
        }

        /*
           This list setup the search order for used render system.
         */
        Ogre::StringVector renderOrder;

        renderOrder.push_back("OpenGL");

        for(auto& iter : renderOrder)
        {
            for(auto* it : rsList)
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

        root->setRenderSystem(rs);

        root->initialise(false);

        loadResources();

        // Register factory for R2VB renderables objects
        s_R2VBRenderableFactory = OGRE_NEW viz::scene3d::factory::R2VBRenderable();
        Ogre::Root::getSingleton().addMovableObjectFactory(s_R2VBRenderableFactory);

        // Register factory for GridProxyGeometry objects
        s_gridProxyGeometryFactory = OGRE_NEW viz::scene3d::vr::GridProxyGeometryFactory();
        Ogre::Root::getSingleton().addMovableObjectFactory(s_gridProxyGeometryFactory);

        // Add the material manager listener that allows us to generate OIT techniques
        s_oitMaterialListener = new viz::scene3d::compositor::MaterialMgrListener();
        Ogre::MaterialManager::getSingleton().addListener(s_oitMaterialListener);
    }

    return root;
}

//------------------------------------------------------------------------------

void Utils::destroyOgreRoot()
{
    Ogre::MaterialManager::getSingleton().removeListener(s_oitMaterialListener);
    delete s_oitMaterialListener;

    Ogre::Root::getSingleton().removeMovableObjectFactory(s_gridProxyGeometryFactory);
    delete s_gridProxyGeometryFactory;

    Ogre::Root::getSingleton().removeMovableObjectFactory(s_R2VBRenderableFactory);
    delete s_R2VBRenderableFactory;

    Ogre::Root* root = viz::scene3d::Utils::getOgreRoot();
    Ogre::ResourceGroupManager::getSingleton().shutdownAll();

    // Processes all dirty and pending deletion passes, needs to be done before the root deletion.
    //
    // Actually in some case, the deletion of the Ogre root may cause a crash when material resources are deleted.
    // This function deletes items in the graveyard and allows to delete the root properly.
    Ogre::Pass::processPendingPassUpdates();

    delete root;
}

//------------------------------------------------------------------------------

void Utils::convertFromOgreTexture(Ogre::TexturePtr _texture, const data::Image::sptr _image, bool flip)
{
    SIGHT_ASSERT("Texture is null", _texture);
    SIGHT_ASSERT("Image is null", _image);

    data::Image::Size imageSize = {_texture->getWidth(), 0, 0};

    if(_texture->getHeight() > 1)
    {
        imageSize[1] = _texture->getHeight();

        if(_texture->getDepth() > 1)
        {
            imageSize[2] = _texture->getDepth();
        }
    }

    auto [type, format] = Utils::getPixelFormatFromOgre(_texture->getFormat());
    data::Image::Spacing spacing = {1., 1., 1.};
    data::Image::Origin origin   = {0., 0., 0.};

    _image->setSpacing(spacing);
    _image->setOrigin(origin);
    _image->resize(imageSize, type, format);

    // Get the pixel buffer
    Ogre::HardwarePixelBufferSharedPtr pixelBuffer = _texture->getBuffer();

    // Lock the pixel buffer and copy it
    {
        const auto dumpLock = _image->dump_lock();

        auto* __restrict dstBuffer = reinterpret_cast<std::uint8_t*>(_image->getBuffer());

        pixelBuffer->lock(Ogre::HardwareBuffer::HBL_READ_ONLY);
        const Ogre::PixelBox& pixelBox           = pixelBuffer->getCurrentLock();
        const std::size_t pitch                  = pixelBox.rowPitch * _image->numComponents();
        const std::uint8_t* __restrict srcBuffer =
            reinterpret_cast<const std::uint8_t*>(pixelBox.data) + (flip ? pixelBox.getConsecutiveSize() : 0);

        // Copy and flip the image
        const auto numRows = pixelBox.getConsecutiveSize() / pitch;
        if(flip)
        {
            for(std::size_t i = 0 ; i < numRows ; ++i)
            {
                srcBuffer -= pitch;
                std::memcpy(dstBuffer, srcBuffer, pitch);
                dstBuffer += pitch;
            }
        }
        else
        {
            for(std::size_t i = 0 ; i < numRows ; ++i)
            {
                std::memcpy(dstBuffer, srcBuffer, pitch);
                dstBuffer += pitch;
                srcBuffer += pitch;
            }
        }

        // Unlock the pixel buffer
        pixelBuffer->unlock();
    }
}

//------------------------------------------------------------------------------

Ogre::PixelFormat Utils::getPixelFormatOgre(const data::Image& _image)
{
    const core::Type pixelType          = _image.getType();
    const std::size_t numberOfComponent = _image.numComponents();

    if(numberOfComponent == 1)
    {
        if(pixelType == core::Type::INT8)
        {
            // int8
            return Ogre::PF_L8;
        }

        if(pixelType == core::Type::UINT8)
        {
            // uint8
            return Ogre::PF_L8;
        }

        if(pixelType == core::Type::INT16)
        {
            // int16
            return Ogre::PF_L16;
        }

        if(pixelType == core::Type::UINT16)
        {
            // uint16
            return Ogre::PF_L16;
        }

        if(pixelType == core::Type::FLOAT)
        {
            // float
            return Ogre::PF_FLOAT32_R;
        }

        if(pixelType == core::Type::INT32)
        {
            // 32 bits are not well handled in our TF approach. However, most 32bits images fits in 16 bits.
            return Ogre::PF_L16;
        }

        if(pixelType == core::Type::UINT32)
        {
            // 32 bits are not well handled in our TF approach. However, most 32bits images fits in 16 bits.
            return Ogre::PF_L16;
        }

        SIGHT_THROW("Format '" + pixelType.name() + "' not handled");
    }

    if(numberOfComponent == 2)
    {
        if(pixelType == core::Type::UINT8)
        {
            // uint8
            return Ogre::PF_RG8;
        }

        if(pixelType == core::Type::INT8)
        {
            // int16
            return Ogre::PF_R8G8_SNORM;
        }

        if(pixelType == core::Type::FLOAT)
        {
            // float
            return Ogre::PF_FLOAT32_GR;
        }

        SIGHT_THROW("Format '" + pixelType.name() + "' not handled");
    }

    // PixelFormat in little endian
    if(pixelType == core::Type::UINT8)
    {
        // uint8
        return numberOfComponent == 3 ? Ogre::PF_BYTE_RGB : Ogre::PF_BYTE_RGBA;
    }

    if(pixelType == core::Type::UINT16)
    {
        // uint16
        return numberOfComponent == 3 ? Ogre::PF_R16G16B16_UINT : Ogre::PF_R16G16B16A16_UINT;
    }

    if(pixelType == core::Type::UINT32)
    {
        // uint32
        return numberOfComponent == 3 ? Ogre::PF_R32G32B32_UINT : Ogre::PF_R32G32B32A32_UINT;
    }

    if(pixelType == core::Type::INT8)
    {
        // int8
        return numberOfComponent == 3 ? Ogre::PF_R8G8B8_SINT : Ogre::PF_R8G8B8A8_SINT;
    }

    if(pixelType == core::Type::INT16)
    {
        // int16
        return numberOfComponent == 3 ? Ogre::PF_R16G16B16_SINT : Ogre::PF_R16G16B16A16_SINT;
    }

    if(pixelType == core::Type::INT32)
    {
        // int32
        return numberOfComponent == 3 ? Ogre::PF_R32G32B32_SINT : Ogre::PF_R32G32B32A32_SINT;
    }

    if(pixelType == core::Type::FLOAT)
    {
        return numberOfComponent == 3 ? Ogre::PF_FLOAT32_RGB : Ogre::PF_FLOAT32_RGBA;
    }

    if(pixelType == core::Type::DOUBLE)
    {
        SIGHT_FATAL("Pixel format not handled.");
    }

    SIGHT_WARN("Pixel format not found, trying with the default 8-bits RGBA.");
    return Ogre::PF_BYTE_RGBA;
}

//------------------------------------------------------------------------------

Ogre::Vector2 Utils::getTextureWindow(core::Type _format)
{
    static const std::map<core::Type, Ogre::Vector2> textureWindow = {
        {core::Type::INT8, {std::numeric_limits<std::int8_t>::min(), std::numeric_limits<std::uint8_t>::max()}},
        {core::Type::UINT8, {std::numeric_limits<std::uint8_t>::min(), std::numeric_limits<std::uint8_t>::max()}},
        {core::Type::INT16, {std::numeric_limits<std::int16_t>::min(), std::numeric_limits<std::uint16_t>::max()}},
        {core::Type::UINT16, {std::numeric_limits<std::uint16_t>::min(), std::numeric_limits<std::uint16_t>::max()}},
        // 32 bits are not well handled in our TF approach. However, most 32bits images fits in 16 bits.
        {core::Type::INT32, {std::numeric_limits<std::int16_t>::min(), std::numeric_limits<std::uint16_t>::max()}},
        {core::Type::UINT32, {std::numeric_limits<std::uint16_t>::min(), std::numeric_limits<std::uint16_t>::max()}},
    };

    SIGHT_ERROR_IF(
        "Texture windowing not supported for 64 bits integers formats",
        _format == core::Type::INT64 || _format == core::Type::UINT64
    );

    auto it = textureWindow.find(_format);
    if(it != textureWindow.end())
    {
        return it->second;
    }

    return {0., 1.};
}

//------------------------------------------------------------------------------

std::pair<core::Type, data::Image::PixelFormat> Utils::getPixelFormatFromOgre(Ogre::PixelFormat _format)
{
    data::Image::PixelFormat pixelFormat = data::Image::PixelFormat::UNDEFINED;

    switch(_format)
    {
        case Ogre::PF_L8:
        case Ogre::PF_L16:
        case Ogre::PF_R16_UINT:
        case Ogre::PF_R32_SINT:
        case Ogre::PF_FLOAT32_R:
            pixelFormat = data::Image::PixelFormat::GRAY_SCALE;
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
            pixelFormat = data::Image::PixelFormat::RGB;
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
            pixelFormat = data::Image::PixelFormat::RGBA;
            break;

        default:
            SIGHT_FATAL("Pixel format " << _format << " not found.");
    }

    core::Type pixelType;
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
            pixelType = core::Type::UINT8;
            break;

        case Ogre::PF_R8G8_SNORM:
        case Ogre::PF_R8G8B8_SINT:
        case Ogre::PF_R8G8B8A8_SINT:
            pixelType = core::Type::INT8;
            break;

        case Ogre::PF_L16:
        case Ogre::PF_R16G16B16_UINT:
        case Ogre::PF_R16_UINT:
        case Ogre::PF_R16G16B16A16_UINT:
            pixelType = core::Type::UINT16;
            break;

        case Ogre::PF_SHORT_RGB:
        case Ogre::PF_SHORT_RGBA:
        case Ogre::PF_R16G16B16_SINT:
        case Ogre::PF_R16G16B16A16_SINT:
            pixelType = core::Type::INT16;
            break;

        case Ogre::PF_R32G32B32_UINT:
        case Ogre::PF_R32G32B32A32_UINT:
            pixelType = core::Type::UINT32;
            break;

        case Ogre::PF_R32G32B32_SINT:
        case Ogre::PF_R32_SINT:
        case Ogre::PF_R32G32B32A32_SINT:
            pixelType = core::Type::INT32;
            break;

        case Ogre::PF_FLOAT32_R:
        case Ogre::PF_FLOAT32_RGB:
        case Ogre::PF_FLOAT32_RGBA:
            pixelType = core::Type::FLOAT;
            break;

        default:
            SIGHT_ERROR("Pixel format " << _format << " not found, defaults to s_UINT8.");
            pixelType = core::Type::UINT8;
    }

    return std::make_pair(pixelType, pixelFormat);
}

//------------------------------------------------------------------------------

void Utils::allocateTexture(
    Ogre::Texture* _texture,
    std::size_t _width,
    std::size_t _height,
    std::size_t _depth,
    Ogre::PixelFormat _format,
    Ogre::TextureType _texType,
    bool _dynamic
)
{
    auto usage = _dynamic ? Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE : Ogre::TU_STATIC_WRITE_ONLY;

    _texture->freeInternalResources();

    _texture->setWidth(static_cast<Ogre::uint32>(_width));
    _texture->setHeight(static_cast<Ogre::uint32>(_height));
    _texture->setDepth(static_cast<Ogre::uint32>(_depth));
    _texture->setTextureType(_texType);
    _texture->setNumMipmaps(0);

    _texture->setFormat(_format);
    _texture->setUsage(usage);

    _texture->createInternalResources();
}

//------------------------------------------------------------------------------

data::Color::sptr Utils::convertOgreColorToFwColor(const Ogre::ColourValue& _ogreColor)
{
    data::Color::sptr fwColor = data::Color::New();
    fwColor->setRGBA(_ogreColor.r, _ogreColor.g, _ogreColor.b, _ogreColor.a);

    return fwColor;
}

//------------------------------------------------------------------------------

Ogre::Matrix4 Utils::convertTM3DToOgreMx(const data::Matrix4::csptr& _tm3d)
{
    std::array<Ogre::Real, 16> floatData {};
    std::transform(_tm3d->begin(), _tm3d->end(), floatData.begin(), boost::numeric_cast<float, double>);

    return Ogre::Matrix4(floatData.data());
}

//------------------------------------------------------------------------------

void Utils::copyOgreMxToTM3D(const Ogre::Matrix4& _mx, const data::Matrix4::sptr& _tm3d)
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

std::pair<Ogre::Vector3, Ogre::Vector3> Utils::convertSpacingAndOrigin(const data::Image::csptr& _img)
{
    return Utils::convertSpacingAndOrigin(*_img);
}

//------------------------------------------------------------------------------

std::pair<Ogre::Vector3, Ogre::Vector3> Utils::convertSpacingAndOrigin(const data::Image& _img)
{
    const auto& imgOrigin = _img.getOrigin();
    const Ogre::Vector3 origin(static_cast<float>(imgOrigin[0]),
                               static_cast<float>(imgOrigin[1]),
                               static_cast<float>(imgOrigin[2]));

    const auto& imgSpacing = _img.getSpacing();
    const Ogre::Vector3 spacing(static_cast<float>(imgSpacing[0]),
                                static_cast<float>(imgSpacing[1]),
                                static_cast<float>(imgSpacing[2]));

    return std::make_pair(spacing, origin);
}

//------------------------------------------------------------------------------

Ogre::Vector3i Utils::worldToSlices(const data::Image& _image, const Ogre::Vector3& _world)
{
    const auto [spacing, origin] = sight::viz::scene3d::Utils::convertSpacingAndOrigin(_image);

    // avoid 0 division

    SIGHT_THROW_EXCEPTION_IF(
        core::Exception("Image spacing cannot be '0'"),
        spacing[0] == 0.F || spacing[1] == 0.F || spacing[2] == 0.F
    );

    const auto point = (_world - origin) / spacing;

    const Ogre::Vector3i slices_idx(point);

    // Ensure that the point is within bounds of the image, do nothing otherwise.
    std::array<std::pair<int, int>, 3> boundaries = {{
        {0, static_cast<int>(_image.getSize()[1])},
        {0, static_cast<int>(_image.getSize()[0])},
        {0, static_cast<int>(_image.getSize()[2])}
    }
    };

    size_t i = 0;
    for(const auto& [min, max] : boundaries)
    {
        if(slices_idx[i] < min || slices_idx[i] > max)
        {
            SIGHT_THROW_EXCEPTION(core::Exception("Point is outside image boundaries"));
        }

        ++i;
    }

    return slices_idx;
}

//------------------------------------------------------------------------------

std::optional<std::pair<Ogre::MovableObject*, Ogre::Vector3> > Utils::pickObject(
    int _x,
    int _y,
    std::uint32_t _queryMask,
    Ogre::SceneManager& _layer
)
{
    const auto* const camera = _layer.getCamera(viz::scene3d::Layer::s_DEFAULT_CAMERA_NAME);
    const auto vpPos         = viz::scene3d::helper::Camera::convertFromWindowToViewportSpace(*camera, _x, _y);
    const Ogre::Ray vpRay    = camera->getCameraToViewportRay(vpPos.x, vpPos.y);

    viz::scene3d::detail::CollisionTools tool(_layer, _queryMask);

    auto&& [entityFound, rayIntersect, selectedObject, _] = tool.raycast(vpRay, _queryMask);

    if(entityFound)
    {
        SIGHT_DEBUG("Entity find and intersect at " << rayIntersect << "(WS)");
    }
    else
    {
        return std::nullopt;
    }

    return std::make_optional(std::make_pair(selectedObject, rayIntersect));
}

//------------------------------------------------------------------------------

std::string Utils::pickImage(
    const data::Image& _image,
    const Ogre::Vector3& _position,
    const Ogre::Vector3& _origin,
    const Ogre::Vector3& _spacing
)
{
    const auto pickedPosImageSpace = (_position - _origin) / _spacing;
    const auto& imgSize            = _image.getSize();
    data::Image::Size pickedVoxel;
    for(size_t i = 0 ; i < pickedVoxel.size() ; ++i)
    {
        pickedVoxel[i] = std::clamp(static_cast<std::size_t>(pickedPosImageSpace[i]), std::size_t(0), imgSize[i] - 1);
    }

    const auto intensity = _image.getPixelAsString(pickedVoxel[0], pickedVoxel[1], pickedVoxel[2]);
    return "(" + std::to_string(pickedVoxel[0]) + ", " + std::to_string(pickedVoxel[1])
           + ", " + std::to_string(pickedVoxel[2]) + "): " + intensity;
}

//------------------------------------------------------------------------------

bool Utils::makePathsAbsolute(
    const std::string& key,
    std::istream& input,
    std::ostream& output,
    const std::filesystem::path& modulePath
)
{
    bool keyFound = false;

    const std::size_t keySize = key.size();

    for(std::string line ; std::getline(input, line) ; )
    {
        // Remove all whitespace from the line.
        line.erase(std::remove_if(line.begin(), line.end(), isspace), line.end());

        // Skip comments, go to the next line.
        if(line[0] != '#')
        {
            if(line.substr(0, keySize) == key)
            {
                SIGHT_FATAL_IF("Key '" + key + "' has no value bound to it.", line.size() < keySize + 1);

                const auto currentPath = std::filesystem::path(line.substr(keySize + 1));

                if(!currentPath.is_absolute())
                {
                    const auto absPath = modulePath / currentPath;
                    output << key << "=" << absPath.string() << std::endl;
                }
                else
                {
                    output << line << std::endl;
                }

                keyFound = true;
            }
            else
            {
                output << line << std::endl;
            }
        }
    }

    return keyFound;
}

//------------------------------------------------------------------------------

} // namespace sight::viz::scene3d
