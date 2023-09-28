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

#include "FiducialsSeriesTest.hpp"

#include <data/FiducialsSeries.hpp>

#include <TestAssert.h>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::FiducialsSeriesTest);

namespace sight::data::ut
{

//------------------------------------------------------------------------------

data::FiducialsSeries::sptr initFiducialsSeries()
{
    data::FiducialsSeries::sptr fs = data::FiducialsSeries::New();
    fs->setContentLabel("TestLabel");
    fs->setContentLabel("1");
    fs->setContentDescription("FS");
    fs->setContentCreatorName("John Doe");

    // Adds a dummy fiducial set
    FiducialsSeries::FiducialSet fiducialSet;

    FiducialsSeries::ReferencedImage referencedImage;
    referencedImage.referencedSOPClassUID    = "1";
    referencedImage.referencedSOPInstanceUID = "2";
    referencedImage.referencedFrameNumber    = {3};
    referencedImage.referencedSegmentNumber  = {4};
    fiducialSet.referencedImageSequence      = {referencedImage};

    fiducialSet.frameOfReferenceUID = "5";

    FiducialsSeries::Fiducial fiducial;
    fiducial.shapeType           = FiducialsSeries::Shape::POINT;
    fiducial.fiducialDescription = "6";
    fiducial.fiducialIdentifier  = "7";

    FiducialsSeries::GraphicCoordinatesData graphicCoordinatesData;
    graphicCoordinatesData.referencedImageSequence.referencedSOPClassUID    = "8";
    graphicCoordinatesData.referencedImageSequence.referencedSOPInstanceUID = "9";
    graphicCoordinatesData.referencedImageSequence.referencedFrameNumber    = {10};
    graphicCoordinatesData.referencedImageSequence.referencedSegmentNumber  = {11};
    graphicCoordinatesData.graphicData                                      = {{12, 13}};
    fiducial.graphicCoordinatesDataSequence                                 = {graphicCoordinatesData};

    fiducial.fiducialUID = "14";
    fiducial.contourData = {{15, 16, 17}};
    fiducialSet.fiducialSequence.push_back(fiducial);

    fiducialSet.groupName  = "18";
    fiducialSet.color      = {{19, 20, 21, 22}};
    fiducialSet.size       = 23.F;
    fiducialSet.shape      = FiducialsSeries::PrivateShape::CUBE;
    fiducialSet.visibility = true;

    // Test setFiducialSets method
    fs->setFiducialSets({fiducialSet});

    return fs;
}

//------------------------------------------------------------------------------

void FiducialsSeriesTest::shallowCopyTest()
{
    data::FiducialsSeries::sptr fs1 = initFiducialsSeries();
    data::FiducialsSeries::sptr fs2 = data::FiducialsSeries::New();

    CPPUNIT_ASSERT(*fs1 != *fs2);
    fs2->shallowCopy(fs1);
    CPPUNIT_ASSERT(*fs1 == *fs2);
}

//------------------------------------------------------------------------------

void FiducialsSeriesTest::deepCopyTest()
{
    data::FiducialsSeries::sptr fs1 = initFiducialsSeries();
    data::FiducialsSeries::sptr fs2 = data::FiducialsSeries::New();

    CPPUNIT_ASSERT(*fs1 != *fs2);
    fs2->deepCopy(fs1);
    CPPUNIT_ASSERT(*fs1 == *fs2);
}

//------------------------------------------------------------------------------

void FiducialsSeriesTest::simpleSetterGetterTest()
{
    // This method checks that plain method getters/setters work correctly.

    using namespace std::literals::string_literals;
    auto fiducialsSeries = FiducialsSeries::New();

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

    COMPARE_GETSETTERS(ShapeType, FiducialsSeries::Shape::INVALID, FiducialsSeries::Shape::POINT, 0, 0);
    COMPARE_GETSETTERS(FiducialDescription, ""s, "9"s, 0, 0);
    COMPARE_GETSETTERS(FiducialIdentifier, ""s, "10"s, 0, 0);

    CPPUNIT_ASSERT(!fiducialsSeries->getGraphicCoordinatesDataSequence(0, 0).has_value());

    COMPARE_GETSETTERS(ReferencedSOPClassUID, ""s, "11"s, 0, 0, 0);
    COMPARE_GETSETTERS(ReferencedSOPInstanceUID, ""s, "12"s, 0, 0, 0);
    COMPARE_GETSETTERS_ARRAYS(ReferencedFrameNumber, std::vector<std::int32_t> {13}, 0, 0, 0);
    COMPARE_GETSETTERS_ARRAYS(ReferencedSegmentNumber, std::vector<std::uint16_t> {14}, 0, 0, 0);

    COMPARE_GETSETTERS_ARRAYS(GraphicData, (std::vector<FiducialsSeries::Point2> {{15, 16}}), 0, 0, 0);

    COMPARE_GETSETTERS_OPTIONAL(FiducialUID, std::optional<std::string> {}, std::optional {"17"s}, 0, 0);
    COMPARE_GETSETTERS_ARRAYS(ContourData, (std::vector<FiducialsSeries::Point3> {{{18, 19, 20}}}), 0, 0);

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
        std::optional<FiducialsSeries::PrivateShape> {},
        std::optional {FiducialsSeries::PrivateShape::SPHERE},
        0
    );
    COMPARE_GETSETTERS_DEMI_OPTIONAL(Visibility, std::optional<bool> {}, std::optional {true}, 0);
}

