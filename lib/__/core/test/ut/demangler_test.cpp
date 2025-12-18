/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
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

#include "core/base.hpp"

#include <doctest/doctest.h>

#include <sstream>

//-----------------------------------------------------------------------------

namespace sight::core::ut
{

class idemangler_test : public sight::core::base_object
{
public:

    SIGHT_DECLARE_CLASS(idemangler_test, sight::core::base_object);
    ~idemangler_test() override = default;
};

//-----------------------------------------------------------------------------

class odemangler_test : public idemangler_test
{
public:

    SIGHT_DECLARE_CLASS(odemangler_test, idemangler_test);
    ~odemangler_test() override = default;
};

} // namespace sight::core::ut

TEST_SUITE("sight::core::demangler")
{
    TEST_CASE("class_name")
    {
        CHECK_EQ(std::string("sight::core::ut::idemangler_test"), sight::core::ut::idemangler_test::classname());
        CHECK_EQ(std::string("sight::core::ut::odemangler_test"), sight::core::ut::odemangler_test::classname());

        CHECK_EQ(std::string("idemangler_test"), sight::core::ut::idemangler_test::leaf_classname());
        CHECK_EQ(std::string("odemangler_test"), sight::core::ut::odemangler_test::leaf_classname());

        auto object                                           = std::make_shared<sight::core::ut::odemangler_test>();
        sight::core::ut::idemangler_test::sptr generic_object = object;
        CHECK_EQ(std::string("sight::core::ut::odemangler_test"), object->get_classname());
        CHECK_EQ(std::string("sight::core::ut::odemangler_test"), generic_object->get_classname());

        CHECK_EQ(std::string("odemangler_test"), object->get_leaf_classname());
        CHECK_EQ(std::string("odemangler_test"), generic_object->get_leaf_classname());
    }
} // TEST_SUITE
