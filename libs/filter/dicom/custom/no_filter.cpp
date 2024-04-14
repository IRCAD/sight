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

#include "filter/dicom/custom/no_filter.hpp"

#include "filter/dicom/exceptions/filter_failure.hpp"
#include "filter/dicom/registry/macros.hpp"

SIGHT_REGISTER_DICOM_FILTER(sight::filter::dicom::custom::no_filter);

namespace sight::filter::dicom::custom
{

const std::string no_filter::FILTER_NAME        = "No filter";
const std::string no_filter::FILTER_DESCRIPTION = "No filter.";

//-----------------------------------------------------------------------------

std::string no_filter::get_name() const
{
    return no_filter::FILTER_NAME;
}

//-----------------------------------------------------------------------------

std::string no_filter::get_description() const
{
    return no_filter::FILTER_DESCRIPTION;
}

//-----------------------------------------------------------------------------

no_filter::dicom_series_container_t no_filter::apply(
    const data::dicom_series::sptr& _series,
    const core::log::logger::sptr&
    /*logger*/
)
const
{
    dicom_series_container_t result;
    result.push_back(_series);
    return result;
}

} // namespace sight::filter::dicom::custom
