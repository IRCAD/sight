/************************************************************************
 *
 * Copyright (C) 2024 IRCAD France
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

#include <sight/data/config.hpp>

#include "data/object.hpp"

#include <core/base.hpp>

#include <ostream>
#include <string>

namespace sight::data
{

/**
 * @brief string_serializable define common operation on simple data.
 */
class SIGHT_DATA_CLASS_API string_serializable : public object
{
public:

    SIGHT_DECLARE_CLASS(string_serializable, object);

    SIGHT_DATA_API friend std::ostream& operator<<(std::ostream& /*_os*/, const string_serializable& /*lf*/);

    SIGHT_DATA_API virtual std::string to_string() const               = 0;
    SIGHT_DATA_API virtual void from_string(const std::string& _value) = 0;

    /// Reset to the default value
    SIGHT_DATA_API virtual void reset() = 0;

    /// Set the current value as the default
    SIGHT_DATA_API virtual void set_default_value() = 0;

private:

    SIGHT_DATA_API virtual std::ostream& to_ostream(std::ostream& _os) const = 0;
};

} // namespace sight::data