//------------------------------------------------------------------------------

static void compareReferencedImages(FiducialsSeries::ReferencedImage expected, FiducialsSeries::ReferencedImage actual)
{
    CPPUNIT_ASSERT_EQUAL(expected.referencedSOPClassUID, actual.referencedSOPClassUID);
    CPPUNIT_ASSERT_EQUAL(expected.referencedSOPInstanceUID, actual.referencedSOPInstanceUID);
    CPPUNIT_ASSERT(expected.referencedFrameNumber == actual.referencedFrameNumber);
    CPPUNIT_ASSERT(expected.referencedSegmentNumber == actual.referencedSegmentNumber);
}

//------------------------------------------------------------------------------

static void compareGraphicCoordinatesData(
    FiducialsSeries::GraphicCoordinatesData expected,
    FiducialsSeries::GraphicCoordinatesData actual
)
{
    compareReferencedImages(expected.referencedImageSequence, actual.referencedImageSequence);
    CPPUNIT_ASSERT(expected.graphicData == actual.graphicData);
}

//------------------------------------------------------------------------------

static void compareFiducials(FiducialsSeries::Fiducial expected, FiducialsSeries::Fiducial actual)
{
    CPPUNIT_ASSERT_EQUAL(expected.shapeType, actual.shapeType);
    CPPUNIT_ASSERT_EQUAL(expected.fiducialDescription, actual.fiducialDescription);
    CPPUNIT_ASSERT_EQUAL(expected.fiducialIdentifier, actual.fiducialIdentifier);

    CPPUNIT_ASSERT_EQUAL(
        expected.graphicCoordinatesDataSequence.has_value(),
        actual.graphicCoordinatesDataSequence.has_value()
    );
    if(expected.graphicCoordinatesDataSequence.has_value())
    {
        CPPUNIT_ASSERT_EQUAL(
            expected.graphicCoordinatesDataSequence->size(),
            actual.graphicCoordinatesDataSequence->size()
        );
        for(std::size_t i = 0 ; i < expected.graphicCoordinatesDataSequence->size() ; i++)
        {
            compareGraphicCoordinatesData(
                expected.graphicCoordinatesDataSequence->at(i),
                actual.graphicCoordinatesDataSequence->at(i)
            );
        }
    }

    CPPUNIT_ASSERT(expected.fiducialUID == actual.fiducialUID);
    CPPUNIT_ASSERT(expected.contourData == actual.contourData);
}

//------------------------------------------------------------------------------

static void compareFiducialSets(FiducialsSeries::FiducialSet expected, FiducialsSeries::FiducialSet actual)
{
    CPPUNIT_ASSERT_EQUAL(expected.referencedImageSequence.has_value(), actual.referencedImageSequence.has_value());
    if(expected.referencedImageSequence.has_value())
    {
        CPPUNIT_ASSERT_EQUAL(expected.referencedImageSequence->size(), actual.referencedImageSequence->size());
        for(std::size_t i = 0 ; i < expected.referencedImageSequence->size() ; i++)
        {
            compareReferencedImages(expected.referencedImageSequence->at(i), actual.referencedImageSequence->at(i));
        }
    }

    CPPUNIT_ASSERT(expected.frameOfReferenceUID == actual.frameOfReferenceUID);

    CPPUNIT_ASSERT_EQUAL(expected.fiducialSequence.size(), actual.fiducialSequence.size());
    for(std::size_t i = 0 ; i < expected.fiducialSequence.size() ; i++)
    {
        compareFiducials(expected.fiducialSequence[i], actual.fiducialSequence[i]);
    }

    CPPUNIT_ASSERT(expected.groupName == actual.groupName);
    CPPUNIT_ASSERT(expected.color == actual.color);
    CPPUNIT_ASSERT(expected.size == actual.size);
    CPPUNIT_ASSERT(expected.shape == actual.shape);
}

