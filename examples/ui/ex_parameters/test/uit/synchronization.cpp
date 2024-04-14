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

#include "synchronization.hpp"

#include <core/runtime/path.hpp>

#include <ui/test/helper/button.hpp>
#include <ui/test/helper/check_box.hpp>
#include <ui/test/helper/color_parameter.hpp>
#include <ui/test/helper/combo_box.hpp>
#include <ui/test/helper/label.hpp>
#include <ui/test/helper/slider.hpp>
#include <ui/test/helper/spin_box.hpp>
#include <ui/test/tester.hpp>

#include <QToolButton>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::examples::ui::ex_parameters::uit::synchronization);

namespace sight::examples::ui::ex_parameters::uit
{

//------------------------------------------------------------------------------

std::filesystem::path synchronization::get_profile_path()
{
    const std::filesystem::path cwd = sight::core::runtime::working_path();
    return cwd / "share/sight/ex_parameters/profile.xml";
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
                helper::check_box::toggle(_tester, selector::from_parent("parameters1Srv", "enable"));
                helper::check_box::should_not_be_checked(_tester, selector::current());
                helper::check_box::should_not_be_checked(_tester, selector::from_parent("parameters2Srv", "enable"));
            }

            {
                auto bt =
                    _tester.add_in_backtrace(
                        "Check that 'Boolean' checkbox doesn't do anything because it is disabled"
                    );
                helper::check_box::toggle(_tester, selector::from_parent("parameters1Srv", "bool_1"));
                helper::check_box::should_not_be_checked(_tester, selector::current());
            }

            {
                auto bt = _tester.add_in_backtrace("Re-enable the parameters by re-checking the 'Enable' checkboxes");
                helper::check_box::toggle(_tester, selector::from_parent("parameters1Srv", "enable"));
                helper::check_box::should_be_checked(_tester, selector::current());
                helper::check_box::should_be_checked(_tester, selector::from_parent("parameters2Srv", "enable"));
            }

            {
                auto bt = _tester.add_in_backtrace("Check synchronization between 'Boolean' checkboxes");
                helper::check_box::toggle(_tester, selector::from_parent("parameters1Srv", "bool_1"));
                helper::check_box::should_be_checked(_tester, selector::current());
                helper::check_box::should_be_checked(_tester, selector::from_parent("parameters2Srv", "bool_1"));
            }

            {
                auto bt = _tester.add_in_backtrace("Check synchronization between 'Color' color selectors");
                helper::color_parameter::select(_tester, selector::from_parent("parameters1Srv", "color_1"), {0, 0, 0});
                helper::color_parameter::color_equals(
                    _tester,
                    selector::from_parent(
                        "parameters1Srv",
                        "color_1"
                    ),
                    {0, 0, 0
                    });
                helper::color_parameter::color_equals(
                    _tester,
                    selector::from_parent(
                        "parameters2Srv",
                        "color_1"
                    ),
                    {0, 0, 0
                    });
            }

            {
                auto bt = _tester.add_in_backtrace("Check synchronization between 'Double 1' spinboxes");
                helper::spin_box::increment(_tester, selector::from_parent("parameters1Srv", "double_1"), 5);
                helper::spin_box::value_equals(_tester, selector::current(), "0.40");
                helper::spin_box::value_equals(_tester, selector::from_parent("parameters2Srv", "double_1"), "0.40");
            }

            {
                auto bt = _tester.add_in_backtrace("Check synchronization between 'Double 2' sliders");
                helper::slider::move(
                    _tester,
                    selector::from_parent("parameters1Srv", "double_2"),
                    helper::slider::position::right,
                    5
                );
                helper::label::exactly_match(
                    _tester,
                    selector::from_parent(
                        "parameters1Srv",
                        "double_2/valueLabel"
                    ),
                    "1.00"
                );
                helper::label::exactly_match(
                    _tester,
                    selector::from_parent(
                        "parameters2Srv",
                        "double_2/valueLabel"
                    ),
                    "1.00"
                );
            }

            {
                auto bt = _tester.add_in_backtrace("Check synchronization between '2 Double' spinboxes");
                helper::spin_box::increment(_tester, selector::from_parent("parameters1Srv", "double2_1"), 5);
                helper::spin_box::value_equals(_tester, selector::current(), "55.000");
                helper::spin_box::decrement(_tester, selector::from_parent("parameters1Srv", "double2_1/1"), 5);
                helper::spin_box::value_equals(_tester, selector::current(), "45.000");
                helper::spin_box::value_equals(_tester, selector::from_parent("parameters2Srv", "double2_1"), "55.000");
                helper::spin_box::value_equals(
                    _tester,
                    selector::from_parent(
                        "parameters2Srv",
                        "double2_1/1"
                    ),
                    "45.000"
                );
            }

