/************************************************************************
 *
 * Copyright (C) 2022 IRCAD France
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

#include "FileHolderTest.hpp"

#include <core/memory/FileHolder.hpp>
#include <core/tools/System.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::core::memory::ut::FileHolderTest);

namespace sight::core::memory::ut
{

//------------------------------------------------------------------------------

void FileHolderTest::fileAutoDeleteTest()
{
    const std::filesystem::path path = std::filesystem::temp_directory_path()
                                       / sight::core::tools::System::genTempFileName();
    {
        sight::core::memory::FileHolder holder(path, true);
        std::ofstream out(path);
        out << 1;
        CPPUNIT_ASSERT(std::filesystem::exists(path));
    }
    CPPUNIT_ASSERT(!std::filesystem::exists(path));
}

} // namespace sight::core::memory::ut