//------------------------------------------------------------------------------

void FiducialsSeriesTest::fiducialSetSetterGetterTest()
{
    // This method checks that adding fiducial sets works correctly.

    auto fiducialsSeries = FiducialsSeries::New();
    CPPUNIT_ASSERT(fiducialsSeries->getFiducialSets().empty());

    FiducialsSeries::FiducialSet fiducialSet;

    FiducialsSeries::ReferencedImage referencedImage;
    referencedImage.referencedSOPClassUID    = "1";
    referencedImage.referencedSOPInstanceUID = "2";
    referencedImage.referencedFrameNumber    = {3};
    referencedImage.referencedSegmentNumber  = {4};
    fiducialSet.referencedImageSequence      = {referencedImage};

    fiducialSet.frameOfReferenceUID = "5";

    FiducialsSeries::Fiducial fiducial;
    fiducial.shapeType           = FiducialsSeries::Shape::POINT;
    fiducial.fiducialDescription = "6";
    fiducial.fiducialIdentifier  = "7";

    FiducialsSeries::GraphicCoordinatesData graphicCoordinatesData;
    graphicCoordinatesData.referencedImageSequence.referencedSOPClassUID    = "8";
    graphicCoordinatesData.referencedImageSequence.referencedSOPInstanceUID = "9";
    graphicCoordinatesData.referencedImageSequence.referencedFrameNumber    = {10};
    graphicCoordinatesData.referencedImageSequence.referencedSegmentNumber  = {11};
    graphicCoordinatesData.graphicData                                      = {{12, 13}};
    fiducial.graphicCoordinatesDataSequence                                 = {graphicCoordinatesData};

    fiducial.fiducialUID = "14";
    fiducial.contourData = {{15, 16, 17}};
    fiducialSet.fiducialSequence.push_back(fiducial);

    fiducialSet.groupName  = "18";
    fiducialSet.color      = {{19, 20, 21, 22}};
    fiducialSet.size       = 23.F;
    fiducialSet.shape      = FiducialsSeries::PrivateShape::CUBE;
    fiducialSet.visibility = true;

    // Test setFiducialSets method
    fiducialsSeries->setFiducialSets({fiducialSet});
    std::vector<FiducialsSeries::FiducialSet> fiducialSets = fiducialsSeries->getFiducialSets();
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), fiducialSets.size());
    compareFiducialSets(fiducialSet, fiducialSets[0]);

    // Test setFiducialSet method
    fiducialsSeries->setFiducialSet(1, fiducialSet);
    fiducialSets = fiducialsSeries->getFiducialSets();
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), fiducialSets.size());
    for(std::size_t i = 0 ; i < 2 ; i++)
    {
        compareFiducialSets(fiducialSet, fiducialSets[i]);
    }

    // Test appendFiducialSet
    fiducialsSeries->appendFiducialSet(fiducialSet);
    fiducialSets = fiducialsSeries->getFiducialSets();
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), fiducialSets.size());
    for(std::size_t i = 0 ; i < 3 ; i++)
    {
        compareFiducialSets(fiducialSet, fiducialSets[i]);
    }
}

//------------------------------------------------------------------------------

