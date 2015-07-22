/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwDicomIOFilter/composite/IComposite.hpp"

#include "fwDicomIOFilter/helper/Filter.hpp"

namespace fwDicomIOFilter
{
namespace helper
{


void Filter::applyFilter(DicomSeriesContainerType& dicomSeriesContainer, ::fwDicomIOFilter::IFilter::sptr filter,
                         bool forcedApply)
{
    DicomSeriesContainerType result;

    // On every DicomSeries
    BOOST_FOREACH(const ::fwDicomData::DicomSeries::sptr& dicomSeries, dicomSeriesContainer)
    {
        // Apply filter and and copy result
        DicomSeriesContainerType tempo;
        // Regular apply
        if(!forcedApply || filter->getFilterType() != ::fwDicomIOFilter::IFilter::COMPOSITE)
        {
            tempo = filter->apply(dicomSeries);
        }
        // Forced apply for composite
        else
        {
            ::fwDicomIOFilter::composite::IComposite::sptr composite =
                ::fwDicomIOFilter::composite::IComposite::dynamicCast(filter);
            tempo = composite->forcedApply(dicomSeries);
        }
        result.reserve(result.size() + tempo.size());
        std::copy(tempo.begin(), tempo.end(), std::back_inserter(result));
    }

    // Copy result to DicomSeries container
    dicomSeriesContainer = result;
}

} // namespace helper
} // namespace fwDicomIOFilter
