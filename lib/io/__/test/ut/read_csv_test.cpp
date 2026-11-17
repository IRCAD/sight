/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "io/__/reader/csv_reader.hpp"

#include <core/exception.hpp>

#include <boost/dll/runtime_symbol_info.hpp>
#include <boost/property_tree/ptree.hpp>

#include <doctest/doctest.h>

#include <filesystem>

TEST_SUITE("sight::io::read_csv")
{
    TEST_CASE("inexistence_csv")
    {
        const auto current_path           = boost::dll::program_location().parent_path().parent_path();
        const auto valid_crlf_file        = current_path / "share/sight/io/valid_crlf_file.csv";
        const auto valid_lf_file          = current_path / "share/sight/io/valid_lf_file.csv";
        const auto invalid_csv_file       = current_path / "share/sight/io/invalid_file.csv";
        const auto invalid_file_directory = current_path / "share/sight/io/invalid_file_path.csv";

        const auto& valid_crlf_csv_directory_path = valid_crlf_file.string();
        const auto& valid_lf_csv_directory_path   = valid_lf_file.string();
        const auto& invalid_csv_directory_path    = invalid_csv_file.string();
        const auto& wrong_filepath_directory      = invalid_file_directory.string();

        CHECK_NOTHROW(sight::io::reader::csv_reader csv_reader(valid_crlf_csv_directory_path));
        CHECK_NOTHROW(sight::io::reader::csv_reader csv_reader(valid_lf_csv_directory_path));
        CHECK_NOTHROW(sight::io::reader::csv_reader csv_reader(invalid_csv_directory_path));
        CHECK_THROWS_AS(sight::io::reader::csv_reader csv_reader(wrong_filepath_directory), sight::core::exception);
    }

    TEST_CASE("valid_crlf_csv")
    {
        const auto current_path                   = boost::dll::program_location().parent_path().parent_path();
        const auto valid_crlf_file                = current_path / "share/sight/io/valid_crlf_file.csv";
        const auto& valid_crlf_csv_directory_path = valid_crlf_file.string();

        // cspell: disable
        sight::io::reader::csv_reader csv_reader(valid_crlf_csv_directory_path);
        sight::io::reader::csv_reader::token_container_t token_vec;

        token_vec = csv_reader.get_line();
        CHECK_EQ("1", token_vec[0]);
        CHECK_EQ("Eldon Base", token_vec[1]);
        CHECK_EQ("platinum", token_vec[2]);
        CHECK_EQ("Muhammed", token_vec[3]);
        CHECK_EQ("3", token_vec[4]);
        CHECK_EQ("-213.25", token_vec[5]);
        CHECK_EQ("38.94", token_vec[6]);
        CHECK_EQ("35", token_vec[7]);
        CHECK_EQ("Name", token_vec[8]);
        CHECK_EQ("Storage", token_vec[9]);
        CHECK_EQ("0.8", token_vec[10]);
        CHECK(token_vec[11].empty());
        CHECK_EQ(12U, token_vec.size());

        token_vec = csv_reader.get_line();
        CHECK_EQ("2", token_vec[0]);
        CHECK_EQ("Eldon Base for stackable storage", token_vec[1]);
        CHECK_EQ("plati", token_vec[2]);
        CHECK_EQ("Muhammed and MacIntyre", token_vec[3]);
        CHECK_EQ("12", token_vec[4]);
        CHECK_EQ("-213.25", token_vec[5]);
        CHECK_EQ("32.94", token_vec[6]);
        CHECK_EQ("35", token_vec[7]);
        CHECK_EQ("Organization", token_vec[8]);
        CHECK_EQ("0.89", token_vec[9]);
        CHECK(token_vec[10].empty());
        CHECK_EQ(11U, token_vec.size());

        token_vec = csv_reader.get_line();
        CHECK_EQ("3", token_vec[0]);
        CHECK_EQ("Eldon", token_vec[1]);
        CHECK_EQ("plati", token_vec[2]);
        CHECK_EQ("MacIntyre", token_vec[3]);
        CHECK_EQ("9", token_vec[4]);
        CHECK_EQ("0.56", token_vec[5]);
        CHECK(token_vec[6].empty());
        CHECK_EQ(7U, token_vec.size());

        //cspell: enable
    }

    TEST_CASE("valid_lf_csv")
    {
        const auto current_path                 = boost::dll::program_location().parent_path().parent_path();
        const auto valid_lf_file                = current_path / "share/sight/io/valid_lf_file.csv";
        const auto& valid_lf_csv_directory_path = valid_lf_file.string();

        // cspell: disable
        sight::io::reader::csv_reader csv_reader(valid_lf_csv_directory_path);
        sight::io::reader::csv_reader::token_container_t token_vec;

        token_vec = csv_reader.get_line();
        CHECK_EQ("1", token_vec[0]);
        CHECK_EQ("Eldon Base", token_vec[1]);
        CHECK_EQ("platinum", token_vec[2]);
        CHECK_EQ("Muhammed", token_vec[3]);
        CHECK_EQ("3", token_vec[4]);
        CHECK_EQ("-213.25", token_vec[5]);
        CHECK_EQ("38.94", token_vec[6]);
        CHECK_EQ("35", token_vec[7]);
        CHECK_EQ("Name", token_vec[8]);
        CHECK_EQ("Storage", token_vec[9]);
        CHECK_EQ("0.8", token_vec[10]);
        CHECK(token_vec[11].empty());
        CHECK_EQ(12U, token_vec.size());

        token_vec = csv_reader.get_line();
        CHECK_EQ("2", token_vec[0]);
        CHECK_EQ("Eldon Base for stackable storage", token_vec[1]);
        CHECK_EQ("plati", token_vec[2]);
        CHECK_EQ("Muhammed and MacIntyre", token_vec[3]);
        CHECK_EQ("12", token_vec[4]);
        CHECK_EQ("-213.25", token_vec[5]);
        CHECK_EQ("32.94", token_vec[6]);
        CHECK_EQ("35", token_vec[7]);
        CHECK_EQ("Organization", token_vec[8]);
        CHECK_EQ("0.89", token_vec[9]);
        CHECK(token_vec[10].empty());
        CHECK_EQ(11U, token_vec.size());

        token_vec = csv_reader.get_line();
        CHECK_EQ("3", token_vec[0]);
        CHECK_EQ("Eldon", token_vec[1]);
        CHECK_EQ("plati", token_vec[2]);
        CHECK_EQ("MacIntyre", token_vec[3]);
        CHECK_EQ("9", token_vec[4]);
        CHECK_EQ("0.56", token_vec[5]);
        CHECK(token_vec[6].empty());
        CHECK_EQ(7U, token_vec.size());

        // cspell: enable
    }

    TEST_CASE("invalid_csv")
    {
        const auto current_path                = boost::dll::program_location().parent_path().parent_path();
        const auto invalid_csv_file            = current_path / "share/sight/io/invalid_file.csv";
        const auto& invalid_csv_directory_path = invalid_csv_file.string();

        sight::io::reader::csv_reader csv_reader(invalid_csv_directory_path);
        auto tag_vec = csv_reader.get_line();

        CHECK(std::filesystem::exists(invalid_csv_directory_path));
        const std::size_t size = tag_vec.size();
        CHECK_EQ(0U, size);
    }
}
