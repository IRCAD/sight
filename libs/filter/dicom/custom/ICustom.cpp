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

#include "filter/dicom/custom/ICustom.hpp"

namespace sight::filter::dicom::custom
{

ICustom::ICustom()
= default;

//-----------------------------------------------------------------------------

ICustom::~ICustom()
= default;

//-----------------------------------------------------------------------------

IFilter::FilterType ICustom::getFilterType() const
{
    return IFilter::CUSTOM;
}

} // namespace sight::filter::dicom::custom
