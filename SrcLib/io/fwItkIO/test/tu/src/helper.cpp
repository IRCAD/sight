/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwData/Image.hpp>

#include "helper.hpp"

namespace fwItkIO
{
namespace ut
{
namespace helper
{

//------------------------------------------------------------------------------

double tolerance(double num)
{
    return std::floor(num * 100. + .5) / 100;
}

void roundSpacing(::fwData::Image::sptr image)
{
    ::fwData::Image::SpacingType spacing = image->getSpacing();
    std::transform (spacing.begin(), spacing.end(), spacing.begin(), tolerance);
    image->setSpacing(spacing);
}

//------------------------------------------------------------------------------


} //namespace helper
} //namespace ut
} //namespace fwItkIO
