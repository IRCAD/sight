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

    auto original_image_series = std::dynamic_pointer_cast<data::image_series>(original->at(0));
    CPPUNIT_ASSERT(original_image_series);
    CPPUNIT_ASSERT(!original_image_series->hasFiducials());
    data::fiducials_series::sptr original_fiducials_series = original_image_series->getFiducials();

    original_fiducials_series->setContentDate("1");
    original_fiducials_series->setContentTime("2");
    original_fiducials_series->setInstanceNumber(3);
    original_fiducials_series->setContentLabel("4");
    original_fiducials_series->setContentDescription("5");
    original_fiducials_series->setContentCreatorName("6");

    data::fiducials_series::FiducialSet fiducial_set;

    data::fiducials_series::ReferencedImage referenced_image;
    referenced_image.referencedSOPClassUID    = data::dicom::sop::get(original_image_series->getSOPKeyword()).m_uid;
    referenced_image.referencedSOPInstanceUID = original_image_series->getSOPInstanceUID();
    referenced_image.referencedFrameNumber    = {7};
    referenced_image.referencedSegmentNumber  = {8};
    fiducial_set.referencedImageSequence      = {referenced_image};

    fiducial_set.frameOfReferenceUID = "9";

    data::fiducials_series::Fiducial fiducial;
    fiducial.shapeType           = data::fiducials_series::Shape::POINT;
    fiducial.fiducialDescription = "10";
    fiducial.fiducialIdentifier  = "11";

    data::fiducials_series::GraphicCoordinatesData graphic_coordinates_data;
    graphic_coordinates_data.referencedImageSequence.referencedSOPClassUID = data::dicom::sop::get(
        original_image_series->getSOPKeyword()
    ).m_uid;
    graphic_coordinates_data.referencedImageSequence.referencedSOPInstanceUID =
        original_image_series->getSOPInstanceUID();
    graphic_coordinates_data.referencedImageSequence.referencedFrameNumber   = {12};
    graphic_coordinates_data.referencedImageSequence.referencedSegmentNumber = {13};
    graphic_coordinates_data.graphicData                                     = {{14, 15}};
    fiducial.graphicCoordinatesDataSequence                                  = {graphic_coordinates_data};

    fiducial.fiducialUID = "16";
    fiducial.contourData = {{17, 18, 19}};
    fiducial_set.fiducialSequence.push_back(fiducial);

    fiducial_set.groupName = "20";

    original_fiducials_series->setSOPInstanceUID(
        std::string(
            data::dicom::sop::get(
                data::dicom::sop::Keyword::
                SpatialFiducialsStorage
            ).m_uid
        ) + ".0"
    );
    original_fiducials_series->appendFiducialSet(fiducial_set);

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

    auto actual_image_series = std::dynamic_pointer_cast<data::image_series>(actual->at(0));
    CPPUNIT_ASSERT(actual_image_series);
    auto actual_fiducials_series = actual_image_series->getFiducials();
    CPPUNIT_ASSERT(actual_fiducials_series);
    std::vector<data::fiducials_series::FiducialSet> actual_fiducial_sets = actual_fiducials_series->getFiducialSets();
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), actual_fiducial_sets.size());

    CPPUNIT_ASSERT_EQUAL(original_fiducials_series->getContentDate(), actual_fiducials_series->getContentDate());
    CPPUNIT_ASSERT_EQUAL(original_fiducials_series->getContentTime(), actual_fiducials_series->getContentTime());
    CPPUNIT_ASSERT(original_fiducials_series->getInstanceNumber() == actual_fiducials_series->getInstanceNumber());
    CPPUNIT_ASSERT_EQUAL(original_fiducials_series->getContentLabel(), actual_fiducials_series->getContentLabel());
    CPPUNIT_ASSERT_EQUAL(
        original_fiducials_series->getContentDescription(),
        actual_fiducials_series->getContentDescription()
    );
    CPPUNIT_ASSERT_EQUAL(
        original_fiducials_series->getContentCreatorName(),
        actual_fiducials_series->getContentCreatorName()
    );
    CPPUNIT_ASSERT(fiducial_set == actual_fiducial_sets[0]);
}

} // namespace sight::io::dicom::ut
