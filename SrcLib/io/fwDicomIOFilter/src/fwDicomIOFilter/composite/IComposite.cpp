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

namespace fwDicomIOFilter
{
namespace composite
{

IComposite::IComposite()
{
}

//-----------------------------------------------------------------------------

IComposite::~IComposite()
{
}

//-----------------------------------------------------------------------------

IFilter::FilterType IComposite::getFilterType() const
{
    return IFilter::COMPOSITE;
}

//-----------------------------------------------------------------------------

IComposite::DicomSeriesContainerType IComposite::apply(
    const ::fwMedData::DicomSeries::sptr& series, const ::fwLog::Logger::sptr& logger) const
{
    DicomSeriesContainerType result;
    result.push_back(series);
    // For every filter
    for(const ::fwDicomIOFilter::IFilter::sptr& filter :  m_filterContainer)
    {
        DicomSeriesContainerType filtered;
        // For every serie
        for(const ::fwMedData::DicomSeries::sptr& s :  result)
        {
            DicomSeriesContainerType tempo = filter->apply(s, logger);
            filtered.reserve(filtered.size() + tempo.size());
            std::copy(tempo.begin(), tempo.end(), std::back_inserter(filtered));
        }
        result = filtered;
    }
    return result;
}

//-----------------------------------------------------------------------------

IComposite::DicomSeriesContainerType IComposite::forcedApply(
    const ::fwMedData::DicomSeries::sptr& series, const ::fwLog::Logger::sptr& logger) const
{
    DicomSeriesContainerType result;
    result.push_back(series);
    // For every filters
    for(const ::fwDicomIOFilter::IFilter::sptr& filter :  m_filterContainer)
    {
        DicomSeriesContainerType filtered;
        // For every series
        for(const ::fwMedData::DicomSeries::sptr& s :  result)
        {
            try
            {
                DicomSeriesContainerType tempo = filter->apply(s, logger);
                filtered.reserve(filtered.size() + tempo.size());
                std::copy(tempo.begin(), tempo.end(), std::back_inserter(filtered));
            }
            catch(::fwDicomIOFilter::exceptions::FilterFailure e)
            {
                OSLM_WARN("Unable to apply the filter \"" << filter->getName() << "\".");
                filtered.push_back(s);
            }
        }
        result = filtered;
    }
    return result;
}

//-----------------------------------------------------------------------------

void IComposite::addChild(const ::fwDicomIOFilter::IFilter::sptr& filter)
{
    m_filterContainer.push_back(filter);
}

//-----------------------------------------------------------------------------

void IComposite::removeChild(const ::fwDicomIOFilter::IFilter::sptr& filter)
{
    FilterContainerType::iterator it = std::find(m_filterContainer.begin(), m_filterContainer.end(), filter);
    if(it != m_filterContainer.end())
    {
        m_filterContainer.erase(it);
    }
}

//-----------------------------------------------------------------------------

IComposite::FilterContainerType& IComposite::getChildren()
{
    return m_filterContainer;
}

} // namespace composite
} // namespace fwDicomIOFilter
