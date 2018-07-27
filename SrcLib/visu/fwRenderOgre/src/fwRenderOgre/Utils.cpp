/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderOgre/Utils.hpp"

#include "fwRenderOgre/compositor/MaterialMgrListener.hpp"
#include "fwRenderOgre/factory/R2VBRenderable.hpp"
#include "fwRenderOgre/factory/Text.hpp"
#include "fwRenderOgre/vr/GridProxyGeometry.hpp"

#include <fwCore/spyLog.hpp>

#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>
#include <fwDataTools/helper/ImageGetter.hpp>

#include <fwRuntime/operations.hpp>

#include <boost/filesystem.hpp>

#include <OgreConfigFile.h>
#include <OgreException.h>
#include <OgreHardwarePixelBuffer.h>
#include <OgreResourceGroupManager.h>
#include <OgreTextureManager.h>

#include <algorithm>
#include <cctype> // Needed for isspace()

#ifdef __MACOSX__
#define PLUGIN_PATH "plugins_osx.cfg"
#elif _WIN32
#   ifdef _DEBUG
#       define PLUGIN_PATH "plugins_win32_d.cfg"
#   else
#       define PLUGIN_PATH "plugins_win32.cfg"
#   endif
#else
#   define PLUGIN_PATH "plugins.cfg"
#endif

