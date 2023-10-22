/************************************************************************
 *
 * Copyright (C) 2016-2023 IRCAD France
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

#include "modules/io/video/config.hpp"

#include <data/frame_tl.hpp>

#include <io/__/service/writer.hpp>

#include <opencv2/videoio.hpp>

namespace sight::module::io::video
{

/**
 * @brief This service allows to save the timeline frames in a video file.
 *
 * - \b saveFrame(timestamp) : add the current frame in the video
 * - \b startRecord() : start recording
 * - \b stopRecord() : stop recording
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="sight::module::io::video::video_writer">
       <in key="data" uid="..." />
   </service>
   @endcode
 * @subsection Input Input
 * - \b data [sight::data::frame_tl]: timeline containing the frame to save.
 */
class MODULE_IO_VIDEO_CLASS_API video_writer : public sight::io::service::writer
{
public:

    SIGHT_DECLARE_SERVICE(video_writer, sight::io::service::writer);

    /// Constructor.
    MODULE_IO_VIDEO_API video_writer() noexcept;

    /// Destructor. Does nothing
    MODULE_IO_VIDEO_API ~video_writer() noexcept override;

    /// Defines auto connection for this service (saveFrame()) to the frame timeline (objectPushed)
    MODULE_IO_VIDEO_API service::connections_t auto_connections() const override;

    /// Display a location dialog allowing to select the video file to save
    MODULE_IO_VIDEO_API void openLocationDialog() override;

    /// Return file type (io::service::FILE)
    MODULE_IO_VIDEO_API sight::io::service::IOPathType getIOPathType() const override;

protected:

    /// Does nothing
    MODULE_IO_VIDEO_API void configuring() override;

    /// Does nothing
    MODULE_IO_VIDEO_API void starting() override;

    /// Does nothing
    MODULE_IO_VIDEO_API void stopping() override;

    /// Does nothing
    MODULE_IO_VIDEO_API void updating() override;

private:

    /// SLOT: adds the current frame in the video
    void saveFrame(core::hires_clock::type _timestamp);

    /// saves current buffer with OpenCV video writer (m_writer must be initialized)
    void writeBuffer(int _width, int _height, CSPTR(data::frame_tl::buffer_t) _buffer);

    /// SLOT: Starts recording
    void startRecord();

    /// SLOT: Stops recording
    void stopRecord();

    /// SLOT: Starts/stops recording, according to the parameter value
    void record(bool /*state*/);

    /// SLOT: Toggles recording.
    void toggleRecording();

    /// opencv video writer
    UPTR(cv::VideoWriter) m_writer;

    /// opencv image type ( CV_8UC3, CV_8UC4, ...)
    int m_imageType {0};

    /// flag if the service is recording.
    bool m_isRecording {false};

    /// container used to store first video frame timestamps to compute framerate of the video stream
    std::vector<core::hires_clock::type> m_timestamps;

    /// Extension selected in file dialog
    std::string m_selectedExtension;

    ///  static string containing the file extension
    static const std::string s_MP4_EXTENSION;

    ///  static string containing the codec to use
    static const std::string s_AVC1_CODEC;
};

} // namespace sight::module::io::video
