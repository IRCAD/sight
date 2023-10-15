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

#include "filter/dicom/helper/Filter.hpp"

#include "filter/dicom/composite/base.hpp"

namespace sight::filter::dicom::helper
{

//------------------------------------------------------------------------------

bool Filter::applyFilter(
    DicomSeriesContainerType& dicomSeriesContainer,
    sight::filter::dicom::filter::sptr filter,
    bool forcedApply,
    const core::log::logger::sptr& logger
)
{
    bool ignoredError = false;
    DicomSeriesContainerType result;

    // On every DicomSeries
    for(const data::dicom_series::sptr& dicomSeries : dicomSeriesContainer)
    {
        // Apply filter and copy result
        DicomSeriesContainerType tempo;
        // Regular filter application
        if(!forcedApply || filter->getFilterType() != sight::filter::dicom::filter::COMPOSITE)
        {
            try
            {
                tempo = filter->apply(dicomSeries, logger);
            }
            catch(sight::filter::dicom::exceptions::FilterFailure& /*e*/)
            {
                if(!forcedApply)
                {
                    throw;
                }

                ignoredError = true;
                tempo.push_back(dicomSeries);
            }
        }
        // Forced filter application for composite
        else
        {
            sight::filter::dicom::composite::base::sptr composite =
                std::dynamic_pointer_cast<sight::filter::dicom::composite::base>(filter);
            tempo = composite->forcedApply(dicomSeries, logger);
        }

        result.reserve(result.size() + tempo.size());
        std::copy(tempo.begin(), tempo.end(), std::back_inserter(result));
    }

    // Copy result to DicomSeries container
    dicomSeriesContainer = result;

    return ignoredError;
}

} // namespace sight::filter::dicom::helper
