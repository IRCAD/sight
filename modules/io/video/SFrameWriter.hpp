/************************************************************************
 *
 * Copyright (C) 2016-2022 IRCAD France
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

#include <data/FrameTL.hpp>

#include <io/base/service/IWriter.hpp>

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
 * - \b saveFrame(timestamp): adds the current frame in the video
 * - \b startRecord(): starts recording
 * - \b stopRecord(): stops recording
 * - \b record(state): starts/stops the recording, according to the state in parameter
 * - \b setFormatParameter(value, key): changes format
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="sight::module::io::video::SFrameWriter">
       <in key="data" uid="..." autoConnect="true" />
       <windowTitle>Select the image file to load</windowTitle>
       <format>.tiff</format>
   </service>
   @endcode
 * @subsection Input Input
 * - \b data [sight::data::FrameTL]: timeline containing the frame to save.
 *
 * @subsection Configuration Configuration
 * - \b windowTitle: allow overriding the default title of the modal file selection window. \see io::IWriter
 * - \b format: optional, file format used to store frames. Possible extensions (.jpeg ,.bmp, .tiff, .png, .jp2,... )
 */
class MODULE_IO_VIDEO_CLASS_API SFrameWriter : public sight::io::base::service::IWriter
{
public:

    SIGHT_DECLARE_SERVICE(SFrameWriter, sight::io::base::service::IWriter);

    /// Constructor.
    MODULE_IO_VIDEO_API SFrameWriter() noexcept;

    /// Destructor. Does nothing
    MODULE_IO_VIDEO_API virtual ~SFrameWriter() noexcept;

    /// Defines auto connection for this service (saveFrame()) to the frame timeline (objectPushed)
    MODULE_IO_VIDEO_API service::IService::KeyConnectionsMap getAutoConnections() const override;

    /// Display a location dialog allowing to select the video file to save
    MODULE_IO_VIDEO_API void openLocationDialog() override;

    /// Return file type (io::base::service::FOLDER)
    MODULE_IO_VIDEO_API sight::io::base::service::IOPathType getIOPathType() const override;

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
    void saveFrame(core::HiResClock::HiResClockType timestamp);

    /// Writes frame on the disk
    void write(core::HiResClock::HiResClockType timestamp);

    /// SLOT: Starts recording
    void startRecord();

    /// SLOT: Stops recording
    void stopRecord();

    /// SLOT: Starts/stops recording, according to the parameter value
    void record(bool);

    /// SLOT: Toggle the recording.
    void toggleRecording();

    /// Slot called when a format is changed
    void setFormatParameter(std::string val, std::string key);

    int m_imageType; ///< opencv image type ( CV_8UC3, CV_8UC4, ...)

    bool m_isRecording; ///< flag if the service is recording.

    std::string m_format; ///< file format (.tiff by default)
};

} // videoOpenCV
