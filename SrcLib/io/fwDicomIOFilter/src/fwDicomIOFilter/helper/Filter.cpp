/************************************************************************
 *
 * Copyright (C) 2009-2016 IRCAD France
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

#include "fwDicomIOFilter/composite/IComposite.hpp"

#include "fwDicomIOFilter/helper/Filter.hpp"

namespace fwDicomIOFilter
{
namespace helper
{


bool Filter::applyFilter(DicomSeriesContainerType& dicomSeriesContainer, ::fwDicomIOFilter::IFilter::sptr filter,
                         bool forcedApply, const ::fwLog::Logger::sptr& logger)
{
    bool ignoredError = false;
    DicomSeriesContainerType result;

    // On every DicomSeries
    for(const ::fwMedData::DicomSeries::sptr& dicomSeries :  dicomSeriesContainer)
    {
        // Apply filter and copy result
        DicomSeriesContainerType tempo;
        // Regular filter application
        if(!forcedApply || filter->getFilterType() != ::fwDicomIOFilter::IFilter::COMPOSITE)
        {
            try
            {
                tempo = filter->apply(dicomSeries, logger);
            }
            catch(::fwDicomIOFilter::exceptions::FilterFailure& e)
            {
                if(!forcedApply)
                {
                    throw e;
                }
                else
                {
                    ignoredError = true;
                    tempo.push_back(dicomSeries);
                }
            }
        }
        // Forced filter application for composite
        else
        {
            ::fwDicomIOFilter::composite::IComposite::sptr composite =
                ::fwDicomIOFilter::composite::IComposite::dynamicCast(filter);
            tempo = composite->forcedApply(dicomSeries, logger);
        }
        result.reserve(result.size() + tempo.size());
        std::copy(tempo.begin(), tempo.end(), std::back_inserter(result));
    }

    // Copy result to DicomSeries container
    dicomSeriesContainer = result;

    return ignoredError;
}

} // namespace helper
} // namespace fwDicomIOFilter
