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

#include <sight/ui/test/config.hpp>

#include <core/runtime/profile.hpp>

#include <ui/test/scenario.hpp>

#include <filesystem>
#include <functional>
#include <string>

namespace sight::ui::test
{

/**
 * @brief Where failure artifacts and image candidates go.
 *
 * IMAGE_OUTPUT_PATH, else a folder of the system temporary directory that outlives the process, since a failure report
 * names files to open after it.
 *
 * @return the folder of this test executable.
 */
[[nodiscard]] SIGHT_UI_TEST_API std::filesystem::path output_root();

/**
 * @brief The application of one SIGHT_GUI_TEST_CASE: its profile, started at construction, and the run of its scenario.
 *
 * Never includes doctest: the assertions belong to the test executable, which alone defines doctest's runtime.
 */
class SIGHT_UI_TEST_CLASS_API gui_session
{
public:

    /**
     * @brief Starts the profile of the application, with the modules of its folder; the preferences of the user stay
     * out.
     *
     * @param _profile_path the profile.xml of the application.
     */
    SIGHT_UI_TEST_API explicit gui_session(const std::filesystem::path& _profile_path);

    /**
     * @brief Releases the profile, which run() stops.
     */
    SIGHT_UI_TEST_API ~gui_session();

    gui_session(const gui_session&)            = delete;
    gui_session(gui_session&&)                 = delete;
    gui_session& operator=(const gui_session&) = delete;
    gui_session& operator=(gui_session&&)      = delete;

    /**
     * @brief Runs a scenario on a thread of its own once the main window shows, then closes the application.
     *
     * Rethrows what the scenario failed with, on the calling thread.
     *
     * @param _name the name of the scenario, for its output folder.
     * @param _body the scenario.
     * @return why the application did not close, or nothing.
     */
    SIGHT_UI_TEST_API std::string run(const std::string& _name, const std::function<void(scenario&)>& _body);

    /**
     * @brief After a scenario that failed: the step, the failure and its artifacts.
     *
     * @return the report, empty after a success.
     */
    [[nodiscard]] const std::string& failure_report() const noexcept
    {
        return m_failure_report;
    }

private:

    core::runtime::profile::sptr m_profile;
    std::string m_failure_report;
};

} // namespace sight::ui::test
