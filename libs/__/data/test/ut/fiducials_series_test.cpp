/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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
    fs->set_content_label("TestLabel");
    fs->set_content_label("1");
    fs->set_content_description("FS");
    fs->set_content_creator_name("John Doe");

    // Adds a dummy fiducial set
    fiducials_series::fiducial_set fiducial_set;

    fiducials_series::referenced_image referenced_image;
    referenced_image.referenced_sop_class_uid    = "1";
    referenced_image.referenced_sop_instance_uid = "2";
    referenced_image.referenced_frame_number     = {3};
    referenced_image.referenced_segment_number   = {4};
    fiducial_set.referenced_image_sequence       = {referenced_image};

    fiducial_set.frame_of_reference_uid = "5";

    fiducials_series::fiducial fiducial;
    fiducial.shape_type           = fiducials_series::shape::point;
    fiducial.fiducial_description = "6";
    fiducial.fiducial_identifier  = "7";

    fiducials_series::graphic_coordinates_data graphic_coordinates_data;
    graphic_coordinates_data.referenced_image_sequence.referenced_sop_class_uid    = "8";
    graphic_coordinates_data.referenced_image_sequence.referenced_sop_instance_uid = "9";
    graphic_coordinates_data.referenced_image_sequence.referenced_frame_number     = {10};
    graphic_coordinates_data.referenced_image_sequence.referenced_segment_number   = {11};
    graphic_coordinates_data.graphic_data                                          = {{12, 13}};
    fiducial.graphic_coordinates_data_sequence                                     = {graphic_coordinates_data};

    fiducial.fiducial_uid = "14";
    fiducial.contour_data = {{15, 16, 17}};
    fiducial_set.fiducial_sequence.push_back(fiducial);

    fiducial_set.group_name = "18";
    fiducial_set.color      = {{19, 20, 21, 22}};
    fiducial_set.size       = 23.F;
    fiducial_set.shape      = fiducials_series::private_shape::cube;
    fiducial_set.visibility = true;

    // Test setFiducialSets method
    fs->set_fiducial_sets({fiducial_set});

    return fs;
}

//------------------------------------------------------------------------------

void fiducials_series_test::shallow_copy_test()
{
    auto fs1 = init_fiducials_series();
    auto fs2 = std::make_shared<data::fiducials_series>();

    CPPUNIT_ASSERT(*fs1 != *fs2);
    fs2->shallow_copy(fs1);
    CPPUNIT_ASSERT(*fs1 == *fs2);
}

//------------------------------------------------------------------------------

void fiducials_series_test::deep_copy_test()
{
    auto fs1 = init_fiducials_series();
    auto fs2 = std::make_shared<data::fiducials_series>();

    CPPUNIT_ASSERT(*fs1 != *fs2);
    fs2->deep_copy(fs1);
    CPPUNIT_ASSERT(*fs1 == *fs2);
}

//------------------------------------------------------------------------------