void FiducialsSeriesTest::referencedImageSetterGetterTest()
{
    // This method checks that adding referenced images works correctly.

    auto fiducialsSeries = FiducialsSeries::New();
    CPPUNIT_ASSERT(!fiducialsSeries->getReferencedImages(0));

    FiducialsSeries::ReferencedImage referencedImage;
    referencedImage.referencedSOPClassUID    = "1";
    referencedImage.referencedSOPInstanceUID = "2";
    referencedImage.referencedFrameNumber    = {3};
    referencedImage.referencedSegmentNumber  = {4};

    // Test setReferencedImages method
    fiducialsSeries->setReferencedImages(0, {{referencedImage}});
    std::optional<std::vector<FiducialsSeries::ReferencedImage> > referencedImages =
        fiducialsSeries->getReferencedImages(0);
    CPPUNIT_ASSERT(referencedImages);
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), referencedImages->size());
    compareReferencedImages(referencedImage, referencedImages->at(0));

    // Test setReferencedImage method
    fiducialsSeries->setReferencedImage(0, 1, referencedImage);
    referencedImages = fiducialsSeries->getReferencedImages(0);
    CPPUNIT_ASSERT(referencedImages);
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), referencedImages->size());
    for(std::size_t i = 0 ; i < 2 ; i++)
    {
        compareReferencedImages(referencedImage, referencedImages->at(i));
    }

    // Test appendReferencedImage method
    fiducialsSeries->appendReferencedImage(0, referencedImage);
    referencedImages = fiducialsSeries->getReferencedImages(0);
    CPPUNIT_ASSERT(referencedImages);
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), referencedImages->size());
    for(std::size_t i = 0 ; i < 3 ; i++)
    {
        compareReferencedImages(referencedImage, referencedImages->at(i));
    }

    // Test setReferencedImages method in a second fiducial set
    fiducialsSeries->setReferencedImages(1, {{referencedImage}});
    referencedImages = fiducialsSeries->getReferencedImages(0);
    CPPUNIT_ASSERT(referencedImages);
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), referencedImages->size());
    for(std::size_t i = 0 ; i < 3 ; i++)
    {
        compareReferencedImages(referencedImage, referencedImages->at(i));
    }

    referencedImages = fiducialsSeries->getReferencedImages(1);
    CPPUNIT_ASSERT(referencedImages);
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), referencedImages->size());
    compareReferencedImages(referencedImage, referencedImages->at(0));

    // Test setReferencedImage method in a second fiducial set
    fiducialsSeries->setReferencedImage(1, 1, referencedImage);
    referencedImages = fiducialsSeries->getReferencedImages(0);
    CPPUNIT_ASSERT(referencedImages);
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), referencedImages->size());
    for(std::size_t i = 0 ; i < 3 ; i++)
    {
        compareReferencedImages(referencedImage, referencedImages->at(i));
    }

    referencedImages = fiducialsSeries->getReferencedImages(1);
    CPPUNIT_ASSERT(referencedImages);
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), referencedImages->size());
    for(std::size_t i = 0 ; i < 2 ; i++)
    {
        compareReferencedImages(referencedImage, referencedImages->at(i));
    }

    // Test appendReferencedImage method in a second fiducial set
    fiducialsSeries->appendReferencedImage(1, referencedImage);
    referencedImages = fiducialsSeries->getReferencedImages(0);
    CPPUNIT_ASSERT(referencedImages);
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), referencedImages->size());
    for(std::size_t i = 0 ; i < 3 ; i++)
    {
        compareReferencedImages(referencedImage, referencedImages->at(i));
    }

    referencedImages = fiducialsSeries->getReferencedImages(1);
    CPPUNIT_ASSERT(referencedImages);
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), referencedImages->size());
    for(std::size_t i = 0 ; i < 3 ; i++)
    {
        compareReferencedImages(referencedImage, referencedImages->at(i));
    }
}

//------------------------------------------------------------------------------

