/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2016-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VIDEOOPENCV_SFRAMEWRITER_HPP__
#define __VIDEOOPENCV_SFRAMEWRITER_HPP__

#include "videoOpenCV/config.hpp"

#include <arData/FrameTL.hpp>

#include <io/IWriter.hpp>

namespace videoOpenCV
{

/**
 * @brief Thi service allows to save the timeline frames in files in a folder.
 *
 * @note The method 'updating' allows to save the timeline frame with the current timestamp. If you want to save all the
 *       frame when they are pushed in the timeline, you must use the slots 'startRecord' and 'stopRecord'
 *
 * @todo Currently, this service writes only image with extension .tiff with no compression.
 * @todo Only image of type 'uint8' (RGB and RGBA) and grayscale image of type 'uint8' and 'uint16' are managed.
 *
 * @section Slots Slots
 * - \b saveFrame(timestamp) : add the current frame in the video
 * - \b startRecord() : start recording
 * - \b stopRecord() : stop recording
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::videoOpenCV::SFrameWriter">
       <in key="data" uid="..." autoConnect="yes" />
       <windowTitle>Select the image file to load</windowTitle>
   </service>
   @endcode
 * @subsection Input Input
 * - \b data [::arData::FrameTL]: timeline containing the frame to save.
 *
 * @subsection Configuration Configuration
 * - \b windowTitle: allow overriding the default title of the modal file selection window. \see io::IWriter
 */
class VIDEOOPENCV_CLASS_API SFrameWriter : public ::io::IWriter
{

public:

    fwCoreServiceClassDefinitionsMacro( (SFrameWriter)(::io::IWriter) );

    /// Constructor.
    VIDEOOPENCV_API SFrameWriter() throw();

    /// Destructor. Does nothing
    VIDEOOPENCV_API virtual ~SFrameWriter() throw();

    /// Defines auto connection for this service (saveFrame()) to the frame timeline (objectPushed)
    VIDEOOPENCV_API virtual ::fwServices::IService::KeyConnectionsMap getAutoConnections() const;

    /// Display a location dialog allowing to select the video file to save
    VIDEOOPENCV_API virtual void configureWithIHM();

    /// Return file type (::io::FOLDER)
    VIDEOOPENCV_API virtual ::io::IOPathType getIOPathType() const;

protected:

    /// Does nothing
    VIDEOOPENCV_API virtual void configuring()  throw ( ::fwTools::Failed );

    /// Does nothing
    VIDEOOPENCV_API virtual void starting()  throw ( ::fwTools::Failed );

    /// Does nothing
    VIDEOOPENCV_API virtual void stopping()  throw ( ::fwTools::Failed );

    /// Does nothing
    VIDEOOPENCV_API virtual void updating() throw ( ::fwTools::Failed );

private:

    /// SLOT: add the current frame in the video
    void saveFrame(::fwCore::HiResClock::HiResClockType timestamp);

    /// write frame on the disk
    void write(::fwCore::HiResClock::HiResClockType timestamp);

    /// SLOT: Start recording
    void startRecord();

    /// SLOT: Strop recording
    void stopRecord();

    int m_imageType; ///< opencv image type ( CV_8UC3, CV_8UC4, ...)

    bool m_isRecording; ///< flag if the service is recording.
};

} // videoOpenCV

#endif // __VIDEOOPENCV_SFRAMEWRITER_HPP__
