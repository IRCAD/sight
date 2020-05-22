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
