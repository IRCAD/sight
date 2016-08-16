/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
