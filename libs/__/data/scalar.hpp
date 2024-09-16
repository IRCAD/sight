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

#include "data/generic.hpp"

#include <core/compound_types.hpp>

namespace sight::data
{

/**
 * @brief   This class contains an floating point value.
 *
 * double object is essentially used as a field in other objects.
 */
template<typename T>
class SIGHT_DATA_CLASS_API scalar : public generic<T>
{
public:

    using generic<T>::generic;
    using generic<T>::operator=;

    //------------------------------------------------------------------------------

    std::string to_string() const override
    {
        return boost::lexical_cast<std::string>(this->m_value);
    }

    //------------------------------------------------------------------------------

    void from_string(const std::string& _value) override
    {
        this->m_value = boost::lexical_cast<T>(_value);
    }
};

} // namespace sight::data
