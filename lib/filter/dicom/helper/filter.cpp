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

#include "filter/dicom/helper/filter.hpp"

#include "filter/dicom/composite/base.hpp"

namespace sight::filter::dicom::helper
{

//------------------------------------------------------------------------------

bool filter::apply_filter(
    dicom_series_container_t& _dicom_series_container,
    sight::filter::dicom::filter::sptr _filter,
    bool _forced_apply,
    const core::log::logger::sptr& _logger
)
{
    bool ignored_error = false;
    dicom_series_container_t result;

    // On every DicomSeries
    for(const data::dicom_series::sptr& dicom_series : _dicom_series_container)
    {
        // Apply filter and copy result
        dicom_series_container_t tempo;
        // Regular filter application
        if(!_forced_apply || _filter->get_filter_type() != sight::filter::dicom::filter::composite)
        {
            try
            {
                tempo = _filter->apply(dicom_series, _logger);
            }
            catch(sight::filter::dicom::exceptions::filter_failure& /*e*/)
            {
                if(!_forced_apply)
                {
                    throw;
                }

                ignored_error = true;
                tempo.push_back(dicom_series);
            }
        }
        // Forced filter application for composite
        else
        {
            sight::filter::dicom::composite::base::sptr composite =
                std::dynamic_pointer_cast<sight::filter::dicom::composite::base>(_filter);
            tempo = composite->forced_apply(dicom_series, _logger);
        }

        result.reserve(result.size() + tempo.size());
        std::copy(tempo.begin(), tempo.end(), std::back_inserter(result));
    }

    // Copy result to DicomSeries container
    _dicom_series_container = result;

    return ignored_error;
}

} // namespace sight::filter::dicom::helper
