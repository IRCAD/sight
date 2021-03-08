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

#include "modules/ui/dicom/config.hpp"

#include <data/SeriesDB.hpp>

#include <ui/base/IAction.hpp>

#include <filesystem>

namespace sight::module::ui::dicom
{

/**
 * @brief   This service is used to merge selected series with a SeriesDB.
 *          All the series in the vector are pushed in the destination SeriesDB.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="sight::module::ui::dicom::SSeriesDBMerger">
            <in key="selectedSeries" uid="..." />
            <inout key="seriesDB" uid="..." />
       </service>
   @endcode
 * @subsection Input Input:
 * - \b selectedSeries [sight::data::Vector]: Vector containing the selected series to merge.
 * @subsection In-Out In-Out:
 * - \b seriesDB [sight::data::SeriesDB]: SeriesDB where to put the series.
 */
class MODULE_UI_DICOM_CLASS_API SSeriesDBMerger : public ::sight::ui::base::IAction
{
public:

    SIGHT_DECLARE_SERVICE(SSeriesDBMerger, sight::ui::base::IAction )

    /// Constructor
    MODULE_UI_DICOM_API SSeriesDBMerger() noexcept;

    /// Destructor
    MODULE_UI_DICOM_API virtual ~SSeriesDBMerger() noexcept;

protected:

    /// Do nothing.
    MODULE_UI_DICOM_API void configuring() override;

    /// Override
    MODULE_UI_DICOM_API void starting() override;

    /// Override
    MODULE_UI_DICOM_API void stopping() override;

    /// Override
    MODULE_UI_DICOM_API void updating() override;

    /// Override
    MODULE_UI_DICOM_API void info(std::ostream& _sstream ) override;
};

} // namespace sight::module::ui::dicom
