/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "fiducials_series_test.hpp"

#include <data/fiducials_series.hpp>

#include <TestAssert.h>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::fiducials_series_test);

namespace sight::data::ut
{

//------------------------------------------------------------------------------

data::fiducials_series::sptr init_fiducials_series()
{
    auto fs = std::make_shared<sight::data::fiducials_series>();
    fs->setContentLabel("TestLabel");
    fs->setContentLabel("1");
    fs->setContentDescription("FS");
    fs->setContentCreatorName("John Doe");

    // Adds a dummy fiducial set
    fiducials_series::FiducialSet fiducial_set;

    fiducials_series::ReferencedImage referenced_image;
    referenced_image.referencedSOPClassUID    = "1";
    referenced_image.referencedSOPInstanceUID = "2";
    referenced_image.referencedFrameNumber    = {3};
    referenced_image.referencedSegmentNumber  = {4};
    fiducial_set.referencedImageSequence      = {referenced_image};

    fiducial_set.frameOfReferenceUID = "5";

    fiducials_series::Fiducial fiducial;
    fiducial.shapeType           = fiducials_series::Shape::POINT;
    fiducial.fiducialDescription = "6";
    fiducial.fiducialIdentifier  = "7";

    fiducials_series::GraphicCoordinatesData graphic_coordinates_data;
    graphic_coordinates_data.referencedImageSequence.referencedSOPClassUID    = "8";
    graphic_coordinates_data.referencedImageSequence.referencedSOPInstanceUID = "9";
    graphic_coordinates_data.referencedImageSequence.referencedFrameNumber    = {10};
    graphic_coordinates_data.referencedImageSequence.referencedSegmentNumber  = {11};
    graphic_coordinates_data.graphicData                                      = {{12, 13}};
    fiducial.graphicCoordinatesDataSequence                                   = {graphic_coordinates_data};

    fiducial.fiducialUID = "14";
    fiducial.contourData = {{15, 16, 17}};
    fiducial_set.fiducialSequence.push_back(fiducial);

    fiducial_set.groupName  = "18";
    fiducial_set.color      = {{19, 20, 21, 22}};
    fiducial_set.size       = 23.F;
    fiducial_set.shape      = fiducials_series::PrivateShape::CUBE;
    fiducial_set.visibility = true;

    // Test setFiducialSets method
    fs->setFiducialSets({fiducial_set});

    return fs;
}

//------------------------------------------------------------------------------

void fiducials_series_test::shallow_copyTest()
{
    auto fs1 = init_fiducials_series();
    auto fs2 = std::make_shared<data::fiducials_series>();

    CPPUNIT_ASSERT(*fs1 != *fs2);
    fs2->shallow_copy(fs1);
    CPPUNIT_ASSERT(*fs1 == *fs2);
}

//------------------------------------------------------------------------------

void fiducials_series_test::deep_copyTest()
{
    auto fs1 = init_fiducials_series();
    auto fs2 = std::make_shared<data::fiducials_series>();

    CPPUNIT_ASSERT(*fs1 != *fs2);
    fs2->deep_copy(fs1);
    CPPUNIT_ASSERT(*fs1 == *fs2);
}

//------------------------------------------------------------------------------

void fiducials_series_test::simpleSetterGetterTest()
{
    // This method checks that plain method getters/setters work correctly.

    using namespace std::literals::string_literals;
    auto fiducialsSeries = std::make_shared<data::fiducials_series>();

    // NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
    #define COMPARE_GETSETTERS(attribute, before, after, ...) \
    CPPUNIT_ASSERT_EQUAL(before, fiducialsSeries->get ## attribute(__VA_ARGS__)); \
    fiducialsSeries->set ## attribute(__VA_ARGS__ __VA_OPT__(, ) after); \
    CPPUNIT_ASSERT_EQUAL(after, fiducialsSeries->get ## attribute(__VA_ARGS__)); \

    // NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
    #define COMPARE_GETSETTERS_DEMI_OPTIONAL(attribute, before, after, ...) \
    CPPUNIT_ASSERT(before == fiducialsSeries->get ## attribute(__VA_ARGS__)); \
    fiducialsSeries->set ## attribute(__VA_ARGS__ __VA_OPT__(, ) * after); \
    CPPUNIT_ASSERT(after == fiducialsSeries->get ## attribute(__VA_ARGS__)); \

    // NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
    #define COMPARE_GETSETTERS_ARRAYS(attribute, after, ...) \
    CPPUNIT_ASSERT(fiducialsSeries->get ## attribute(__VA_ARGS__).empty()); \
    fiducialsSeries->set ## attribute(__VA_ARGS__ __VA_OPT__(, ) after); \
    CPPUNIT_ASSERT(after == fiducialsSeries->get ## attribute(__VA_ARGS__)); \

    // NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
    #define COMPARE_GETSETTERS_OPTIONAL(attribute, before, after, ...) \
    CPPUNIT_ASSERT(before == fiducialsSeries->get ## attribute(__VA_ARGS__)); \
    fiducialsSeries->set ## attribute(__VA_ARGS__ __VA_OPT__(, ) after); \
    CPPUNIT_ASSERT(after == fiducialsSeries->get ## attribute(__VA_ARGS__)); \

    COMPARE_GETSETTERS(ContentDate, ""s, "0"s);
    COMPARE_GETSETTERS(ContentLabel, ""s, "1"s);
    COMPARE_GETSETTERS(ContentDescription, ""s, "2"s);
    COMPARE_GETSETTERS(ContentCreatorName, ""s, "3"s);

    CPPUNIT_ASSERT(fiducialsSeries->getFiducialSets().empty());
    CPPUNIT_ASSERT(!fiducialsSeries->getReferencedImages(0));

    COMPARE_GETSETTERS_DEMI_OPTIONAL(ReferencedSOPClassUID, std::optional<std::string> {}, std::optional {"4"s}, 0, 0);
    COMPARE_GETSETTERS_DEMI_OPTIONAL(
        ReferencedSOPInstanceUID,
        std::optional<std::string> {},
        std::optional {"5"s},
        0,
        0
    );
    COMPARE_GETSETTERS_ARRAYS(ReferencedFrameNumber, std::vector<std::int32_t> {6}, 0, 0);
    COMPARE_GETSETTERS_ARRAYS(ReferencedSegmentNumber, std::vector<std::uint16_t> {7}, 0, 0);

    COMPARE_GETSETTERS_OPTIONAL(FrameOfReferenceUID, std::optional<std::string> {}, std::optional {"8"s}, 0);

    CPPUNIT_ASSERT(fiducialsSeries->getFiducials(0).empty());

    COMPARE_GETSETTERS(_shape_type, fiducials_series::Shape::INVALID, fiducials_series::Shape::POINT, 0, 0);
    COMPARE_GETSETTERS(FiducialDescription, ""s, "9"s, 0, 0);
    COMPARE_GETSETTERS(FiducialIdentifier, ""s, "10"s, 0, 0);

    CPPUNIT_ASSERT(!fiducialsSeries->getGraphicCoordinatesDataSequence(0, 0).has_value());

    COMPARE_GETSETTERS(ReferencedSOPClassUID, ""s, "11"s, 0, 0, 0);
    COMPARE_GETSETTERS(ReferencedSOPInstanceUID, ""s, "12"s, 0, 0, 0);
    COMPARE_GETSETTERS_ARRAYS(ReferencedFrameNumber, std::vector<std::int32_t> {13}, 0, 0, 0);
    COMPARE_GETSETTERS_ARRAYS(ReferencedSegmentNumber, std::vector<std::uint16_t> {14}, 0, 0, 0);

    COMPARE_GETSETTERS_ARRAYS(GraphicData, (std::vector<fiducials_series::Point2> {{15, 16}}), 0, 0, 0);

    COMPARE_GETSETTERS_OPTIONAL(FiducialUID, std::optional<std::string> {}, std::optional {"17"s}, 0, 0);
    COMPARE_GETSETTERS_ARRAYS(ContourData, (std::vector<fiducials_series::Point3> {{{18, 19, 20}}}), 0, 0);

    COMPARE_GETSETTERS_DEMI_OPTIONAL(GroupName, std::optional<std::string> {}, std::optional {"21"s}, 0);
    COMPARE_GETSETTERS_DEMI_OPTIONAL(
        Color,
        (std::optional<std::array<float, 4> > {}),
        (std::optional {std::array {22.F, 23.F, 24.F, 25.F}}),
        0
    );
    COMPARE_GETSETTERS_DEMI_OPTIONAL(Size, std::optional<float> {}, std::optional {26.F}, 0);
    COMPARE_GETSETTERS_DEMI_OPTIONAL(
        Shape,
        std::optional<fiducials_series::PrivateShape> {},
        std::optional {fiducials_series::PrivateShape::SPHERE},
        0
    );
    COMPARE_GETSETTERS_DEMI_OPTIONAL(Visibility, std::optional<bool> {}, std::optional {true}, 0);
}

//------------------------------------------------------------------------------

static void compare_referenced_images(
    fiducials_series::ReferencedImage _expected,
    fiducials_series::ReferencedImage _actual
)
{
    CPPUNIT_ASSERT_EQUAL(_expected.referencedSOPClassUID, _actual.referencedSOPClassUID);
    CPPUNIT_ASSERT_EQUAL(_expected.referencedSOPInstanceUID, _actual.referencedSOPInstanceUID);
    CPPUNIT_ASSERT(_expected.referencedFrameNumber == _actual.referencedFrameNumber);
    CPPUNIT_ASSERT(_expected.referencedSegmentNumber == _actual.referencedSegmentNumber);
}

//------------------------------------------------------------------------------

static void compare_graphic_coordinates_data(
    fiducials_series::GraphicCoordinatesData _expected,
    fiducials_series::GraphicCoordinatesData _actual
)
{
    compare_referenced_images(_expected.referencedImageSequence, _actual.referencedImageSequence);
    CPPUNIT_ASSERT(_expected.graphicData == _actual.graphicData);
}

//------------------------------------------------------------------------------

static void compare_fiducials(fiducials_series::Fiducial _expected, fiducials_series::Fiducial _actual)
{
    CPPUNIT_ASSERT_EQUAL(_expected.shapeType, _actual.shapeType);
    CPPUNIT_ASSERT_EQUAL(_expected.fiducialDescription, _actual.fiducialDescription);
    CPPUNIT_ASSERT_EQUAL(_expected.fiducialIdentifier, _actual.fiducialIdentifier);

    CPPUNIT_ASSERT_EQUAL(
        _expected.graphicCoordinatesDataSequence.has_value(),
        _actual.graphicCoordinatesDataSequence.has_value()
    );
    if(_expected.graphicCoordinatesDataSequence.has_value())
    {
        CPPUNIT_ASSERT_EQUAL(
            _expected.graphicCoordinatesDataSequence->size(),
            _actual.graphicCoordinatesDataSequence->size()
        );
        for(std::size_t i = 0 ; i < _expected.graphicCoordinatesDataSequence->size() ; i++)
        {
            compare_graphic_coordinates_data(
                _expected.graphicCoordinatesDataSequence->at(i),
                _actual.graphicCoordinatesDataSequence->at(i)
            );
        }
    }

    CPPUNIT_ASSERT(_expected.fiducialUID == _actual.fiducialUID);
    CPPUNIT_ASSERT(_expected.contourData == _actual.contourData);
}

//------------------------------------------------------------------------------

static void compare_fiducial_sets(fiducials_series::FiducialSet _expected, fiducials_series::FiducialSet _actual)
{
    CPPUNIT_ASSERT_EQUAL(_expected.referencedImageSequence.has_value(), _actual.referencedImageSequence.has_value());
    if(_expected.referencedImageSequence.has_value())
    {
        CPPUNIT_ASSERT_EQUAL(_expected.referencedImageSequence->size(), _actual.referencedImageSequence->size());
        for(std::size_t i = 0 ; i < _expected.referencedImageSequence->size() ; i++)
        {
            compare_referenced_images(_expected.referencedImageSequence->at(i), _actual.referencedImageSequence->at(i));
        }
    }

    CPPUNIT_ASSERT(_expected.frameOfReferenceUID == _actual.frameOfReferenceUID);

    CPPUNIT_ASSERT_EQUAL(_expected.fiducialSequence.size(), _actual.fiducialSequence.size());
    for(std::size_t i = 0 ; i < _expected.fiducialSequence.size() ; i++)
    {
        compare_fiducials(_expected.fiducialSequence[i], _actual.fiducialSequence[i]);
    }

    CPPUNIT_ASSERT(_expected.groupName == _actual.groupName);
    CPPUNIT_ASSERT(_expected.color == _actual.color);
    CPPUNIT_ASSERT(_expected.size == _actual.size);
    CPPUNIT_ASSERT(_expected.shape == _actual.shape);
}

//------------------------------------------------------------------------------

void fiducials_series_test::fiducialSetSetterGetterTest()
{
    // This method checks that adding fiducial sets works correctly.

    auto fiducials_series = std::make_shared<data::fiducials_series>();
    CPPUNIT_ASSERT(fiducials_series->getFiducialSets().empty());

    fiducials_series::FiducialSet fiducial_set;

    fiducials_series::ReferencedImage referenced_image;
    referenced_image.referencedSOPClassUID    = "1";
    referenced_image.referencedSOPInstanceUID = "2";
    referenced_image.referencedFrameNumber    = {3};
    referenced_image.referencedSegmentNumber  = {4};
    fiducial_set.referencedImageSequence      = {referenced_image};

    fiducial_set.frameOfReferenceUID = "5";

    fiducials_series::Fiducial fiducial;
    fiducial.shapeType           = fiducials_series::Shape::POINT;
    fiducial.fiducialDescription = "6";
    fiducial.fiducialIdentifier  = "7";

    fiducials_series::GraphicCoordinatesData graphic_coordinates_data;
    graphic_coordinates_data.referencedImageSequence.referencedSOPClassUID    = "8";
    graphic_coordinates_data.referencedImageSequence.referencedSOPInstanceUID = "9";
    graphic_coordinates_data.referencedImageSequence.referencedFrameNumber    = {10};
    graphic_coordinates_data.referencedImageSequence.referencedSegmentNumber  = {11};
    graphic_coordinates_data.graphicData                                      = {{12, 13}};
    fiducial.graphicCoordinatesDataSequence                                   = {graphic_coordinates_data};

    fiducial.fiducialUID = "14";
    fiducial.contourData = {{15, 16, 17}};
    fiducial_set.fiducialSequence.push_back(fiducial);

    fiducial_set.groupName  = "18";
    fiducial_set.color      = {{19, 20, 21, 22}};
    fiducial_set.size       = 23.F;
    fiducial_set.shape      = fiducials_series::PrivateShape::CUBE;
    fiducial_set.visibility = true;

    // Test setFiducialSets method
    fiducials_series->setFiducialSets({fiducial_set});
    std::vector<fiducials_series::FiducialSet> fiducial_sets = fiducials_series->getFiducialSets();
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), fiducial_sets.size());
    compare_fiducial_sets(fiducial_set, fiducial_sets[0]);

    // Test setFiducialSet method
    fiducials_series->setFiducialSet(1, fiducial_set);
    fiducial_sets = fiducials_series->getFiducialSets();
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), fiducial_sets.size());
    for(std::size_t i = 0 ; i < 2 ; i++)
    {
        compare_fiducial_sets(fiducial_set, fiducial_sets[i]);
    }

    // Test appendFiducialSet
    fiducials_series->appendFiducialSet(fiducial_set);
    fiducial_sets = fiducials_series->getFiducialSets();
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), fiducial_sets.size());
    for(std::size_t i = 0 ; i < 3 ; i++)
    {
        compare_fiducial_sets(fiducial_set, fiducial_sets[i]);
    }
}

//------------------------------------------------------------------------------

void fiducials_series_test::referencedImageSetterGetterTest()
{
    // This method checks that adding referenced images works correctly.

    auto fiducials_series = std::make_shared<data::fiducials_series>();
    CPPUNIT_ASSERT(!fiducials_series->getReferencedImages(0));

    fiducials_series::ReferencedImage referenced_image;
    referenced_image.referencedSOPClassUID    = "1";
    referenced_image.referencedSOPInstanceUID = "2";
    referenced_image.referencedFrameNumber    = {3};
    referenced_image.referencedSegmentNumber  = {4};

    // Test setReferencedImages method
    fiducials_series->setReferencedImages(0, {{referenced_image}});
    std::optional<std::vector<fiducials_series::ReferencedImage> > referenced_images =
        fiducials_series->getReferencedImages(0);
    CPPUNIT_ASSERT(referenced_images);
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), referenced_images->size());
    compare_referenced_images(referenced_image, referenced_images->at(0));

    // Test setReferencedImage method
    fiducials_series->setReferencedImage(0, 1, referenced_image);
    referenced_images = fiducials_series->getReferencedImages(0);
    CPPUNIT_ASSERT(referenced_images);
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), referenced_images->size());
    for(std::size_t i = 0 ; i < 2 ; i++)
    {
        compare_referenced_images(referenced_image, referenced_images->at(i));
    }

    // Test appendReferencedImage method
    fiducials_series->appendReferencedImage(0, referenced_image);
    referenced_images = fiducials_series->getReferencedImages(0);
    CPPUNIT_ASSERT(referenced_images);
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), referenced_images->size());
    for(std::size_t i = 0 ; i < 3 ; i++)
    {
        compare_referenced_images(referenced_image, referenced_images->at(i));
    }

    // Test setReferencedImages method in a second fiducial set
    fiducials_series->setReferencedImages(1, {{referenced_image}});
    referenced_images = fiducials_series->getReferencedImages(0);
    CPPUNIT_ASSERT(referenced_images);
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), referenced_images->size());
    for(std::size_t i = 0 ; i < 3 ; i++)
    {
        compare_referenced_images(referenced_image, referenced_images->at(i));
    }

    referenced_images = fiducials_series->getReferencedImages(1);
    CPPUNIT_ASSERT(referenced_images);
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), referenced_images->size());
    compare_referenced_images(referenced_image, referenced_images->at(0));

    // Test setReferencedImage method in a second fiducial set
    fiducials_series->setReferencedImage(1, 1, referenced_image);
    referenced_images = fiducials_series->getReferencedImages(0);
    CPPUNIT_ASSERT(referenced_images);
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), referenced_images->size());
    for(std::size_t i = 0 ; i < 3 ; i++)
    {
        compare_referenced_images(referenced_image, referenced_images->at(i));
    }

    referenced_images = fiducials_series->getReferencedImages(1);
    CPPUNIT_ASSERT(referenced_images);
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), referenced_images->size());
    for(std::size_t i = 0 ; i < 2 ; i++)
    {
        compare_referenced_images(referenced_image, referenced_images->at(i));
    }

    // Test appendReferencedImage method in a second fiducial set
    fiducials_series->appendReferencedImage(1, referenced_image);
    referenced_images = fiducials_series->getReferencedImages(0);
    CPPUNIT_ASSERT(referenced_images);
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), referenced_images->size());
    for(std::size_t i = 0 ; i < 3 ; i++)
    {
        compare_referenced_images(referenced_image, referenced_images->at(i));
    }

    referenced_images = fiducials_series->getReferencedImages(1);
    CPPUNIT_ASSERT(referenced_images);
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), referenced_images->size());
    for(std::size_t i = 0 ; i < 3 ; i++)
    {
        compare_referenced_images(referenced_image, referenced_images->at(i));
    }
}

