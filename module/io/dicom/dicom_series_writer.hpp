/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include <io/__/service/writer.hpp>

#include <filesystem>
#include <string>

namespace sight::core::jobs
{

class base;

} // namespace sight::core::jobs

namespace sight::data
{

class dicom_series;

} // namespace sight::data

namespace sight::module::io::dicom
{

/**
 * @brief DICOM Writer (DicomSeries)
 *
 * @section Signals Signals
 * - \b job_created( SPTR(core::jobs::base) ) : Emitted when a job is created.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
       <service uid="..." type="sight::module::io::dicom::dicom_series_writer" >
           <in key="data" uid="..." />
       </service>
 * @endcode
 *
 * @subsection Input Input
 * - \b data [sight::data::dicom_series]: data to save in Dicom.
 */
class dicom_series_writer : public sight::io::service::writer
{
public:

    using job_created_signal_t = core::com::signal<void (std::shared_ptr<core::jobs::base>)>;

    SIGHT_DECLARE_SERVICE(dicom_series_writer, sight::io::service::writer);

    /**
     * @brief   constructor
     */
    dicom_series_writer() noexcept;

    /**
     * @brief   destructor
     */
    ~dicom_series_writer() noexcept override;

    /// Propose select a directory where to save the DICOM files.
    void open_location_dialog() override;

protected:

    /// Does nothing
    void starting() override;

    /// Does nothing
    void stopping() override;

    /// Configuring method. This method is used to configure the service.
    void configuring() override;

    /// Write the DicomSeries in DICOM format.
    void updating() override;

    /// Return path type managed by the service, here FOLDER
    sight::io::service::path_type_t get_path_type() const override;

private:

    /// Save the selected Dicom series
    void save_dicom_series(
        const std::filesystem::path _folder,
        const CSPTR(data::dicom_series)& _series
    ) const;

    /// Signal emitted when a job is created
    SPTR(job_created_signal_t) m_sig_job_created;
};

} // namespace sight::module::io::dicom
