/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ImagePositionPatientSorterTest.hpp"

#include <fwDicomIOFilter/factory/new.hpp>
#include <fwDicomIOFilter/helper/Filter.hpp>
#include <fwDicomIOFilter/IFilter.hpp>

#include <fwGdcmIO/reader/SeriesDB.hpp>

#include <fwMath/VectorFunctions.hpp>

#include <fwTest/Data.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/lexical_cast.hpp>

#include <gdcmScanner.h>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwDicomIOFilter::ut::ImagePositionPatientSorterTest );

namespace fwDicomIOFilter
{
namespace ut
{

//------------------------------------------------------------------------------

void ImagePositionPatientSorterTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void ImagePositionPatientSorterTest::tearDown()
{
    // Clean up after the test run.
}

//-----------------------------------------------------------------------------

void ImagePositionPatientSorterTest::simpleApplication()
{
    ::fwMedData::SeriesDB::sptr seriesDB = ::fwMedData::SeriesDB::New();

    const std::string filename           = "01-CT-DICOM_LIVER";
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
    ::fwMedData::DicomSeries::sptr dicomSeries = ::fwMedData::DicomSeries::dynamicCast((*seriesDB)[0]);
    CPPUNIT_ASSERT(dicomSeries);
    std::vector< ::fwMedData::DicomSeries::sptr > dicomSeriesContainer;
    dicomSeriesContainer.push_back(dicomSeries);

    // Apply filter
    ::fwDicomIOFilter::IFilter::sptr filter = ::fwDicomIOFilter::factory::New(
        "::fwDicomIOFilter::sorter::ImagePositionPatientSorter");
    CPPUNIT_ASSERT(filter);
    ::fwDicomIOFilter::helper::Filter::applyFilter(dicomSeriesContainer, filter, true);
    CPPUNIT_ASSERT_EQUAL(size_t(1), dicomSeriesContainer.size());
    dicomSeries = dicomSeriesContainer[0];

    // Create the list of files
    std::vector< std::string > filenames;
    ::boost::filesystem::directory_iterator currentEntry(path);
    ::boost::filesystem::directory_iterator endEntry;
    for(; currentEntry != endEntry; ++currentEntry)
    {
        const ::boost::filesystem::path entryPath = *currentEntry;
        if (::boost::filesystem::is_regular_file(entryPath))
        {
            filenames.push_back(entryPath.string());
        }
    }

    // Read the instance number of each file
    ::gdcm::Scanner scanner;
    scanner.AddTag(::gdcm::Tag(0x0020, 0x0032));    // ImagePositionPatient
    scanner.AddTag(::gdcm::Tag(0x0020, 0x0037));    // ImageOrientationPatient
    CPPUNIT_ASSERT(scanner.Scan(filenames));

    // Loop through instance number
    ::gdcm::Directory::FilenamesType keys = scanner.GetKeys();
    ::gdcm::Directory::FilenamesType::const_iterator itFilename;
    double oldPosition = -1;
    for(itFilename = keys.begin(); itFilename != keys.end(); ++itFilename)
    {
        const std::string filename = itFilename->c_str();

        // Retrieve image position patient
        std::string imagePositionPatientStr = scanner.GetValue(filename.c_str(), ::gdcm::Tag(0x0020, 0x0032));
        ::boost::algorithm::trim(imagePositionPatientStr);
        CPPUNIT_ASSERT(!imagePositionPatientStr.empty());
        std::vector<std::string> imagePositionPatient;
        ::boost::split(imagePositionPatient, imagePositionPatientStr, ::boost::is_any_of("\\"));
        CPPUNIT_ASSERT_EQUAL(size_t(3), imagePositionPatient.size());

        // Retrieve image orientation patient
        std::string imageOrientationPatientStr = scanner.GetValue(filename.c_str(), ::gdcm::Tag(0x0020, 0x0037));
        ::boost::algorithm::trim(imageOrientationPatientStr);
        CPPUNIT_ASSERT(!imagePositionPatientStr.empty());
        std::vector<std::string> imageOrientationPatient;
        ::boost::split(imageOrientationPatient, imageOrientationPatientStr, ::boost::is_any_of("\\"));
        CPPUNIT_ASSERT_EQUAL(size_t(6), imageOrientationPatient.size());

        fwVec3d imagePosition;
        imagePosition[0] = ::boost::lexical_cast<double>(imagePositionPatient[0]);
        imagePosition[1] = ::boost::lexical_cast<double>(imagePositionPatient[1]);
        imagePosition[2] = ::boost::lexical_cast<double>(imagePositionPatient[2]);

        fwVec3d imageOrientationU;
        imageOrientationU[0] = ::boost::lexical_cast<double>(imageOrientationPatient[0]);
        imageOrientationU[1] = ::boost::lexical_cast<double>(imageOrientationPatient[1]);
        imageOrientationU[2] = ::boost::lexical_cast<double>(imageOrientationPatient[2]);

        fwVec3d imageOrientationV;
        imageOrientationV[0] = ::boost::lexical_cast<double>(imageOrientationPatient[3]);
        imageOrientationV[1] = ::boost::lexical_cast<double>(imageOrientationPatient[4]);
        imageOrientationV[2] = ::boost::lexical_cast<double>(imageOrientationPatient[5]);

        //Compute Z direction (cross product)
        fwVec3d zVector = ::fwMath::cross(imageOrientationU, imageOrientationV);

        //Compute dot product to get the position
        double position = ::fwMath::dot(imagePosition, zVector);

        // Check that the position is bigger that the previous one
        std::stringstream ss;
        ss << position << " is not lower than " << oldPosition;
        CPPUNIT_ASSERT_MESSAGE(ss.str(), position > oldPosition);
        oldPosition = position;
    }
}

//------------------------------------------------------------------------------

void ImagePositionPatientSorterTest::applyFilterOnMultipleVolumeImage()
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
    ::fwMedData::DicomSeries::sptr dicomSeries = ::fwMedData::DicomSeries::dynamicCast((*seriesDB)[0]);
    CPPUNIT_ASSERT(dicomSeries);
    std::vector< ::fwMedData::DicomSeries::sptr > dicomSeriesContainer;
    dicomSeriesContainer.push_back(dicomSeries);

    // Apply filter
    ::fwDicomIOFilter::IFilter::sptr filter = ::fwDicomIOFilter::factory::New(
        "::fwDicomIOFilter::sorter::ImagePositionPatientSorter");
    CPPUNIT_ASSERT(filter);
    CPPUNIT_ASSERT_THROW(::fwDicomIOFilter::helper::Filter::applyFilter(dicomSeriesContainer, filter, false),
                         ::fwDicomIOFilter::exceptions::FilterFailure);
}

//------------------------------------------------------------------------------

} // namespace ut
} // namespace fwDicomIOFilter
