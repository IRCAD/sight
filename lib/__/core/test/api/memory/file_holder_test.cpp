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

#include "file_holder_test.hpp"

#include <core/memory/file_holder.hpp>
#include <core/os/temp_path.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::core::memory::ut::file_holder_test);

namespace sight::core::memory::ut
{

//------------------------------------------------------------------------------

void file_holder_test::file_auto_delete_test()
{
    core::os::temp_file temp_file;
    {
        sight::core::memory::file_holder holder(temp_file, true);
        std::ofstream out(temp_file);
        out << 1;
        out.close();

        CPPUNIT_ASSERT(
            std::filesystem::exists(temp_file)
            && std::filesystem::is_regular_file(temp_file)
            && std::filesystem::file_size(temp_file) > 0
        );
    }

    CPPUNIT_ASSERT(!std::filesystem::exists(temp_file));
}

} // namespace sight::core::memory::ut
