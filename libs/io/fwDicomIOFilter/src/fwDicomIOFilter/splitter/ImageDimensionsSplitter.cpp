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

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmnet/diutil.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmimgle/dcmimage.h>

#include "fwDicomIOFilter/helper/Filter.hpp"
#include "fwDicomIOFilter/registry/macros.hpp"
#include "fwDicomIOFilter/exceptions/FilterFailure.hpp"
#include "fwDicomIOFilter/splitter/TagValueSplitter.hpp"
#include "fwDicomIOFilter/splitter/ImageDimensionsSplitter.hpp"

fwDicomIOFilterRegisterMacro( ::fwDicomIOFilter::splitter::ImageDimensionsSplitter );

namespace fwDicomIOFilter
{
namespace splitter
{

const std::string ImageDimensionsSplitter::s_FILTER_NAME        = "Image dimensions splitter";
const std::string ImageDimensionsSplitter::s_FILTER_DESCRIPTION =
    "Split instances according to image dimensions.";

//-----------------------------------------------------------------------------

ImageDimensionsSplitter::ImageDimensionsSplitter(::fwDicomIOFilter::IFilter::Key key) : ISplitter()
{
}

//-----------------------------------------------------------------------------

ImageDimensionsSplitter::~ImageDimensionsSplitter()
{
}

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
    const ::fwMedData::DicomSeries::sptr& series, const ::fwLog::Logger::sptr& logger) const
{
    DicomSeriesContainerType result;
    result.push_back(series);

    // Columns splitter
    auto columnsSplitter = ::fwDicomIOFilter::splitter::TagValueSplitter::New();
    columnsSplitter->setTag(DCM_Columns);
    ::fwDicomIOFilter::helper::Filter::applyFilter(result, columnsSplitter, false, logger);

    // Rows splitter
    auto rowsSplitter = ::fwDicomIOFilter::splitter::TagValueSplitter::New();
    rowsSplitter->setTag(DCM_Rows);
    ::fwDicomIOFilter::helper::Filter::applyFilter(result, rowsSplitter, false, logger);

    return result;

}

} // namespace splitter
} // namespace fwDicomIOFilter
