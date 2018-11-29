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

#include "NoFilterCustomTest.hpp"

#include <fwDicomIOFilter/factory/new.hpp>
#include <fwDicomIOFilter/helper/Filter.hpp>
#include <fwDicomIOFilter/IFilter.hpp>

#include <fwGdcmIO/reader/SeriesDB.hpp>

#include <fwTest/Data.hpp>

#include <boost/filesystem/path.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwDicomIOFilter::ut::NoFilterCustomTest );

namespace fwDicomIOFilter
{
namespace ut
{

//------------------------------------------------------------------------------

void NoFilterCustomTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void NoFilterCustomTest::tearDown()
{
    // Clean up after the test run.
}

//-----------------------------------------------------------------------------

void NoFilterCustomTest::simpleApplication()
{
    ::fwMedData::SeriesDB::sptr seriesDB = ::fwMedData::SeriesDB::New();

    const std::string filename           = "71-CT-DICOM_SEG";
    const ::boost::filesystem::path path = ::fwTest::Data::dir() / "sight/Patient/Dicom/DicomDB" / filename;

    CPPUNIT_ASSERT_MESSAGE("The dicom directory '" + path.string() + "' does not exist",
                           ::boost::filesystem::exists(path));

    // Read DicomSeries
    ::fwGdcmIO::reader::SeriesDB::sptr reader = ::fwGdcmIO::reader::SeriesDB::New();
    reader->setObject(seriesDB);
    reader->setFolder(path);
    CPPUNIT_ASSERT_NO_THROW(reader->readDicomSeries());
    CPPUNIT_ASSERT_EQUAL(size_t(1), seriesDB->size());

    // Retrieve DicomSeries
    ::fwMedData::DicomSeries::sptr dicomSeriesA = ::fwMedData::DicomSeries::dynamicCast((*seriesDB)[0]);
    ::fwMedData::DicomSeries::sptr dicomSeriesB = ::fwMedData::DicomSeries::New();
    dicomSeriesB->deepCopy(dicomSeriesA);
    CPPUNIT_ASSERT(dicomSeriesA);
    CPPUNIT_ASSERT(dicomSeriesB);
    std::vector< ::fwMedData::DicomSeries::sptr > dicomSeriesContainerA;
    std::vector< ::fwMedData::DicomSeries::sptr > dicomSeriesContainerB;
    dicomSeriesContainerA.push_back(dicomSeriesA);
    dicomSeriesContainerB.push_back(dicomSeriesB);

    // Apply filter
    ::fwDicomIOFilter::IFilter::sptr filter = ::fwDicomIOFilter::factory::New("::fwDicomIOFilter::custom::NoFilter");
    CPPUNIT_ASSERT(filter);
    ::fwDicomIOFilter::helper::Filter::applyFilter(dicomSeriesContainerA, filter, true);
    CPPUNIT_ASSERT_EQUAL(size_t(1), dicomSeriesContainerA.size());
    dicomSeriesA = dicomSeriesContainerA[0];

    // Check Number of SOP Class UID
    CPPUNIT_ASSERT_EQUAL(size_t(2), dicomSeriesA->getSOPClassUIDs().size());

    // Compare the two series
    CPPUNIT_ASSERT_EQUAL(dicomSeriesContainerA.size(), dicomSeriesContainerB.size());
    for(unsigned int j = 0; j < dicomSeriesContainerA.size(); ++j)  // For every series
    {
        for(unsigned int i = 0; i < dicomSeriesContainerA[j]->getDicomContainer().size(); ++i) // For every instances
        {
            CPPUNIT_ASSERT_EQUAL(dicomSeriesContainerA[j]->getDicomContainer().at(i),
                                 dicomSeriesContainerB[j]->getDicomContainer().at(i));
        }
    }

}

//------------------------------------------------------------------------------

} // namespace ut
} // namespace fwDicomIOFilter
