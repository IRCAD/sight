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

namespace sight::ui::test_core::helper
{

class Slider
{
public:

    enum class Position
    {
        TOP,
        RIGHT,
        BOTTOM,
        LEFT
    };

    /**
     * Move the slider by clicking on the slider, thus moving by a set number of steps
     * @param tester The current tester
     * @param slider How to get the slider to be moved
     * @param position The direction where the slider must be moved
     * @param times The number of times the slider must be moved
     * @{
     */
    GUITEST_API static void move(Tester& _tester, const Select& _slider, Position _pos);
    GUITEST_API static void move(Tester& _tester, const Select& _slider, Position _pos, int _times);
    /// @}

    /**
     * Set the value of the slider by calling setValue slot
     * @param tester The current tester
     * @param slider How to get the slider to be setted
     * @param value The new value of the slider
     */
    GUITEST_API static void set(Tester& _tester, const Select& _slider, int _value);
};

} // namespace sight::ui::testCore::helper
