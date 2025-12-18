/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
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

#include <core/object.hpp>

#include <doctest/doctest.h>

TEST_SUITE("sight::core::tools::object")
{
//------------------------------------------------------------------------------

    TEST_CASE("id")
    {
        const std::string uuid = "uid";

        // process
        auto obj = std::make_shared<sight::core::object>();

        obj->set_id(uuid);

        // check
        CHECK(obj->has_id());
        CHECK_EQ(obj->get_id(), uuid);
    }
} // TEST_SUITE
