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

#include "color_test.hpp"

#include <app/parser/color.hpp>

#include <data/color.hpp>

#include <boost/property_tree/ptree.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::app::parser::ut::color_test);

namespace sight::app::parser::ut
{

//------------------------------------------------------------------------------

void color_test::basic_test()
{
    boost::property_tree::ptree ptree;
    ptree.put("value", "#0077FFA3");
    auto color = std::make_shared<data::color>();
    parser::color color_parser;
    CPPUNIT_ASSERT(color_parser.is_a("sight::app::parser::color"));
    color_parser.set_object_config(ptree);
    color_parser.create_config(color);
    CPPUNIT_ASSERT_EQUAL(0.F, color->red());
    CPPUNIT_ASSERT_EQUAL(119.F / 255.F, color->green());
    CPPUNIT_ASSERT_EQUAL(1.F, color->blue());
    CPPUNIT_ASSERT_EQUAL(163.F / 255.F, color->alpha());
}

} // namespace sight::app::parser::ut
