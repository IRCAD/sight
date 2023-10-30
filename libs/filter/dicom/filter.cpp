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

#include "filter.hpp"

namespace sight::filter::dicom
{

filter::filter()
= default;

//-----------------------------------------------------------------------------

filter::~filter()
= default;

//-----------------------------------------------------------------------------

bool filter::is_configuration_required() const
{
    return false;
}

//-----------------------------------------------------------------------------

bool filter::is_configurable_with_gui() const
{
    return false;
}

//-----------------------------------------------------------------------------

void filter::configure_with_gui()
{
    SIGHT_WARN("You should override this function in order to configure your filter using a GUI frame.");
}

} // namespace sight::filter::dicom
