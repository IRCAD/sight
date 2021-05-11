/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#pragma once

#include "modules/io/atoms/config.hpp"

#include <core/macros.hpp>

#include <io/atoms/filter/IFilter.hpp>

namespace sight::atoms
{
class Object;
}

namespace sight::module::io::atoms
{

/**
 * @brief Filters out unknown activities from sight::atoms::Object.
 *
 * @see sight::io::atoms::filter::IFilter
 */
class MODULE_IO_ATOMS_CLASS_API FilterUnknownActivities : public sight::io::atoms::filter::IFilter
{

public:
    /// Constructor.
    FilterUnknownActivities(sight::io::atoms::filter::IFilter::Key key);

    /// Destructor.
    ~FilterUnknownActivities();

    /// Apply filter onto given atom object.
    MODULE_IO_ATOMS_API virtual void apply(const SPTR(sight::atoms::Object)& atom);
};

} // namespace sight::module::io::atoms
