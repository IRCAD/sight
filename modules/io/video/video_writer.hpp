/************************************************************************
 *
 * Copyright (C) 2016-2024 IRCAD France
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

#include <data/frame_tl.hpp>

#include <io/__/service/writer.hpp>

#include <opencv2/videoio.hpp>

namespace sight::module::io::video
{

/**
 * @brief This service allows to save the timeline frames in a video file.
 *
 * - \b save_frame(timestamp) : add the current frame in the video
 * - \b start_record() : start recording
 * - \b stop_record() : stop recording
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
class video_writer : public sight::io::service::writer
{
public:

    SIGHT_DECLARE_SERVICE(video_writer, sight::io::service::writer);

    /// Constructor.
    video_writer() noexcept;

    /// Destructor. Does nothing
    ~video_writer() noexcept override;

    /// Defines auto connection for this service (saveFrame()) to the frame timeline (objectPushed)
    service::connections_t auto_connections() const override;

    /// Display a location dialog allowing to select the video file to save
    void open_location_dialog() override;

    /// Return file type (io::service::FILE)
    sight::io::service::path_type_t get_path_type() const override;

protected:

    /// Does nothing
    void configuring() override;

    /// Does nothing
    void starting() override;

    /// Does nothing
    void stopping() override;

    /// Does nothing
    void updating() override;

private:

    /// SLOT: adds the current frame in the video
    void save_frame(core::clock::type _timestamp);

    /// saves current buffer with OpenCV video writer (m_writer must be initialized)
    void write_buffer(int _width, int _height, CSPTR(data::frame_tl::buffer_t) _buffer);

    /// SLOT: Starts recording
    void start_record();

    /// SLOT: Stops recording
    void stop_record();

    /// SLOT: Starts/stops recording, according to the parameter value
    void record(bool /*state*/);

    /// SLOT: Toggles recording.
    void toggle_recording();

    /// opencv video writer
    UPTR(cv::VideoWriter) m_writer;

    /// opencv image type ( CV_8UC3, CV_8UC4, ...)
    int m_image_type {0};

    /// flag if the service is recording.
    bool m_is_recording {false};

    /// container used to store first video frame timestamps to compute framerate of the video stream
    std::vector<core::clock::type> m_timestamps;

    /// Extension selected in file dialog
    std::string m_selected_extension;

    ///  static string containing the file extension
    static const std::string P4_EXTENSION;

    ///  static string containing the codec to use
    static const std::string AV_C1_CODEC;
};

} // namespace sight::module::io::video