void fiducials_series_test::simple_setter_getter_test()
{
    // This method checks that plain method getters/setters work correctly.

    using namespace std::literals::string_literals;
    auto fiducialsSeries = std::make_shared<data::fiducials_series>();

    // NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
    #define COMPARE_GETSETTERS(attribute, before, after, ...) \
            CPPUNIT_ASSERT_EQUAL(before, fiducialsSeries->get_ ## attribute(__VA_ARGS__)); \
            fiducialsSeries->set_ ## attribute(__VA_ARGS__ __VA_OPT__( , ) after); \
            CPPUNIT_ASSERT_EQUAL(after, fiducialsSeries->get_ ## attribute(__VA_ARGS__)); \

    // NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
    #define COMPARE_GETSETTERS_DEMI_OPTIONAL(attribute, before, after, ...) \
            CPPUNIT_ASSERT(before == fiducialsSeries->get_ ## attribute(__VA_ARGS__)); \
            fiducialsSeries->set_ ## attribute(__VA_ARGS__ __VA_OPT__( , ) * after); \
            CPPUNIT_ASSERT(after == fiducialsSeries->get_ ## attribute(__VA_ARGS__)); \

    // NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
    #define COMPARE_GETSETTERS_ARRAYS(attribute, after, ...) \
            CPPUNIT_ASSERT(fiducialsSeries->get_ ## attribute(__VA_ARGS__).empty()); \
            fiducialsSeries->set_ ## attribute(__VA_ARGS__ __VA_OPT__( , ) after); \
            CPPUNIT_ASSERT(after == fiducialsSeries->get_ ## attribute(__VA_ARGS__)); \

    // NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
    #define COMPARE_GETSETTERS_OPTIONAL(attribute, before, after, ...) \
            CPPUNIT_ASSERT(before == fiducialsSeries->get_ ## attribute(__VA_ARGS__)); \
            fiducialsSeries->set_ ## attribute(__VA_ARGS__ __VA_OPT__( , ) after); \
            CPPUNIT_ASSERT(after == fiducialsSeries->get_ ## attribute(__VA_ARGS__)); \

    COMPARE_GETSETTERS(content_date, ""s, "0"s);
    COMPARE_GETSETTERS(content_label, ""s, "1"s);
    COMPARE_GETSETTERS(content_description, ""s, "2"s);
    COMPARE_GETSETTERS(content_creator_name, ""s, "3"s);

    CPPUNIT_ASSERT(fiducialsSeries->get_fiducial_sets().empty());
    CPPUNIT_ASSERT(!fiducialsSeries->get_referenced_images(0));

    COMPARE_GETSETTERS_DEMI_OPTIONAL(
        referenced_sop_class_uid,
        std::optional<std::string> {},
        std::optional {"4"s},
        0,
        0
    );
    COMPARE_GETSETTERS_DEMI_OPTIONAL(
        referenced_sop_instance_uid,
        std::optional<std::string> {},
        std::optional {"5"s},
        0,
        0
    );
    COMPARE_GETSETTERS_ARRAYS(referenced_frame_number, std::vector<std::int32_t> {6}, 0, 0);
    COMPARE_GETSETTERS_ARRAYS(referenced_segment_number, std::vector<std::uint16_t> {7}, 0, 0);

    COMPARE_GETSETTERS_OPTIONAL(frame_of_reference_uid, std::optional<std::string> {}, std::optional {"8"s}, 0);

    CPPUNIT_ASSERT(fiducialsSeries->get_fiducials(0).empty());

    COMPARE_GETSETTERS(shape_type, fiducials_series::shape::invalid, fiducials_series::shape::point, 0, 0);
    COMPARE_GETSETTERS(fiducial_description, ""s, "9"s, 0, 0);
    COMPARE_GETSETTERS(fiducial_identifier, ""s, "10"s, 0, 0);

    CPPUNIT_ASSERT(!fiducialsSeries->get_graphic_coordinates_data_sequence(0, 0).has_value());

    COMPARE_GETSETTERS(referenced_sop_class_uid, ""s, "11"s, 0, 0, 0);
    COMPARE_GETSETTERS(referenced_sop_instance_uid, ""s, "12"s, 0, 0, 0);
    COMPARE_GETSETTERS_ARRAYS(referenced_frame_number, std::vector<std::int32_t> {13}, 0, 0, 0);
    COMPARE_GETSETTERS_ARRAYS(referenced_segment_number, std::vector<std::uint16_t> {14}, 0, 0, 0);

    COMPARE_GETSETTERS_ARRAYS(graphic_data, (std::vector<fiducials_series::point2> {{15, 16}}), 0, 0, 0);

    COMPARE_GETSETTERS_OPTIONAL(fiducial_uid, std::optional<std::string> {}, std::optional {"17"s}, 0, 0);
    COMPARE_GETSETTERS_ARRAYS(contour_data, (std::vector<fiducials_series::point3> {{{18, 19, 20}}}), 0, 0);

    COMPARE_GETSETTERS_DEMI_OPTIONAL(group_name, std::optional<std::string> {}, std::optional {"21"s}, 0);
    COMPARE_GETSETTERS_DEMI_OPTIONAL(
        color,
        (std::optional<std::array<float, 4> > {}),
        (std::optional {std::array {22.F, 23.F, 24.F, 25.F}}),
        0
    );
    COMPARE_GETSETTERS_DEMI_OPTIONAL(size, std::optional<float> {}, std::optional {26.F}, 0);
    COMPARE_GETSETTERS_DEMI_OPTIONAL(
        shape,
        std::optional<fiducials_series::private_shape> {},
        std::optional {fiducials_series::private_shape::sphere},
        0
    );
    COMPARE_GETSETTERS_DEMI_OPTIONAL(visibility, std::optional<bool> {}, std::optional {true}, 0);
}

//------------------------------------------------------------------------------

static void compare_referenced_images(
    fiducials_series::referenced_image _expected,
    fiducials_series::referenced_image _actual
)
{
    CPPUNIT_ASSERT_EQUAL(_expected.referenced_sop_class_uid, _actual.referenced_sop_class_uid);
    CPPUNIT_ASSERT_EQUAL(_expected.referenced_sop_instance_uid, _actual.referenced_sop_instance_uid);
    CPPUNIT_ASSERT(_expected.referenced_frame_number == _actual.referenced_frame_number);
    CPPUNIT_ASSERT(_expected.referenced_segment_number == _actual.referenced_segment_number);
}

//------------------------------------------------------------------------------

static void compare_graphic_coordinates_data(
    fiducials_series::graphic_coordinates_data _expected,
    fiducials_series::graphic_coordinates_data _actual
)
{
    compare_referenced_images(_expected.referenced_image_sequence, _actual.referenced_image_sequence);
    CPPUNIT_ASSERT(_expected.graphic_data == _actual.graphic_data);
}

//------------------------------------------------------------------------------

static void compare_fiducials(fiducials_series::fiducial _expected, fiducials_series::fiducial _actual)
{
    CPPUNIT_ASSERT_EQUAL(_expected.shape_type, _actual.shape_type);
    CPPUNIT_ASSERT_EQUAL(_expected.fiducial_description, _actual.fiducial_description);
    CPPUNIT_ASSERT_EQUAL(_expected.fiducial_identifier, _actual.fiducial_identifier);

    CPPUNIT_ASSERT_EQUAL(
        _expected.graphic_coordinates_data_sequence.has_value(),
        _actual.graphic_coordinates_data_sequence.has_value()
    );
    if(_expected.graphic_coordinates_data_sequence.has_value())
    {
        CPPUNIT_ASSERT_EQUAL(
            _expected.graphic_coordinates_data_sequence->size(),
            _actual.graphic_coordinates_data_sequence->size()
        );
        for(std::size_t i = 0 ; i < _expected.graphic_coordinates_data_sequence->size() ; i++)
        {
            compare_graphic_coordinates_data(
                _expected.graphic_coordinates_data_sequence->at(i),
                _actual.graphic_coordinates_data_sequence->at(i)
            );
        }
    }

    CPPUNIT_ASSERT(_expected.fiducial_uid == _actual.fiducial_uid);
    CPPUNIT_ASSERT(_expected.contour_data == _actual.contour_data);
}

//------------------------------------------------------------------------------

static void compare_fiducial_sets(fiducials_series::fiducial_set _expected, fiducials_series::fiducial_set _actual)
{
    CPPUNIT_ASSERT_EQUAL(
        _expected.referenced_image_sequence.has_value(),
        _actual.referenced_image_sequence.has_value()
    );
    if(_expected.referenced_image_sequence.has_value())
    {
        CPPUNIT_ASSERT_EQUAL(_expected.referenced_image_sequence->size(), _actual.referenced_image_sequence->size());
        for(std::size_t i = 0 ; i < _expected.referenced_image_sequence->size() ; i++)
        {
            compare_referenced_images(
                _expected.referenced_image_sequence->at(i),
                _actual.referenced_image_sequence->at(i)
            );
        }
    }

    CPPUNIT_ASSERT(_expected.frame_of_reference_uid == _actual.frame_of_reference_uid);

    CPPUNIT_ASSERT_EQUAL(_expected.fiducial_sequence.size(), _actual.fiducial_sequence.size());
    for(std::size_t i = 0 ; i < _expected.fiducial_sequence.size() ; i++)
    {
        compare_fiducials(_expected.fiducial_sequence[i], _actual.fiducial_sequence[i]);
    }

    CPPUNIT_ASSERT(_expected.group_name == _actual.group_name);
    CPPUNIT_ASSERT(_expected.color == _actual.color);
    CPPUNIT_ASSERT(_expected.size == _actual.size);
    CPPUNIT_ASSERT(_expected.shape == _actual.shape);
}

//------------------------------------------------------------------------------

void fiducials_series_test::fiducial_set_setter_getter_test()
{
    // This method checks that adding fiducial sets works correctly.

    auto fiducials_series = std::make_shared<data::fiducials_series>();
    CPPUNIT_ASSERT(fiducials_series->get_fiducial_sets().empty());

    fiducials_series::fiducial_set fiducial_set;

    fiducials_series::referenced_image referenced_image;
    referenced_image.referenced_sop_class_uid    = "1";
    referenced_image.referenced_sop_instance_uid = "2";
    referenced_image.referenced_frame_number     = {3};
    referenced_image.referenced_segment_number   = {4};
    fiducial_set.referenced_image_sequence       = {referenced_image};

    fiducial_set.frame_of_reference_uid = "5";

    fiducials_series::fiducial fiducial;
    fiducial.shape_type           = fiducials_series::shape::point;
    fiducial.fiducial_description = "6";
    fiducial.fiducial_identifier  = "7";

    fiducials_series::graphic_coordinates_data graphic_coordinates_data;
    graphic_coordinates_data.referenced_image_sequence.referenced_sop_class_uid    = "8";
    graphic_coordinates_data.referenced_image_sequence.referenced_sop_instance_uid = "9";
    graphic_coordinates_data.referenced_image_sequence.referenced_frame_number     = {10};
    graphic_coordinates_data.referenced_image_sequence.referenced_segment_number   = {11};
    graphic_coordinates_data.graphic_data                                          = {{12, 13}};
    fiducial.graphic_coordinates_data_sequence                                     = {graphic_coordinates_data};

    fiducial.fiducial_uid = "14";
    fiducial.contour_data = {{15, 16, 17}};
    fiducial_set.fiducial_sequence.push_back(fiducial);

    fiducial_set.group_name = "18";
    fiducial_set.color      = {{19, 20, 21, 22}};
    fiducial_set.size       = 23.F;
    fiducial_set.shape      = fiducials_series::private_shape::cube;
    fiducial_set.visibility = true;

    // Test setFiducialSets method
    fiducials_series->set_fiducial_sets({fiducial_set});
    std::vector<fiducials_series::fiducial_set> fiducial_sets = fiducials_series->get_fiducial_sets();
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), fiducial_sets.size());
    compare_fiducial_sets(fiducial_set, fiducial_sets[0]);

    // Test setFiducialSet method
    fiducials_series->set_fiducial_set(1, fiducial_set);
    fiducial_sets = fiducials_series->get_fiducial_sets();
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), fiducial_sets.size());
    for(std::size_t i = 0 ; i < 2 ; i++)
    {
        compare_fiducial_sets(fiducial_set, fiducial_sets[i]);
    }

    // Test appendFiducialSet
    fiducials_series->append_fiducial_set(fiducial_set);
    fiducial_sets = fiducials_series->get_fiducial_sets();
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), fiducial_sets.size());
    for(std::size_t i = 0 ; i < 3 ; i++)
    {
        compare_fiducial_sets(fiducial_set, fiducial_sets[i]);
    }
}

//------------------------------------------------------------------------------

void fiducials_series_test::referenced_image_setter_getter_test()
{
    // This method checks that adding referenced images works correctly.

    auto fiducials_series = std::make_shared<data::fiducials_series>();
    CPPUNIT_ASSERT(!fiducials_series->get_referenced_images(0));

    fiducials_series::referenced_image referenced_image;
    referenced_image.referenced_sop_class_uid    = "1";
    referenced_image.referenced_sop_instance_uid = "2";
    referenced_image.referenced_frame_number     = {3};
    referenced_image.referenced_segment_number   = {4};

    // Test setReferencedImages method
    fiducials_series->set_referenced_images(0, {{referenced_image}});
    std::optional<std::vector<fiducials_series::referenced_image> > referenced_images =
        fiducials_series->get_referenced_images(0);
    CPPUNIT_ASSERT(referenced_images);
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), referenced_images->size());
    compare_referenced_images(referenced_image, referenced_images->at(0));

    // Test setReferencedImage method
    fiducials_series->set_referenced_image(0, 1, referenced_image);
    referenced_images = fiducials_series->get_referenced_images(0);
    CPPUNIT_ASSERT(referenced_images);
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), referenced_images->size());
    for(std::size_t i = 0 ; i < 2 ; i++)
    {
        compare_referenced_images(referenced_image, referenced_images->at(i));
    }

    // Test appendReferencedImage method
    fiducials_series->append_referenced_image(0, referenced_image);
    referenced_images = fiducials_series->get_referenced_images(0);
    CPPUNIT_ASSERT(referenced_images);
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), referenced_images->size());
    for(std::size_t i = 0 ; i < 3 ; i++)
    {
        compare_referenced_images(referenced_image, referenced_images->at(i));
    }

    // Test setReferencedImages method in a second fiducial set
    fiducials_series->set_referenced_images(1, {{referenced_image}});
    referenced_images = fiducials_series->get_referenced_images(0);
    CPPUNIT_ASSERT(referenced_images);
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), referenced_images->size());
    for(std::size_t i = 0 ; i < 3 ; i++)
    {
        compare_referenced_images(referenced_image, referenced_images->at(i));
    }

    referenced_images = fiducials_series->get_referenced_images(1);
    CPPUNIT_ASSERT(referenced_images);
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), referenced_images->size());
    compare_referenced_images(referenced_image, referenced_images->at(0));

    // Test setReferencedImage method in a second fiducial set
    fiducials_series->set_referenced_image(1, 1, referenced_image);
    referenced_images = fiducials_series->get_referenced_images(0);
    CPPUNIT_ASSERT(referenced_images);
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), referenced_images->size());
    for(std::size_t i = 0 ; i < 3 ; i++)
    {
        compare_referenced_images(referenced_image, referenced_images->at(i));
    }

    referenced_images = fiducials_series->get_referenced_images(1);
    CPPUNIT_ASSERT(referenced_images);
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), referenced_images->size());
    for(std::size_t i = 0 ; i < 2 ; i++)
    {
        compare_referenced_images(referenced_image, referenced_images->at(i));
    }

    // Test appendReferencedImage method in a second fiducial set
    fiducials_series->append_referenced_image(1, referenced_image);
    referenced_images = fiducials_series->get_referenced_images(0);
    CPPUNIT_ASSERT(referenced_images);
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), referenced_images->size());
    for(std::size_t i = 0 ; i < 3 ; i++)
    {
        compare_referenced_images(referenced_image, referenced_images->at(i));
    }

    referenced_images = fiducials_series->get_referenced_images(1);
    CPPUNIT_ASSERT(referenced_images);
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), referenced_images->size());
    for(std::size_t i = 0 ; i < 3 ; i++)
    {
        compare_referenced_images(referenced_image, referenced_images->at(i));
    }
}

