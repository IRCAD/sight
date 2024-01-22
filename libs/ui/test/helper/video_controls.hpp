/************************************************************************
 *
 * Copyright (C) 2023-2024 IRCAD France
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

#include <ui/test/tester.hpp>

namespace sight::ui::test::helper
{

class video_controls
{
public:

    /**
     * Load a video file using the video controls interface.
     * @param _tester The current tester
     * @param _controls_name The objectName of the video controls interface
     * @param _path The path of the video file to be loaded
     */
    UI_TEST_API static void load(
        tester& _tester,
        const std::string& _controls_name,
        const std::filesystem::path& _path
    );

    /**
     * Start the video being played using the video controls interface.
     * @param _tester The current tester
     * @param _controls_name The objectName of the video controls interface
     */
    UI_TEST_API static void start(tester& _tester, const std::string& _controls_name);

    /**
     * Pause the video being played using the video controls interface.
     * @param _tester The current tester
     * @param _controls_name The objectName of the video controls interface
     */
    UI_TEST_API static void pause(tester& _tester, const std::string& _controls_name);

    /**
     * Play the video being played using the video controls interface.
     * @param _tester The current tester
     * @param _controls_name The objectName of the video controls interface
     */
    UI_TEST_API static void play(tester& _tester, const std::string& _controls_name);

    /**
     * Stop the video being played using the video controls interface.
     * @param _tester The current tester
     * @param _controls_name The objectName of the video controls interface
     */
    UI_TEST_API static void stop(tester& _tester, const std::string& _controls_name);

    /**
     * Loop the video being played using the video controls interface.
     * @param _tester The current tester
     * @param _controls_name The objectName of the video controls interface
     */
    UI_TEST_API static void loop(tester& _tester, const std::string& _controls_name);
};

}
