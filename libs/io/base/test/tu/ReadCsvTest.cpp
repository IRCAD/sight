/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "io/base/reader/CsvReader.hpp"

#include <core/Exception.hpp>

#include <boost/dll/runtime_symbol_info.hpp>
#include <boost/property_tree/ptree.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::base::ut::ReadCsvTest);

namespace sight::io::base
{

namespace ut
{

//------------------------------------------------------------------------------

void ReadCsvTest::setUp()
{
    // Set up context before running a test.
    const auto currentPath          = boost::dll::program_location().parent_path().parent_path();
    const auto validCRLF_file       = currentPath / "share/sight/io_base/valid_crlf_file.csv";
    const auto validLF_file         = currentPath / "share/sight/io_base/valid_lf_file.csv";
    const auto invalidCsv_file      = currentPath / "share/sight/io_base/invalid_file.csv";
    const auto invalidFileDirectory = currentPath / "share/sight/io_base/invalid_file_path.csv";

    // Open an existing file
    m_validCRLFCsvDirectoryPath = validCRLF_file.string();
    m_validLFCsvDirectoryPath   = validLF_file.string();
    m_invalidCsvDirectoryPath   = invalidCsv_file.string();
    m_wrongFilepathDirectory    = invalidFileDirectory.string();
}

//------------------------------------------------------------------------------

void ReadCsvTest::tearDown()
{
}

//------------------------------------------------------------------------------

void ReadCsvTest::test_inexistenceCsv()
{
    CPPUNIT_ASSERT_NO_THROW(io::base::reader::CsvReader csvReader(m_validCRLFCsvDirectoryPath));
    CPPUNIT_ASSERT_NO_THROW(io::base::reader::CsvReader csvReader(m_validLFCsvDirectoryPath));
    CPPUNIT_ASSERT_NO_THROW(io::base::reader::CsvReader csvReader(m_invalidCsvDirectoryPath));
    CPPUNIT_ASSERT_THROW(io::base::reader::CsvReader csvReader(m_wrongFilepathDirectory), core::Exception);
}

//------------------------------------------------------------------------------

void ReadCsvTest::test_validCRLFCsv()
{
    io::base::reader::CsvReader csvReader(m_validCRLFCsvDirectoryPath);
    io::base::reader::CsvReader::TokenContainerType tokenVec;

    tokenVec = csvReader.getLine();
    CPPUNIT_ASSERT("1" == tokenVec[0]);
    CPPUNIT_ASSERT("Eldon Base" == tokenVec[1]);
    CPPUNIT_ASSERT("platinum" == tokenVec[2]);
    CPPUNIT_ASSERT("Muhammed" == tokenVec[3]);
    CPPUNIT_ASSERT("3" == tokenVec[4]);
    CPPUNIT_ASSERT("-213.25" == tokenVec[5]);
    CPPUNIT_ASSERT("38.94" == tokenVec[6]);
    CPPUNIT_ASSERT("35" == tokenVec[7]);
    CPPUNIT_ASSERT("Name" == tokenVec[8]);
    CPPUNIT_ASSERT("Storage" == tokenVec[9]);
    CPPUNIT_ASSERT("0.8" == tokenVec[10]);
    CPPUNIT_ASSERT(tokenVec[11].empty());
    CPPUNIT_ASSERT(12 == tokenVec.size());

    tokenVec = csvReader.getLine();
    CPPUNIT_ASSERT("2" == tokenVec[0]);
    CPPUNIT_ASSERT("Eldon Base for stackable storage" == tokenVec[1]);
    CPPUNIT_ASSERT("plati" == tokenVec[2]);
    CPPUNIT_ASSERT("Muhammed and MacIntyre" == tokenVec[3]);
    CPPUNIT_ASSERT("12" == tokenVec[4]);
    CPPUNIT_ASSERT("-213.25" == tokenVec[5]);
    CPPUNIT_ASSERT("32.94" == tokenVec[6]);
    CPPUNIT_ASSERT("35" == tokenVec[7]);
    CPPUNIT_ASSERT("Organization" == tokenVec[8]);
    CPPUNIT_ASSERT("0.89" == tokenVec[9]);
    CPPUNIT_ASSERT(tokenVec[10].empty());
    CPPUNIT_ASSERT(11 == tokenVec.size());

    tokenVec = csvReader.getLine();
    CPPUNIT_ASSERT("3" == tokenVec[0]);
    CPPUNIT_ASSERT("Eldon" == tokenVec[1]);
    CPPUNIT_ASSERT("plati" == tokenVec[2]);
    CPPUNIT_ASSERT("MacIntyre" == tokenVec[3]);
    CPPUNIT_ASSERT("9" == tokenVec[4]);
    CPPUNIT_ASSERT("0.56" == tokenVec[5]);
    CPPUNIT_ASSERT(tokenVec[6].empty());
    CPPUNIT_ASSERT(7 == tokenVec.size());
}

//------------------------------------------------------------------------------

void ReadCsvTest::test_validLFCsv()
{
    io::base::reader::CsvReader csvReader(m_validLFCsvDirectoryPath);
    io::base::reader::CsvReader::TokenContainerType tokenVec;

    tokenVec = csvReader.getLine();
    CPPUNIT_ASSERT("1" == tokenVec[0]);
    CPPUNIT_ASSERT("Eldon Base" == tokenVec[1]);
    CPPUNIT_ASSERT("platinum" == tokenVec[2]);
    CPPUNIT_ASSERT("Muhammed" == tokenVec[3]);
    CPPUNIT_ASSERT("3" == tokenVec[4]);
    CPPUNIT_ASSERT("-213.25" == tokenVec[5]);
    CPPUNIT_ASSERT("38.94" == tokenVec[6]);
    CPPUNIT_ASSERT("35" == tokenVec[7]);
    CPPUNIT_ASSERT("Name" == tokenVec[8]);
    CPPUNIT_ASSERT("Storage" == tokenVec[9]);
    CPPUNIT_ASSERT("0.8" == tokenVec[10]);
    CPPUNIT_ASSERT(tokenVec[11].empty());
    CPPUNIT_ASSERT(12 == tokenVec.size());

    tokenVec = csvReader.getLine();
    CPPUNIT_ASSERT("2" == tokenVec[0]);
    CPPUNIT_ASSERT("Eldon Base for stackable storage" == tokenVec[1]);
    CPPUNIT_ASSERT("plati" == tokenVec[2]);
    CPPUNIT_ASSERT("Muhammed and MacIntyre" == tokenVec[3]);
    CPPUNIT_ASSERT("12" == tokenVec[4]);
    CPPUNIT_ASSERT("-213.25" == tokenVec[5]);
    CPPUNIT_ASSERT("32.94" == tokenVec[6]);
    CPPUNIT_ASSERT("35" == tokenVec[7]);
    CPPUNIT_ASSERT("Organization" == tokenVec[8]);
    CPPUNIT_ASSERT("0.89" == tokenVec[9]);
    CPPUNIT_ASSERT(tokenVec[10].empty());
    CPPUNIT_ASSERT(11 == tokenVec.size());

    tokenVec = csvReader.getLine();
    CPPUNIT_ASSERT("3" == tokenVec[0]);
    CPPUNIT_ASSERT("Eldon" == tokenVec[1]);
    CPPUNIT_ASSERT("plati" == tokenVec[2]);
    CPPUNIT_ASSERT("MacIntyre" == tokenVec[3]);
    CPPUNIT_ASSERT("9" == tokenVec[4]);
    CPPUNIT_ASSERT("0.56" == tokenVec[5]);
    CPPUNIT_ASSERT(tokenVec[6].empty());
    CPPUNIT_ASSERT(7 == tokenVec.size());
}

//------------------------------------------------------------------------------

void ReadCsvTest::test_invalidCsv()
{
    io::base::reader::CsvReader csvReader(m_invalidCsvDirectoryPath);
    io::base::reader::CsvReader::TokenContainerType tagVec = csvReader.getLine();

    CPPUNIT_ASSERT(std::filesystem::exists(m_invalidCsvDirectoryPath));
    const std::size_t size = tagVec.size();
    CPPUNIT_ASSERT(size == 0);
}

} //namespace ut

} //namespace sight::io::base