            {
                auto bt = _tester.add_in_backtrace("Check synchronization between '3 Double' spinboxes");
                helper::spin_box::increment(_tester, selector::from_parent("parameters1Srv", "double3_1"), 4);
                helper::spin_box::value_equals(_tester, selector::current(), "29.000");
                helper::spin_box::decrement(_tester, selector::from_parent("parameters1Srv", "double3_1/1"), 4);
                helper::spin_box::value_equals(_tester, selector::current(), "21.000");
                helper::spin_box::increment(_tester, selector::from_parent("parameters1Srv", "double3_1/2"), 4);
                helper::spin_box::value_equals(_tester, selector::current(), "29.000");
                helper::spin_box::value_equals(_tester, selector::from_parent("parameters2Srv", "double3_1"), "29.000");
                helper::spin_box::value_equals(
                    _tester,
                    selector::from_parent(
                        "parameters2Srv",
                        "double3_1/2"
                    ),
                    "29.000"
                );
                helper::spin_box::value_equals(
                    _tester,
                    selector::from_parent(
                        "parameters2Srv",
                        "double3_1/1"
                    ),
                    "21.000"
                );
            }

            {
                auto bt = _tester.add_in_backtrace("Check synchronization between 'Integer 1' spinboxes");
                helper::spin_box::decrement(_tester, selector::from_parent("parameters1Srv", "int_1"), 5);
                helper::spin_box::value_equals(_tester, selector::current(), 15);
                helper::spin_box::value_equals(_tester, selector::from_parent("parameters2Srv", "int_1"), 15);
            }

            {
                auto bt = _tester.add_in_backtrace("Check synchronization between 'Integer 2' sliders");
                helper::slider::move(
                    _tester,
                    selector::from_parent("parameters1Srv", "int_2"),
                    helper::slider::position::left,
                    2
                );
                helper::label::exactly_match(_tester, selector::from_parent("parameters1Srv", "int_2/valueLabel"), "0");
                helper::label::exactly_match(_tester, selector::from_parent("parameters2Srv", "int_2/valueLabel"), "0");
            }

            {
                auto bt = _tester.add_in_backtrace("Check synchronization between '2 Integer' spinboxes");
                helper::spin_box::increment(_tester, selector::from_parent("parameters1Srv", "int2_1"), 5);
                helper::spin_box::value_equals(_tester, selector::current(), 13);
                helper::spin_box::decrement(_tester, selector::from_parent("parameters1Srv", "int2_1/1"), 5);
                helper::spin_box::value_equals(_tester, selector::current(), 3);
                helper::spin_box::value_equals(_tester, selector::from_parent("parameters2Srv", "int2_1"), 13);
                helper::spin_box::value_equals(_tester, selector::from_parent("parameters2Srv", "int2_1/1"), 3);
            }

            {
                auto bt = _tester.add_in_backtrace("Check synchronization between '3 Integer' spinboxes");
                helper::spin_box::increment(_tester, selector::from_parent("parameters1Srv", "int3_1"), 5);
                helper::spin_box::value_equals(_tester, selector::current(), 17);
                helper::spin_box::decrement(_tester, selector::from_parent("parameters1Srv", "int3_1/1"), 5);
                helper::spin_box::value_equals(_tester, selector::current(), 7);
                helper::spin_box::increment(_tester, selector::from_parent("parameters1Srv", "int3_1/2"), 5);
                helper::spin_box::value_equals(_tester, selector::current(), 17);
                helper::spin_box::value_equals(_tester, selector::from_parent("parameters2Srv", "int3_1"), 17);
                helper::spin_box::value_equals(_tester, selector::from_parent("parameters2Srv", "int3_1/1"), 7);
                helper::spin_box::value_equals(_tester, selector::from_parent("parameters2Srv", "int3_1/2"), 17);
            }

            {
                auto bt = _tester.add_in_backtrace("Check synchronization between 'Enum' comboboxes");
                helper::combo_box::select(_tester, selector::from_parent("parameters1Srv", "enum_1"), "V1");
                helper::combo_box::value_equals(_tester, selector::current(), "V1");
                helper::combo_box::value_equals(_tester, selector::from_parent("parameters2Srv", "enum_1"), "V1");
            }

            {
                auto bt = _tester.add_in_backtrace(
                    "Check synchronization from 'Nonlinear' non-linear slider to combobox"
                );
                helper::slider::move(
                    _tester,
                    selector::from_parent(
                        "parameters1Srv",
                        "nonlin"
                    ),
                    helper::slider::position::right
                );
                helper::label::exactly_match(
                    _tester,
                    selector::from_parent("parameters1Srv", "nonlin/valueLabel"),
                    "120"
                );
                helper::combo_box::value_equals(_tester, selector::from_parent("parameters2Srv", "nonlin"), "120");
            }

            {
                auto bt = _tester.add_in_backtrace(
                    "Check synchronization from 'Nonlinear' combobox to non-linear slider"
                );
                helper::combo_box::select(_tester, selector::from_parent("parameters2Srv", "nonlin"), "50");
                helper::combo_box::value_equals(_tester, selector::current(), "50");
                helper::label::exactly_match(
                    _tester,
                    selector::from_parent(
                        "parameters1Srv",
                        "nonlin/valueLabel"
                    ),
                    "50"
                );
            }

            {
                auto bt = _tester.add_in_backtrace("Check synchronization button bar enum to button bar enum");
                helper::button::push(_tester, selector::from_parent("parameters1Srv", "buttonRaw_button1"));

                selector::from_parent("parameters2Srv", "buttonRaw_button1").select(_tester);
                _tester.doubt<QToolButton*>(
                    "buttonRaw_button1 should be checked",
                    [](QToolButton* _obj)
                {
                    return _obj->isChecked();
                });
            }
        },
        true
    );
}

} // namespace sight::examples::ui::ex_parameters::uit
