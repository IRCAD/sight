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

#include "gui_test_support.hpp"

#include <ui/test/detail/deadline.hpp>
#include <ui/test/scenario.hpp>

#include <doctest/doctest.h>

#include <QLabel>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

#include <chrono>
#include <stdexcept>
#include <thread>
#include <tuple>

// cspell:ignore qputenv qunsetenv

namespace sight::ui::test::detail
{

namespace
{

/// The scale of one test, and none after it.
struct timeout_scale_guard
{
    explicit timeout_scale_guard(const char* _scale)
    {
        qputenv("SIGHT_UI_TEST_TIMEOUT_SCALE", _scale);
    }

    timeout_scale_guard(const timeout_scale_guard&)            = delete;
    timeout_scale_guard(timeout_scale_guard&&)                 = delete;
    timeout_scale_guard& operator=(const timeout_scale_guard&) = delete;
    timeout_scale_guard& operator=(timeout_scale_guard&&)      = delete;

    ~timeout_scale_guard()
    {
        qunsetenv("SIGHT_UI_TEST_TIMEOUT_SCALE");
    }
};

} // namespace

TEST_SUITE("sight::ui::test::timeout_scale")
{
    TEST_CASE("is_one_when_unset")
    {
        qunsetenv("SIGHT_UI_TEST_TIMEOUT_SCALE");
        CHECK(timeout_scale() == doctest::Approx(1.));
        CHECK(scaled(std::chrono::seconds(30)) == std::chrono::seconds(30));
    }

    TEST_CASE("multiplies_the_durations")
    {
        const timeout_scale_guard scale("2.5");
        CHECK(scaled(std::chrono::seconds(2)) == std::chrono::seconds(5));
    }

    TEST_CASE("refuses_a_value_that_is_not_a_positive_number")
    {
        for(const char* const text : {"fast", "0", "-1", "inf", "nan", "2,5"})
        {
            CAPTURE(text);
            const timeout_scale_guard scale(text);
            CHECK_THROWS_WITH_AS(
                std::ignore = deadline_after(std::chrono::seconds(1)),
                doctest::Contains(text),
                std::invalid_argument
            );
        }
    }

    TEST_CASE("keeps_a_huge_scale_within_the_clock")
    {
        // The scaled duration exceeds what the clock counts: the latest deadline it can tell, never a wrapped one.
        const timeout_scale_guard scale("1e300");
        const auto now = std::chrono::steady_clock::now();
        CHECK(scaled(std::chrono::seconds(30)) == std::chrono::steady_clock::duration::max());
        CHECK(deadline_after(std::chrono::seconds(30)) == std::chrono::steady_clock::time_point::max());
        CHECK(deadline_after(std::chrono::hours(1)) > now);
    }

    TEST_CASE("never_scales_a_positive_duration_to_zero")
    {
        const timeout_scale_guard scale("1e-30");
        CHECK(scaled(std::chrono::seconds(1)) == std::chrono::steady_clock::duration(1));
        const auto now = std::chrono::steady_clock::now();
        CHECK(deadline_after(std::chrono::seconds(1)) > now);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "shortens_a_deadline")
    {
        const timeout_scale_guard scale("0.001");
        scenario ui(application);
        run_scenario(
            *this,
            [&]
            {
                // Two minutes, unless scaled: a guard against a loaded machine, not a measure.
                const auto start = std::chrono::steady_clock::now();
                CHECK_THROWS_AS(ui.click("missing", {}, std::chrono::minutes(2)), std::runtime_error);
                CHECK(std::chrono::steady_clock::now() - start < std::chrono::minutes(1));
            });
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "lengthens_a_deadline")
    {
        const timeout_scale_guard scale("6000");
        QLabel label("late");
        label.setObjectName("late");
        // The event loop only starts below, so the timer cannot fire before the 5 ms have passed.
        QTimer::singleShot(std::chrono::milliseconds(300), &label, [&label]{label.show();});
        scenario ui(application);
        run_scenario(
            *this,
            [&]
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                ui.expect("late").within(std::chrono::milliseconds(5)).text("late");
            });
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "lengthens_the_grace_of_an_ambiguous_locator")
    {
        const timeout_scale_guard scale("60");
        QWidget window;
        QVBoxLayout layout(&window);
        QLabel first("same");
        QLabel second("same");
        first.setObjectName("same");
        second.setObjectName("same");
        layout.addWidget(&first);
        layout.addWidget(&second);
        window.show();
        // Later than the second of grace of an unscaled run.
        QTimer::singleShot(std::chrono::milliseconds(2500), &second, [&second]{second.hide();});
        scenario ui(application);
        run_scenario(*this, [&]{ui.expect("same").text("same");});
    }
}

} // namespace sight::ui::test::detail
