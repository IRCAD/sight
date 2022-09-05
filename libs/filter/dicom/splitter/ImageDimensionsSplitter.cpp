/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "filter/dicom/splitter/ImageDimensionsSplitter.hpp"

#include "filter/dicom/exceptions/FilterFailure.hpp"
#include "filter/dicom/helper/Filter.hpp"
#include "filter/dicom/registry/macros.hpp"
#include "filter/dicom/splitter/TagValueSplitter.hpp"

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmimgle/dcmimage.h>
#include <dcmtk/dcmnet/diutil.h>

fwDicomIOFilterRegisterMacro(sight::filter::dicom::splitter::ImageDimensionsSplitter);

namespace sight::filter::dicom::splitter
{

const std::string ImageDimensionsSplitter::s_FILTER_NAME        = "Image dimensions splitter";
const std::string ImageDimensionsSplitter::s_FILTER_DESCRIPTION =
    "Split instances according to image dimensions.";

//-----------------------------------------------------------------------------

ImageDimensionsSplitter::ImageDimensionsSplitter(filter::dicom::IFilter::Key /*unused*/)
{
}

//-----------------------------------------------------------------------------

ImageDimensionsSplitter::~ImageDimensionsSplitter()
= default;

//-----------------------------------------------------------------------------

std::string ImageDimensionsSplitter::getName() const
{
    return ImageDimensionsSplitter::s_FILTER_NAME;
}

//-----------------------------------------------------------------------------

std::string ImageDimensionsSplitter::getDescription() const
{
    return ImageDimensionsSplitter::s_FILTER_DESCRIPTION;
}

//-----------------------------------------------------------------------------

ImageDimensionsSplitter::DicomSeriesContainerType ImageDimensionsSplitter::apply(
    const data::DicomSeries::sptr& series,
    const core::log::Logger::sptr& logger
) const
{
    DicomSeriesContainerType result;
    result.push_back(series);

    // Columns splitter
    auto columnsSplitter = filter::dicom::splitter::TagValueSplitter::New();
    columnsSplitter->setTag(DCM_Columns);
    filter::dicom::helper::Filter::applyFilter(result, columnsSplitter, false, logger);

    // Rows splitter
    auto rowsSplitter = filter::dicom::splitter::TagValueSplitter::New();
    rowsSplitter->setTag(DCM_Rows);
    filter::dicom::helper::Filter::applyFilter(result, rowsSplitter, false, logger);

    return result;
}

} // namespace sight::filter::dicom::splitter
