/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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

#include <ui/test/gui_session.hpp>
#include <ui/test/locator.hpp> // NOLINT(misc-include-cleaner): a scenario names items through this header alone.
#include <ui/test/scenario.hpp>

#include <boost/dll/runtime_symbol_info.hpp>

#include <doctest/doctest.h>

#include <filesystem>
#include <string>

namespace sight::ui::test::detail
{

/**
 * @brief The gui_session of a SIGHT_GUI_TEST_CASE, which fails the doctest case when its scenario fails.
 */
class gui_test_fixture final : public gui_session
{
public:

    /**
     * @brief Starts the application of a profile.
     *
     * @param _profile the profile, relative to the folder above the test executable.
     */
    explicit gui_test_fixture(const std::filesystem::path& _profile) :
        gui_session(
            std::filesystem::path(boost::dll::program_location().string()).parent_path().parent_path() / _profile
        )
    {
    }

    //------------------------------------------------------------------------------

    /**
     * @brief Runs the scenario, then checks the names of what it shows; a failure, a name outside the naming
     * contract or an application that did not close fails the test case.
     *
     * @param _name the name of the scenario.
     * @param _body the scenario.
     */
    void run(const std::string& _name, void (*_body)(scenario&))
    {
        // Read when the scenario has failed, thrown or not.
        INFO(failure_report());

        // Every scenario ends by checking the names of what it shows: a widget named outside the contract fails it.
        const auto body_then_names = [_body](scenario& _ui)
                                     {
                                         _body(_ui);
                                         _ui.expect_valid_names();
                                     };
        const std::string failure_message = gui_session::run(_name, body_then_names);
        INFO(failure_message);
        REQUIRE(failure_message.empty());
    }
};

//------------------------------------------------------------------------------

/// A doctest decorator: the suite of a scenario is its application, unless the scenario is in a TEST_SUITE.
struct application_suite
{
    const char* m_name;

    //------------------------------------------------------------------------------

    void fill(doctest::detail::TestCase& _state) const noexcept
    {
        if(_state.m_test_suite == nullptr || *_state.m_test_suite == '\0')
        {
            _state.m_test_suite = m_name;
        }
    }
};

} // namespace sight::ui::test::detail

// A test declaration needs a macro, as doctest's TEST_CASE does.
// NOLINTBEGIN(cppcoreguidelines-macro-usage)
#define SIGHT_GUI_TEST_DETAIL_CONCAT_IMPL(_left, _right) _left ## _right
#define SIGHT_GUI_TEST_DETAIL_CONCAT(_left, _right) SIGHT_GUI_TEST_DETAIL_CONCAT_IMPL(_left, _right)

#define SIGHT_GUI_TEST_DETAIL(_name, _id) \
        static void SIGHT_GUI_TEST_DETAIL_CONCAT(sight_gui_test_body_, _id)(::sight::ui::test::scenario & ui); \
        TEST_CASE((_name) * ::sight::ui::test::detail::application_suite {SIGHT_GUI_TEST_PROFILE_NAME}) \
        { \
            ::sight::ui::test::detail::gui_test_fixture(SIGHT_GUI_TEST_PROFILE).run( \
                _name, \
                &SIGHT_GUI_TEST_DETAIL_CONCAT(sight_gui_test_body_, _id) \
            ); \
        } \
        static void SIGHT_GUI_TEST_DETAIL_CONCAT( \
            sight_gui_test_body_, \
            _id \
        )([[maybe_unused]] ::sight::ui::test::scenario & ui)

/**
 * @brief Declares a GUI scenario: a doctest case that starts the application of its target and drives it with `ui`, a
 * scenario.
 *
 * Its suite is the name of the application, or the enclosing TEST_SUITE when there is one, which keeps the hierarchy of
 * doctest: in CTest, `sight_viewer::add_distance`, or `sight_viewer::fiducials::add_distance` below.
 *
 * The target is declared with sight_gui_test() in CMake, which gives the profile to start: anywhere else,
 * SIGHT_GUI_TEST_PROFILE is undeclared. Each scenario runs in a process of its own, and its name is the doctest case
 * name, in snake_case. At its end, a scenario checks the names of the widgets it shows (ui.expect_valid_names()). A
 * failure writes failure.png, ui_tree.txt and steps.txt, and names them.
 *
 * @code{.cpp}
 * #include <ui/test/gui_test.hpp>
 *
 * SIGHT_GUI_TEST_CASE("video_controls")
 * {
 *     ui.click("videoToolbarView/Start");
 *     ui.expect("videoToolbarView/Start").hidden();
 *     ui.expect("videoSliderSrv/total_duration").text("00:00:05");
 * }
 *
 * TEST_SUITE("sight_viewer::fiducials")
 * {
 *     SIGHT_GUI_TEST_CASE("add_distance")
 *     {
 *         ...
 *     }
 * }
 * @endcode
 *
 * @param _name the name of the scenario, in snake_case.
 */
#define SIGHT_GUI_TEST_CASE(_name) SIGHT_GUI_TEST_DETAIL(_name, __COUNTER__)
// NOLINTEND(cppcoreguidelines-macro-usage)
