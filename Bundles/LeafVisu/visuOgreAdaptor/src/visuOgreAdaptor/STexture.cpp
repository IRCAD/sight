/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuOgreAdaptor/STexture.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/Material.hpp>
#include <fwData/Image.hpp>
#include <fwData/mt/ObjectReadLock.hpp>

#include <fwRenderOgre/Utils.hpp>

#include <fwServices/macros.hpp>

#include <fwTools/Type.hpp>

#include <OGRE/OgreHardwarePixelBuffer.h>
#include <OGRE/OgreTextureManager.h>

fwServicesRegisterMacro(::fwRenderOgre::IAdaptor, ::visuOgreAdaptor::STexture,
                        ::fwData::Image);

namespace visuOgreAdaptor
{

const ::fwCom::Signals::SignalKeyType visuOgreAdaptor::STexture::s_TEXTURE_SWAPPED_SIG = "textureSwapped";

const std::string STexture::DEFAULT_TEXTURE_FILENAME = "default.png";

//------------------------------------------------------------------------------

STexture::STexture() throw() :
    m_textureName(""),
    m_filtering("linear"),
    m_wrapping("repeat"),
    m_useAlpha(true),
    m_isDynamic(false)
{
    m_sigTextureSwapped = newSignal< TextureSwappedSignalType >( s_TEXTURE_SWAPPED_SIG );
}

//------------------------------------------------------------------------------

STexture::~STexture() throw()
{

}

//------------------------------------------------------------------------------

bool STexture::isValid() const
{
    if(!m_texture.isNull())
    {
        if(m_texture->getFormat() != ::Ogre::PF_UNKNOWN)
        {
            return true;
        }
    }

    return false;
}
//------------------------------------------------------------------------------

int STexture::getStartPriority()
{
    return -20;
}

//------------------------------------------------------------------------------

void STexture::doConfigure() throw(fwTools::Failed)
{
    if(m_configuration->hasAttribute("textureName"))
    {
        m_textureName = m_configuration->getAttributeValue("textureName");
    }
    else
    {
        // Choose a default name if not provided, this is very important otherwise
        // the texture may be lost if it is unloaded (which is very likely to happen when playing with techniques)
        m_textureName = this->getID();
    }

    if ( m_configuration->hasAttribute( "filtering" ) )
    {
        m_filtering = m_configuration->getAttributeValue("filtering");
    }

    if ( m_configuration->hasAttribute( "wrapping" ) )
    {
        m_wrapping = m_configuration->getAttributeValue("wrapping");
    }

    if ( m_configuration->hasAttribute( "useAlpha" ) )
    {
        bool useAlpha = (m_configuration->getAttributeValue("useAlpha") == "true");
        m_useAlpha = useAlpha;
    }
    if(m_configuration->hasAttribute("dynamic"))
    {
        std::string dynamic = m_configuration->getAttributeValue("dynamic");
        m_isDynamic = ( dynamic == "true" );
    }
}

//------------------------------------------------------------------------------

void STexture::doStart() throw(fwTools::Failed)
{
    m_texture = ::Ogre::TextureManager::getSingleton().createOrRetrieve(
        m_textureName,
        ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
        true).first.dynamicCast< ::Ogre::Texture>();

    this->updating();
}

//------------------------------------------------------------------------------

void STexture::doUpdate() throw(fwTools::Failed)
{
    // Retrieves associated f4s image
    ::fwData::Image::sptr imageF4s = ::fwData::Image::dynamicCast(this->getObject());

    SLM_ASSERT("Failed object dynamic cast", imageF4s);

    if(imageF4s->getAllocatedSizeInBytes() != 0)
    {
        ::fwData::mt::ObjectReadLock lock(imageF4s);

        // Loads the new image
        this->getRenderService()->makeCurrent();
        ::fwRenderOgre::Utils::loadOgreTexture(imageF4s, m_texture, ::Ogre::TEX_TYPE_2D, m_isDynamic);
        lock.unlock();

        m_sigTextureSwapped->asyncEmit();
    }
}

//------------------------------------------------------------------------------

void STexture::doSwap() throw(fwTools::Failed)
{
    this->updating();
}

//------------------------------------------------------------------------------

void STexture::doStop() throw(fwTools::Failed)
{
    // This is necessary, otherwise we have "ghost" textures later we reload a new texture
    m_texture->freeInternalResources();
    m_texture.setNull();
}

//-----------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsType STexture::getObjSrvConnections() const
{
    ::fwServices::IService::KeyConnectionsType connections;
    connections.push_back( std::make_pair( ::fwData::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::Image::s_MODIFIED_SIG, s_UPDATE_SLOT ) );
    return connections;
}

//------------------------------------------------------------------------------

} // namespace visuOgreAdaptor
