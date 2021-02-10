/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include <services/IController.hpp>

namespace sight::core::jobs
{
class IJob;
}

namespace sight::data
{
class SeriesDB;
}

namespace sight::modules::io::dicom
{

/**
 * @brief   Service to anonymize a DicomSeries.
 *
 * @section Signals Signals
 * - \b jobCreated( SPTR(core::jobs::IJob) ) : Emitted when a job is created.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="::modules::io::dicom::SDicomSeriesAnonymizer">
            <inout key="seriesDB" uid="..." />
            <inout key="selectedSeries" uid="..." />
       </service>
   @endcode
 * @subsection In-Out In-Out:
 * - \b seriesDB [data::SeriesDB]: SeriesDB where the dicom series comes from.
 * - \b selectedSeries [data::Vector]: List of DICOM series to be anonymized.
 */
class MODULE_IO_DICOM_CLASS_API SDicomSeriesAnonymizer : public services::IController
{

public:
    typedef core::com::Signal< void ( SPTR(core::jobs::IJob) ) > JobCreatedSignal;

    fwCoreServiceMacro(SDicomSeriesAnonymizer,  services::IController)

    /// Constructor
    MODULE_IO_DICOM_API SDicomSeriesAnonymizer() noexcept;

    /// Destructor
    MODULE_IO_DICOM_API virtual ~SDicomSeriesAnonymizer() noexcept override;

protected:

    /// Do nothing.
    MODULE_IO_DICOM_API virtual void configuring() override;

    /// Override
    MODULE_IO_DICOM_API virtual void starting() override;

    /// Override
    MODULE_IO_DICOM_API virtual void stopping() override;

    /// Override
    MODULE_IO_DICOM_API void updating() override;

    /// Override
    MODULE_IO_DICOM_API void info(std::ostream& _sstream ) override;

    /// Override
    MODULE_IO_DICOM_API void anonymize();

    /// Signal emitted when a job is created
    SPTR(JobCreatedSignal) m_sigJobCreated;

    /// Cancel information for jobs
    bool m_cancelled;
};

} // namespace sight::modules::io::dicom
