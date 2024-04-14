/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#include "utils_test.hpp"

#include "../../../../../modules/ui/viz/helper/utils.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::ui::viz::ut::utils_test);

namespace sight::module::ui::viz::ut
{

//------------------------------------------------------------------------------

void utils_test::setUp()
{
}

//------------------------------------------------------------------------------

void utils_test::tearDown()
{
}

//------------------------------------------------------------------------------

void utils_test::convert_ogre_color_to_q_color()
{
    QColor ref_color(255, 255, 255);

    QColor result_color = module::ui::viz::helper::utils::convert_ogre_color_to_q_color(Ogre::ColourValue());
    CPPUNIT_ASSERT(result_color.red() == ref_color.red());
    CPPUNIT_ASSERT(result_color.green() == ref_color.green());
    CPPUNIT_ASSERT(result_color.blue() == ref_color.blue());
    CPPUNIT_ASSERT(result_color.alpha() == ref_color.alpha());
}

//------------------------------------------------------------------------------

void utils_test::convert_q_color_to_ogre_color()
{
    Ogre::ColourValue ref_color;

    Ogre::ColourValue result_color =
        module::ui::viz::helper::utils::convert_q_color_to_ogre_color(QColor(255, 255, 255));
    CPPUNIT_ASSERT(static_cast<int>(result_color.r) == static_cast<int>(ref_color.r));
    CPPUNIT_ASSERT(static_cast<int>(result_color.g) == static_cast<int>(ref_color.g));
    CPPUNIT_ASSERT(static_cast<int>(result_color.b) == static_cast<int>(ref_color.b));
    CPPUNIT_ASSERT(static_cast<int>(result_color.a) == static_cast<int>(ref_color.a));
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::viz::ut
