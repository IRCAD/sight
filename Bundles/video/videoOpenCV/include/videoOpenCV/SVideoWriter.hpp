/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2016-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "videoOpenCV/config.hpp"

#include <arData/FrameTL.hpp>

#include <fwIO/IWriter.hpp>

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
class VIDEOOPENCV_CLASS_API SVideoWriter : public ::fwIO::IWriter
{

public:

    fwCoreServiceClassDefinitionsMacro( (SVideoWriter)(::fwIO::IWriter) );

    /// Constructor.
    VIDEOOPENCV_API SVideoWriter() noexcept;

    /// Destructor. Does nothing
    VIDEOOPENCV_API virtual ~SVideoWriter() noexcept;

    /// Defines auto connection for this service (saveFrame()) to the frame timeline (objectPushed)
    VIDEOOPENCV_API virtual ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;

    /// Display a location dialog allowing to select the video file to save
    VIDEOOPENCV_API virtual void configureWithIHM() override;

    /// Return file type (::fwIO::FILE)
    VIDEOOPENCV_API virtual ::fwIO::IOPathType getIOPathType() const override;

protected:

    /// Does nothing
    VIDEOOPENCV_API virtual void configuring() override;

    /// Does nothing
    VIDEOOPENCV_API virtual void starting() override;

    /// Does nothing
    VIDEOOPENCV_API virtual void stopping() override;

    /// Does nothing
    VIDEOOPENCV_API virtual void updating() override;

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