void FiducialsSeriesTest::fiducialSetterGetterTest()
{
    // This method checks that adding fiducials works correctly.

    auto fiducialsSeries = FiducialsSeries::New();
    CPPUNIT_ASSERT(fiducialsSeries->getFiducials(0).empty());

    FiducialsSeries::Fiducial fiducial;
    fiducial.shapeType           = FiducialsSeries::Shape::POINT;
    fiducial.fiducialDescription = "1";
    fiducial.fiducialIdentifier  = "2";

    FiducialsSeries::GraphicCoordinatesData graphicCoordinatesData;
    graphicCoordinatesData.referencedImageSequence.referencedSOPClassUID    = "3";
    graphicCoordinatesData.referencedImageSequence.referencedSOPInstanceUID = "4";
    graphicCoordinatesData.referencedImageSequence.referencedFrameNumber    = {5};
    graphicCoordinatesData.referencedImageSequence.referencedSegmentNumber  = {6};
    graphicCoordinatesData.graphicData                                      = {{7, 8}};
    fiducial.graphicCoordinatesDataSequence                                 = {graphicCoordinatesData};

    fiducial.fiducialUID = "9";
    fiducial.contourData = {{10, 11, 12}};

    // Test setFiducials method
    fiducialsSeries->setFiducials(0, {fiducial});
    std::vector<FiducialsSeries::Fiducial> fiducials = fiducialsSeries->getFiducials(0);
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), fiducials.size());
    compareFiducials(fiducial, fiducials[0]);

    // Test setFiducial method
    fiducialsSeries->setFiducial(0, 1, fiducial);
    fiducials = fiducialsSeries->getFiducials(0);
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), fiducials.size());
    for(std::size_t i = 0 ; i < 2 ; i++)
    {
        compareFiducials(fiducial, fiducials[i]);
    }

    // Test appendFiducial method
    fiducialsSeries->appendFiducial(0, fiducial);
    fiducials = fiducialsSeries->getFiducials(0);
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), fiducials.size());
    for(std::size_t i = 0 ; i < 3 ; i++)
    {
        compareFiducials(fiducial, fiducials[i]);
    }

    // Test setFiducials method in a second fiducial set
    fiducialsSeries->setFiducials(1, {fiducial});
    fiducials = fiducialsSeries->getFiducials(0);
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), fiducials.size());
    for(std::size_t i = 0 ; i < 3 ; i++)
    {
        compareFiducials(fiducial, fiducials[i]);
    }

    fiducials = fiducialsSeries->getFiducials(1);
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), fiducials.size());
    compareFiducials(fiducial, fiducials[0]);

    // Test setFiducial method in a second fiducial set
    fiducialsSeries->setFiducial(1, 1, fiducial);
    fiducials = fiducialsSeries->getFiducials(0);
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), fiducials.size());
    for(std::size_t i = 0 ; i < 3 ; i++)
    {
        compareFiducials(fiducial, fiducials[i]);
    }

    fiducials = fiducialsSeries->getFiducials(1);
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), fiducials.size());
    for(std::size_t i = 0 ; i < 2 ; i++)
    {
        compareFiducials(fiducial, fiducials[i]);
    }

    // Test appendFiducial method in a second fiducial set
    fiducialsSeries->appendFiducial(1, fiducial);
    fiducials = fiducialsSeries->getFiducials(0);
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), fiducials.size());
    for(std::size_t i = 0 ; i < 3 ; i++)
    {
        compareFiducials(fiducial, fiducials[i]);
    }

    fiducials = fiducialsSeries->getFiducials(1);
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), fiducials.size());
    for(std::size_t i = 0 ; i < 3 ; i++)
    {
        compareFiducials(fiducial, fiducials[i]);
    }
}

//------------------------------------------------------------------------------

