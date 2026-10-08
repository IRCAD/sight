/************************************************************************
 *
 * Copyright (C) 2023-2026 IRCAD France
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

#include "fixture.hpp"

#include <ui/test/helper/button.hpp>
#include <ui/test/helper/label.hpp>
#include <ui/test/helper/preferences_configuration.hpp>
#include <ui/test/tester.hpp>

#include <doctest/doctest.h>

namespace sight::example::ui::ex_parameters::uit
{

TEST_SUITE("ex_parameters")
{
//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "non_linear_slider_values_change")
    {
        const std::string failure_message = start(
            "NonLinearSliderValuesChange",
            [](sight::ui::test::tester& _tester)
            {
                namespace helper = sight::ui::test::helper;
                using selector   = helper::selector;

                helper::label::exactly_match(
                    _tester,
                    selector::from_parent(
                        "parameters1Srv",
                        "nonlin/minValueLabel"
                    ),
                    "25"
                );
                helper::label::exactly_match(
                    _tester,
                    selector::from_parent(
                        "parameters1Srv",
                        "nonlin/maxValueLabel"
                    ),
                    "150"
                );

                helper::button::push_menu_bar_item(_tester, "menuPreferencesView/Preference sync test");
                helper::preferences_configuration::fill(_tester, {{"nonlin", "37,70,105,135,160"}});

                helper::label::exactly_match(
                    _tester,
                    selector::from_parent(
                        "parameters1Srv",
                        "nonlin/minValueLabel"
                    ),
                    "37"
                );
                helper::label::exactly_match(
                    _tester,
                    selector::from_parent(
                        "parameters1Srv",
                        "nonlin/maxValueLabel"
                    ),
                    "160"
                );
            },
            true
        );

        // Runs on the main thread, after start() has returned: the only doctest assertion for
        // this scenario. See sight::ui::test::base::start().
        INFO(failure_message);
        REQUIRE(failure_message.empty());
    }
} // TEST_SUITE

} // namespace sight::example::ui::ex_parameters::uit
