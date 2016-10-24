/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "CTImageStorageDefaultCompositeTest.hpp"

#include <fwDicomIOFilter/IFilter.hpp>
#include <fwDicomIOFilter/factory/new.hpp>
#include <fwDicomIOFilter/helper/Filter.hpp>

#include <fwGdcmIO/reader/SeriesDB.hpp>

#include <fwTest/Data.hpp>

#include <boost/filesystem/path.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwDicomIOFilter::ut::CTImageStorageDefaultCompositeTest );

namespace fwDicomIOFilter
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
    ::fwMedData::SeriesDB::sptr seriesDB = ::fwMedData::SeriesDB::New();

    const std::string filename           = "08-CT-PACS";
    const ::boost::filesystem::path path = ::fwTest::Data::dir() / "fw4spl/Patient/Dicom/DicomDB" / filename;

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

    // Apply composite filter on the first DicomSeries
    ::fwDicomIOFilter::IFilter::sptr filter = ::fwDicomIOFilter::factory::New(
        "::fwDicomIOFilter::composite::CTImageStorageDefaultComposite");
    CPPUNIT_ASSERT(filter);
    ::fwDicomIOFilter::helper::Filter::applyFilter(dicomSeriesContainerA, filter, true);

    // Apply other filters on the second DicomSeries
    filter = ::fwDicomIOFilter::factory::New("::fwDicomIOFilter::splitter::AcquisitionNumberSplitter");
    CPPUNIT_ASSERT(filter);
    ::fwDicomIOFilter::helper::Filter::applyFilter(dicomSeriesContainerB, filter, true);
    filter = ::fwDicomIOFilter::factory::New("::fwDicomIOFilter::sorter::InstanceNumberSorter");
    CPPUNIT_ASSERT(filter);
    ::fwDicomIOFilter::helper::Filter::applyFilter(dicomSeriesContainerB, filter, true);
    filter = ::fwDicomIOFilter::factory::New("::fwDicomIOFilter::sorter::ImagePositionPatientSorter");
    CPPUNIT_ASSERT(filter);
    ::fwDicomIOFilter::helper::Filter::applyFilter(dicomSeriesContainerB, filter, true);
    filter = ::fwDicomIOFilter::factory::New("::fwDicomIOFilter::splitter::ImagePositionPatientSplitter");
    CPPUNIT_ASSERT(filter);
    ::fwDicomIOFilter::helper::Filter::applyFilter(dicomSeriesContainerB, filter, true);
    filter = ::fwDicomIOFilter::factory::New("::fwDicomIOFilter::sorter::ImagePositionPatientSorter");
    CPPUNIT_ASSERT(filter);
    ::fwDicomIOFilter::helper::Filter::applyFilter(dicomSeriesContainerB, filter, true);
    filter = ::fwDicomIOFilter::factory::New("::fwDicomIOFilter::modifier::SliceThicknessModifier");
    CPPUNIT_ASSERT(filter);
    ::fwDicomIOFilter::helper::Filter::applyFilter(dicomSeriesContainerB, filter, true);

    // Compare the two series
    CPPUNIT_ASSERT_EQUAL(dicomSeriesContainerA.size(), dicomSeriesContainerB.size());
    for(unsigned int j = 0; j < dicomSeriesContainerA.size(); ++j)  // For every series
    {
        for(unsigned int i = 0; i < dicomSeriesContainerA[j]->getLocalDicomPaths().size(); ++i) // For every instances
        {
            CPPUNIT_ASSERT_EQUAL(dicomSeriesContainerA[j]->getLocalDicomPaths().at(i),
                                 dicomSeriesContainerB[j]->getLocalDicomPaths().at(i));
        }
    }

}

//------------------------------------------------------------------------------

} // namespace ut
} // namespace fwDicomIOFilter
