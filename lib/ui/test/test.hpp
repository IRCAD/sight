/************************************************************************
 *
 * Copyright (C) 2021-2026 IRCAD France
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

#include <sight/ui/test/config.hpp>

#include "tester.hpp"

#include <core/runtime/profile.hpp>

#include <QEvent>
#include <QObject>

namespace sight::ui::test
{

/**
 * @brief Base class for GUI (`uit`) test fixtures.
 * @details Loads and starts the application profile whose path is given at construction. Doctest
 * constructs the derived fixture once per test case, so this constructor/destructor pair plays
 * the role a setUp()/tearDown() pair would.
 *
 * @warning The scenario passed to @ref start runs on a secondary thread while the Qt event loop
 * runs on the main thread (see @ref tester::start). Never call a throwing doctest assertion
 * (REQUIRE, FAIL, ...) from that thread or from any helper it calls: it would call std::terminate
 * instead of failing the test. Report failures from the scenario through @ref tester::fail
 * instead.
 *
 * @note This header, and everything compiled into the ui_test library, must never include
 * `<doctest/doctest.h>`. Merely including that header - even without using any of its macros -
 * unconditionally emits a reference to a doctest runtime symbol that is only defined in the one
 * translation unit of each test executable that sets DOCTEST_CONFIG_IMPLEMENT (its generated
 * doctest_main.cpp). ui_test is a shared library linked by several independently-built `_uit`
 * executables, none of which export that symbol to their shared libraries; including doctest.h
 * here would make every one of them fail to link. This is why @ref start returns the failure
 * message as a plain std::string instead of asserting itself: the doctest assertion
 * (`INFO(...); REQUIRE(...);`) is the caller's job, in the TEST_CASE_FIXTURE body, which is
 * compiled directly into the test executable and never into this library.
 */
class SIGHT_UI_TEST_CLASS_API base
{
public:

    /**
     * @brief Loads the modules of the given profile, then starts it and initializes the tester.
     * @param _profile_path absolute path to the application's profile.xml.
     */
    SIGHT_UI_TEST_API explicit base(const std::filesystem::path& _profile_path);

    SIGHT_UI_TEST_API virtual ~base();

    base(const base&)            = delete;
    base(base&&)                 = delete;
    base& operator=(const base&) = delete;
    base& operator=(base&&)      = delete;

protected:

    /**
     * @brief Runs the scenario against the application profile.
     * @param _test_name a name used in log messages and in the failure screenshot's file name.
     * @param _test the scenario to run.
     * @param _verbose_mode logs each step of the tester to the console when true.
     * @return the failure message if the scenario failed, or an empty string if it passed.
     *
     * @par Usage
     * The caller (a TEST_CASE_FIXTURE body) must assert on the result itself, right after calling
     * this method:
     * @code
     * const std::string failure_message = start("my_scenario", [](tester& _tester) { ... });
     * INFO(failure_message);
     * REQUIRE(failure_message.empty());
     * @endcode
     */
    SIGHT_UI_TEST_API std::string start(
        const std::string& _test_name,
        std::function<void(tester&)> _test,
        bool _verbose_mode = false
    );

    /**
     * @brief Fails the calling scenario (via @ref tester::fail) if the two images differ.
     * @warning Must be called from the scenario thread, never from the main thread: it reports
     * through tester::fail(), not through a doctest assertion.
     */
    SIGHT_UI_TEST_API static void compare_images(const std::filesystem::path& _a, const std::filesystem::path& _b);

private:

    sight::core::runtime::profile::sptr m_profile;
};

} // namespace sight::ui::test
