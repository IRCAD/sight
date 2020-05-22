/************************************************************************
 *
 * Copyright (C) 2016-2019 IRCAD France
 * Copyright (C) 2016-2019 IHU Strasbourg
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

#include <arData/FrameTL.hpp>

#include <fwIO/IWriter.hpp>

namespace videoOpenCV
{

/**
 * @brief Thi service allows to save the timeline frames in files in a folder.
 *
 * @note The method 'updating' allows to save the timeline frame with the current timestamp. If you want to save all the
 *       frame when they are pushed in the timeline, you must use the slots 'startRecord' and 'stopRecord'
 *
 * @todo Only image of type 'uint8' (RGB and RGBA) and grayscale image of type 'uint8' and 'uint16' are managed.
 *
 * @section Slots Slots
 * - \b saveFrame(timestamp) : add the current frame in the video
 * - \b startRecord() : start recording
 * - \b stopRecord() : stop recording
 * - \b setFormatParameter(value, key) : change format
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::videoOpenCV::SFrameWriter">
       <in key="data" uid="..." autoConnect="yes" />
       <windowTitle>Select the image file to load</windowTitle>
       <format>.tiff</format>
   </service>
   @endcode
 * @subsection Input Input
 * - \b data [::arData::FrameTL]: timeline containing the frame to save.
 *
 * @subsection Configuration Configuration
 * - \b windowTitle: allow overriding the default title of the modal file selection window. \see io::IWriter
 * - \b format: optional, file format used to store frames. Possible extensions (.jpeg ,.bmp, .tiff, .png, .jp2,... )
 */
class VIDEOOPENCV_CLASS_API SFrameWriter : public ::fwIO::IWriter
{

public:

    fwCoreServiceMacro(SFrameWriter, ::fwIO::IWriter);

    /// Constructor.
    VIDEOOPENCV_API SFrameWriter() noexcept;

    /// Destructor. Does nothing
    VIDEOOPENCV_API virtual ~SFrameWriter() noexcept;

    /// Defines auto connection for this service (saveFrame()) to the frame timeline (objectPushed)
    VIDEOOPENCV_API virtual ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;

    /// Display a location dialog allowing to select the video file to save
    VIDEOOPENCV_API virtual void configureWithIHM() override;

    /// Return file type (::fwIO::FOLDER)
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

    /// write frame on the disk
    void write(::fwCore::HiResClock::HiResClockType timestamp);

    /// SLOT: Start recording
    void startRecord();

    /// SLOT: Strop recording
    void stopRecord();

    ///Slot called when a format is changed
    void setFormatParameter(std::string val, std::string key);

    int m_imageType; ///< opencv image type ( CV_8UC3, CV_8UC4, ...)

    bool m_isRecording; ///< flag if the service is recording.

    std::string m_format;  ///< file format (.tiff by default)
};

} // videoOpenCV
