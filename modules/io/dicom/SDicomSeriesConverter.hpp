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

#include <core/com/signal.hpp>

#include <data/SeriesSet.hpp>

#include <ui/__/action.hpp>

namespace sight::core::jobs
{

class base;

} // namespace sight::core::jobs

namespace sight::module::io::dicom
{

/**
 * @brief   This service is used to convert DicomSeries from source SeriesSet
 * and push result (ImageSeries, ModelSeries, ...) in target SeriesSet
 *
 * @section Signals Signals
 * - \b jobCreated( SPTR(core::jobs::base) ) : Emitted when a job is created.
 *
 * @section XML XML Configuration
 *
   @code{.xml}
   <service uid="..." impl="sight::module::io::dicom::SDicomSeriesConverter">
     <in key="source" uid="..." />
     <inout key="target" uid="..." />
   </service>
   @endcode
 * @subsection Input Input:
 * - \b source [sight::data::SeriesSet]: Source SeriesSet containing DicomSeries.
 * @subsection In-Out In-Out:
 * - \b target [sight::data::SeriesSet]: Destination SeriesSet.
 */
class MODULE_IO_DICOM_CLASS_API SDicomSeriesConverter : public sight::ui::action
{
public:

    SIGHT_DECLARE_SERVICE(SDicomSeriesConverter, sight::ui::action);

    typedef core::com::signal<void (SPTR(core::jobs::base))> JobCreatedSignal;

    /**
     * @brief Constructor
     */
    MODULE_IO_DICOM_API SDicomSeriesConverter() noexcept;

    /**
     * @brief Destructor
     */
    MODULE_IO_DICOM_API ~SDicomSeriesConverter() noexcept override;

protected:

    /// Configuring method. This method is used to configure the service.
    MODULE_IO_DICOM_API void configuring() override;

    /// Override
    MODULE_IO_DICOM_API void starting() override;

    /// Override
    MODULE_IO_DICOM_API void stopping() override;

    /// Override
    MODULE_IO_DICOM_API void updating() override;

    /// Override
    MODULE_IO_DICOM_API void info(std::ostream& _sstream) override;

protected:

    /// Signal emitted when a job is created
    SPTR(JobCreatedSignal) m_sigJobCreated;

private:

    sight::data::ptr<sight::data::SeriesSet, sight::data::Access::in> m_source {this, "source"};
    sight::data::ptr<sight::data::SeriesSet, sight::data::Access::inout> m_target {this, "target"};
};

} // namespace sight::module::io::dicom
