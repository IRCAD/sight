/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "CTImageStorageDefaultCompositeTest.hpp"

#include <filter/dicom/factory/new.hpp>
#include <filter/dicom/helper/Filter.hpp>
#include <filter/dicom/IFilter.hpp>

#include <io/dicom/reader/SeriesDB.hpp>

#include <utestData/Data.hpp>

#include <filesystem>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::sight::filter::dicom::ut::CTImageStorageDefaultCompositeTest );

namespace sight::filter::dicom
{
namespace ut
{

//------------------------------------------------------------------------------

void CTImageStorageDefaultCompositeTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void CTImageStorageDefaultCompositeTest::tearDown()
{
    // Clean up after the test run.
}

//-----------------------------------------------------------------------------

void CTImageStorageDefaultCompositeTest::simpleApplication()
{
    data::SeriesDB::sptr seriesDB = data::SeriesDB::New();

    const std::string filename       = "08-CT-PACS";
    const std::filesystem::path path = utestData::Data::dir() / "sight/Patient/Dicom/DicomDB" / filename;

    CPPUNIT_ASSERT_MESSAGE("The dicom directory '" + path.string() + "' does not exist",
                           std::filesystem::exists(path));

    // Read DicomSeries
    io::dicom::reader::SeriesDB::sptr reader = io::dicom::reader::SeriesDB::New();
    reader->setObject(seriesDB);
    reader->setFolder(path);
    CPPUNIT_ASSERT_NO_THROW(reader->readDicomSeries());
    CPPUNIT_ASSERT_EQUAL(size_t(1), seriesDB->size());

    // Retrieve DicomSeries
    data::DicomSeries::sptr dicomSeriesA = data::DicomSeries::dynamicCast((*seriesDB)[0]);
    data::DicomSeries::sptr dicomSeriesB = data::DicomSeries::New();
    dicomSeriesB->deepCopy(dicomSeriesA);
    CPPUNIT_ASSERT(dicomSeriesA);
    CPPUNIT_ASSERT(dicomSeriesB);
    std::vector< data::DicomSeries::sptr > dicomSeriesContainerA;
    std::vector< data::DicomSeries::sptr > dicomSeriesContainerB;
    dicomSeriesContainerA.push_back(dicomSeriesA);
    dicomSeriesContainerB.push_back(dicomSeriesB);

    // Apply composite filter on the first DicomSeries
    filter::dicom::IFilter::sptr filter = filter::dicom::factory::New(
        "sight::filter::dicom::composite::CTImageStorageDefaultComposite");
    CPPUNIT_ASSERT(filter);
    filter::dicom::helper::Filter::applyFilter(dicomSeriesContainerA, filter, true);

    // Apply other filters on the second DicomSeries
    filter = filter::dicom::factory::New("sight::filter::dicom::splitter::AcquisitionNumberSplitter");
    CPPUNIT_ASSERT(filter);
    filter::dicom::helper::Filter::applyFilter(dicomSeriesContainerB, filter, true);
    filter = filter::dicom::factory::New("sight::filter::dicom::sorter::InstanceNumberSorter");
    CPPUNIT_ASSERT(filter);
    filter::dicom::helper::Filter::applyFilter(dicomSeriesContainerB, filter, true);
    filter = filter::dicom::factory::New("sight::filter::dicom::sorter::ImagePositionPatientSorter");
    CPPUNIT_ASSERT(filter);
    filter::dicom::helper::Filter::applyFilter(dicomSeriesContainerB, filter, true);
    filter = filter::dicom::factory::New("sight::filter::dicom::splitter::ImagePositionPatientSplitter");
    CPPUNIT_ASSERT(filter);
    filter::dicom::helper::Filter::applyFilter(dicomSeriesContainerB, filter, true);
    filter = filter::dicom::factory::New("sight::filter::dicom::sorter::ImagePositionPatientSorter");
    CPPUNIT_ASSERT(filter);
    filter::dicom::helper::Filter::applyFilter(dicomSeriesContainerB, filter, true);
    filter = filter::dicom::factory::New("sight::filter::dicom::modifier::SliceThicknessModifier");
    CPPUNIT_ASSERT(filter);
    filter::dicom::helper::Filter::applyFilter(dicomSeriesContainerB, filter, true);

    // Compare the two series
    CPPUNIT_ASSERT_EQUAL(dicomSeriesContainerA.size(), dicomSeriesContainerB.size());
    for(unsigned int j = 0; j < dicomSeriesContainerA.size(); ++j)  // For every series
    {
        for(unsigned int i = 0; i < dicomSeriesContainerA[j]->getDicomContainer().size(); ++i) // For every instances
        {
            CPPUNIT_ASSERT(std::memcmp(dicomSeriesContainerA[j]->getDicomContainer().at(i)->getBuffer(),
                                       dicomSeriesContainerB[j]->getDicomContainer().at(i)->getBuffer(),
                                       dicomSeriesContainerA[j]->getDicomContainer().at(i)->getSize()) == 0 );
        }
    }

}

//------------------------------------------------------------------------------

} // namespace ut
} // namespace sight::filter::dicom
