/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
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
    m_previousWidth(0),
    m_previousHeight(0)
{
    m_sigTextureSwapped = newSignal< TextureSwappedSignalType >( s_TEXTURE_SWAPPED_SIG );
}

//------------------------------------------------------------------------------

STexture::~STexture() throw()
{

}

//------------------------------------------------------------------------------

void STexture::doConfigure() throw(fwTools::Failed)
{
    SLM_ASSERT("Not a \"config\" configuration", m_configuration->getName() == "config");

    if(m_configuration->hasAttribute("textureName"))
    {
        m_textureName = m_configuration->getAttributeValue("textureName");
    }

    if ( m_configuration->hasAttribute( "filtering" ) )
    {
        m_filtering = m_configuration->getAttributeValue("filtering");
    }

    if ( m_configuration->hasAttribute( "wrapping" ) )
    {
        m_wrapping = m_configuration->getAttributeValue("wrapping");
    }
}

//------------------------------------------------------------------------------

void STexture::doStart() throw(fwTools::Failed)
{
    m_texture = ::Ogre::TextureManager::getSingletonPtr()->create(
        m_textureName,
        ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
        true);

    this->doUpdate();
}

//------------------------------------------------------------------------------

void STexture::doUpdate() throw(fwTools::Failed)
{
    // Retrieves associated f4s image
    ::fwData::Image::sptr imageF4s = ::fwData::Image::dynamicCast(this->getObject());

    SLM_ASSERT("Failed object dynamic cast", imageF4s);

    if(imageF4s->getSizeInBytes() != 0)
    {
        ::fwData::mt::ObjectReadLock lock(imageF4s);

        ::fwData::Image::SizeType imageSize = imageF4s->getSize();
        size_t width  = imageSize[0];
        size_t height = (imageSize.size() >= 2) ? imageSize[1] : 0;

        // Check if the image is the same as before
        if ( width != m_previousWidth || height != m_previousHeight )
        {
            // Loads the new image
            ::fwRenderOgre::Utils::loadOgreTexture(imageF4s, m_texture, ::Ogre::TEX_TYPE_2D);

            m_sigTextureSwapped->asyncEmit();

            m_previousWidth  = width;
            m_previousHeight = height;
        }

        lock.unlock();
    }
}

//------------------------------------------------------------------------------

void STexture::doSwap() throw(fwTools::Failed)
{
    this->doUpdate();
}

//------------------------------------------------------------------------------

void STexture::doStop() throw(fwTools::Failed)
{

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
