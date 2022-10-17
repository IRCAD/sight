/************************************************************************
 *
 * Copyright (C) 2017-2022 IRCAD France
 * Copyright (C) 2017-2020 IHU Strasbourg
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

#include "modules/io/matrix/config.hpp"

#include <data/MatrixTL.hpp>

#include <io/base/service/IWriter.hpp>

#include <fstream>

namespace sight::module::io::matrix
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
 * - \b write(core::HiResClock::HiResClockType) : write matrix in a csv file
 * - \b saveMatrix(core::HiResClock::HiResClockType) : save current matrices
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="sight::module::io::matrix::SMatrixWriter">
       <in key="data" uid="..." autoConnect="true" />
       <windowTitle>Select the file to save the matrix timeline to</windowTitle>
   </service>
   @endcode
 * @subsection Input Input
 * - \b data [sight::data::MatrixTL]: timeline containing the matrices to save.
 *
 * @subsection Configuration Configuration
 * - \b windowTitle: allow overriding the default title of the modal file selection window. \see io::IWriter
 */
class MODULE_IO_MATRIX_CLASS_API SMatrixWriter : public sight::io::base::service::IWriter
{
public:

    SIGHT_DECLARE_SERVICE(SMatrixWriter, sight::io::base::service::IWriter);

    /// Constructor.
    MODULE_IO_MATRIX_API SMatrixWriter() noexcept;

    /// Destructor. Does nothing
    MODULE_IO_MATRIX_API ~SMatrixWriter() noexcept override;

    /// Defines auto connection for this service (saveFrame()) to the frame timeline (objectPushed)
    MODULE_IO_MATRIX_API service::IService::KeyConnectionsMap getAutoConnections() const override;

    /// Display a location dialog allowing to select the video file to save
    MODULE_IO_MATRIX_API void openLocationDialog() override;

    /// Return file type (io::base::service::FOLDER)
    MODULE_IO_MATRIX_API sight::io::base::service::IOPathType getIOPathType() const override;

protected:

    /// Does nothing
    MODULE_IO_MATRIX_API void configuring() override;

    /// Does nothing
    MODULE_IO_MATRIX_API void starting() override;

    /// Does nothing
    MODULE_IO_MATRIX_API void stopping() override;

    /// Does nothing
    MODULE_IO_MATRIX_API void updating() override;

private:

    /// SLOT: Save current matrices.
    void saveMatrix(core::HiResClock::HiResClockType timestamp);

    /// SLOT: Write matrices in csv file
    void write(core::HiResClock::HiResClockType timestamp);

    /// SLOT: Start recording
    void startRecord();

    /// SLOT: Stop recording
    void stopRecord();

    bool m_isRecording {false}; ///< flag if the service is recording.

    std::ofstream m_filestream;
};

} // namespace sight::module::io::matrix