//------------------------------------------------------------------------------

void fiducials_series_test::fiducialSetterGetterTest()
{
    // This method checks that adding fiducials works correctly.

    auto fiducials_series = std::make_shared<data::fiducials_series>();
    CPPUNIT_ASSERT(fiducials_series->getFiducials(0).empty());

    fiducials_series::Fiducial fiducial;
    fiducial.shapeType           = fiducials_series::Shape::POINT;
    fiducial.fiducialDescription = "1";
    fiducial.fiducialIdentifier  = "2";

    fiducials_series::GraphicCoordinatesData graphic_coordinates_data;
    graphic_coordinates_data.referencedImageSequence.referencedSOPClassUID    = "3";
    graphic_coordinates_data.referencedImageSequence.referencedSOPInstanceUID = "4";
    graphic_coordinates_data.referencedImageSequence.referencedFrameNumber    = {5};
    graphic_coordinates_data.referencedImageSequence.referencedSegmentNumber  = {6};
    graphic_coordinates_data.graphicData                                      = {{7, 8}};
    fiducial.graphicCoordinatesDataSequence                                   = {graphic_coordinates_data};

    fiducial.fiducialUID = "9";
    fiducial.contourData = {{10, 11, 12}};

    // Test setFiducials method
    fiducials_series->setFiducials(0, {fiducial});
    std::vector<fiducials_series::Fiducial> fiducials = fiducials_series->getFiducials(0);
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), fiducials.size());
    compare_fiducials(fiducial, fiducials[0]);

    // Test setFiducial method
    fiducials_series->setFiducial(0, 1, fiducial);
    fiducials = fiducials_series->getFiducials(0);
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), fiducials.size());
    for(std::size_t i = 0 ; i < 2 ; i++)
    {
        compare_fiducials(fiducial, fiducials[i]);
    }

    // Test appendFiducial method
    fiducials_series->appendFiducial(0, fiducial);
    fiducials = fiducials_series->getFiducials(0);
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), fiducials.size());
    for(std::size_t i = 0 ; i < 3 ; i++)
    {
        compare_fiducials(fiducial, fiducials[i]);
    }

    // Test setFiducials method in a second fiducial set
    fiducials_series->setFiducials(1, {fiducial});
    fiducials = fiducials_series->getFiducials(0);
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), fiducials.size());
    for(std::size_t i = 0 ; i < 3 ; i++)
    {
        compare_fiducials(fiducial, fiducials[i]);
    }

    fiducials = fiducials_series->getFiducials(1);
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), fiducials.size());
    compare_fiducials(fiducial, fiducials[0]);

    // Test setFiducial method in a second fiducial set
    fiducials_series->setFiducial(1, 1, fiducial);
    fiducials = fiducials_series->getFiducials(0);
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), fiducials.size());
    for(std::size_t i = 0 ; i < 3 ; i++)
    {
        compare_fiducials(fiducial, fiducials[i]);
    }

    fiducials = fiducials_series->getFiducials(1);
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), fiducials.size());
    for(std::size_t i = 0 ; i < 2 ; i++)
    {
        compare_fiducials(fiducial, fiducials[i]);
    }

    // Test appendFiducial method in a second fiducial set
    fiducials_series->appendFiducial(1, fiducial);
    fiducials = fiducials_series->getFiducials(0);
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), fiducials.size());
    for(std::size_t i = 0 ; i < 3 ; i++)
    {
        compare_fiducials(fiducial, fiducials[i]);
    }

    fiducials = fiducials_series->getFiducials(1);
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), fiducials.size());
    for(std::size_t i = 0 ; i < 3 ; i++)
    {
        compare_fiducials(fiducial, fiducials[i]);
    }
}

