/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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
