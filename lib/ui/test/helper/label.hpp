/************************************************************************
 *
 * Copyright (C) 2023-2025 IRCAD France
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

class label
{
public:

    /**
     * Check whether the tooltip of a label exactly matches the expected result.
     * @param _tester The current tester
     * @param _label How to get the label to check
     * @param _expected The expected content of the tooltip
     */
    SIGHT_UI_TEST_API static void tool_tip_matches(
        tester& _tester,
        const selector& _label,
        const std::string& _expected
    );

    /**
     * Check whether the label contains the expected substring.
     * @param _tester The current tester
     * @param _label How to get the label to check
     * @param _expected The substring expected to be contained in the label
     */
    SIGHT_UI_TEST_API static void contain(tester& _tester, const selector& _label, const std::string& _expected);

    /**
     * Check whether the label exactly matches the expected result.
     * @param _tester The current tester
     * @param _label How to get the label to check
     * @param _expected The expected content of the label
     */
    SIGHT_UI_TEST_API static void exactly_match(tester& _tester, const selector& _label, const std::string& _expected);

    /**
     * Check whether the label is equal to the number with a tolerance.
     * @param _tester The current tester
     * @param _label How to get the label to check
     * @param _expected The expected value of the label
     * @param _tolerance The tolerance for the expected value
     * @param _re A regexp describing how to find the number to be checked in the label, it should be the first capture
     *            group.
     */
    SIGHT_UI_TEST_API static void equal(
        tester& _tester,
        const selector& _label,
        double _expected,
        double _tolerance,
        QRegularExpression _re = QRegularExpression("(.*)")
    );

    /**
     * Get the value within a label.
     * @param _tester The current tester
     * @param _label_name The objectName of the label
     */
    SIGHT_UI_TEST_API static std::string get(tester& _tester, const selector& _label_name);
};

} // namespace sight::ui::testCore::helper
