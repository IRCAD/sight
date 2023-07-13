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

#include "FileHolderTest.hpp"

#include <core/memory/FileHolder.hpp>
#include <core/os/TempPath.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::core::memory::ut::FileHolderTest);

namespace sight::core::memory::ut
{

//------------------------------------------------------------------------------

void FileHolderTest::fileAutoDeleteTest()
{
    core::os::TempFile tempFile;
    {
        sight::core::memory::FileHolder holder(tempFile, true);
        std::ofstream out(tempFile);
        out << 1;
        out.close();

        CPPUNIT_ASSERT(
            std::filesystem::exists(tempFile)
            && std::filesystem::is_regular_file(tempFile)
            && std::filesystem::file_size(tempFile) > 0
        );
    }

    CPPUNIT_ASSERT(!std::filesystem::exists(tempFile));
}

} // namespace sight::core::memory::ut
