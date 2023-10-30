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

#include "filter/dicom/splitter/image_dimensions_splitter.hpp"

#include "filter/dicom/exceptions/filter_failure.hpp"
#include "filter/dicom/helper/filter.hpp"
#include "filter/dicom/registry/macros.hpp"
#include "filter/dicom/splitter/tag_value_splitter.hpp"

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmimgle/dcmimage.h>
#include <dcmtk/dcmnet/diutil.h>

SIGHT_REGISTER_DICOM_FILTER(sight::filter::dicom::splitter::image_dimensions_splitter);

namespace sight::filter::dicom::splitter
{

const std::string image_dimensions_splitter::FILTER_NAME        = "Image dimensions splitter";
const std::string image_dimensions_splitter::FILTER_DESCRIPTION =
    "Split instances according to image dimensions.";

//-----------------------------------------------------------------------------

std::string image_dimensions_splitter::get_name() const
{
    return image_dimensions_splitter::FILTER_NAME;
}

//-----------------------------------------------------------------------------

std::string image_dimensions_splitter::get_description() const
{
    return image_dimensions_splitter::FILTER_DESCRIPTION;
}

//-----------------------------------------------------------------------------

image_dimensions_splitter::dicom_series_container_t image_dimensions_splitter::apply(
    const data::dicom_series::sptr& _series,
    const core::log::logger::sptr& _logger
) const
{
    dicom_series_container_t result;
    result.push_back(_series);

    // Columns splitter
    auto columns_splitter = std::make_shared<sight::filter::dicom::splitter::tag_value_splitter>();
    columns_splitter->set_tag(DCM_Columns);
    sight::filter::dicom::helper::filter::apply_filter(result, columns_splitter, false, _logger);

    // Rows splitter
    auto rows_splitter = std::make_shared<sight::filter::dicom::splitter::tag_value_splitter>();
    rows_splitter->set_tag(DCM_Rows);
    sight::filter::dicom::helper::filter::apply_filter(result, rows_splitter, false, _logger);

    return result;
}

} // namespace sight::filter::dicom::splitter
