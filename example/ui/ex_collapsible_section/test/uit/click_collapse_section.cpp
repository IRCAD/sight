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

#include <ui/test/tester.hpp>

#include <QLabel>
#include <QPushButton>
#include <QSignalSpy>
#include <QToolButton>

#include <doctest/doctest.h>

namespace sight::example::ui::ex_collapsible_section::uit
{

TEST_SUITE("ex_collapsible_section")
{
//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "click_collapse_section")
    {
        const std::string failure_message = start(
            "click_collapse_section",
            [](sight::ui::test::tester& _tester)
            {
                int pos_y_before_unfold = 0;

                _tester.take(
                    "collapsible section",
                    "collapsible_section"
                );
                auto* section = _tester.get<QObject*>();
                QSignalSpy animation_finished(section, SIGNAL(animation_finished()));

                const auto wait_for_animation = [&animation_finished]()
                                                {
                                                    if(!animation_finished.wait(
                                                           sight::ui::test::tester::DEFAULT_TIMEOUT
                                                    ))
                                                    {
                                                        sight::ui::test::tester::fail(
                                                            "The collapsible section animation did not finish"
                                                        );
                                                    }
                                                };

                // Get the pos y of under_section_label before to unfold collapsible section.
                _tester.take(
                    "QToolButton for unfold collapsible section",
                    [&_tester, &pos_y_before_unfold]() -> QObject*
                {
                    auto* label = _tester.get_main_window()->findChild<QLabel*>("under_section_label");

                    if(label != nullptr)
                    {
                        pos_y_before_unfold = label->y();
                    }

                    return label;
                });

                _tester.take(
                    "QToolButton for unfold collapsible section",
                    [&_tester]() -> QObject*
                {
                    return _tester.get_main_window()->findChild<QToolButton*>();
                });

                // Unfold the collapsible section
                _tester.interact(std::make_unique<sight::ui::test::mouse_click>());
                wait_for_animation();

                _tester.doubt<QToolButton*>(
                    "After click, the QToolButton of collapsible section should set at true",
                    [](QToolButton* _obj)
                {
                    return _obj->isChecked();
                });

                int pos_y_after_unfold = 0;

                _tester.take(
                    "QToolButton for unfold collapsible section",
                    [&_tester, &pos_y_after_unfold]() -> QObject*
                {
                    auto* label = _tester.get_main_window()->findChild<QLabel*>("under_section_label");

                    if(label != nullptr)
                    {
                        pos_y_after_unfold = label->y();
                    }

                    return label;
                });

                // Called from the scenario thread: report through tester::fail(), never a doctest assertion.
                if(pos_y_after_unfold <= pos_y_before_unfold)
                {
                    sight::ui::test::tester::fail(
                        "The position y of under_label should be bigger after unfolded the collapsible section"
                    );
                }

                _tester.take(
                    "QPushButton for add a new label inside collapsible section",
                    [&_tester]() -> QObject*
                {
                    return _tester.get_main_window()->findChild<QPushButton*>("add_label_button");
                });

                // Add a new label dynamically inside the unfolded collapsible section.
                // The content height updates after reaching a certain limit, so we add 2 QLabel.
                _tester.interact(std::make_unique<sight::ui::test::mouse_click>());
                wait_for_animation();
                _tester.interact(std::make_unique<sight::ui::test::mouse_click>());
                wait_for_animation();

                int pos_y_after_add = 0;

                _tester.take(
                    "QToolButton for unfold collapsible section",
                    [&_tester, &pos_y_after_add]() -> QObject*
                {
                    auto* label = _tester.get_main_window()->findChild<QLabel*>("under_section_label");

                    if(label != nullptr)
                    {
                        pos_y_after_add = label->y();
                    }

                    return label;
                });

                if(pos_y_after_add <= pos_y_after_unfold)
                {
                    sight::ui::test::tester::fail(
                        "The position y of under_label should be bigger after add a new QLabel in the collapsible section"
                    );
                }

                _tester.take(
                    "QPushButton for remove a label from collapsible section",
                    [&_tester]() -> QObject*
                {
                    return _tester.get_main_window()->findChild<QPushButton*>("remove_label_button");
                });

                // Remove the previously added QLabel from collapsible section.
                _tester.interact(std::make_unique<sight::ui::test::mouse_click>());
                wait_for_animation();
                _tester.interact(std::make_unique<sight::ui::test::mouse_click>());
                wait_for_animation();

                int pos_y_after_remove = 0;

                _tester.take(
                    "QToolButton for unfold collapsible section",
                    [&_tester, &pos_y_after_remove]() -> QObject*
                {
                    auto* label = _tester.get_main_window()->findChild<QLabel*>("under_section_label");

                    if(label != nullptr)
                    {
                        pos_y_after_remove = label->y();
                    }

                    return label;
                });

                if(pos_y_after_remove >= pos_y_after_add)
                {
                    sight::ui::test::tester::fail(
                        "The position y of under_label should be smaller after remove from collapsible section"
                    );
                }
            },
            true
        );

        // Runs on the main thread, after start() has returned: the only doctest assertion for
        // this scenario. See sight::ui::test::base::start().
        INFO(failure_message);
        REQUIRE(failure_message.empty());
    }
} // TEST_SUITE

} // namespace sight::example::ui::ex_collapsible_section::uit
