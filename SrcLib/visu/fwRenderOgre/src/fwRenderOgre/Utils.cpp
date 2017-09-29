/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderOgre/Utils.hpp"

#include "fwRenderOgre/compositor/MaterialMgrListener.hpp"
#include "fwRenderOgre/factory/R2VBRenderable.hpp"

#include <fwCore/spyLog.hpp>

#include <fwData/TransferFunction.hpp>

#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>
#include <fwDataTools/helper/ImageGetter.hpp>

#include <fwRuntime/operations.hpp>

#include <OgreConfigFile.h>
#include <OgreException.h>
#include <OgreHardwarePixelBuffer.h>
#include <OgreLog.h>
#include <OgreResourceGroupManager.h>
#include <OgreTextureManager.h>

#include <cstdint>

#ifdef __MACOSX__
#define PLUGIN_PATH "plugins_osx.cfg"
#elif _DEBUG
#define PLUGIN_PATH "plugins_d.cfg"
#else
#define PLUGIN_PATH "plugins.cfg"
#endif

namespace fwRenderOgre
{

static std::set<std::string> s_resourcesPath;

::Ogre::OverlaySystem* Utils::s_overlaySystem                           = nullptr;
::fwRenderOgre::factory::R2VBRenderable* Utils::s_R2VBRenderableFactory = nullptr;

//------------------------------------------------------------------------------

void loadResources()
{
    ::Ogre::ConfigFile cf;
    ::Ogre::String resourceGroupName, typeName, archName;

    for(const auto& path : s_resourcesPath)
    {
        try
        {
            cf.load(path);

            ::Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();

            while (seci.hasMoreElements())
            {
                resourceGroupName                              = seci.peekNextKey();
                ::Ogre::ConfigFile::SettingsMultiMap* settings = seci.getNext();
                ::Ogre::ConfigFile::SettingsMultiMap::iterator i;
                for (i = settings->begin(); i != settings->end(); ++i)
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
            SLM_WARN("Unable to find Ogre resources path : " + path);
        }
        catch (...)
        {
            SLM_WARN("Unable to load resource from " + path);
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
    ::Ogre::Root* root = ::Ogre::Singleton< ::Ogre::Root >::getSingletonPtr();

    if(root == nullptr)
    {
        root = new ::Ogre::Root(::fwRuntime::getLibraryResourceFilePath("fwRenderOgre_0-1/" PLUGIN_PATH).string());

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
        for (::Ogre::StringVector::iterator iter = renderOrder.begin(); iter != renderOrder.end(); iter++)
        {
            for (::Ogre::RenderSystemList::const_iterator it = rsList.begin(); it != rsList.end(); it++)
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

        /*
           Setting size and VSync on windows will solve a lot of problems
         */
//        std::string dimensions = this->width() + " x " + this->height();
//        rs->setConfigOption("Video Mode", dimensions.toStdString());
        rs->setConfigOption("Full Screen", "No");
#ifdef __MACOSX__
        rs->setConfigOption("vsync", "No");
#else
        rs->setConfigOption("VSync", "No");
        rs->setConfigOption("Display Frequency", "60");
#endif

        root->setRenderSystem(rs);

        root->initialise(false);

        auto resourcePath = ::fwRuntime::getLibraryResourceFilePath("fwRenderOgre_0-1/resources.cfg" );
        ::fwRenderOgre::Utils::addResourcesPath( resourcePath );

        loadResources();

        // Register factory for R2VB renderables objects
        s_R2VBRenderableFactory = OGRE_NEW ::fwRenderOgre::factory::R2VBRenderable();
        ::Ogre::Root::getSingleton().addMovableObjectFactory(s_R2VBRenderableFactory);

        // Add the material manager listener that allows us to generate OIT techniques
        ::Ogre::MaterialManager::getSingleton().addListener(new ::fwRenderOgre::compositor::MaterialMgrListener());
    }

    return root;
}

//------------------------------------------------------------------------------

void Utils::destroyOgreRoot()
{
    ::Ogre::Root::getSingleton().removeMovableObjectFactory(s_R2VBRenderableFactory);
    delete s_R2VBRenderableFactory;

    ::Ogre::Root* root = ::fwRenderOgre::Utils::getOgreRoot();
    ::Ogre::ResourceGroupManager::getSingleton().shutdownAll();

    delete s_overlaySystem;

    root->shutdown();
    delete root;
}

//------------------------------------------------------------------------------

::Ogre::Image Utils::convertFwDataImageToOgreImage( const ::fwData::Image::csptr imageFw)
{
    SLM_ASSERT("Image is null", imageFw);

    ::Ogre::Image imageOgre;
    uint32_t width = 1, height = 1, depth = 1;

    // If image is flipped, try to switch image
    ::fwData::Image::SizeType imageSize = imageFw->getSize();

    width = static_cast<uint32_t>(imageSize[0]);

    if(imageSize.size() >= 2)
    {
        height = static_cast<uint32_t>(imageSize[1]);

        if(imageSize.size() == 3)
        {
            depth = static_cast<uint32_t>(imageSize[2]);
        }
    }

    ::Ogre::PixelFormat pixelFormat = getPixelFormatOgre( imageFw );

    ::fwDataTools::helper::ImageGetter imageHelper(imageFw);

    imageOgre.loadDynamicImage(static_cast<uint8_t*>(imageHelper.getBuffer()), width, height, depth, pixelFormat);

    return imageOgre;
}

//------------------------------------------------------------------------------

// Only handles RGB for now, since fwData::Image only does so.
::Ogre::PixelFormat Utils::getPixelFormatOgre(::fwData::Image::csptr imageFw)
{
    std::string pixelType    = ::fwTools::getString( imageFw->getPixelType() );
    size_t numberOfComponent = imageFw->getNumberOfComponents();

    if(numberOfComponent == 1)
    {
        if(pixelType == "unsigned char")
        {
            // uint8
            return ::Ogre::PF_L8;
        }
        else if(pixelType == "signed short")
        {
            // int16
            return ::Ogre::PF_L16;
        }
        else if (pixelType == "float")
        {
            return ::Ogre::PF_FLOAT32_R;
        }
    }

    if(numberOfComponent == 2)
    {
        if(pixelType == "unsigned char")
        {
            // uint8
            return ::Ogre::PF_RG8;
        }
        else if(pixelType == "signed short")
        {
            // int16
            return ::Ogre::PF_R8G8_SNORM;
        }
    }

    // PixelFormat in little endian
    if(pixelType == "unsigned char")
    {
        // uint8
        return numberOfComponent == 3 ? ::Ogre::PF_BYTE_RGB : ::Ogre::PF_BYTE_RGBA;
    }
    else if (pixelType == "unsigned short")
    {
        // uint16
        return numberOfComponent == 3 ? ::Ogre::PF_R16G16B16_UINT : ::Ogre::PF_R16G16B16A16_UINT;
    }
    else if (pixelType == "unsigned int")
    {
        // uint32
        return numberOfComponent == 3 ? ::Ogre::PF_R32G32B32_UINT : ::Ogre::PF_R32G32B32A32_UINT;
    }
    else if (pixelType == "signed char" )
    {
        // int8
        return numberOfComponent == 3 ? ::Ogre::PF_R8G8B8_SINT : ::Ogre::PF_R8G8B8A8_SINT;
    }
    else if (pixelType == "signed short")
    {
        // int16
        return numberOfComponent == 3 ? ::Ogre::PF_R16G16B16_SINT : ::Ogre::PF_R16G16B16A16_SINT;
    }
    else if (pixelType == "signed int")
    {
        // int32
        return numberOfComponent == 3 ? ::Ogre::PF_R32G32B32_SINT : ::Ogre::PF_R32G32B32A32_SINT;
    }
    else if (pixelType == "unsigned long" )
    {
        // int64
        return numberOfComponent == 3 ? ::Ogre::PF_SHORT_RGB : ::Ogre::PF_SHORT_RGBA;
    }
    else if (pixelType == "signed long")
    {
        // uint64
        return numberOfComponent == 3 ? ::Ogre::PF_R16G16B16_UINT : ::Ogre::PF_R16G16B16A16_UINT;
    }
    else if (pixelType == "float")
    {
        return numberOfComponent == 3 ? ::Ogre::PF_FLOAT32_RGB : ::Ogre::PF_FLOAT32_RGBA;
    }
    else if (pixelType == "double")
    {
        OSLM_FATAL("Pixel format not handled.");
    }
    SLM_WARN("Pixel format not found, trying with the default 8-bits RGBA.");
    return ::Ogre::PF_BYTE_RGBA;
}

//------------------------------------------------------------------------------

void Utils::loadOgreTexture(const ::fwData::Image::csptr& _image, ::Ogre::TexturePtr _texture,
                            ::Ogre::TextureType _texType, bool _dynamic)
{
    bool imageIsValid = ::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity(_image);

    if(imageIsValid)
    {
        ::Ogre::PixelFormat pixelFormat = getPixelFormatOgre( _image );

        // Conversion from fwData::Image to ::Ogre::Image
        ::Ogre::Image ogreImage = ::fwRenderOgre::Utils::convertFwDataImageToOgreImage(_image);

        if( _texture->getWidth() != ogreImage.getWidth() ||
            _texture->getHeight() != ogreImage.getHeight() ||
            _texture->getDepth() != ogreImage.getDepth() ||
            _texture->getTextureType() != _texType ||
            _texture->getFormat() != pixelFormat )
        {

            ::fwRenderOgre::Utils::allocateTexture(_texture.get(), _image->getSize()[0], _image->getSize()[1],
                                                   _image->getSize()[2], pixelFormat, _texType, _dynamic);
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
        auto pDest                       = static_cast< unsignedType*>(pixelBox.data);

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
    auto srcType = _image->getType();

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

} // namespace fwRenderOgre