//------------------------------------------------------------------------------

void fiducials_series_test::fiducial_setter_getter_test()
{
    // This method checks that adding fiducials works correctly.

    auto fiducials_series = std::make_shared<data::fiducials_series>();
    CPPUNIT_ASSERT(fiducials_series->get_fiducials(0).empty());

    fiducials_series::fiducial fiducial;
    fiducial.shape_type           = fiducials_series::shape::point;
    fiducial.fiducial_description = "1";
    fiducial.fiducial_identifier  = "2";

    fiducials_series::graphic_coordinates_data graphic_coordinates_data;
    graphic_coordinates_data.referenced_image_sequence.referenced_sop_class_uid    = "3";
    graphic_coordinates_data.referenced_image_sequence.referenced_sop_instance_uid = "4";
    graphic_coordinates_data.referenced_image_sequence.referenced_frame_number     = {5};
    graphic_coordinates_data.referenced_image_sequence.referenced_segment_number   = {6};
    graphic_coordinates_data.graphic_data                                          = {{7, 8}};
    fiducial.graphic_coordinates_data_sequence                                     = {graphic_coordinates_data};

    fiducial.fiducial_uid = "9";
    fiducial.contour_data = {{10, 11, 12}};

    // Test setFiducials method
    fiducials_series->set_fiducials(0, {fiducial});
    std::vector<fiducials_series::fiducial> fiducials = fiducials_series->get_fiducials(0);
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), fiducials.size());
    compare_fiducials(fiducial, fiducials[0]);

    // Test setFiducial method
    fiducials_series->set_fiducial(0, 1, fiducial);
    fiducials = fiducials_series->get_fiducials(0);
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), fiducials.size());
    for(std::size_t i = 0 ; i < 2 ; i++)
    {
        compare_fiducials(fiducial, fiducials[i]);
    }

    // Test appendFiducial method
    fiducials_series->append_fiducial(0, fiducial);
    fiducials = fiducials_series->get_fiducials(0);
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), fiducials.size());
    for(std::size_t i = 0 ; i < 3 ; i++)
    {
        compare_fiducials(fiducial, fiducials[i]);
    }

    // Test setFiducials method in a second fiducial set
    fiducials_series->set_fiducials(1, {fiducial});
    fiducials = fiducials_series->get_fiducials(0);
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), fiducials.size());
    for(std::size_t i = 0 ; i < 3 ; i++)
    {
        compare_fiducials(fiducial, fiducials[i]);
    }

    fiducials = fiducials_series->get_fiducials(1);
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), fiducials.size());
    compare_fiducials(fiducial, fiducials[0]);

    // Test setFiducial method in a second fiducial set
    fiducials_series->set_fiducial(1, 1, fiducial);
    fiducials = fiducials_series->get_fiducials(0);
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), fiducials.size());
    for(std::size_t i = 0 ; i < 3 ; i++)
    {
        compare_fiducials(fiducial, fiducials[i]);
    }

    fiducials = fiducials_series->get_fiducials(1);
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), fiducials.size());
    for(std::size_t i = 0 ; i < 2 ; i++)
    {
        compare_fiducials(fiducial, fiducials[i]);
    }

    // Test appendFiducial method in a second fiducial set
    fiducials_series->append_fiducial(1, fiducial);
    fiducials = fiducials_series->get_fiducials(0);
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), fiducials.size());
    for(std::size_t i = 0 ; i < 3 ; i++)
    {
        compare_fiducials(fiducial, fiducials[i]);
    }

    fiducials = fiducials_series->get_fiducials(1);
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), fiducials.size());
    for(std::size_t i = 0 ; i < 3 ; i++)
    {
        compare_fiducials(fiducial, fiducials[i]);
    }
}

