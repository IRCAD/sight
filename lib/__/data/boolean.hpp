/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include <sight/data/config.hpp>

#include "data/exception.hpp"
#include "data/factory/new.hpp"
#include "data/object.hpp"
#include "data/scalar.hpp"

#include <boost/algorithm/string.hpp>

namespace sight::data
{

/**
 * @brief This class contains a boolean value.
 *
 * boolean object is essentially used as a field in other objects.
 */
class SIGHT_DATA_CLASS_API boolean final : public scalar<bool>
{
public:

    SIGHT_DECLARE_CLASS(boolean, scalar<bool>);

    using scalar<bool>::scalar;
    using scalar<bool>::operator=;

    //------------------------------------------------------------------------------

    std::string to_string() const final
    {
        return this->value() ? "true" : "false";
    }

    //------------------------------------------------------------------------------

    void from_string(const std::string& _value) final
    {
        const auto value = boost::trim_copy(_value);
        if(value == "true")
        {
            this->value() = true;
        }
        else if(value == "false")
        {
            this->value() = false;
        }
        else
        {
            throw data::exception("Value " + _value + " can not be converted to a boolean.");
        }
    }
};

} // namespace sight::data
