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

#include <ui/qt/widget/collapsible_section.hpp>
#include <ui/test/gui_test.hpp>

#include <QLabel>
#include <QWidget>

#include <string_view>

SIGHT_GUI_TEST_CASE("click_collapse_section")
{
    constexpr std::string_view section = "section_widget/section";
    constexpr std::string_view toggle  = "section_widget/section/toggle";
    constexpr std::string_view label   = "section_widget/under_label";
    constexpr std::string_view add     = "section_widget/add_label";
    constexpr std::string_view remove  = "section_widget/remove_label";

    // Arm before the click: the animation may finish before the action returns.
    const auto click_and_wait = [&](std::string_view _button)
                                {
                                    auto finished = ui.watch_signal(
                                        section,
                                        &sight::ui::qt::widget::collapsible_section::animation_finished
                                    );
                                    ui.click(_button);
                                    finished.wait();
                                };

    const int before_unfold = ui.read<QLabel>(label, &QWidget::y);
    click_and_wait(toggle);
    ui.expect(toggle).checked();
    ui.expect(label).that<QLabel>(
        "moves down when unfolded",
        [before_unfold](const QLabel& _label)
    {
        return _label.y() > before_unfold;
    });
    const int after_unfold = ui.read<QLabel>(label, &QWidget::y);

    // Two additions exceed the section's initial content height.
    click_and_wait(add);
    click_and_wait(add);
    ui.expect(label).that<QLabel>(
        "moves down after adding labels",
        [after_unfold](const QLabel& _label)
    {
        return _label.y() > after_unfold;
    });
    const int after_add = ui.read<QLabel>(label, &QWidget::y);

    click_and_wait(remove);
    click_and_wait(remove);
    ui.expect(label).that<QLabel>(
        "moves up after removing labels",
        [after_add](const QLabel& _label)
    {
        return _label.y() < after_add;
    });
}
