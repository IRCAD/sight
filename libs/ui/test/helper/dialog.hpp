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

#include <ui/test/tester.hpp>

namespace sight::ui::test::helper
{

class dialog
{
public:

    /**
     * Take the current modal window, setting it as the current graphic component in tester. If the template version is
     * used, it checks if the modal window has the good type first. Fails if there is no modal window. @see Tester::take
     * @param _tester The current tester
     * @param _desc The description of the window
     * @param _child_name The objectName of a child the dialog must have. Useful to disambiguate if multiple dialogs
     * might
     *                    appear in a row
     * @{
     */
    template<typename T>
    static void take(tester& _tester, const std::string& _desc, const std::string& _child_name                   = "");
    SIGHT_UI_TEST_API static void take(tester& _tester, const std::string& _desc, const std::string& _child_name = "");
    /// @}

    /**
     * Take the current modal window, setting it as the current graphic component in tester. If the template version is
     * used, it checks if the modal window has the good type first. Continues if there is no modal window.
     * @see Tester::maybe_take
     * @param _tester The current tester
     * @param _desc The description of the window
     * @param _child_name The objectName of a child the dialog must have. Useful to disambiguate if multiple dialogs
     * might
     *                    appear in a row
     * @{
     */
    template<typename T>
    static void maybe_take(
        tester& _tester,
        const std::string& _desc,
        const std::string& _child_name = ""
    );
    SIGHT_UI_TEST_API static void maybe_take(
        tester& _tester,
        const std::string& _desc,
        const std::string& _child_name = ""
    );
    /// @}
};

} // namespace sight::ui::testCore::helper

#include "dialog.hxx"
