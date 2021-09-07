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

#include "ImagePositionPatientSorterTest.hpp"

#include <filter/dicom/factory/new.hpp>
#include <filter/dicom/helper/Filter.hpp>
#include <filter/dicom/IFilter.hpp>

#include <geometry/data/VectorFunctions.hpp>

#include <io/dicom/reader/SeriesDB.hpp>

#include <utestData/Data.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/lexical_cast.hpp>

#include <gdcmImageReader.h>

#include <filesystem>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(::sight::filter::dicom::ut::ImagePositionPatientSorterTest);

namespace sight::filter::dicom
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

//------------------------------------------------------------------------------

double getInstanceZPosition(const core::memory::BufferObject::sptr& bufferObj)
{
    ::gdcm::ImageReader reader;
    const core::memory::BufferManager::StreamInfo streamInfo = bufferObj->getStreamInfo();
    reader.SetStream(*streamInfo.stream);

    if(!reader.Read())
    {
        return 0;
    }

    // Retrieve dataset
    const ::gdcm::DataSet& dataset = reader.GetFile().GetDataSet();

    // Check tags availability
    if(!dataset.FindDataElement(::gdcm::Tag(0x0020, 0x0032)) || !dataset.FindDataElement(::gdcm::Tag(0x0020, 0x0037)))
    {
        const std::string msg = "Unable to compute the spacing between slices of the series.";
        throw io::dicom::exception::Failed(msg);
    }

    // Retrieve image position
    const ::gdcm::Image& gdcmImage = reader.GetImage();
    const double* gdcmOrigin       = gdcmImage.GetOrigin();
    const fwVec3d imagePosition    = {{gdcmOrigin[0], gdcmOrigin[1], gdcmOrigin[2]}};

    // Retrieve image orientation
    const double* directionCosines  = gdcmImage.GetDirectionCosines();
    const fwVec3d imageOrientationU = {{
        std::round(directionCosines[0]),
        std::round(directionCosines[1]),
        std::round(directionCosines[2])
    }
    };
    const fwVec3d imageOrientationV = {{
        std::round(directionCosines[3]),
        std::round(directionCosines[4]),
        std::round(directionCosines[5])
    }
    };

    //Compute Z direction (cross product)
    const fwVec3d zVector = geometry::data::cross(imageOrientationU, imageOrientationV);

    //Compute dot product to get the index
    const double index = geometry::data::dot(imagePosition, zVector);

    return index;
}

//-----------------------------------------------------------------------------

void ImagePositionPatientSorterTest::simpleApplication()
{
    data::SeriesDB::sptr seriesDB = data::SeriesDB::New();

    const std::string filename       = "01-CT-DICOM_LIVER";
    const std::filesystem::path path = utestData::Data::dir() / "sight/Patient/Dicom/DicomDB" / filename;

    CPPUNIT_ASSERT_MESSAGE(
        "The dicom directory '" + path.string() + "' does not exist",
        std::filesystem::exists(path)
    );

    // Read DicomSeries
    io::dicom::reader::SeriesDB::sptr reader = io::dicom::reader::SeriesDB::New();
    reader->setObject(seriesDB);
    reader->setFolder(path);
    CPPUNIT_ASSERT_NO_THROW(reader->readDicomSeries());
    CPPUNIT_ASSERT_EQUAL(size_t(1), seriesDB->size());

    // Retrieve DicomSeries
    data::DicomSeries::sptr dicomSeries = data::DicomSeries::dynamicCast((*seriesDB)[0]);
    CPPUNIT_ASSERT(dicomSeries);
    std::vector<data::DicomSeries::sptr> dicomSeriesContainer;
    dicomSeriesContainer.push_back(dicomSeries);

    // Apply filter
    filter::dicom::IFilter::sptr filter = filter::dicom::factory::New(
        "sight::filter::dicom::sorter::ImagePositionPatientSorter"
    );
    CPPUNIT_ASSERT(filter);
    CPPUNIT_ASSERT_NO_THROW(filter::dicom::helper::Filter::applyFilter(dicomSeriesContainer, filter, true));

    CPPUNIT_ASSERT_EQUAL(size_t(1), dicomSeriesContainer.size());
    dicomSeries = dicomSeriesContainer[0];

    double oldPosition = -1.0;

    const data::DicomSeries::DicomContainerType& dicomContainer = dicomSeries->getDicomContainer();
    for(size_t index = dicomSeries->getFirstInstanceNumber() ; index < dicomSeries->getNumberOfInstances() ; ++index)
    {
        const double position = getInstanceZPosition(dicomContainer.at(index));

        // Check that the position is bigger that the previous one
        CPPUNIT_ASSERT_LESS(position, oldPosition);
        oldPosition = position;
    }
}

//------------------------------------------------------------------------------

void ImagePositionPatientSorterTest::applyFilterOnMultipleVolumeImage()
{
    data::SeriesDB::sptr seriesDB = data::SeriesDB::New();

    const std::string filename       = "08-CT-PACS";
    const std::filesystem::path path = utestData::Data::dir() / "sight/Patient/Dicom/DicomDB" / filename;

    CPPUNIT_ASSERT_MESSAGE(
        "The dicom directory '" + path.string() + "' does not exist",
        std::filesystem::exists(path)
    );

    // Read DicomSeries
    io::dicom::reader::SeriesDB::sptr reader = io::dicom::reader::SeriesDB::New();
    reader->setObject(seriesDB);
    reader->setFolder(path);
    CPPUNIT_ASSERT_NO_THROW(reader->readDicomSeries());
    CPPUNIT_ASSERT_EQUAL(size_t(1), seriesDB->size());

    // Retrieve DicomSeries
    data::DicomSeries::sptr dicomSeries = data::DicomSeries::dynamicCast((*seriesDB)[0]);
    CPPUNIT_ASSERT(dicomSeries);
    std::vector<data::DicomSeries::sptr> dicomSeriesContainer;
    dicomSeriesContainer.push_back(dicomSeries);

    // Apply filter
    filter::dicom::IFilter::sptr filter = filter::dicom::factory::New(
        "sight::filter::dicom::sorter::ImagePositionPatientSorter"
    );
    CPPUNIT_ASSERT(filter);
    CPPUNIT_ASSERT_THROW(
        filter::dicom::helper::Filter::applyFilter(dicomSeriesContainer, filter, false),
        filter::dicom::exceptions::FilterFailure
    );
}

//------------------------------------------------------------------------------

} // namespace ut

} // namespace sight::filter::dicom
