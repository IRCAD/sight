/************************************************************************
 *
 * Copyright (C) 2016-2021 IRCAD France
 * Copyright (C) 2016-2020 IHU Strasbourg
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

#pragma once

#include "videoOpenCV/config.hpp"

#include <data/FrameTL.hpp>

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
 * - \b data [data::FrameTL]: timeline containing the frame to save.
 */
class VIDEOOPENCV_CLASS_API SVideoWriter : public ::fwIO::IWriter
{

public:

    fwCoreServiceMacro(SVideoWriter, ::fwIO::IWriter)

    /// Constructor.
    VIDEOOPENCV_API SVideoWriter() noexcept;

    /// Destructor. Does nothing
    VIDEOOPENCV_API virtual ~SVideoWriter() noexcept;

    /// Defines auto connection for this service (saveFrame()) to the frame timeline (objectPushed)
    VIDEOOPENCV_API virtual ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;

    /**
     * @brief Display a location dialog allowing to select the video file to save
     * @deprecated Will be removed in sight 22.0. Use openLocationDialog() instead.
     */
    [[deprecated("Will be removed in sight 22.0. Use openLocationDialog() instead.")]]
    VIDEOOPENCV_API virtual void configureWithIHM() override;

    /// Display a location dialog allowing to select the video file to save
    VIDEOOPENCV_API virtual void openLocationDialog() override;

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
    void saveFrame(core::HiResClock::HiResClockType timestamp);

    /// save current buffer with OpenCV video writer (m_writer must be initialized)
    void writeBuffer(int width, int height, CSPTR(data::FrameTL::BufferType) buffer);

    /// SLOT: Start recording
    void startRecord();

    /// SLOT: Stop recording
    void stopRecord();

    /// opencv video writer
    UPTR(::cv::VideoWriter) m_writer;

    /// opencv image type ( CV_8UC3, CV_8UC4, ...)
    int m_imageType{0};

    /// flag if the service is recording.
    bool m_isRecording{false};

    /// container used to store first video frame timestamps to compute framerate of the video stream
    std::vector< core::HiResClock::HiResClockType > m_timestamps;

    /// Extension selected in file dialog
    std::string m_selectedExtension;

    ///  static string containing the file extension
    static const std::string s_MP4_EXTENSION;

    ///  static string containing the codec to use
    static const std::string s_AVC1_CODEC;

};

} // videoOpenCV
