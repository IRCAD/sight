/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include "core/config.hpp"
#include "core/macros.hpp"

namespace sight::core::location
{

/**
 * @brief Abstract base class to define a location (file path, directory, archive) that can be used in Sight reader /
 * writer
 */
class CORE_CLASS_API ILocation
{
public:

    SIGHT_DECLARE_CLASS(ILocation)

    /// String serialization function
    inline virtual std::string toString() const = 0;

protected:

    /// Constructor
    ILocation() = default;

    /// Destructor
    virtual ~ILocation() = default;
};

/// Default noop implementation
inline std::string ILocation::toString() const
{
    return std::string();
}

} // namespace sight::io::base::location
