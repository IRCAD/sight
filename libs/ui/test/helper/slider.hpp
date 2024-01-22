/************************************************************************
 *
 * Copyright (C) 2023-2024 IRCAD France
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

#include "select.hpp"

#include <ui/test/tester.hpp>

namespace sight::ui::test::helper
{

class slider
{
public:

    enum class position
    {
        top,
        right,
        bottom,
        left
    };

    /**
     * Move the slider by clicking on the slider, thus moving by a set number of steps
     * @param _tester The current tester
     * @param _slider How to get the slider to be moved
     * @param _position The direction where the slider must be moved
     * @param _times The number of times the slider must be moved
     * @{
     */
    UI_TEST_API static void move(tester& _tester, const selector& _slider, position _position, int _times);
    UI_TEST_API static void move(tester& _tester, const selector& _slider, position _position);
    /// @}

    /**
     * Set the value of the slider by calling setValue slot
     * @param _tester The current tester
     * @param _slider How to get the slider to be setted
     * @param _value The new value of the slider
     */
    UI_TEST_API static void set(tester& _tester, const selector& _slider, int _value);
};

} // namespace sight::ui::testCore::helper
