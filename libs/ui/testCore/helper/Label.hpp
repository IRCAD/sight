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

class Label
{
public:

    /**
     * Check whether the tooltip of a label exactly matches the expected result.
     * @param tester The current tester
     * @param label How to get the label to check
     * @param expected The expected content of the tooltip
     */
    GUITEST_API static void toolTipMatches(Tester& _tester, const Select& _label, const std::string& _expected);

    /**
     * Check whether the label contains the expected substring.
     * @param tester The current tester
     * @param label How to get the label to check
     * @param expected The substring expected to be contained in the label
     */
    GUITEST_API static void contain(Tester& _tester, const Select& _label, const std::string& _expected);

    /**
     * Check whether the label exactly matches the expected result.
     * @param tester The current tester
     * @param label How to get the label to check
     * @param expected The expected content of the label
     */
    GUITEST_API static void exactlyMatch(Tester& _tester, const Select& _label, const std::string& _expected);

    /**
     * Check whether the label is equal to the number with a tolerance.
     * @param tester The current tester
     * @param label How to get the label to check
     * @param expected The expected value of the label
     * @param tolerance The tolerance for the expected value
     * @param re A regexp describing how to find the number to be checked in the label, it should be the first capture
     *           group.
     */
    GUITEST_API static void equal(
        Tester& _tester,
        const Select& _label,
        double _expected,
        double _tolerance,
        QRegExp _re = QRegExp("(.*)")
    );

    /**
     * Get the value within a label.
     * @param tester The current tester
     * @param label The objectName of the label
     */
    GUITEST_API static std::string get(Tester& _tester, const Select& _label_name);
};

} // namespace sight::ui::testCore::helper
