/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include <fwGdcmIO/helper/DicomDir.hpp>

#include <fwLog/Logger.hpp>

#include <fwMedData/DicomSeries.hpp>

#include <fwTest/Data.hpp>

#include <boost/algorithm/string/replace.hpp>
#include <boost/foreach.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION( ::fwGdcmIO::ut::DicomDirTest );

namespace fwGdcmIO
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
    const ::boost::filesystem::path path = ::fwTest::Data::dir() /
                                           "fw4spl/Patient/Dicom/DicomDB/82-MR-SAGITTAL-KNEE-DICOMDIR";
    const std::string pathStr = ::boost::algorithm::replace_all_copy(path.string(), "\\", "/");

    CPPUNIT_ASSERT_MESSAGE("The dicom directory '" + path.string() + "' does not exist",
                           ::boost::filesystem::exists(path));

    std::vector< ::fwMedData::DicomSeries::sptr > seriesDB;

    ::fwLog::Logger::sptr logger = ::fwLog::Logger::New();

    // Read DICOMDIR file
    ::fwGdcmIO::helper::DicomDir::retrieveDicomSeries(path / "DICOMDIR", seriesDB, logger);
    CPPUNIT_ASSERT_EQUAL( size_t( 1 ), seriesDB.size());
    CPPUNIT_ASSERT( logger->empty() );

    auto series = *seriesDB.begin();
    CPPUNIT_ASSERT_EQUAL( size_t( 84 ), series->getDicomContainer().size());
}

//------------------------------------------------------------------------------

} // namespace ut

} // namespace fwGdcmIO