//------------------------------------------------------------------------------

void fiducials_series_test::graphicCoordinatesDataSetterGetterTest()
{
    // This method checks that adding graphic coordinates data works correctly.

    auto fiducials_series = std::make_shared<data::fiducials_series>();
    CPPUNIT_ASSERT(!fiducials_series->getGraphicCoordinatesDataSequence(0, 0));

    fiducials_series::GraphicCoordinatesData graphic_coordinates_data;
    graphic_coordinates_data.referencedImageSequence.referencedSOPClassUID    = "1";
    graphic_coordinates_data.referencedImageSequence.referencedSOPInstanceUID = "2";
    graphic_coordinates_data.referencedImageSequence.referencedFrameNumber    = {3};
    graphic_coordinates_data.referencedImageSequence.referencedSegmentNumber  = {4};
    graphic_coordinates_data.graphicData                                      = {{5, 6}};

    // Test setGraphicCoordinatesDataSequence method
    fiducials_series->setGraphicCoordinatesDataSequence(0, 0, {{graphic_coordinates_data}});
    std::optional<std::vector<fiducials_series::GraphicCoordinatesData> > graphic_coordinates_data_sequence =
        fiducials_series->getGraphicCoordinatesDataSequence(0, 0);
    CPPUNIT_ASSERT(graphic_coordinates_data_sequence);
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), graphic_coordinates_data_sequence->size());
    compare_graphic_coordinates_data(graphic_coordinates_data, graphic_coordinates_data_sequence->at(0));

    // Test setGraphicCoordinatesData method
    fiducials_series->setGraphicCoordinatesData(0, 0, 1, graphic_coordinates_data);
    graphic_coordinates_data_sequence = fiducials_series->getGraphicCoordinatesDataSequence(0, 0);
    CPPUNIT_ASSERT(graphic_coordinates_data_sequence);
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), graphic_coordinates_data_sequence->size());
    for(std::size_t i = 0 ; i < 2 ; i++)
    {
        compare_graphic_coordinates_data(graphic_coordinates_data, graphic_coordinates_data_sequence->at(i));
    }

    // Test appendGraphicCoordinatesData method
    fiducials_series->appendGraphicCoordinatesData(0, 0, graphic_coordinates_data);
    graphic_coordinates_data_sequence = fiducials_series->getGraphicCoordinatesDataSequence(0, 0);
    CPPUNIT_ASSERT(graphic_coordinates_data_sequence);
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), graphic_coordinates_data_sequence->size());
    for(std::size_t i = 0 ; i < 3 ; i++)
    {
        compare_graphic_coordinates_data(graphic_coordinates_data, graphic_coordinates_data_sequence->at(i));
    }

    // Test setGraphicCoordinatesDataSequence method in a second fiducial
    fiducials_series->setGraphicCoordinatesDataSequence(0, 1, {{graphic_coordinates_data}});
    graphic_coordinates_data_sequence = fiducials_series->getGraphicCoordinatesDataSequence(0, 0);
    CPPUNIT_ASSERT(graphic_coordinates_data_sequence);
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), graphic_coordinates_data_sequence->size());
    for(std::size_t i = 0 ; i < 3 ; i++)
    {
        compare_graphic_coordinates_data(graphic_coordinates_data, graphic_coordinates_data_sequence->at(i));
    }

    graphic_coordinates_data_sequence = fiducials_series->getGraphicCoordinatesDataSequence(0, 1);
    CPPUNIT_ASSERT(graphic_coordinates_data_sequence);
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), graphic_coordinates_data_sequence->size());
    compare_graphic_coordinates_data(graphic_coordinates_data, graphic_coordinates_data_sequence->at(0));

    // Test setGraphicCoordinatesData method in a second fiducial
    fiducials_series->setGraphicCoordinatesData(0, 1, 1, graphic_coordinates_data);
    graphic_coordinates_data_sequence = fiducials_series->getGraphicCoordinatesDataSequence(0, 0);
    CPPUNIT_ASSERT(graphic_coordinates_data_sequence);
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), graphic_coordinates_data_sequence->size());
    for(std::size_t i = 0 ; i < 3 ; i++)
    {
        compare_graphic_coordinates_data(graphic_coordinates_data, graphic_coordinates_data_sequence->at(i));
    }

    graphic_coordinates_data_sequence = fiducials_series->getGraphicCoordinatesDataSequence(0, 1);
    CPPUNIT_ASSERT(graphic_coordinates_data_sequence);
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), graphic_coordinates_data_sequence->size());
    for(std::size_t i = 0 ; i < 2 ; i++)
    {
        compare_graphic_coordinates_data(graphic_coordinates_data, graphic_coordinates_data_sequence->at(i));
    }

    // Test appendGraphicCoordinatesData method in a second fiducial
    fiducials_series->appendGraphicCoordinatesData(0, 1, graphic_coordinates_data);
    graphic_coordinates_data_sequence = fiducials_series->getGraphicCoordinatesDataSequence(0, 0);
    CPPUNIT_ASSERT(graphic_coordinates_data_sequence);
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), graphic_coordinates_data_sequence->size());
    for(std::size_t i = 0 ; i < 3 ; i++)
    {
        compare_graphic_coordinates_data(graphic_coordinates_data, graphic_coordinates_data_sequence->at(i));
    }

    graphic_coordinates_data_sequence = fiducials_series->getGraphicCoordinatesDataSequence(0, 1);
    CPPUNIT_ASSERT(graphic_coordinates_data_sequence);
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), graphic_coordinates_data_sequence->size());
    for(std::size_t i = 0 ; i < 3 ; i++)
    {
        compare_graphic_coordinates_data(graphic_coordinates_data, graphic_coordinates_data_sequence->at(i));
    }
}

//------------------------------------------------------------------------------

} // namespace sight::data::ut
