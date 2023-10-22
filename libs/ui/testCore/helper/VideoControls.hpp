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

#pragma once

#include <ui/testCore/Tester.hpp>

namespace sight::ui::test_core::helper
{

class VideoControls
{
public:

    /**
     * Load a video file using the video controls interface.
     * @param tester The current tester
     * @param controlsName The objectName of the video controls interface
     * @param path The path of the video file to be loaded
     */
    GUITEST_API static void load(
        Tester& _tester,
        const std::string& _controls_name,
        const std::filesystem::path& _path
    );

    /**
     * Start the video being played using the video controls interface.
     * @param tester The current tester
     * @param controlsName The objectName of the video controls interface
     */
    GUITEST_API static void start(Tester& _tester, const std::string& _controls_name);

    /**
     * Pause the video being played using the video controls interface.
     * @param tester The current tester
     * @param controlsName The objectName of the video controls interface
     */
    GUITEST_API static void pause(Tester& _tester, const std::string& _controls_name);

    /**
     * Play the video being played using the video controls interface.
     * @param tester The current tester
     * @param controlsName The objectName of the video controls interface
     */
    GUITEST_API static void play(Tester& _tester, const std::string& _controls_name);

    /**
     * Stop the video being played using the video controls interface.
     * @param tester The current tester
     * @param controlsName The objectName of the video controls interface
     */
    GUITEST_API static void stop(Tester& _tester, const std::string& _controls_name);

    /**
     * Loop the video being played using the video controls interface.
     * @param tester The current tester
     * @param controlsName The objectName of the video controls interface
     */
    GUITEST_API static void loop(Tester& _tester, const std::string& _controls_name);
};

}
