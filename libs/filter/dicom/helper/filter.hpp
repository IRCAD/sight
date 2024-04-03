/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include <sight/filter/dicom/config.hpp>

#include "filter/dicom/filter.hpp"

#include <data/dicom_series.hpp>

namespace sight::filter::dicom::helper
{

/**
 * @brief filter Helper
 */
class SIGHT_FILTER_DICOM_CLASS_API filter
{
public:

    using dicom_series_container_t = std::vector<data::dicom_series::sptr>;

    /**
     * @brief Apply a filter to the DicomSeries
     * @param[in,out] _dicom_series_container DicomSeries that must be filtered
     * @param[in] _filter filter that must be applied
     * @return Return true if errors have been ignored
     */
    SIGHT_FILTER_DICOM_API static bool apply_filter(
        dicom_series_container_t& _dicom_series_container,
        sight::filter::dicom::filter::sptr _filter,
        bool _forced_apply                     = false,
        const core::log::logger::sptr& _logger = std::make_shared<core::log::logger>()
    );
};

} // namespace sight::filter::dicom::helper
