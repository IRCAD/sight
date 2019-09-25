/************************************************************************
 *
 * Copyright (C) 2017-2019 IRCAD France
 * Copyright (C) 2017-2019 IHU Strasbourg
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

#include "ioTimeline/config.hpp"

#include <arData/MatrixTL.hpp>

#include <fwIO/IWriter.hpp>

namespace ioTimeline
{

/**
 * @brief This service allows the user to save the timeline matrices in a csv file.
 *
 * @note The method 'updating' allows to save the timeline matrix with the current timestamp. If you want to save all
 * the
 *       matrices when they are pushed in the timeline, you must use the slots 'startRecord' and 'stopRecord'
 *
 * @section Slots Slots
 * - \b startRecord() : start recording
 * - \b stopRecord() : stop recording
 * - \b write(::fwCore::HiResClock::HiResClockType) : write matrix in a csv file
 * - \b saveMatrix(::fwCore::HiResClock::HiResClockType) : save current matrices
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::ioTimeline::SMatrixWriter">
       <in key="data" uid="..." autoConnect="yes" />
       <windowTitle>Select the file to save the matrix timeline to</windowTitle>
   </service>
   @endcode
 * @subsection Input Input
 * - \b data [::arData::MatrixTL]: timeline containing the matrices to save.
 *
 * @subsection Configuration Configuration
 * - \b windowTitle: allow overriding the default title of the modal file selection window. \see io::IWriter
 */
class IOTIMELINE_CLASS_API SMatrixWriter : public ::fwIO::IWriter
{

public:

    fwCoreServiceMacro(SMatrixWriter, ::fwIO::IWriter);

    /// Constructor.
    IOTIMELINE_API SMatrixWriter() noexcept;

    /// Destructor. Does nothing
    IOTIMELINE_API virtual ~SMatrixWriter() noexcept;

    /// Defines auto connection for this service (saveFrame()) to the frame timeline (objectPushed)
    IOTIMELINE_API virtual ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;

    /// Display a location dialog allowing to select the video file to save
    IOTIMELINE_API virtual void configureWithIHM() override;

    /// Return file type (::fwIO::FOLDER)
    IOTIMELINE_API virtual ::fwIO::IOPathType getIOPathType() const override;

protected:

    /// Does nothing
    IOTIMELINE_API virtual void configuring() override;

    /// Does nothing
    IOTIMELINE_API virtual void starting() override;

    /// Does nothing
    IOTIMELINE_API virtual void stopping() override;

    /// Does nothing
    IOTIMELINE_API virtual void updating() override;

private:

    /// SLOT: Save current matrices.
    void saveMatrix(::fwCore::HiResClock::HiResClockType timestamp);

    /// SLOT: Write matrices in csv file
    void write(::fwCore::HiResClock::HiResClockType timestamp);

    /// SLOT: Start recording
    void startRecord();

    /// SLOT: Stop recording
    void stopRecord();

    bool m_isRecording; ///< flag if the service is recording.

    std::ofstream* m_filestream;
};

} // ioTimeline
