/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwDicomIOFilter/splitter/ImageTypeSplitter.hpp"
#include "fwDicomIOFilter/registry/macros.hpp"

#include <dcmtk/dcmdata/dcdeftag.h>

#include <string>

fwDicomIOFilterRegisterMacro( ::fwDicomIOFilter::splitter::ImageTypeSplitter );

namespace fwDicomIOFilter
{
namespace splitter
{

const std::string ImageTypeSplitter::s_FILTER_NAME        = "Image type splitter";
const std::string ImageTypeSplitter::s_FILTER_DESCRIPTION =
    "Split instances according to image type tag.";

//-----------------------------------------------------------------------------

ImageTypeSplitter::ImageTypeSplitter(::fwDicomIOFilter::IFilter::Key key) : TagValueSplitter(key)
{
    this->setTag(DCM_ImageType);
}

//-----------------------------------------------------------------------------

ImageTypeSplitter::~ImageTypeSplitter()
{
}

//-----------------------------------------------------------------------------

std::string ImageTypeSplitter::getName() const
{
    return ImageTypeSplitter::s_FILTER_NAME;
}

//-----------------------------------------------------------------------------

std::string ImageTypeSplitter::getDescription() const
{
    return ImageTypeSplitter::s_FILTER_DESCRIPTION;
}

//-----------------------------------------------------------------------------

bool ImageTypeSplitter::isConfigurationRequired() const
{
    return false;
}

//-----------------------------------------------------------------------------

} // namespace splitter
} // namespace fwDicomIOFilter
