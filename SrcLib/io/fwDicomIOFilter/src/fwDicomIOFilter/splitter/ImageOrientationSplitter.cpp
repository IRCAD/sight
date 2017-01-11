/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwDicomIOFilter/splitter/ImageOrientationSplitter.hpp"
#include "fwDicomIOFilter/registry/macros.hpp"

#include <dcmtk/dcmdata/dcdeftag.h>

#include <string>

fwDicomIOFilterRegisterMacro( ::fwDicomIOFilter::splitter::ImageOrientationSplitter );

namespace fwDicomIOFilter
{
namespace splitter
{

const std::string ImageOrientationSplitter::s_FILTER_NAME        = "Image orientation splitter";
const std::string ImageOrientationSplitter::s_FILTER_DESCRIPTION =
    "Split instances according to image orientation tag.";

//-----------------------------------------------------------------------------

ImageOrientationSplitter::ImageOrientationSplitter(::fwDicomIOFilter::IFilter::Key key) : TagValueSplitter(key)
{
    this->setTag(DCM_ImageOrientationPatient);
}

//-----------------------------------------------------------------------------

ImageOrientationSplitter::~ImageOrientationSplitter()
{
}

//-----------------------------------------------------------------------------

std::string ImageOrientationSplitter::getName() const
{
    return ImageOrientationSplitter::s_FILTER_NAME;
}

//-----------------------------------------------------------------------------

std::string ImageOrientationSplitter::getDescription() const
{
    return ImageOrientationSplitter::s_FILTER_DESCRIPTION;
}

//-----------------------------------------------------------------------------

bool ImageOrientationSplitter::isConfigurationRequired() const
{
    return false;
}

//-----------------------------------------------------------------------------

} // namespace splitter
} // namespace fwDicomIOFilter
