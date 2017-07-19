/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VIDEOOPENCV_SVIDEOWRITER_HPP__
#define __VIDEOOPENCV_SVIDEOWRITER_HPP__

#include "videoOpenCV/config.hpp"

#include <arData/FrameTL.hpp>

#include <io/IWriter.hpp>

#include <opencv2/videoio.hpp>

namespace videoOpenCV
{

/**
 * @brief Thi service allows to save the timeline frames in a video file.
 *
 * - \b saveFrame(timestamp) : add the current frame in the video
 * - \b startRecord() : start recording
 * - \b stopRecord() : stop recording
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::videoOpenCV::SVideoWriter">
       <in key="data" uid="..." />
   </service>
   @endcode
 * @subsection Input Input
 * - \b data [::arData::FrameTL]: timeline containing the frame to save.
 */
class VIDEOOPENCV_CLASS_API SVideoWriter : public ::io::IWriter
{

public:

    fwCoreServiceClassDefinitionsMacro ( (SVideoWriter)(::io::IWriter) );

    /// Constructor.
    VIDEOOPENCV_API SVideoWriter() noexcept;

    /// Destructor. Does nothing
    VIDEOOPENCV_API virtual ~SVideoWriter() noexcept;

    /// Defines auto connection for this service (saveFrame()) to the frame timeline (objectPushed)
    VIDEOOPENCV_API virtual ::fwServices::IService::KeyConnectionsMap getAutoConnections() const;

    /// Display a location dialog allowing to select the video file to save
    VIDEOOPENCV_API virtual void configureWithIHM();

    /// Return file type (::io::FILE)
    VIDEOOPENCV_API virtual ::io::IOPathType getIOPathType() const;

protected:

    /// Does nothing
    VIDEOOPENCV_API virtual void configuring();

    /// Does nothing
    VIDEOOPENCV_API virtual void starting();

    /// Does nothing
    VIDEOOPENCV_API virtual void stopping();

    /// Does nothing
    VIDEOOPENCV_API virtual void updating();

private:

    /// SLOT: add the current frame in the video
    void saveFrame(::fwCore::HiResClock::HiResClockType timestamp);

    /// SLOT: Start recording
    void startRecord();

    /// SLOT: Strop recording
    void stopRecord();

    UPTR(::cv::VideoWriter) m_writer; ///< opencv video writer

    int m_imageType; ///< opencv image type ( CV_8UC3, CV_8UC4, ...)

};

} // videoOpenCV

#endif // __VIDEOOPENCV_SVIDEOWRITER_HPP__
