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

#include "click_collapse_section.hpp"

#include <core/runtime/path.hpp>

#include <ui/test/helper/button.hpp>
#include <ui/test/tester.hpp>

#include <QLabel>
#include <QPushButton>
#include <QToolButton>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::example::ui::ex_collapsible_section::uit::click_collapse_section);

namespace sight::example::ui::ex_collapsible_section::uit
{

//------------------------------------------------------------------------------

std::filesystem::path click_collapse_section::get_profile_path()
{
    const std::filesystem::path cwd = sight::core::runtime::working_path();
    return cwd / "share/sight/ex_collapsible_section/profile.xml";
}

//------------------------------------------------------------------------------

void click_collapse_section::test()
{
    start(
        "click_collapse_section",
        [](sight::ui::test::tester& _tester)
        {
            // Get the pos y of under_section_label before to unfold collapsible section.
            auto* under_label       = _tester.get_main_window()->findChild<QLabel*>("under_section_label");
            int pos_y_before_unfold = under_label->y();

            _tester.take(
                "QToolButton for unfold collapsible section",
                [&_tester]() -> QObject*
            {
                return _tester.get_main_window()->findChild<QToolButton*>();
            });

            // Unfold the collapsible section
            _tester.interact(std::make_unique<sight::ui::test::mouse_click>());

            // Wait for the unfold collapsible section animation ends.
            // This time is passed when we instatiate section collapsible widget.
            QTest::qWait(1000);

            _tester.doubt<QToolButton*>(
                "After click, the QToolButton of collapsible section should set at true",
                [](QToolButton* _obj)
            {
                return _obj->isChecked();
            });

            under_label            = _tester.get_main_window()->findChild<QLabel*>("under_section_label");
            int pos_y_after_unfold = under_label->y();

            CPPUNIT_ASSERT_EQUAL_MESSAGE(
                "The position y of under_label should be bigger after unfolded the collapsible section",
                true,
                pos_y_after_unfold > pos_y_before_unfold
            );

            _tester.take(
                "QPushButton for add a new label inside collapsible section",
                [&_tester]() -> QObject*
            {
                return _tester.get_main_window()->findChild<QPushButton*>("add_label_button");
            });

            // Add a new label dynamically inside the unfolded collapsible section.
            // The content height updates after reaching a certain limit, so we add 2 QLabel.
            _tester.interact(std::make_unique<sight::ui::test::mouse_click>());
            QTest::qWait(1000);
            _tester.interact(std::make_unique<sight::ui::test::mouse_click>());
            QTest::qWait(1000);

            under_label         = _tester.get_main_window()->findChild<QLabel*>("under_section_label");
            int pos_y_after_add = under_label->y();

            CPPUNIT_ASSERT_EQUAL_MESSAGE(
                "The position y of under_label should be bigger after add a new QLabel in the collapsible section",
                true,
                pos_y_after_add > pos_y_after_unfold
            );

            _tester.take(
                "QPushButton for remove a label from collapsible section",
                [&_tester]() -> QObject*
            {
                return _tester.get_main_window()->findChild<QPushButton*>("remove_label_button");
            });

            // Remove the previously added QLabel from collapsible section.
            _tester.interact(std::make_unique<sight::ui::test::mouse_click>());
            QTest::qWait(1000);
            _tester.interact(std::make_unique<sight::ui::test::mouse_click>());
            QTest::qWait(1000);

            under_label            = _tester.get_main_window()->findChild<QLabel*>("under_section_label");
            int pos_y_after_remove = under_label->y();

            CPPUNIT_ASSERT_EQUAL_MESSAGE(
                "The position y of under_label should be smaller after remove from collapsible section",
                true,
                pos_y_after_remove < pos_y_after_add
            );
        },
        true
    );
}

} // namespace sight::example::ui::ex_collapsible_section::uit.
