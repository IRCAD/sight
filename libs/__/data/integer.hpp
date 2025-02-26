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

#include "data/factory/new.hpp"
#include "data/object.hpp"
#include "data/scalar.hpp"

namespace sight::data
{

/**
 * @brief   This class contains an integer value.
 * integer object is essentially used as a field in other objects.
 */
class SIGHT_DATA_CLASS_API integer final : public scalar<std::int64_t>
{
public:

    SIGHT_DECLARE_CLASS(integer, data::string_serializable);

    using scalar<std::int64_t>::scalar;
    using scalar<std::int64_t>::operator=;

    /**
     * @brief Destructor.
     */
    SIGHT_DATA_API ~integer() noexcept override = default;
};

} // namespace sight::data
