/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
