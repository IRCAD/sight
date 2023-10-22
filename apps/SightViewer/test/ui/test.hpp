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

#include <ui/testCore/test.hpp>

namespace sight::sightviewer::test::ui
{

class test : public sight::ui::test_core::test
{
public:

    std::filesystem::path getProfilePath() override;

    static void openFile(
        sight::ui::test_core::Tester& _tester,
        const std::string& _format,
        const std::filesystem::path& _path
    );
    static void saveSnapshot(sight::ui::test_core::Tester& _tester, const std::filesystem::path& _path);
    static void resetNegatos(sight::ui::test_core::Tester& _tester);
};

} // namespace sight::sightviewer::test::ui
