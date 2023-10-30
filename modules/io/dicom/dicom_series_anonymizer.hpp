/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include <data/vector.hpp>

#include <service/controller.hpp>

namespace sight::core::jobs
{

class base;

} // namespace sight::core::jobs

namespace sight::data
{

class series_set;

} // namespace sight::data

namespace sight::module::io::dicom
{

/**
 * @brief   Service to anonymize a DicomSeries.
 *
 * @section Signals Signals
 * - \b jobCreated( SPTR(core::jobs::base) ) : Emitted when a job is created.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="sight::module::io::dicom::dicom_series_anonymizer">
            <inout key="seriesSet" uid="..." />
            <inout key="selectedSeries" uid="..." />
       </service>
   @endcode
 * @subsection In-Out In-Out:
 * - \b seriesSet [sight::data::series_set]: series_set where the dicom series comes from.
 * - \b selectedSeries [sight::data::vector]: List of DICOM series to be anonymized.
 */
class MODULE_IO_DICOM_CLASS_API dicom_series_anonymizer : public service::controller
{
public:

    using job_created_signal_t = core::com::signal<void (std::shared_ptr<core::jobs::base>)>;

    SIGHT_DECLARE_SERVICE(dicom_series_anonymizer, service::controller);

    /// Constructor
    MODULE_IO_DICOM_API dicom_series_anonymizer() noexcept;

    /// Destructor
    MODULE_IO_DICOM_API ~dicom_series_anonymizer() noexcept override;

protected:

    /// Do nothing.
    MODULE_IO_DICOM_API void configuring() override;

    /// Override
    MODULE_IO_DICOM_API void starting() override;

    /// Override
    MODULE_IO_DICOM_API void stopping() override;

    /// Override
    MODULE_IO_DICOM_API void updating() override;

    /// Override
    MODULE_IO_DICOM_API void info(std::ostream& _sstream) override;

    /// Override
    MODULE_IO_DICOM_API void anonymize(sight::data::vector& _vector);

    /// Signal emitted when a job is created
    SPTR(job_created_signal_t) m_sig_job_created;

    /// Cancel information for jobs
    bool m_cancelled {false};

    sight::data::ptr<sight::data::series_set, sight::data::access::inout> m_series_set {this, "seriesSet"};
    sight::data::ptr<sight::data::vector, sight::data::access::inout> m_selected_series {this, "selectedSeries"};
};

} // namespace sight::module::io::dicom
