/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "ReadCsvTest.hpp"

#include "io/__/reader/csv_reader.hpp"

#include <core/exception.hpp>

#include <boost/dll/runtime_symbol_info.hpp>
#include <boost/property_tree/ptree.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::ut::read_csv_test);

namespace sight::io::ut
{

//------------------------------------------------------------------------------

void read_csv_test::setUp()
{
    // Set up context before running a test.
    const auto current_path           = boost::dll::program_location().parent_path().parent_path();
    const auto valid_crlf_file        = current_path / "share/sight/io/valid_crlf_file.csv";
    const auto valid_lf_file          = current_path / "share/sight/io/valid_lf_file.csv";
    const auto invalid_csv_file       = current_path / "share/sight/io/invalid_file.csv";
    const auto invalid_file_directory = current_path / "share/sight/io/invalid_file_path.csv";

    // Open an existing file
    m_valid_crlf_csv_directory_path = valid_crlf_file.string();
    m_valid_lf_csv_directory_path   = valid_lf_file.string();
    m_invalid_csv_directory_path    = invalid_csv_file.string();
    m_wrong_filepath_directory      = invalid_file_directory.string();
}

//------------------------------------------------------------------------------

void read_csv_test::tearDown()
{
}

//------------------------------------------------------------------------------

void read_csv_test::test_inexistence_csv()
{
    CPPUNIT_ASSERT_NO_THROW(io::reader::csv_reader csv_reader(m_valid_crlf_csv_directory_path));
    CPPUNIT_ASSERT_NO_THROW(io::reader::csv_reader csv_reader(m_valid_lf_csv_directory_path));
    CPPUNIT_ASSERT_NO_THROW(io::reader::csv_reader csv_reader(m_invalid_csv_directory_path));
    CPPUNIT_ASSERT_THROW(io::reader::csv_reader csv_reader(m_wrong_filepath_directory), core::exception);
}

//------------------------------------------------------------------------------

void read_csv_test::test_valid_crlf_csv()
{
    // cspell: disable
    io::reader::csv_reader csv_reader(m_valid_crlf_csv_directory_path);
    io::reader::csv_reader::token_container_t token_vec;

    token_vec = csv_reader.get_line();
    CPPUNIT_ASSERT("1" == token_vec[0]);
    CPPUNIT_ASSERT("Eldon Base" == token_vec[1]);
    CPPUNIT_ASSERT("platinum" == token_vec[2]);
    CPPUNIT_ASSERT("Muhammed" == token_vec[3]);
    CPPUNIT_ASSERT("3" == token_vec[4]);
    CPPUNIT_ASSERT("-213.25" == token_vec[5]);
    CPPUNIT_ASSERT("38.94" == token_vec[6]);
    CPPUNIT_ASSERT("35" == token_vec[7]);
    CPPUNIT_ASSERT("Name" == token_vec[8]);
    CPPUNIT_ASSERT("Storage" == token_vec[9]);
    CPPUNIT_ASSERT("0.8" == token_vec[10]);
    CPPUNIT_ASSERT(token_vec[11].empty());
    CPPUNIT_ASSERT(12 == token_vec.size());

    token_vec = csv_reader.get_line();
    CPPUNIT_ASSERT("2" == token_vec[0]);
    CPPUNIT_ASSERT("Eldon Base for stackable storage" == token_vec[1]);
    CPPUNIT_ASSERT("plati" == token_vec[2]);
    CPPUNIT_ASSERT("Muhammed and MacIntyre" == token_vec[3]);
    CPPUNIT_ASSERT("12" == token_vec[4]);
    CPPUNIT_ASSERT("-213.25" == token_vec[5]);
    CPPUNIT_ASSERT("32.94" == token_vec[6]);
    CPPUNIT_ASSERT("35" == token_vec[7]);
    CPPUNIT_ASSERT("Organization" == token_vec[8]);
    CPPUNIT_ASSERT("0.89" == token_vec[9]);
    CPPUNIT_ASSERT(token_vec[10].empty());
    CPPUNIT_ASSERT(11 == token_vec.size());

    token_vec = csv_reader.get_line();
    CPPUNIT_ASSERT("3" == token_vec[0]);
    CPPUNIT_ASSERT("Eldon" == token_vec[1]);
    CPPUNIT_ASSERT("plati" == token_vec[2]);
    CPPUNIT_ASSERT("MacIntyre" == token_vec[3]);
    CPPUNIT_ASSERT("9" == token_vec[4]);
    CPPUNIT_ASSERT("0.56" == token_vec[5]);
    CPPUNIT_ASSERT(token_vec[6].empty());
    CPPUNIT_ASSERT(7 == token_vec.size());

    //cspell: enable
}

//------------------------------------------------------------------------------

void read_csv_test::test_valid_lf_csv()
{
    // cspell: disable
    io::reader::csv_reader csv_reader(m_valid_lf_csv_directory_path);
    io::reader::csv_reader::token_container_t token_vec;

    token_vec = csv_reader.get_line();
    CPPUNIT_ASSERT("1" == token_vec[0]);
    CPPUNIT_ASSERT("Eldon Base" == token_vec[1]);
    CPPUNIT_ASSERT("platinum" == token_vec[2]);
    CPPUNIT_ASSERT("Muhammed" == token_vec[3]);
    CPPUNIT_ASSERT("3" == token_vec[4]);
    CPPUNIT_ASSERT("-213.25" == token_vec[5]);
    CPPUNIT_ASSERT("38.94" == token_vec[6]);
    CPPUNIT_ASSERT("35" == token_vec[7]);
    CPPUNIT_ASSERT("Name" == token_vec[8]);
    CPPUNIT_ASSERT("Storage" == token_vec[9]);
    CPPUNIT_ASSERT("0.8" == token_vec[10]);
    CPPUNIT_ASSERT(token_vec[11].empty());
    CPPUNIT_ASSERT(12 == token_vec.size());

    token_vec = csv_reader.get_line();
    CPPUNIT_ASSERT("2" == token_vec[0]);
    CPPUNIT_ASSERT("Eldon Base for stackable storage" == token_vec[1]);
    CPPUNIT_ASSERT("plati" == token_vec[2]);
    CPPUNIT_ASSERT("Muhammed and MacIntyre" == token_vec[3]);
    CPPUNIT_ASSERT("12" == token_vec[4]);
    CPPUNIT_ASSERT("-213.25" == token_vec[5]);
    CPPUNIT_ASSERT("32.94" == token_vec[6]);
    CPPUNIT_ASSERT("35" == token_vec[7]);
    CPPUNIT_ASSERT("Organization" == token_vec[8]);
    CPPUNIT_ASSERT("0.89" == token_vec[9]);
    CPPUNIT_ASSERT(token_vec[10].empty());
    CPPUNIT_ASSERT(11 == token_vec.size());

    token_vec = csv_reader.get_line();
    CPPUNIT_ASSERT("3" == token_vec[0]);
    CPPUNIT_ASSERT("Eldon" == token_vec[1]);
    CPPUNIT_ASSERT("plati" == token_vec[2]);
    CPPUNIT_ASSERT("MacIntyre" == token_vec[3]);
    CPPUNIT_ASSERT("9" == token_vec[4]);
    CPPUNIT_ASSERT("0.56" == token_vec[5]);
    CPPUNIT_ASSERT(token_vec[6].empty());
    CPPUNIT_ASSERT(7 == token_vec.size());

    // cspell: enable
}

//------------------------------------------------------------------------------

void read_csv_test::test_invalid_csv()
{
    io::reader::csv_reader csv_reader(m_invalid_csv_directory_path);
    io::reader::csv_reader::token_container_t tag_vec = csv_reader.get_line();

    CPPUNIT_ASSERT(std::filesystem::exists(m_invalid_csv_directory_path));
    const std::size_t size = tag_vec.size();
    CPPUNIT_ASSERT(size == 0);
}

} // namespace sight::io::ut
