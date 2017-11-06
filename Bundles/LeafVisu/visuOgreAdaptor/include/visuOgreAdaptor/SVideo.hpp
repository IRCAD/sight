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

 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="::visuOgreAdaptor::SVideo" autoConnect="yes" >
            <in key="image" uid="..." />
            <in key="camera" uid="..." />
            <config renderer="default" reversed="true" />
        </service>
   @endcode
 * @subsection Input Input:
 * - \b image [::fwData::Image]: frame displayed.
 * - \b camera [::arData::Camera] (optional): camera calibration, recenters the video using the (cx, cy) offsets.
 * @subsection Configuration Configuration:
 * - \b reverse (optional)(default: true) : if true, the actor is rotated by 180° along the z and y axis.
 */
class VISUOGREADAPTOR_CLASS_API SVideo : public ::fwRenderOgre::IAdaptor
{

public:

    fwCoreServiceClassDefinitionsMacro( (SVideo)(::fwRenderOgre::IAdaptor) )

    typedef ::fwCom::Signal< void ( double* ) > UpdateSizeSigType;
    VISUOGREADAPTOR_API static const ::fwCom::Signals::SignalKeyType s_UPDATE_SIZE_SIG;

    /// Constructor.
    VISUOGREADAPTOR_API SVideo() noexcept;

    /// Destructor. Does nothing
    VISUOGREADAPTOR_API virtual ~SVideo() noexcept;

protected:

    /// Configure the options
    VISUOGREADAPTOR_API void configuring() override;

    /// Create the ogre texture and mapper used to show the video frame.
    VISUOGREADAPTOR_API void starting() override;

    /// Removes the actor from the renderer
    VISUOGREADAPTOR_API void stopping() override;

    /// Updated the frame from the current Image.
    VISUOGREADAPTOR_API void updating() override;

    /// Returns proposals to connect service slots to associated object signals
    VISUOGREADAPTOR_API ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;

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