//------------------------------------------------------------------------------

void fiducials_series_test::graphic_coordinates_data_setter_getter_test()
{
    // This method checks that adding graphic coordinates data works correctly.

    auto fiducials_series = std::make_shared<data::fiducials_series>();
    CPPUNIT_ASSERT(!fiducials_series->get_graphic_coordinates_data_sequence(0, 0));

    fiducials_series::graphic_coordinates_data graphic_coordinates_data;
    graphic_coordinates_data.referenced_image_sequence.referenced_sop_class_uid    = "1";
    graphic_coordinates_data.referenced_image_sequence.referenced_sop_instance_uid = "2";
    graphic_coordinates_data.referenced_image_sequence.referenced_frame_number     = {3};
    graphic_coordinates_data.referenced_image_sequence.referenced_segment_number   = {4};
    graphic_coordinates_data.graphic_data                                          = {{5, 6}};

    // Test setGraphicCoordinatesDataSequence method
    fiducials_series->set_graphic_coordinates_data_sequence(0, 0, {{graphic_coordinates_data}});
    std::optional<std::vector<fiducials_series::graphic_coordinates_data> > graphic_coordinates_data_sequence =
        fiducials_series->get_graphic_coordinates_data_sequence(0, 0);
    CPPUNIT_ASSERT(graphic_coordinates_data_sequence);
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), graphic_coordinates_data_sequence->size());
    compare_graphic_coordinates_data(graphic_coordinates_data, graphic_coordinates_data_sequence->at(0));

    // Test setGraphicCoordinatesData method
    fiducials_series->set_graphic_coordinates_data(0, 0, 1, graphic_coordinates_data);
    graphic_coordinates_data_sequence = fiducials_series->get_graphic_coordinates_data_sequence(0, 0);
    CPPUNIT_ASSERT(graphic_coordinates_data_sequence);
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), graphic_coordinates_data_sequence->size());
    for(std::size_t i = 0 ; i < 2 ; i++)
    {
        compare_graphic_coordinates_data(graphic_coordinates_data, graphic_coordinates_data_sequence->at(i));
    }

    // Test appendGraphicCoordinatesData method
    fiducials_series->append_graphic_coordinates_data(0, 0, graphic_coordinates_data);
    graphic_coordinates_data_sequence = fiducials_series->get_graphic_coordinates_data_sequence(0, 0);
    CPPUNIT_ASSERT(graphic_coordinates_data_sequence);
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), graphic_coordinates_data_sequence->size());
    for(std::size_t i = 0 ; i < 3 ; i++)
    {
        compare_graphic_coordinates_data(graphic_coordinates_data, graphic_coordinates_data_sequence->at(i));
    }

    // Test setGraphicCoordinatesDataSequence method in a second fiducial
    fiducials_series->set_graphic_coordinates_data_sequence(0, 1, {{graphic_coordinates_data}});
    graphic_coordinates_data_sequence = fiducials_series->get_graphic_coordinates_data_sequence(0, 0);
    CPPUNIT_ASSERT(graphic_coordinates_data_sequence);
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), graphic_coordinates_data_sequence->size());
    for(std::size_t i = 0 ; i < 3 ; i++)
    {
        compare_graphic_coordinates_data(graphic_coordinates_data, graphic_coordinates_data_sequence->at(i));
    }

    graphic_coordinates_data_sequence = fiducials_series->get_graphic_coordinates_data_sequence(0, 1);
    CPPUNIT_ASSERT(graphic_coordinates_data_sequence);
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), graphic_coordinates_data_sequence->size());
    compare_graphic_coordinates_data(graphic_coordinates_data, graphic_coordinates_data_sequence->at(0));

    // Test setGraphicCoordinatesData method in a second fiducial
    fiducials_series->set_graphic_coordinates_data(0, 1, 1, graphic_coordinates_data);
    graphic_coordinates_data_sequence = fiducials_series->get_graphic_coordinates_data_sequence(0, 0);
    CPPUNIT_ASSERT(graphic_coordinates_data_sequence);
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), graphic_coordinates_data_sequence->size());
    for(std::size_t i = 0 ; i < 3 ; i++)
    {
        compare_graphic_coordinates_data(graphic_coordinates_data, graphic_coordinates_data_sequence->at(i));
    }

    graphic_coordinates_data_sequence = fiducials_series->get_graphic_coordinates_data_sequence(0, 1);
    CPPUNIT_ASSERT(graphic_coordinates_data_sequence);
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), graphic_coordinates_data_sequence->size());
    for(std::size_t i = 0 ; i < 2 ; i++)
    {
        compare_graphic_coordinates_data(graphic_coordinates_data, graphic_coordinates_data_sequence->at(i));
    }

    // Test appendGraphicCoordinatesData method in a second fiducial
    fiducials_series->append_graphic_coordinates_data(0, 1, graphic_coordinates_data);
    graphic_coordinates_data_sequence = fiducials_series->get_graphic_coordinates_data_sequence(0, 0);
    CPPUNIT_ASSERT(graphic_coordinates_data_sequence);
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), graphic_coordinates_data_sequence->size());
    for(std::size_t i = 0 ; i < 3 ; i++)
    {
        compare_graphic_coordinates_data(graphic_coordinates_data, graphic_coordinates_data_sequence->at(i));
    }

    graphic_coordinates_data_sequence = fiducials_series->get_graphic_coordinates_data_sequence(0, 1);
    CPPUNIT_ASSERT(graphic_coordinates_data_sequence);
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), graphic_coordinates_data_sequence->size());
    for(std::size_t i = 0 ; i < 3 ; i++)
    {
        compare_graphic_coordinates_data(graphic_coordinates_data, graphic_coordinates_data_sequence->at(i));
    }
}

//------------------------------------------------------------------------------

} // namespace sight::data::ut