namespace fwRenderOgre
{

static std::set<std::string> s_resourcesPath;

::Ogre::OverlaySystem* Utils::s_overlaySystem                                   = nullptr;
::fwRenderOgre::factory::R2VBRenderable* Utils::s_R2VBRenderableFactory         = nullptr;
::fwRenderOgre::factory::Text* Utils::s_textFactory                             = nullptr;
::fwRenderOgre::vr::GridProxyGeometryFactory* Utils::s_gridProxyGeometryFactory = nullptr;
::fwRenderOgre::compositor::MaterialMgrListener* Utils::s_oitMaterialListener   = nullptr;

//------------------------------------------------------------------------------

void Utils::loadResources()
{
    ::Ogre::ConfigFile cf;
    ::Ogre::String resourceGroupName, typeName, archName;

    for(const auto& path : s_resourcesPath)
    {
        try
        {
            // Check file existence
            if(!::boost::filesystem::exists(path))
            {
                OSLM_FATAL("File '" + path +"' doesn't exist. Ogre needs it to load resources");
            }

            const auto tmpPath = boost::filesystem::temp_directory_path() / boost::filesystem::unique_path();
            std::ofstream newResourceFile(tmpPath.string());

            if(!::boost::filesystem::exists(tmpPath))
            {
                OSLM_FATAL("Can't create the file '" + tmpPath.string() + "'");
            }

            // Copy the resource file and make paths absolute.
            std::ifstream resourceFile(path);

            makePathsAbsolute("FileSystem", resourceFile, newResourceFile);

            resourceFile.close();
            newResourceFile.close();
            cf.load(tmpPath.string());
            ::boost::filesystem::remove(tmpPath);

            const ::Ogre::ConfigFile::SettingsBySection_ secis = cf.getSettingsBySection();

            for(const auto& seci : secis)
            {
                resourceGroupName                             = seci.first;
                ::Ogre::ConfigFile::SettingsMultiMap settings = seci.second;
                ::Ogre::ConfigFile::SettingsMultiMap::iterator i;
                for (i = settings.begin(); i != settings.end(); ++i)
                {
                    typeName = i->first;
                    archName = i->second;
                    ::Ogre::ResourceGroupManager::getSingleton().addResourceLocation(archName, typeName,
                                                                                     resourceGroupName);
                }
            }
        }
        catch ( ::Ogre::FileNotFoundException& )
        {
            SLM_ERROR("Unable to find Ogre resources path : " + path);
        }
        catch (...)
        {
            SLM_ERROR("Unable to load resource from " + path);
        }
    }
}

//------------------------------------------------------------------------------

void Utils::addResourcesPath(const ::boost::filesystem::path& path)
{
    SLM_ASSERT("Empty resource path", !path.empty());
    s_resourcesPath.insert(path.string());
}

//------------------------------------------------------------------------------

::Ogre::OverlaySystem* Utils::getOverlaySystem()
{
    return s_overlaySystem;
}

//------------------------------------------------------------------------------

::Ogre::Root* Utils::getOgreRoot()
{
    ::Ogre::Root* root = ::Ogre::Root::getSingletonPtr();

    if(root == nullptr)
    {
        const auto& confPath = ::fwRuntime::getLibraryResourceFilePath("fwRenderOgre-0.1/" PLUGIN_PATH);

        // Check file existence
        if(!::boost::filesystem::exists(confPath))
        {
            OSLM_FATAL("File '" + confPath.string() +"' doesn't exist. Ogre needs it to be configured");
        }

        const auto tmpPluginCfg = ::boost::filesystem::temp_directory_path() / ::boost::filesystem::unique_path();

        // Set the actual plugin path in the plugin config file.
        std::ifstream pluginCfg(confPath.string());
        std::ofstream newPlugin(tmpPluginCfg.string());

        if(!::boost::filesystem::exists(tmpPluginCfg))
        {
            OSLM_FATAL("Can't create temporary config file'" + tmpPluginCfg.string() + "'");
        }

        const bool tokenFound = makePathsAbsolute("PluginFolder", pluginCfg, newPlugin);

        pluginCfg.close();
        newPlugin.close();

        SLM_FATAL_IF("No 'PluginFolder' folder set in " + confPath.string(), !tokenFound);

        root = new ::Ogre::Root(tmpPluginCfg.string().c_str());

        ::boost::filesystem::remove(tmpPluginCfg);

        s_overlaySystem = new ::Ogre::OverlaySystem();

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
        //renderOrder.push_back("OpenGL 3+");
        for (::Ogre::StringVector::iterator iter = renderOrder.begin(); iter != renderOrder.end(); ++iter)
        {
            for (::Ogre::RenderSystemList::const_iterator it = rsList.begin(); it != rsList.end(); ++it)
            {
                if ((*it)->getName().find(*iter) != Ogre::String::npos)
                {
                    rs = *it;
                    break;
                }
            }
            if (rs != nullptr)
            {
                break;
            }
        }
        SLM_ASSERT("Abort render system configuration, no render system found", rs);

        rs->setConfigOption("Full Screen", "No");
        rs->setConfigOption("VSync", "No");
#ifndef __APPLE__
        rs->setConfigOption("Display Frequency", "60");
#endif

        root->setRenderSystem(rs);

        root->initialise(false);

        auto resourcePath = ::fwRuntime::getLibraryResourceFilePath("fwRenderOgre-0.1/resources.cfg" );
        ::fwRenderOgre::Utils::addResourcesPath( resourcePath );

        loadResources();

        // Register factory for Text objects
        s_textFactory = OGRE_NEW ::fwRenderOgre::factory::Text();
        ::Ogre::Root::getSingleton().addMovableObjectFactory(s_textFactory);

        // Register factory for R2VB renderables objects
        s_R2VBRenderableFactory = OGRE_NEW ::fwRenderOgre::factory::R2VBRenderable();
        ::Ogre::Root::getSingleton().addMovableObjectFactory(s_R2VBRenderableFactory);

        // Register factory for GridProxyGeometry objects
        s_gridProxyGeometryFactory = OGRE_NEW ::fwRenderOgre::vr::GridProxyGeometryFactory();
        ::Ogre::Root::getSingleton().addMovableObjectFactory(s_gridProxyGeometryFactory);

        // Add the material manager listener that allows us to generate OIT techniques
        s_oitMaterialListener = new ::fwRenderOgre::compositor::MaterialMgrListener();
        ::Ogre::MaterialManager::getSingleton().addListener(s_oitMaterialListener);
    }

    return root;
}

//------------------------------------------------------------------------------

void Utils::destroyOgreRoot()
{
    ::Ogre::MaterialManager::getSingleton().removeListener(s_oitMaterialListener);
    delete s_oitMaterialListener;

    ::Ogre::Root::getSingleton().removeMovableObjectFactory(s_textFactory);
    delete s_textFactory;

    ::Ogre::Root::getSingleton().removeMovableObjectFactory(s_gridProxyGeometryFactory);
    delete s_gridProxyGeometryFactory;

    ::Ogre::Root::getSingleton().removeMovableObjectFactory(s_R2VBRenderableFactory);
    delete s_R2VBRenderableFactory;

    ::Ogre::Root* root = ::fwRenderOgre::Utils::getOgreRoot();
    ::Ogre::ResourceGroupManager::getSingleton().shutdownAll();

    delete s_overlaySystem;

    delete root;
}

//------------------------------------------------------------------------------

::Ogre::Image Utils::convertToOgreImage( const ::fwData::Image::csptr imageFw)
{
    SLM_ASSERT("Image is null", imageFw);

    ::Ogre::Image imageOgre;

    // If image is flipped, try to switch image
    const ::fwData::Image::SizeType imageSize = imageFw->getSize();

    const uint32_t width = static_cast<uint32_t>(imageSize[0]);
    uint32_t height = 1, depth = 1;

    if(imageSize.size() >= 2)
    {
        height = static_cast<uint32_t>(imageSize[1]);

        if(imageSize.size() == 3)
        {
            depth = static_cast<uint32_t>(imageSize[2]);
        }
    }

    const ::Ogre::PixelFormat pixelFormat = getPixelFormatOgre( imageFw );

    ::fwDataTools::helper::ImageGetter imageHelper(imageFw);

    imageOgre.loadDynamicImage(static_cast<uint8_t*>(imageHelper.getBuffer()), width, height, depth, pixelFormat);

    return imageOgre;
}

//------------------------------------------------------------------------------

void Utils::convertFromOgreTexture( ::Ogre::TexturePtr _texture, const ::fwData::Image::sptr _imageFw, bool flip)
{
    SLM_ASSERT("Texture is null", _texture);
    SLM_ASSERT("Image is null", _imageFw);

    ::fwData::Image::SizeType imageSize;

    imageSize.push_back(_texture->getWidth());

    if(_texture->getHeight() > 1)
    {
        imageSize.push_back(_texture->getHeight());

        if(_texture->getDepth() > 1)
        {
            imageSize.push_back(_texture->getDepth());
        }
    }
    _imageFw->setSize(imageSize);

    Utils::setPixelFormatFromOgre(_imageFw, _texture->getFormat());
    ::fwData::Image::SpacingType spacing(3, 1);
    ::fwData::Image::OriginType origin(3, 0);

    _imageFw->setSpacing(spacing);
    _imageFw->setOrigin(origin);
    _imageFw->allocate();

    // Get the pixel buffer
    ::Ogre::HardwarePixelBufferSharedPtr pixelBuffer = _texture->getBuffer();

    // Lock the pixel buffer and copy it
    {
        ::fwDataTools::helper::Image imageHelper(_imageFw);

        std::uint8_t* __restrict dstBuffer = reinterpret_cast< std::uint8_t* >(imageHelper.getBuffer());

        pixelBuffer->lock(::Ogre::HardwareBuffer::HBL_READ_ONLY);
        const ::Ogre::PixelBox& pixelBox         = pixelBuffer->getCurrentLock();
        const size_t pitch                       = pixelBox.rowPitch * _imageFw->getNumberOfComponents();
        const std::uint8_t* __restrict srcBuffer =
            reinterpret_cast< const std::uint8_t* >(pixelBox.data) + (flip ? pixelBox.getConsecutiveSize() : 0);

        // Copy and flip the image
        const auto numRows = pixelBox.getConsecutiveSize() / pitch;
        if(flip)
        {
            for(size_t i = 0; i < numRows; ++i)
            {
                srcBuffer -= pitch;
                std::memcpy(dstBuffer, srcBuffer, pitch);
                dstBuffer += pitch;
            }
        }
        else
        {
            for(size_t i = 0; i < numRows; ++i)
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

::Ogre::PixelFormat Utils::getPixelFormatOgre(::fwData::Image::csptr imageFw)
{
    const ::fwTools::Type pixelType = imageFw->getType();
    const size_t numberOfComponent  = imageFw->getNumberOfComponents();

    if(numberOfComponent == 1)
    {
        if(pixelType == ::fwTools::Type::s_UINT8)
        {
            // uint8
            return ::Ogre::PF_L8;
        }
        else if(pixelType == ::fwTools::Type::s_INT16)
        {
            // int16
            return ::Ogre::PF_L16;
        }
        else if(pixelType == ::fwTools::Type::s_UINT16)
        {
            // uint16
            return ::Ogre::PF_R16_UINT;
        }
        else if (pixelType == ::fwTools::Type::s_FLOAT)
        {
            // float
            return ::Ogre::PF_FLOAT32_R;
        }
        FW_RAISE("Format '" + pixelType.string() + "' not handled");
    }

    if(numberOfComponent == 2)
    {
        if(pixelType == ::fwTools::Type::s_UINT8)
        {
            // uint8
            return ::Ogre::PF_RG8;
        }
        else if(pixelType == ::fwTools::Type::s_INT8)
        {
            // int16
            return ::Ogre::PF_R8G8_SNORM;
        }
        else if(pixelType == ::fwTools::Type::s_FLOAT)
        {
            // float
            return ::Ogre::PF_FLOAT32_GR;
        }
        FW_RAISE("Format '" + pixelType.string() + "' not handled");
    }

    // PixelFormat in little endian
    if(pixelType == ::fwTools::Type::s_UINT8)
    {
        // uint8
        return numberOfComponent == 3 ? ::Ogre::PF_BYTE_RGB : ::Ogre::PF_BYTE_RGBA;
    }
    else if (pixelType == ::fwTools::Type::s_UINT16)
    {
        // uint16
        return numberOfComponent == 3 ? ::Ogre::PF_R16G16B16_UINT : ::Ogre::PF_R16G16B16A16_UINT;
    }
    else if (pixelType == ::fwTools::Type::s_UINT32)
    {
        // uint32
        return numberOfComponent == 3 ? ::Ogre::PF_R32G32B32_UINT : ::Ogre::PF_R32G32B32A32_UINT;
    }
    else if (pixelType == ::fwTools::Type::s_INT8 )
    {
        // int8
        return numberOfComponent == 3 ? ::Ogre::PF_R8G8B8_SINT : ::Ogre::PF_R8G8B8A8_SINT;
    }
    else if (pixelType == ::fwTools::Type::s_INT16)
    {
        // int16
        return numberOfComponent == 3 ? ::Ogre::PF_R16G16B16_SINT : ::Ogre::PF_R16G16B16A16_SINT;
    }
    else if (pixelType == ::fwTools::Type::s_INT32)
    {
        // int32
        return numberOfComponent == 3 ? ::Ogre::PF_R32G32B32_SINT : ::Ogre::PF_R32G32B32A32_SINT;
    }
    else if (pixelType == ::fwTools::Type::s_FLOAT)
    {
        return numberOfComponent == 3 ? ::Ogre::PF_FLOAT32_RGB : ::Ogre::PF_FLOAT32_RGBA;
    }
    else if (pixelType == ::fwTools::Type::s_DOUBLE)
    {
        OSLM_FATAL("Pixel format not handled.");
    }
    SLM_WARN("Pixel format not found, trying with the default 8-bits RGBA.");
    return ::Ogre::PF_BYTE_RGBA;
}

//------------------------------------------------------------------------------

void Utils::setPixelFormatFromOgre( ::fwData::Image::sptr _image, ::Ogre::PixelFormat _format )
{
    // Set the number of components;
    size_t numComponents;
    switch(_format)
    {
        case ::Ogre::PF_L8:
        case ::Ogre::PF_L16:
        case ::Ogre::PF_R16_UINT:
        case ::Ogre::PF_FLOAT32_R: numComponents = 1; break;

        case ::Ogre::PF_RG8:
        case ::Ogre::PF_R8G8_SNORM:  numComponents = 2; break;

        case ::Ogre::PF_BYTE_RGB:
        case ::Ogre::PF_R8G8B8:
        case ::Ogre::PF_R16G16B16_UINT:
        case ::Ogre::PF_R32G32B32_UINT:
        case ::Ogre::PF_R8G8B8_SINT:
        case ::Ogre::PF_R16G16B16_SINT:
        case ::Ogre::PF_R32G32B32_SINT:
        case ::Ogre::PF_SHORT_RGB:
        case ::Ogre::PF_FLOAT32_RGB:  numComponents = 3; break;

        case ::Ogre::PF_BYTE_RGBA:
        case ::Ogre::PF_A8R8G8B8:
        case ::Ogre::PF_B8G8R8A8:
        case ::Ogre::PF_R8G8B8A8:
        case ::Ogre::PF_X8R8G8B8:
        case ::Ogre::PF_X8B8G8R8:
        case ::Ogre::PF_R16G16B16A16_UINT:
        case ::Ogre::PF_R32G32B32A32_UINT:
        case ::Ogre::PF_R8G8B8A8_SINT:
        case ::Ogre::PF_R16G16B16A16_SINT:
        case ::Ogre::PF_R32G32B32A32_SINT:
        case ::Ogre::PF_SHORT_RGBA:
        case ::Ogre::PF_FLOAT32_RGBA: numComponents = 4; break;

        default:
            OSLM_ERROR("Pixel format " << _format << " not found, defaults to 4 components.");
            numComponents = 4;
    }
    _image->setNumberOfComponents(numComponents);

    // Set the pixel type

    ::fwTools::Type pixelType;
    switch(_format)
    {
        case ::Ogre::PF_L8:
        case ::Ogre::PF_RG8:
        case ::Ogre::PF_R8G8B8:
        case ::Ogre::PF_A8R8G8B8:
        case ::Ogre::PF_B8G8R8A8:
        case ::Ogre::PF_R8G8B8A8:
        case ::Ogre::PF_X8R8G8B8:
        case ::Ogre::PF_X8B8G8R8:
        case ::Ogre::PF_BYTE_RGB:
        case ::Ogre::PF_BYTE_RGBA: pixelType = ::fwTools::Type::s_UINT8; break;

        case ::Ogre::PF_R8G8_SNORM:
        case ::Ogre::PF_R8G8B8_SINT:
        case ::Ogre::PF_R8G8B8A8_SINT: pixelType = ::fwTools::Type::s_INT8; break;

        case ::Ogre::PF_L16:
        case ::Ogre::PF_R16G16B16_UINT:
        case ::Ogre::PF_R16_UINT:
        case ::Ogre::PF_R16G16B16A16_UINT: pixelType = ::fwTools::Type::s_UINT16; break;

        case ::Ogre::PF_SHORT_RGB:
        case ::Ogre::PF_SHORT_RGBA:
        case ::Ogre::PF_R16G16B16_SINT:
        case ::Ogre::PF_R16G16B16A16_SINT: pixelType = ::fwTools::Type::s_INT16; break;

        case ::Ogre::PF_R32G32B32_UINT:
        case ::Ogre::PF_R32G32B32A32_UINT:  pixelType = ::fwTools::Type::s_UINT32; break;

        case ::Ogre::PF_R32G32B32_SINT:
        case ::Ogre::PF_R32G32B32A32_SINT: pixelType = ::fwTools::Type::s_INT32; break;

        case ::Ogre::PF_FLOAT32_R:
        case ::Ogre::PF_FLOAT32_RGB:
        case ::Ogre::PF_FLOAT32_RGBA: pixelType = ::fwTools::Type::s_FLOAT; break;

        default:
            OSLM_ERROR("Pixel format " << _format << " not found, defaults to s_UINT8.");
            pixelType = ::fwTools::Type::s_UINT8;
    }
    _image->setType(pixelType);
}

//------------------------------------------------------------------------------

void Utils::loadOgreTexture(const ::fwData::Image::csptr& _image, ::Ogre::TexturePtr _texture,
                            ::Ogre::TextureType _texType, bool _dynamic)
{
    const bool imageIsValid = ::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity(_image);

    if(imageIsValid)
    {
        const ::Ogre::PixelFormat pixelFormat = getPixelFormatOgre( _image );

        // Conversion from fwData::Image to ::Ogre::Image
        ::Ogre::Image ogreImage = ::fwRenderOgre::Utils::convertToOgreImage(_image);

        if( _texture->getWidth() != ogreImage.getWidth() ||
            _texture->getHeight() != ogreImage.getHeight() ||
            _texture->getDepth() != ogreImage.getDepth() ||
            _texture->getTextureType() != _texType ||
            _texture->getFormat() != pixelFormat )
        {
            const auto& size = _image->getSize();
            SLM_ASSERT("Only handle 2D and 3D textures", size.size() >= 2);
            const size_t depth = size.size() == 2 ? 1 : size[2];

            ::fwRenderOgre::Utils::allocateTexture(_texture.get(), size[0], size[1], depth,
                                                   pixelFormat, _texType, _dynamic);
        }

        // Copy image's pixel box into texture buffer
        _texture->getBuffer(0, 0)->blitFromMemory(ogreImage.getPixelBox(0, 0));
    }
}

//------------------------------------------------------------------------------

template <typename SRC_TYPE, typename DST_TYPE>
void copyNegatoImage( ::Ogre::Texture* _texture, const ::fwData::Image::sptr& _image )
{
    // Get the pixel buffer
    ::Ogre::HardwarePixelBufferSharedPtr pixelBuffer = _texture->getBuffer();

    // Lock the pixel buffer and copy it
    {
        ::fwDataTools::helper::Image srcImageHelper(_image);
        typedef typename std::make_unsigned< DST_TYPE>::type unsignedType;

        auto srcBuffer = static_cast< const SRC_TYPE* >(srcImageHelper.getBuffer());

        pixelBuffer->lock(::Ogre::HardwareBuffer::HBL_DISCARD);
        const ::Ogre::PixelBox& pixelBox = pixelBuffer->getCurrentLock();
        auto pDest                       = reinterpret_cast< unsignedType*>(pixelBox.data);

        const DST_TYPE lowBound = std::numeric_limits< DST_TYPE >::min();

        const ::Ogre::uint32 size = _texture->getWidth() * _texture->getHeight() * _texture->getDepth();

        for(::Ogre::uint32 i = 0; i < size; ++i)
        {
            OSLM_ASSERT("Pixel value '" << *srcBuffer << "' doesn't fit in texture range.",
                        *srcBuffer > std::numeric_limits< DST_TYPE >::min() &&
                        *srcBuffer < std::numeric_limits< DST_TYPE >::max());
            *pDest++ = static_cast< unsignedType >(*srcBuffer++ - lowBound);
        }

        // Unlock the pixel buffer
        pixelBuffer->unlock();
    }
}

//------------------------------------------------------------------------------

void Utils::convertImageForNegato( ::Ogre::Texture* _texture, const ::fwData::Image::sptr& _image )
{
    const auto srcType = _image->getType();

    if(srcType == ::fwTools::Type::s_INT16)
    {
        if( _texture->getWidth() != _image->getSize()[0] ||
            _texture->getHeight() != _image->getSize()[1] ||
            _texture->getDepth() != _image->getSize()[2]    )
        {
            ::fwRenderOgre::Utils::allocateTexture(_texture, _image->getSize()[0], _image->getSize()[1],
                                                   _image->getSize()[2], ::Ogre::PF_L16, ::Ogre::TEX_TYPE_3D, false);

        }

        copyNegatoImage< std::int16_t, std::int16_t >(_texture, _image);
    }
    else if(srcType == ::fwTools::Type::s_INT32)
    {
        if( _texture->getWidth() != _image->getSize()[0] ||
            _texture->getHeight() != _image->getSize()[1] ||
            _texture->getDepth() != _image->getSize()[2]    )
        {
            ::fwRenderOgre::Utils::allocateTexture(_texture, _image->getSize()[0], _image->getSize()[1],
                                                   _image->getSize()[2], ::Ogre::PF_L16, ::Ogre::TEX_TYPE_3D,
                                                   false);

        }

        copyNegatoImage< std::int32_t, std::int16_t >(_texture, _image);
    }
    else if(srcType == ::fwTools::Type::s_UINT8)
    {
        if( _texture->getWidth() != _image->getSize()[0] ||
            _texture->getHeight() != _image->getSize()[1] ||
            _texture->getDepth() != _image->getSize()[2]    )
        {
            ::fwRenderOgre::Utils::allocateTexture(_texture, _image->getSize()[0], _image->getSize()[1],
                                                   _image->getSize()[2], ::Ogre::PF_L16, ::Ogre::TEX_TYPE_3D, false);

        }

        copyNegatoImage< std::uint8_t, std::int16_t >(_texture, _image);
    }
    else
    {
        SLM_FATAL("Image format not supported.");
    }
}

//------------------------------------------------------------------------------

void Utils::allocateTexture(::Ogre::Texture* _texture, size_t _width, size_t _height, size_t _depth,
                            ::Ogre::PixelFormat _format, ::Ogre::TextureType _texType, bool _dynamic)
{

    auto usage = _dynamic ? ::Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE : ::Ogre::TU_STATIC_WRITE_ONLY;

    _texture->freeInternalResources();

    _texture->setWidth(static_cast< ::Ogre::uint32>(_width));
    _texture->setHeight(static_cast< ::Ogre::uint32>(_height));
    _texture->setDepth(static_cast< ::Ogre::uint32>(_depth));
    _texture->setTextureType(_texType);
    _texture->setNumMipmaps(0);

    _texture->setFormat(_format);
    _texture->setUsage(usage);

    _texture->createInternalResources();
}

//------------------------------------------------------------------------------

::fwData::Color::sptr Utils::convertOgreColorToFwColor(const ::Ogre::ColourValue& _ogreColor)
{
    ::fwData::Color::sptr fwColor = ::fwData::Color::New();
    fwColor->setRGBA(_ogreColor.r, _ogreColor.g, _ogreColor.b, _ogreColor.a);

    return fwColor;
}

//------------------------------------------------------------------------------

bool Utils::makePathsAbsolute(const std::string& key, std::istream& input, std::ostream& output)
{
    bool keyFound = false;

    const size_t keySize = key.size();

    for(std::string line; std::getline(input, line);)
    {
        // Remove all whitespace from the line.
        line.erase(std::remove_if(line.begin(), line.end(), isspace), line.end());

        // Skip comments, go to the next line.
        if(line[0] != '#')
        {
            if(line.substr(0, keySize) == key)
            {
                SLM_FATAL_IF("Key '" + key + "' has no value bound to it.", line.size() < keySize + 1 );

                const auto currentPath = ::boost::filesystem::path(line.substr(keySize + 1));

                if(!currentPath.is_absolute())
                {
                    const auto absPath = ::fwRuntime::Runtime::getDefault()->getWorkingPath() / currentPath;

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

} // namespace fwRenderOgre
