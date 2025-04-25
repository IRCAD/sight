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

#include "temp_path_test.hpp"

#include <core/os/temp_path.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::core::tools::ut::temp_path_test);

namespace sight::core::tools::ut
{

//------------------------------------------------------------------------------

void temp_path_test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void temp_path_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void temp_path_test::temp_dir_test()
{
    // basic nominal test
    {
        // Get the temporary "root" which is deleted at application exit
        const auto& shared = core::os::temp_dir::shared_directory();
        std::filesystem::path path_about_to_be_deleted;

        {
            // Create a temporary directory
            core::os::temp_dir tmp_dir;

            // Check that the directory has been created and is at the right place
            CPPUNIT_ASSERT(
                std::filesystem::exists(tmp_dir)
                && std::filesystem::is_directory(tmp_dir)
                && tmp_dir.path().parent_path() == shared
            );

            path_about_to_be_deleted = tmp_dir.path();
        }

        // Check that the directory has been deleted
        CPPUNIT_ASSERT(!std::filesystem::exists(path_about_to_be_deleted));
    }

    // test shared directory
    std::filesystem::path directory_to_be_kept;
    {
        // Get an unique temporary directory path
        directory_to_be_kept = core::os::temp_dir::shared_directory("unique");

        // Check that the directory has been created and is at the right place
        CPPUNIT_ASSERT(
            std::filesystem::exists(directory_to_be_kept)
            && std::filesystem::is_directory(directory_to_be_kept)
            && directory_to_be_kept.parent_path() == core::os::temp_dir::shared_directory()
        );
    }

    // Check that the directory has not been deleted
    CPPUNIT_ASSERT(std::filesystem::exists(directory_to_be_kept));
    std::filesystem::remove_all(directory_to_be_kept);
}

//------------------------------------------------------------------------------

void temp_path_test::temp_file_test()
{
    // basic nominal test
    {
        // Get the temporary "root" which is deleted at application exit
        const auto& shared = core::os::temp_dir::shared_directory();
        std::filesystem::path directory_about_to_be_deleted;

        {
            // Create a temporary file
            core::os::temp_file tmp_file;

            // Check that the file has been created and is at the right place
            CPPUNIT_ASSERT(
                std::filesystem::exists(tmp_file)
                && std::filesystem::is_regular_file(tmp_file)
                && tmp_file.path().parent_path() == shared
            );

            directory_about_to_be_deleted = tmp_file.path();
        }

        // Check that the file has been deleted
        CPPUNIT_ASSERT(!std::filesystem::exists(directory_about_to_be_deleted));

        // test shared file
        std::filesystem::path file_to_be_kept;

        {
            // Get an unique temporary file path
            const auto& unique_path = core::os::temp_file::unique_path();

            // Check that the file has been created and is at the right place
            CPPUNIT_ASSERT(
                std::filesystem::exists(unique_path)
                && std::filesystem::is_regular_file(unique_path)
                && std::filesystem::is_empty(unique_path)
                && unique_path.parent_path() == shared
            );

            file_to_be_kept = unique_path;
        }

        // Check that the file has NOT been deleted
        CPPUNIT_ASSERT(std::filesystem::exists(file_to_be_kept));
    }

    // Stream test
    {
        // Get the temporary "root" which is deleted at application exit
        const auto& shared = core::os::temp_dir::shared_directory();
        std::filesystem::path file_about_to_be_deleted;
        std::filesystem::path file_to_be_kept;

        {
            // Create a temporary directory, but provide an output stream
            core::os::temp_file tmp_file(std::ios_base::out | std::ios_base::trunc);

            // Use the file...
            static const std::string s_CONTENT = "Hello world!";
            tmp_file << s_CONTENT;
            tmp_file.stream().close();

            // Check that the directory has been created and is at the right place
            CPPUNIT_ASSERT(
                std::filesystem::exists(tmp_file)
                && std::filesystem::is_regular_file(tmp_file)
                && tmp_file.path().parent_path() == shared
            );

            // Read it back and test the content
            std::ifstream ifs(tmp_file);
            std::stringstream ss;
            ss << ifs.rdbuf();

            CPPUNIT_ASSERT_EQUAL(s_CONTENT, ss.str());

            file_about_to_be_deleted = tmp_file.path();
        }

        // Check that the file has been deleted
        CPPUNIT_ASSERT(!std::filesystem::exists(file_about_to_be_deleted));
    }
}

} // namespace sight::core::tools::ut
