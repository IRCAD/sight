/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwDicomIOFilter/custom/NoFilter.hpp"
#include "fwDicomIOFilter/exceptions/FilterFailure.hpp"
#include "fwDicomIOFilter/registry/macros.hpp"

fwDicomIOFilterRegisterMacro( ::fwDicomIOFilter::custom::NoFilter );

namespace fwDicomIOFilter
{
namespace custom
{

const std::string NoFilter::s_FILTER_NAME        = "No filter";
const std::string NoFilter::s_FILTER_DESCRIPTION = "No filter.";

//-----------------------------------------------------------------------------

NoFilter::NoFilter(::fwDicomIOFilter::IFilter::Key key) : ICustom()
{
}

//-----------------------------------------------------------------------------

NoFilter::~NoFilter()
{
}

//-----------------------------------------------------------------------------

std::string NoFilter::getName() const
{
    return NoFilter::s_FILTER_NAME;
}

//-----------------------------------------------------------------------------

std::string NoFilter::getDescription() const
{
    return NoFilter::s_FILTER_DESCRIPTION;
}

//-----------------------------------------------------------------------------

NoFilter::DicomSeriesContainerType NoFilter::apply(
    const ::fwMedData::DicomSeries::sptr& series, const ::fwLog::Logger::sptr& logger)
const
{
    DicomSeriesContainerType result;
    result.push_back(series);
    return result;
}

} // namespace custom
} // namespace fwDicomIOFilter
