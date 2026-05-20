/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
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

#include "../../../../../module/ui/viz/helper/utils.hpp"

#include <doctest/doctest.h>

TEST_SUITE("sight::module::ui::viz::utils")
{
//------------------------------------------------------------------------------

    TEST_CASE("convert_ogre_color_to_q_color")
    {
        QColor ref_color(255, 255, 255);

        QColor result_color = sight::module::ui::viz::helper::utils::convert_ogre_color_to_q_color(Ogre::ColourValue());
        CHECK(result_color.red() == ref_color.red());
        CHECK(result_color.green() == ref_color.green());
        CHECK(result_color.blue() == ref_color.blue());
        CHECK(result_color.alpha() == ref_color.alpha());
    }

//------------------------------------------------------------------------------

    TEST_CASE("convert_q_color_to_ogre_color")
    {
        Ogre::ColourValue ref_color;

        Ogre::ColourValue result_color =
            sight::module::ui::viz::helper::utils::convert_q_color_to_ogre_color(QColor(255, 255, 255));
        CHECK(static_cast<int>(result_color.r) == static_cast<int>(ref_color.r));
        CHECK(static_cast<int>(result_color.g) == static_cast<int>(ref_color.g));
        CHECK(static_cast<int>(result_color.b) == static_cast<int>(ref_color.b));
        CHECK(static_cast<int>(result_color.a) == static_cast<int>(ref_color.a));
    }

//------------------------------------------------------------------------------
} // TEST_SUITE
