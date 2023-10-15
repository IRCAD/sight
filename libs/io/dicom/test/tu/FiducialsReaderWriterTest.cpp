/************************************************************************
 *
 * Copyright (C) 2023 IRCAD France
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

#include "FiducialsReaderWriterTest.hpp"

#include <data/image_series.hpp>

#include <io/dicom/Reader.hpp>
#include <io/dicom/Writer.hpp>

#include <utest_data/Data.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::dicom::ut::FiducialsReaderWriterTest);

namespace sight::io::dicom::ut
{

//------------------------------------------------------------------------------

void FiducialsReaderWriterTest::basicTest()
{
    auto original = std::make_shared<data::series_set>();
    auto reader   = std::make_shared<io::dicom::Reader>();
    reader->set_object(original);
    reader->set_folder(utest_data::Data::dir() / "us/Enhanced US Volume Storage/GE, 3D+t, lossy JPEG");
    CPPUNIT_ASSERT_NO_THROW(reader->read());
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), original->size());

    auto originalImageSeries = std::dynamic_pointer_cast<data::image_series>(original->at(0));
    CPPUNIT_ASSERT(originalImageSeries);
    CPPUNIT_ASSERT(!originalImageSeries->hasFiducials());
    data::fiducials_series::sptr originalFiducialsSeries = originalImageSeries->getFiducials();

    originalFiducialsSeries->setContentDate("1");
    originalFiducialsSeries->setContentTime("2");
    originalFiducialsSeries->setInstanceNumber(3);
    originalFiducialsSeries->setContentLabel("4");
    originalFiducialsSeries->setContentDescription("5");
    originalFiducialsSeries->setContentCreatorName("6");

    data::fiducials_series::FiducialSet fiducialSet;

    data::fiducials_series::ReferencedImage referencedImage;
    referencedImage.referencedSOPClassUID    = data::dicom::sop::get(originalImageSeries->getSOPKeyword()).m_uid;
    referencedImage.referencedSOPInstanceUID = originalImageSeries->getSOPInstanceUID();
    referencedImage.referencedFrameNumber    = {7};
    referencedImage.referencedSegmentNumber  = {8};
    fiducialSet.referencedImageSequence      = {referencedImage};

    fiducialSet.frameOfReferenceUID = "9";

    data::fiducials_series::Fiducial fiducial;
    fiducial.shapeType           = data::fiducials_series::Shape::POINT;
    fiducial.fiducialDescription = "10";
    fiducial.fiducialIdentifier  = "11";

    data::fiducials_series::GraphicCoordinatesData graphicCoordinatesData;
    graphicCoordinatesData.referencedImageSequence.referencedSOPClassUID = data::dicom::sop::get(
        originalImageSeries->getSOPKeyword()
    ).m_uid;
    graphicCoordinatesData.referencedImageSequence.referencedSOPInstanceUID = originalImageSeries->getSOPInstanceUID();
    graphicCoordinatesData.referencedImageSequence.referencedFrameNumber    = {12};
    graphicCoordinatesData.referencedImageSequence.referencedSegmentNumber  = {13};
    graphicCoordinatesData.graphicData                                      = {{14, 15}};
    fiducial.graphicCoordinatesDataSequence                                 = {graphicCoordinatesData};

    fiducial.fiducialUID = "16";
    fiducial.contourData = {{17, 18, 19}};
    fiducialSet.fiducialSequence.push_back(fiducial);

    fiducialSet.groupName = "20";

    originalFiducialsSeries->setSOPInstanceUID(
        std::string(
            data::dicom::sop::get(
                data::dicom::sop::Keyword::
                SpatialFiducialsStorage
            ).m_uid
        ) + ".0"
    );
    originalFiducialsSeries->appendFiducialSet(fiducialSet);

    auto writer = std::make_shared<io::dicom::Writer>();
    writer->set_object(original);
    std::filesystem::create_directories("/tmp/FiducialsReaderWriterTest");
    writer->set_folder("/tmp/FiducialsReaderWriterTest");
    CPPUNIT_ASSERT_NO_THROW(writer->write());

    auto actual = std::make_shared<data::series_set>();
    reader->set_object(actual);
    reader->set_folder("/tmp/FiducialsReaderWriterTest");
    CPPUNIT_ASSERT_NO_THROW(reader->read());
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), actual->size());

    auto actualImageSeries = std::dynamic_pointer_cast<data::image_series>(actual->at(0));
    CPPUNIT_ASSERT(actualImageSeries);
    auto actualFiducialsSeries = actualImageSeries->getFiducials();
    CPPUNIT_ASSERT(actualFiducialsSeries);
    std::vector<data::fiducials_series::FiducialSet> actualFiducialSets = actualFiducialsSeries->getFiducialSets();
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), actualFiducialSets.size());

    CPPUNIT_ASSERT_EQUAL(originalFiducialsSeries->getContentDate(), actualFiducialsSeries->getContentDate());
    CPPUNIT_ASSERT_EQUAL(originalFiducialsSeries->getContentTime(), actualFiducialsSeries->getContentTime());
    CPPUNIT_ASSERT(originalFiducialsSeries->getInstanceNumber() == actualFiducialsSeries->getInstanceNumber());
    CPPUNIT_ASSERT_EQUAL(originalFiducialsSeries->getContentLabel(), actualFiducialsSeries->getContentLabel());
    CPPUNIT_ASSERT_EQUAL(
        originalFiducialsSeries->getContentDescription(),
        actualFiducialsSeries->getContentDescription()
    );
    CPPUNIT_ASSERT_EQUAL(
        originalFiducialsSeries->getContentCreatorName(),
        actualFiducialsSeries->getContentCreatorName()
    );
    CPPUNIT_ASSERT(fiducialSet == actualFiducialSets[0]);
}

} // namespace sight::io::dicom::ut
