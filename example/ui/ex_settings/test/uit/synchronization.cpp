/************************************************************************
 *
 * Copyright (C) 2024-2025 IRCAD France
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

#include "synchronization.hpp"

#include <core/runtime/path.hpp>

#include <ui/test/helper/button.hpp>
#include <ui/test/helper/check_box.hpp>
#include <ui/test/helper/color_parameter.hpp>
#include <ui/test/helper/combo_box.hpp>
#include <ui/test/helper/label.hpp>
#include <ui/test/helper/line_edit.hpp>
#include <ui/test/helper/slider.hpp>
#include <ui/test/helper/spin_box.hpp>
#include <ui/test/tester.hpp>

#include <QToolButton>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::example::ui::ex_settings::uit::synchronization);

namespace sight::example::ui::ex_settings::uit
{

//------------------------------------------------------------------------------

std::filesystem::path synchronization::get_profile_path()
{
    const std::filesystem::path cwd = sight::core::runtime::working_path();
    return cwd / "share/sight/ex_settings/profile.xml";
}

//------------------------------------------------------------------------------

void synchronization::test()
{
    namespace helper = sight::ui::test::helper;
    using selector   = helper::selector;

    start(
        "Synchronization",
        [](sight::ui::test::tester& _tester)
        {
            {
                auto bt = _tester.add_in_backtrace("Check synchronization between 'Enable' checkboxes");
                helper::check_box::toggle(_tester, selector::from_parent("properties1_srv", "enable"));
                helper::check_box::should_not_be_checked(_tester, selector::current());
                helper::check_box::should_not_be_checked(
                    _tester,
                    selector::from_parent("properties2_srv", "enable_obj")
                );
            }

            {
                auto bt =
                    _tester.add_in_backtrace(
                        "Check that 'Boolean' checkbox doesn't do anything because it is disabled"
                    );
                helper::check_box::toggle(_tester, selector::from_parent("properties1_srv", "bool_1"));
                helper::check_box::should_not_be_checked(_tester, selector::current());
            }

            {
                auto bt = _tester.add_in_backtrace("Re-enable the parameters by re-checking the 'Enable' checkboxes");
                helper::check_box::toggle(_tester, selector::from_parent("properties1_srv", "enable"));
                helper::check_box::should_be_checked(_tester, selector::current());
                helper::check_box::should_be_checked(_tester, selector::from_parent("properties2_srv", "enable_obj"));
            }

            {
                auto bt = _tester.add_in_backtrace("Check synchronization between 'Boolean' checkboxes");
                helper::check_box::toggle(_tester, selector::from_parent("properties1_srv", "bool_1"));
                helper::check_box::should_be_checked(_tester, selector::current());
                helper::check_box::should_be_checked(_tester, selector::from_parent("properties2_srv", "bool_obj"));
            }

            {
                auto bt = _tester.add_in_backtrace("Check synchronization between 'Color' color selectors");
                helper::color_parameter::select(
                    _tester,
                    selector::from_parent("properties1_srv", "color_1"),
                    {0, 0, 0
                    });
                helper::color_parameter::color_equals(
                    _tester,
                    selector::from_parent(
                        "properties1_srv",
                        "color_1"
                    ),
                    {0, 0, 0
                    });
                helper::color_parameter::color_equals(
                    _tester,
                    selector::from_parent("properties2_srv", "color_obj"),
                    {0, 0, 0
                    });
            }

            {
                auto bt = _tester.add_in_backtrace("Check synchronization between 'Double 1' spinboxes");
                helper::spin_box::increment(_tester, selector::from_parent("properties1_srv", "double_1"), 5);
                helper::spin_box::value_equals(_tester, selector::current(), "0.08");
                helper::spin_box::value_equals(
                    _tester,
                    selector::from_parent("properties2_srv", "double_1_obj"),
                    "0.08"
                );
            }

            {
                auto bt = _tester.add_in_backtrace("Check synchronization between 'Double 2' sliders");
                helper::slider::move(
                    _tester,
                    selector::from_parent("properties1_srv", "double_2"),
                    helper::slider::position::right,
                    5
                );
                helper::label::exactly_match(
                    _tester,
                    selector::from_parent(
                        "properties1_srv",
                        "double_2/valueLabel"
                    ),
                    "500.31"
                );
                helper::label::exactly_match(
                    _tester,
                    selector::from_parent(
                        "properties2_srv",
                        "double_2_obj/valueLabel"
                    ),
                    "500.31"
                );
            }

            {
                auto bt = _tester.add_in_backtrace("Check synchronization between '2 Double' spinboxes");
                helper::spin_box::increment(_tester, selector::from_parent("properties1_srv", "double2_1"), 5);
                helper::spin_box::value_equals(_tester, selector::current(), "10.000");
                helper::spin_box::decrement(_tester, selector::from_parent("properties1_srv", "double2_1/1"), 5);
                helper::spin_box::value_equals(_tester, selector::current(), "2.000");
                helper::spin_box::value_equals(
                    _tester,
                    selector::from_parent("properties2_srv", "double2_1_obj"),
                    "10.000"
                );
                helper::spin_box::value_equals(
                    _tester,
                    selector::from_parent("properties2_srv", "double2_1_obj/1"),
                    "2.000"
                );
            }

            {
                auto bt = _tester.add_in_backtrace("Check synchronization between '3 Double' spinboxes");
                helper::spin_box::increment(_tester, selector::from_parent("properties1_srv", "double3_1"), 4);
                helper::spin_box::value_equals(_tester, selector::current(), "9.000");
                helper::spin_box::decrement(_tester, selector::from_parent("properties1_srv", "double3_1/1"), 4);
                helper::spin_box::value_equals(_tester, selector::current(), "4.000");
                helper::spin_box::increment(_tester, selector::from_parent("properties1_srv", "double3_1/2"), 4);
                helper::spin_box::value_equals(_tester, selector::current(), "24.000");
                helper::spin_box::value_equals(
                    _tester,
                    selector::from_parent("properties2_srv", "double3_1_obj"),
                    "9.000"
                );
                helper::spin_box::value_equals(
                    _tester,
                    selector::from_parent(
                        "properties2_srv",
                        "double3_1_obj/1"
                    ),
                    "4.000"
                );
                helper::spin_box::value_equals(
                    _tester,
                    selector::from_parent(
                        "properties2_srv",
                        "double3_1_obj/2"
                    ),
                    "24.000"
                );
            }

            {
                auto bt = _tester.add_in_backtrace("Check synchronization between 'Integer 1' spinboxes");
                helper::spin_box::decrement(_tester, selector::from_parent("properties1_srv", "int_1"), 5);
                helper::spin_box::value_equals(_tester, selector::current(), 45);
                helper::spin_box::value_equals(_tester, selector::from_parent("properties2_srv", "int_1_obj"), 45);
            }

            {
                auto bt = _tester.add_in_backtrace("Check synchronization between 'Integer 2' sliders");
                helper::slider::move(
                    _tester,
                    selector::from_parent("properties1_srv", "int_2"),
                    helper::slider::position::left,
                    2
                );
                helper::label::exactly_match(
                    _tester,
                    selector::from_parent("properties1_srv", "int_2/valueLabel"),
                    "30"
                );
                helper::label::exactly_match(
                    _tester,
                    selector::from_parent("properties2_srv", "int_2_obj/valueLabel"),
                    "30"
                );
            }

            {
                auto bt = _tester.add_in_backtrace("Check synchronization between '2 Integer' spinboxes");
                helper::spin_box::increment(_tester, selector::from_parent("properties1_srv", "int2_1"), 5);
                helper::spin_box::value_equals(_tester, selector::current(), 10);
                helper::spin_box::decrement(_tester, selector::from_parent("properties1_srv", "int2_1/1"), 5);
                helper::spin_box::value_equals(_tester, selector::current(), 5);
                helper::spin_box::value_equals(
                    _tester,
                    selector::from_parent("properties2_srv", "int2_1_obj"),
                    10
                );
                helper::spin_box::value_equals(
                    _tester,
                    selector::from_parent("properties2_srv", "int2_1_obj/1"),
                    5
                );
            }

            {
                auto bt = _tester.add_in_backtrace("Check synchronization between '3 Integer' spinboxes");
                helper::spin_box::increment(_tester, selector::from_parent("properties1_srv", "int3_1"), 5);
                helper::spin_box::value_equals(_tester, selector::current(), 10);
                helper::spin_box::decrement(_tester, selector::from_parent("properties1_srv", "int3_1/1"), 5);
                helper::spin_box::value_equals(_tester, selector::current(), 7);
                helper::spin_box::increment(_tester, selector::from_parent("properties1_srv", "int3_1/2"), 5);
                helper::spin_box::value_equals(_tester, selector::current(), 17);
                helper::spin_box::value_equals(
                    _tester,
                    selector::from_parent("properties2_srv", "int3_1_obj"),
                    10
                );
                helper::spin_box::value_equals(
                    _tester,
                    selector::from_parent("properties2_srv", "int3_1_obj/1"),
                    7
                );
                helper::spin_box::value_equals(
                    _tester,
                    selector::from_parent("properties2_srv", "int3_1_obj/2"),
                    17
                );
            }

            {
                auto bt = _tester.add_in_backtrace("Check synchronization between 'Enum' comboboxes");
                helper::combo_box::select(_tester, selector::from_parent("properties1_srv", "enum_1"), "V1");
                helper::combo_box::value_equals(_tester, selector::current(), "V1");
                helper::combo_box::value_equals(_tester, selector::from_parent("properties2_srv", "enum_1_obj"), "V1");
            }

            {
                auto bt = _tester.add_in_backtrace(
                    "Check synchronization from 'Nonlinear' non-linear slider to combobox"
                );
                helper::slider::move(
                    _tester,
                    selector::from_parent(
                        "properties1_srv",
                        "nonlin"
                    ),
                    helper::slider::position::right
                );
                helper::label::exactly_match(
                    _tester,
                    selector::from_parent("properties1_srv", "nonlin/valueLabel"),
                    "120"
                );
                helper::combo_box::value_equals(_tester, selector::from_parent("properties2_srv", "nonlin_obj"), "120");
            }

            {
                auto bt = _tester.add_in_backtrace(
                    "Check synchronization from 'Nonlinear' combobox to non-linear slider"
                );
                helper::combo_box::select(_tester, selector::from_parent("properties2_srv", "nonlin_obj"), "50");
                helper::combo_box::value_equals(_tester, selector::current(), "50");
                helper::label::exactly_match(
                    _tester,
                    selector::from_parent(
                        "properties1_srv",
                        "nonlin/valueLabel"
                    ),
                    "50"
                );
            }

            {
                auto bt = _tester.add_in_backtrace("Check synchronization button bar enum to button bar enum");
                helper::button::push(_tester, selector::from_parent("properties1_srv", "buttonRaw_button1"));

                selector::from_parent("properties2_srv", "button_obj_button1").select(_tester);
                _tester.doubt<QToolButton*>(
                    "buttonRaw_button1 should be checked",
                    [](QToolButton* _obj)
                {
                    return _obj->isChecked();
                });
            }
            {
                auto bt = _tester.add_in_backtrace("Check synchronization text string");
                helper::line_edit::set_current_text(_tester, selector::from_parent("properties1_srv", "text"), "test");
                helper::line_edit::match(_tester, selector::from_parent("properties2_srv", "text_obj"), "test");
            }
            {
                auto bt = _tester.add_in_backtrace("Check synchronization text string");
                helper::line_edit::set_current_text(_tester, selector::from_parent("properties1_srv", "file"), "ff");
                helper::line_edit::match(_tester, selector::from_parent("properties2_srv", "file_obj"), "ff");
            }
            {
                auto bt = _tester.add_in_backtrace("Check synchronization text string");
                helper::line_edit::set_current_text(_tester, selector::from_parent("properties1_srv", "dir"), "dd");
                helper::line_edit::match(_tester, selector::from_parent("properties2_srv", "dir_obj"), "dd");
            }
        },
        true
    );
}

} // namespace sight::example::ui::ex_settings::uit
