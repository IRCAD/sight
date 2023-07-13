/************************************************************************
 *
 * Copyright (C) 2023 IRCAD France
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

#include "Select.hpp"

#include <ui/testCore/Tester.hpp>

namespace sight::ui::testCore::helper
{

class ColorParameter
{
public:

    /**
     * Select a color in a color parameter
     * @param tester The current tester
     * @param colorParam How to get the color parameter to modify
     * @param color The color to be selected
     */
    GUITEST_API static void select(Tester& tester, const Select& colorParam, const QColor& color);

    /**
     * Check whether the color parameter has the good color
     * @param tester The current tester
     * @param colorParam How to get the color parameter to check
     * @param color The color it should have
     */
    GUITEST_API static void colorEquals(Tester& tester, const Select& colorParam, const QColor& color);
};

} // namespace sight::ui::testCore::helper
