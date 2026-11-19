/************************************************************************
 *
 * Copyright (C) 2024-2026 IRCAD France
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

#include <ui/test/gui_test.hpp>

#include <QColor>
#include <QPushButton>
#include <QVariant>

#include <string>
#include <string_view>
#include <utility>

/// A setting of the first panel; the second one edits the same data.
static std::string first(std::string_view _key)
{
    return "properties1_srv/" + std::string(_key);
}

//------------------------------------------------------------------------------

static std::string second(std::string_view _key)
{
    return "properties2_srv/" + std::string(_key);
}

//------------------------------------------------------------------------------

SIGHT_GUI_TEST_CASE("synchronization")
{
    using sight::ui::test::at;

    // Steps as a user takes them, one key at a time.
    const auto press = [&ui](std::string_view _target, std::string_view _key, int _times)
                       {
                           for(int i = 0 ; i < _times ; ++i)
                           {
                               ui.press(_target, _key);
                           }
                       };
    const auto expect_both = [&ui](std::string_view _key, std::string_view _text)
                             {
                                 ui.expect(first(_key)).text(_text);
                                 ui.expect(second(_key)).text(_text);
                             };

    // The 'Enable' check box of one panel drives the other, and what depends on it.
    ui.click(first("enable"));
    ui.expect(first("enable")).unchecked();
    ui.expect(second("enable_obj")).unchecked();
    ui.expect(first("bool_1")).disabled();
    ui.expect(first("bool_1")).unchecked();

    ui.click(first("enable"));
    ui.expect(first("enable")).checked();
    ui.expect(second("enable_obj")).checked();

    ui.click(first("bool_1"));
    ui.expect(first("bool_1")).checked();
    ui.expect(second("bool_1")).checked();

    // The same through the switch buttons.
    ui.click(first("enable"));
    ui.expect(first("enable")).unchecked();
    ui.expect(second("enable_obj")).unchecked();
    ui.expect(first("bool_2")).disabled();
    ui.expect(first("bool_2")).unchecked();

    ui.click(first("enable"));
    ui.expect(first("enable")).checked();
    ui.expect(second("enable_obj")).checked();

    ui.click(first("bool_2"));
    ui.expect(first("bool_2")).checked();
    ui.expect(second("bool_2")).checked();

    // From the second panel to the first.
    ui.click(second("bool_1"));
    ui.expect(second("bool_1")).unchecked();
    ui.expect(first("bool_1")).unchecked();

    // The color, as each button shows it.
    const QColor black(0, 0, 0);
    ui.set(first("color_1"), black);
    for(const auto& color : {first("color_1"), second("color_1")})
    {
        ui.expect(color).that<QPushButton>(
            "shows black",
            [black](const QPushButton& _button){return _button.icon().pixmap(1).toImage().pixelColor(0, 0) == black;});
    }

    // Spin boxes and sliders, by their steps.
    press(first("double_1"), "Up", 5);
    expect_both("double_1", "0.08");

    press(first("double_2"), "PgUp", 5);
    expect_both("double_2/value_label", "500.31");

    press(first("double2_1"), "Up", 5);
    press(first("double2_1/1"), "Down", 5);
    expect_both("double2_1", "10.000");
    expect_both("double2_1/1", "2.000");

    press(first("double3_1"), "Up", 4);
    press(first("double3_1/1"), "Down", 4);
    press(first("double3_1/2"), "Up", 4);
    expect_both("double3_1", "9.000");
    expect_both("double3_1/1", "4.000");
    expect_both("double3_1/2", "24.000");

    press(first("int_1"), "Down", 5);
    expect_both("int_1", "45");

    press(first("int_2"), "PgDown", 2);
    expect_both("int_2/value_label", "30");

    press(first("int2_1"), "Up", 5);
    press(first("int2_1/1"), "Down", 5);
    expect_both("int2_1", "10");
    expect_both("int2_1/1", "5");

    press(first("int3_1"), "Up", 5);
    press(first("int3_1/1"), "Down", 5);
    press(first("int3_1/2"), "Up", 5);
    expect_both("int3_1", "10");
    expect_both("int3_1/1", "7");
    expect_both("int3_1/2", "17");

    ui.select(first("enum_1"), "V1");
    expect_both("enum_1", "V1");

    // The non-linear slider and the combo box of the same value.
    ui.scroll_to(first("nonlin"));
    ui.set(first("nonlin"), 120);
    ui.expect(first("nonlin/value_label")).text("120");
    ui.expect(second("nonlin")).text("120");
    ui.select(second("nonlin"), "50");
    ui.expect(first("nonlin/value_label")).text("50");

    ui.scroll_to(first("buttonRaw/button1"));
    ui.click(first("buttonRaw/button1"));
    ui.expect(second("buttonRaw/button1")).checked();

    for(const auto& [key, text] : {std::pair {"text", "test"}, std::pair {"file", "ff"}, std::pair {"dir", "dd"}})
    {
        ui.scroll_to(first(key));
        ui.set(first(key), text);
        ui.expect(second(key)).text(text);
    }

    // A tickmarks slider only follows the mouse: a drag of a width moves it by twelve ticks the other way.
    const auto expect_ticks = [&ui](std::string_view _key, std::string_view _value)
                              {
                                  for(const auto& slider : {first(_key), second(_key)})
                                  {
                                      ui.expect(slider).that<QWidget>(
                                          "is at " + std::string(_value),
                                          [value = std::string(_value)](const QWidget& _slider)
            {
                return _slider.property("current_value").toString().toStdString() == value;
            });
                                  }
                              };
    const auto drag_ticks = [&ui](std::string_view _key, int _ticks)
                            {
                                const double width = static_cast<double>(_ticks) / 12.;
                                ui.scroll_to(first(_key));
                                ui.drag(first(_key), at(0.5 + width / 2., 0.5), at(0.5 - width / 2., 0.5));
                            };

    for(const auto* const key : {"text_ticks", "int_ticks"})
    {
        // Back to the first of its 21 ticks whatever the start, then on the third one.
        drag_ticks(key, -11);
        drag_ticks(key, -11);
        drag_ticks(key, 2);
    }

    expect_ticks("text_ticks", "100");
    expect_ticks("int_ticks", "100");

    drag_ticks("text_ticks", 1);
    expect_ticks("text_ticks", "150");
    drag_ticks("text_ticks", -1);
    expect_ticks("text_ticks", "100");

    for(const auto* const key : {"text_ticks", "int_ticks"})
    {
        drag_ticks(key, -11);
    }

    expect_ticks("text_ticks", "45");
    expect_ticks("int_ticks", "45");
}
