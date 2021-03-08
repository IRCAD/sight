/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "modules/io/dicom/config.hpp"

#include <io/base/service/IWriter.hpp>

#include <filesystem>
#include <string>

namespace sight::core::jobs
{
class IJob;
}

namespace sight::data
{
class DicomSeries;
}

namespace sight::module::io::dicom
{

/**
 * @brief DICOM Writer (DicomSeries)
 *
 * @section Signals Signals
 * - \b jobCreated( SPTR(core::jobs::IJob) ) : Emitted when a job is created.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
       <service uid="..." type="sight::module::io::dicom::SDicomSeriesWriter" >
           <in key="data" uid="..." />
       </service>
 * @endcode
 *
 * @subsection Input Input
 * - \b data [sight::data::DicomSeries]: data to save in Dicom.
 */
class MODULE_IO_DICOM_CLASS_API SDicomSeriesWriter : public sight::io::base::service::IWriter
{

public:
    typedef core::com::Signal< void ( SPTR(core::jobs::IJob) ) > JobCreatedSignal;

    SIGHT_DECLARE_SERVICE(SDicomSeriesWriter, sight::io::base::service::IWriter)

    /**
     * @brief   constructor
     */
    MODULE_IO_DICOM_API SDicomSeriesWriter() noexcept;

    /**
     * @brief   destructor
     */
    MODULE_IO_DICOM_API ~SDicomSeriesWriter() noexcept override;

    /**
     * @brief Propose select a directory where to save the DICOM files.
     * @deprecated Will be removed in sight 22.0. Use openLocationDialog() instead.
     */
    [[deprecated("Will be removed in sight 22.0. Use openLocationDialog() instead.")]]
    MODULE_IO_DICOM_API void configureWithIHM() override;

    /// Propose select a directory where to save the DICOM files.
    MODULE_IO_DICOM_API void openLocationDialog() override;

protected:

    /// Does nothing
    MODULE_IO_DICOM_API void starting() override;

    /// Does nothing
    MODULE_IO_DICOM_API void stopping() override;

    /// Configuring method. This method is used to configure the service.
    MODULE_IO_DICOM_API void configuring() override;

    /// Write the DicomSeries in DICOM format.
    MODULE_IO_DICOM_API void updating() override;

    /// Return path type managed by the service, here FOLDER
    MODULE_IO_DICOM_API sight::io::base::service::IOPathType getIOPathType() const override;

private:
    /// Save the selected Dicom series
    void saveDicomSeries( const std::filesystem::path folder,
                          const CSPTR(data::DicomSeries)& series ) const;

    /// Signal emitted when a job is created
    SPTR(JobCreatedSignal) m_sigJobCreated;

    /// Cancel information for jobs
    bool m_cancelled;
};

} // namespace sight::module::io::dicom
