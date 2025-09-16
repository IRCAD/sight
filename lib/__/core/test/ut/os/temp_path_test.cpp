/************************************************************************
 *
 * Copyright (C) 2023-2025 IRCAD France
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

#include <core/os/temp_path.hpp>

#include <doctest/doctest.h>

TEST_SUITE("sight::core::os::temp_path")
{
//------------------------------------------------------------------------------

    TEST_CASE("temp_dir")
    {
        // basic nominal test
        {
            // Get the temporary "root" which is deleted at application exit
            const auto& shared = sight::core::os::temp_dir::shared_directory();
            std::filesystem::path path_about_to_be_deleted;

            {
                // Create a temporary directory
                sight::core::os::temp_dir tmp_dir;

                // Check that the directory has been created and is at the right place
                REQUIRE(std::filesystem::exists(tmp_dir));
                CHECK(std::filesystem::is_directory(tmp_dir));
                CHECK(tmp_dir.path().parent_path() == shared);

                path_about_to_be_deleted = tmp_dir.path();
            }

            // Check that the directory has been deleted
            CHECK(!std::filesystem::exists(path_about_to_be_deleted));
        }

        // test shared directory
        std::filesystem::path directory_to_be_kept;
        {
            // Get an unique temporary directory path
            directory_to_be_kept = sight::core::os::temp_dir::shared_directory("unique");

            // Check that the directory has been created and is at the right place
            REQUIRE(std::filesystem::exists(directory_to_be_kept));
            CHECK(std::filesystem::is_directory(directory_to_be_kept));
            CHECK(directory_to_be_kept.parent_path() == sight::core::os::temp_dir::shared_directory());
        }

        // Check that the directory has not been deleted
        CHECK(std::filesystem::exists(directory_to_be_kept));
        std::filesystem::remove_all(directory_to_be_kept);
    }

//------------------------------------------------------------------------------

    TEST_CASE("temp_file")
    {
        // basic nominal test
        {
            // Get the temporary "root" which is deleted at application exit
            const auto& shared = sight::core::os::temp_dir::shared_directory();
            std::filesystem::path directory_about_to_be_deleted;

            {
                // Create a temporary file
                sight::core::os::temp_file tmp_file;

                // Check that the file has been created and is at the right place
                REQUIRE(std::filesystem::exists(tmp_file));
                CHECK(std::filesystem::is_regular_file(tmp_file));
                CHECK(tmp_file.path().parent_path() == shared);

                directory_about_to_be_deleted = tmp_file.path();
            }

            // Check that the file has been deleted
            CHECK(!std::filesystem::exists(directory_about_to_be_deleted));

            // test shared file
            std::filesystem::path file_to_be_kept;

            {
                // Get an unique temporary file path
                const auto& unique_path = sight::core::os::temp_file::unique_path();

                // Check that the file has been created and is at the right place
                REQUIRE(std::filesystem::exists(unique_path));
                CHECK(std::filesystem::is_regular_file(unique_path));
                CHECK(unique_path.parent_path() == shared);

                file_to_be_kept = unique_path;
            }

            // Check that the file has NOT been deleted
            CHECK(std::filesystem::exists(file_to_be_kept));
        }

        // Stream test
        {
            // Get the temporary "root" which is deleted at application exit
            const auto& shared = sight::core::os::temp_dir::shared_directory();
            std::filesystem::path file_about_to_be_deleted;

            {
                // Create a temporary file, but provide an output stream
                sight::core::os::temp_file tmp_file(std::ios_base::out | std::ios_base::trunc);

                // Use the file...
                static const std::string s_CONTENT = "Hello world!";
                tmp_file << s_CONTENT;
                tmp_file.stream().close();

                // Check that the file has been created and is at the right place
                REQUIRE(std::filesystem::exists(tmp_file));
                CHECK(std::filesystem::is_regular_file(tmp_file));
                CHECK(tmp_file.path().parent_path() == shared);

                // Read it back and test the content
                std::ifstream ifs(tmp_file);
                std::stringstream ss;
                ss << ifs.rdbuf();

                CHECK_EQ(ss.str(), s_CONTENT);

                file_about_to_be_deleted = tmp_file.path();
            }

            // Check that the file has been deleted
            CHECK(!std::filesystem::exists(file_about_to_be_deleted));
        }
    }
} // TEST_SUITE
