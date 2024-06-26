/************************************************************************
 *
 * Copyright (C) 2017-2024 IRCAD France
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

#include <data/matrix_tl.hpp>

#include <io/__/service/writer.hpp>

#include <fstream>
#include <mutex>

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
 * - \b start_record() : start recording
 * - \b stop_record() : stop recording
 * - \b write(core::clock::type) : write matrix in a csv file
 * - \b saveMatrix(core::clock::type) : save current matrices
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="sight::module::io::matrix::matrix_writer">
       <in key="data" uid="..." auto_connect="true" />
       <windowTitle>Select the file to save the matrix timeline to</windowTitle>
       <config interactive="true" />
   </service>
   @endcode
 * @subsection Input Input
 * - \b data [sight::data::matrix_tl]: timeline containing the matrices to save.
 *
 * @subsection Configuration Configuration
 * - \b windowTitle: allow overriding the default title of the modal file selection window. \see io::writer
 * - \b config:
 *   - \b interactive: if true, the service will display a dialog box to select the file to save. If false, no dialog
 *                     box will be shown. In this case, for practical reasons, the recording will start when setting a
 *                     baseFolder.
 */
class matrix_writer : public sight::io::service::writer
{
public:

    SIGHT_DECLARE_SERVICE(matrix_writer, sight::io::service::writer);

    /// Constructor.
    matrix_writer() noexcept;

    /// Destructor. Does nothing
    ~matrix_writer() noexcept override;

    /// Defines auto connection for this service (saveFrame()) to the frame timeline (objectPushed)
    service::connections_t auto_connections() const override;

    /// Display a location dialog allowing to select the video file to save
    void open_location_dialog() override;

    /// Return file type (io::service::FOLDER)
    sight::io::service::path_type_t get_path_type() const override;

    /// SLOT: Save current matrices.
    void save_matrix(core::clock::type _timestamp);

    /// SLOT: Write matrices in csv file
    void write(core::clock::type _timestamp);

    /// SLOT: Start recording
    void start_record();

    /// SLOT: Stop recording
    void stop_record();

    /// SLOT: Change base folder
    void set_base_folder(std::string _path) override;

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

    /// True if the service is recording.
    bool m_is_recording {false};

    /// flag if the service is in "interactive" mode. IE if a dialog box is displayed to select the file.
    bool m_interactive {true};

    /// File stream to write the matrices
    std::ofstream m_filestream;

    /// Mutex to protect concurrent access on the file stream and service state
    std::recursive_mutex m_mutex;
};

} // namespace sight::module::io::matrix
