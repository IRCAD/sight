/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "filter/dicom/custom/NoFilter.hpp"

#include "filter/dicom/exceptions/FilterFailure.hpp"
#include "filter/dicom/registry/macros.hpp"

fwDicomIOFilterRegisterMacro(::sight::filter::dicom::custom::NoFilter);

namespace sight::filter::dicom
{

namespace custom
{

const std::string NoFilter::s_FILTER_NAME        = "No filter";
const std::string NoFilter::s_FILTER_DESCRIPTION = "No filter.";

//-----------------------------------------------------------------------------

NoFilter::NoFilter(filter::dicom::IFilter::Key) :
    ICustom()
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
    const data::DicomSeries::sptr& series,
    const core::log::Logger::sptr&
)
const
{
    DicomSeriesContainerType result;
    result.push_back(series);
    return result;
}

} // namespace custom

} // namespace sight::filter::dicom
