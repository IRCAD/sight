/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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

#include "filter/dicom/config.hpp"
#include "filter/dicom/filter.hpp"

#include <data/dicom_series.hpp>

namespace sight::filter::dicom::helper
{

/**
 * @brief Filter Helper
 */
class FILTER_DICOM_CLASS_API Filter
{
public:

    typedef std::vector<data::dicom_series::sptr> DicomSeriesContainerType;

    /**
     * @brief Apply a filter to the DicomSeries
     * @param[in,out] dicomSeriesContainer DicomSeries that must be filtered
     * @param[in] filter Filter that must be applied
     * @return Return true if errors have been ignored
     */
    FILTER_DICOM_API static bool applyFilter(
        DicomSeriesContainerType& dicomSeriesContainer,
        sight::filter::dicom::filter::sptr filter,
        bool forcedApply                      = false,
        const core::log::logger::sptr& logger = std::make_shared<core::log::logger>()
    );
};

} // namespace sight::filter::dicom::helper
