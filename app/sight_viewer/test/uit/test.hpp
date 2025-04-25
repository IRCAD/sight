/************************************************************************
 *
 * Copyright (C) 2022-2023 IRCAD France
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

#include <ui/test/test.hpp>

namespace sight::sight_viewer::uit
{

class test : public sight::ui::test::base
{
public:

    std::filesystem::path get_profile_path() override;

    static void open_file(
        sight::ui::test::tester& _tester,
        const std::string& _format,
        const std::filesystem::path& _path
    );
    static void save_snapshot(sight::ui::test::tester& _tester, const std::filesystem::path& _path);
    static void reset_negatos(sight::ui::test::tester& _tester);
};

} // namespace sight::sight_viewer::uit