void FiducialsSeriesTest::graphicCoordinatesDataSetterGetterTest()
{
    // This method checks that adding graphic coordinates data works correctly.

    auto fiducialsSeries = FiducialsSeries::New();
    CPPUNIT_ASSERT(!fiducialsSeries->getGraphicCoordinatesDataSequence(0, 0));

    FiducialsSeries::GraphicCoordinatesData graphicCoordinatesData;
    graphicCoordinatesData.referencedImageSequence.referencedSOPClassUID    = "1";
    graphicCoordinatesData.referencedImageSequence.referencedSOPInstanceUID = "2";
    graphicCoordinatesData.referencedImageSequence.referencedFrameNumber    = {3};
    graphicCoordinatesData.referencedImageSequence.referencedSegmentNumber  = {4};
    graphicCoordinatesData.graphicData                                      = {{5, 6}};

    // Test setGraphicCoordinatesDataSequence method
    fiducialsSeries->setGraphicCoordinatesDataSequence(0, 0, {{graphicCoordinatesData}});
    std::optional<std::vector<FiducialsSeries::GraphicCoordinatesData> > graphicCoordinatesDataSequence =
        fiducialsSeries->getGraphicCoordinatesDataSequence(0, 0);
    CPPUNIT_ASSERT(graphicCoordinatesDataSequence);
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), graphicCoordinatesDataSequence->size());
    compareGraphicCoordinatesData(graphicCoordinatesData, graphicCoordinatesDataSequence->at(0));

    // Test setGraphicCoordinatesData method
    fiducialsSeries->setGraphicCoordinatesData(0, 0, 1, graphicCoordinatesData);
    graphicCoordinatesDataSequence = fiducialsSeries->getGraphicCoordinatesDataSequence(0, 0);
    CPPUNIT_ASSERT(graphicCoordinatesDataSequence);
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), graphicCoordinatesDataSequence->size());
    for(std::size_t i = 0 ; i < 2 ; i++)
    {
        compareGraphicCoordinatesData(graphicCoordinatesData, graphicCoordinatesDataSequence->at(i));
    }

    // Test appendGraphicCoordinatesData method
    fiducialsSeries->appendGraphicCoordinatesData(0, 0, graphicCoordinatesData);
    graphicCoordinatesDataSequence = fiducialsSeries->getGraphicCoordinatesDataSequence(0, 0);
    CPPUNIT_ASSERT(graphicCoordinatesDataSequence);
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), graphicCoordinatesDataSequence->size());
    for(std::size_t i = 0 ; i < 3 ; i++)
    {
        compareGraphicCoordinatesData(graphicCoordinatesData, graphicCoordinatesDataSequence->at(i));
    }

    // Test setGraphicCoordinatesDataSequence method in a second fiducial
    fiducialsSeries->setGraphicCoordinatesDataSequence(0, 1, {{graphicCoordinatesData}});
    graphicCoordinatesDataSequence = fiducialsSeries->getGraphicCoordinatesDataSequence(0, 0);
    CPPUNIT_ASSERT(graphicCoordinatesDataSequence);
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), graphicCoordinatesDataSequence->size());
    for(std::size_t i = 0 ; i < 3 ; i++)
    {
        compareGraphicCoordinatesData(graphicCoordinatesData, graphicCoordinatesDataSequence->at(i));
    }

    graphicCoordinatesDataSequence = fiducialsSeries->getGraphicCoordinatesDataSequence(0, 1);
    CPPUNIT_ASSERT(graphicCoordinatesDataSequence);
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), graphicCoordinatesDataSequence->size());
    compareGraphicCoordinatesData(graphicCoordinatesData, graphicCoordinatesDataSequence->at(0));

    // Test setGraphicCoordinatesData method in a second fiducial
    fiducialsSeries->setGraphicCoordinatesData(0, 1, 1, graphicCoordinatesData);
    graphicCoordinatesDataSequence = fiducialsSeries->getGraphicCoordinatesDataSequence(0, 0);
    CPPUNIT_ASSERT(graphicCoordinatesDataSequence);
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), graphicCoordinatesDataSequence->size());
    for(std::size_t i = 0 ; i < 3 ; i++)
    {
        compareGraphicCoordinatesData(graphicCoordinatesData, graphicCoordinatesDataSequence->at(i));
    }

    graphicCoordinatesDataSequence = fiducialsSeries->getGraphicCoordinatesDataSequence(0, 1);
    CPPUNIT_ASSERT(graphicCoordinatesDataSequence);
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), graphicCoordinatesDataSequence->size());
    for(std::size_t i = 0 ; i < 2 ; i++)
    {
        compareGraphicCoordinatesData(graphicCoordinatesData, graphicCoordinatesDataSequence->at(i));
    }

    // Test appendGraphicCoordinatesData method in a second fiducial
    fiducialsSeries->appendGraphicCoordinatesData(0, 1, graphicCoordinatesData);
    graphicCoordinatesDataSequence = fiducialsSeries->getGraphicCoordinatesDataSequence(0, 0);
    CPPUNIT_ASSERT(graphicCoordinatesDataSequence);
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), graphicCoordinatesDataSequence->size());
    for(std::size_t i = 0 ; i < 3 ; i++)
    {
        compareGraphicCoordinatesData(graphicCoordinatesData, graphicCoordinatesDataSequence->at(i));
    }

    graphicCoordinatesDataSequence = fiducialsSeries->getGraphicCoordinatesDataSequence(0, 1);
    CPPUNIT_ASSERT(graphicCoordinatesDataSequence);
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), graphicCoordinatesDataSequence->size());
    for(std::size_t i = 0 ; i < 3 ; i++)
    {
        compareGraphicCoordinatesData(graphicCoordinatesData, graphicCoordinatesDataSequence->at(i));
    }
}

//------------------------------------------------------------------------------

} // namespace sight::data::ut
