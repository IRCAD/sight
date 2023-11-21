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

namespace sight::module::io::video
{

/**
 * @brief Allows to save the timeline frames in files, in a folder.
 *
 * @note The method 'updating' allows to save the timeline frame with the current timestamp. If you want to save all the
 *       frame when they are pushed in the timeline, you must use the slots 'startRecord' and 'stopRecord'
 *
 * @todo Only image of type 'uint8' (RGB and RGBA) and grayscale image of type 'uint8' and 'uint16' are managed.
 *
 * @section Slots Slots
 * - \b save_frame(timestamp): adds the current frame in the video
 * - \b start_record(): starts recording
 * - \b stop_record(): stops recording
 * - \b record(state): starts/stops the recording, according to the state in parameter
 * - \b set_format_parameter(value, key): changes format
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="sight::module::io::video::frame_writer">
       <in key="data" uid="..." auto_connect="true" />
       <windowTitle>Select the image file to load</windowTitle>
       <format>.tiff</format>
   </service>
   @endcode
 * @subsection Input Input
 * - \b data [sight::data::frame_tl]: timeline containing the frame to save.
 *
 * @subsection Configuration Configuration
 * - \b windowTitle: allow overriding the default title of the modal file selection window. \see io::writer
 * - \b format: optional, file format used to store frames. Possible extensions (.jpeg ,.bmp, .tiff, .png, .jp2,... )
 */
class MODULE_IO_VIDEO_CLASS_API frame_writer : public sight::io::service::writer
{
public:

    SIGHT_DECLARE_SERVICE(frame_writer, sight::io::service::writer);

    /// Constructor.
    MODULE_IO_VIDEO_API frame_writer() noexcept;

    /// Destructor. Does nothing
    MODULE_IO_VIDEO_API ~frame_writer() noexcept override;

    /// Defines auto connection for this service (saveFrame()) to the frame timeline (objectPushed)
    MODULE_IO_VIDEO_API service::connections_t auto_connections() const override;

    /// Display a location dialog allowing to select the video file to save
    MODULE_IO_VIDEO_API void open_location_dialog() override;

    /// Return file type (io::service::FOLDER)
    MODULE_IO_VIDEO_API sight::io::service::path_type_t get_path_type() const override;

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

    /// SLOT: Adds the current frame in the video
    void save_frame(core::clock::type _timestamp);

    /// Writes frame on the disk
    void write(core::clock::type _timestamp);

    /// SLOT: Starts recording
    void start_record();

    /// SLOT: Stops recording
    void stop_record();

    /// SLOT: Starts/stops recording, according to the parameter value
    void record(bool /*state*/);

    /// SLOT: Toggle the recording.
    void toggle_recording();

    /// Slot called when a format is changed
    void set_format_parameter(std::string _val, std::string _key);

    int m_image_type {0}; ///< opencv image type ( CV_8UC3, CV_8UC4, ...)

    bool m_is_recording {false}; ///< flag if the service is recording.

    std::string m_format; ///< file format (.tiff by default)
};

} // namespace sight::module::io::video
