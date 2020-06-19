/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
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

#include "DicomDirTest.hpp"

#include <fwDcmtkIO/helper/DicomDir.hpp>

#include <fwTest/Data.hpp>

#include <boost/algorithm/string/replace.hpp>
#include <boost/foreach.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION( ::fwDcmtkIO::ut::DicomDirTest );

namespace fwDcmtkIO
{
namespace ut
{

//------------------------------------------------------------------------------

void DicomDirTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void DicomDirTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void DicomDirTest::readDicomDir()
{
    const std::filesystem::path path = ::fwTest::Data::dir() /
                                       "sight/Patient/Dicom/DicomDB/82-MR-SAGITTAL-KNEE-DICOMDIR";
    const std::string pathStr = ::boost::algorithm::replace_all_copy(path.string(), "\\", "/");

    CPPUNIT_ASSERT_MESSAGE("The dicom directory '" + path.string() + "' does not exist",
                           std::filesystem::exists(path));

    std::vector<std::string> filenames;

    // Read DICOMDIR file
    CPPUNIT_ASSERT(::fwDcmtkIO::helper::DicomDir::readDicomDir(path, filenames));
    CPPUNIT_ASSERT_EQUAL( size_t( 84 ), filenames.size());

    // Verify filenames
    unsigned int i = 0;
    for(const std::string& filename: filenames)
    {
        std::string newFilename = ::boost::algorithm::replace_all_copy(filename, "\\", "/");

        std::stringstream ss;
        ss << pathStr << "/DICOM/PT0/ST0/SE0/IM" << i++;
        CPPUNIT_ASSERT_EQUAL(ss.str(), newFilename);
    }
}

//------------------------------------------------------------------------------

} // namespace ut

} // namespace fwDcmtkIO
