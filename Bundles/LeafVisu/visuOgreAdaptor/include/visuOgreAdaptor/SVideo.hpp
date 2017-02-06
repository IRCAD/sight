/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUOGREADAPTOR_SVIDEO_HPP__
#define __VISUOGREADAPTOR_SVIDEO_HPP__

#include "visuOgreAdaptor/config.hpp"

#include <fwRenderOgre/IAdaptor.hpp>

#include <OGRE/OgreImage.h>
#include <OGRE/OgreTexture.h>

namespace visuOgreAdaptor
{

/**
 * @brief   Adaptor to render a video frame from a 2D-image.
 */
class VISUOGREADAPTOR_CLASS_API SVideo : public ::fwRenderOgre::IAdaptor
{

public:

    fwCoreServiceClassDefinitionsMacro( (SVideo)(::fwRenderOgre::IAdaptor) );

    typedef ::fwCom::Signal< void ( double* ) > UpdateSizeSigType;
    VISUOGREADAPTOR_API static const ::fwCom::Signals::SignalKeyType s_UPDATE_SIZE_SIG;

    /// Constructor.
    VISUOGREADAPTOR_API SVideo() throw();

    /// Destructor. Does nothing
    VISUOGREADAPTOR_API virtual ~SVideo() throw();

protected:

    /**
     * @brief method description:
     * @code{.xml}
        <service uid="SVideoInstance" type="::visuOgreAdaptor::SVideo">
             <parameter>param</parameter>
        </service>
       @endcode
     * - \b Parameter : parameter description.
     */
    VISUOGREADAPTOR_API void doConfigure() throw ( ::fwTools::Failed );

    /// Create the ogre texture and mapper used to show the video frame.
    VISUOGREADAPTOR_API void doStart() throw ( ::fwTools::Failed );

    /// Removes the actor from the renderer
    VISUOGREADAPTOR_API void doStop() throw ( ::fwTools::Failed );

    /// Calls updating()
    VISUOGREADAPTOR_API void doSwap() throw(fwTools::Failed);

    /// Updated the frame from the current Image.
    VISUOGREADAPTOR_API void doUpdate() throw ( ::fwTools::Failed );

    /// Returns proposals to connect service slots to associated object signals
    VISUOGREADAPTOR_API ::fwServices::IService::KeyConnectionsMap getAutoConnections() const;

private:

    /// Ogre image created from current data Image. It is shown in the frame.
    ::Ogre::Image* m_imageData;

    /// Ogre texture used to store the fwImage
    ::Ogre::TexturePtr m_texture;

    /// True if the texture is initialized
    bool m_isTextureInit;

    /// UID of the camera
    std::string m_cameraUID;

    /// Store previous image size
    unsigned int m_previousWidth;

    /// Store previous image spacing
    unsigned int m_previousHeight;

    /// If true, the actor is rotated in z and y axis.
    bool m_reverse;
};

} // visuOgreAdaptor

#endif // __VISUOGREADAPTOR_SVIDEO_HPP__
