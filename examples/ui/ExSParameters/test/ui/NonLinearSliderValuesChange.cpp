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

// cspell:ignore exsparameters

#include "NonLinearSliderValuesChange.hpp"

#include <core/runtime/path.hpp>

#include <ui/testCore/helper/Button.hpp>
#include <ui/testCore/helper/Label.hpp>
#include <ui/testCore/helper/PreferencesConfiguration.hpp>
#include <ui/testCore/Tester.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::examples::ui::exsparameters::test::ui::NonLinearSliderValuesChange);

namespace sight::examples::ui::exsparameters::test::ui
{

//------------------------------------------------------------------------------

std::filesystem::path NonLinearSliderValuesChange::getProfilePath()
{
    const std::filesystem::path cwd = sight::core::runtime::getWorkingPath();
    return cwd / "share/sight/ExSParameters/profile.xml";
}

//------------------------------------------------------------------------------

void NonLinearSliderValuesChange::test()
{
    start(
        "NonLinearSliderValuesChange",
        [](sight::ui::testCore::Tester& tester)
        {
            tester.getMainWindow()->dumpObjectTree();

            namespace helper = sight::ui::testCore::helper;
            using Select     = helper::Select;

            helper::Label::exactlyMatch(tester, Select::fromParent("parameters1Srv", "nonlin/minValueLabel"), "25");
            helper::Label::exactlyMatch(tester, Select::fromParent("parameters1Srv", "nonlin/maxValueLabel"), "150");

            helper::Button::pushMenuBarItem(tester, "menuPreferencesView/Preference sync test");
            helper::PreferencesConfiguration::fill(tester, {{"nonlin", "37,70,105,135,160"}});

            helper::Label::exactlyMatch(tester, Select::fromParent("parameters1Srv", "nonlin/minValueLabel"), "37");
            helper::Label::exactlyMatch(tester, Select::fromParent("parameters1Srv", "nonlin/maxValueLabel"), "160");
        },
        true
    );
}

} // namespace sight::examples::ui::exsparameters::test::ui
