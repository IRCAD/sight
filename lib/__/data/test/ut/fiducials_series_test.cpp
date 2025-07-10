/************************************************************************
 *
 * Copyright (C) 2024-2025 IRCAD France
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

#include <core/tools/uuid.hpp>

#include <data/fiducials_series.hpp>

#include <gdcmDataSet.h>
#include <gdcmExplicitDataElement.h>
#include <gdcmTagKeywords.h>
#include <gdcmTagToVR.h>
#include <gdcmUIDs.h>
#include <TestAssert.h>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::fiducials_series_test);

namespace sight::data::ut
{

namespace
{

//------------------------------------------------------------------------------

const std::array TEST_VECTOR {
    data::fiducials_series::query_result {
        .m_fiducial_set_index   = 0,
        .m_fiducial_index       = 0,
        .m_shape_index          = 0,
        .m_group_name           = "group_0",
        .m_visible              = true,
        .m_size                 = 16.F,
        .m_private_shape        = data::fiducials_series::private_shape::cube,
        .m_color                = std::array {0.21F, 0.22F, 0.23F, 0.24F},
        .m_shape                = data::fiducials_series::shape::point,
        .m_contour_data         = std::vector {32.0, 33.0, 34.0},
        .m_fiducial_description = core::tools::uuid::generate(),
        .m_fiducial_identifier  = core::tools::uuid::generate(),
        .m_fiducial_uid         = core::tools::uuid::generate()
    },
    data::fiducials_series::query_result {
        .m_fiducial_set_index   = 1,
        .m_fiducial_index       = 0,
        .m_shape_index          = 0,
        .m_group_name           = "group_1",
        .m_visible              = false,
        .m_size                 = 32.F,
        .m_private_shape        = data::fiducials_series::private_shape::sphere,
        .m_color                = std::array {0.13F, 0.14F, 0.15F, 0.16F},
        .m_shape                = data::fiducials_series::shape::point,
        .m_contour_data         = std::vector {23.0, 24.0, 25.0},
        .m_fiducial_description = core::tools::uuid::generate(),
        .m_fiducial_identifier  = core::tools::uuid::generate(),
        .m_fiducial_uid         = core::tools::uuid::generate()
    },
    data::fiducials_series::query_result {
        .m_fiducial_set_index   = 1,
        .m_fiducial_index       = 1,
        .m_shape_index          = 1,
        .m_group_name           = "group_1",
        .m_visible              = false,
        .m_size                 = 32.F,
        .m_private_shape        = data::fiducials_series::private_shape::sphere,
        .m_color                = std::array {0.13F, 0.14F, 0.15F, 0.16F},
        .m_shape                = data::fiducials_series::shape::point,
        .m_contour_data         = std::vector {26.0, 27.0, 28.0, 29.0, 30.0, 31.0},
        .m_fiducial_description = core::tools::uuid::generate(),
        .m_fiducial_identifier  = core::tools::uuid::generate(),
        .m_fiducial_uid         = core::tools::uuid::generate()
    },
    data::fiducials_series::query_result {
        .m_fiducial_set_index   = 2,
        .m_fiducial_index       = 0,
        .m_shape_index          = 0,
        .m_group_name           = "group_2",
        .m_visible              = true,
        .m_size                 = 16.F,
        .m_private_shape        = data::fiducials_series::private_shape::cube,
        .m_color                = std::array {0.1F, 0.2F, 0.3F, 0.4F},
        .m_shape                = data::fiducials_series::shape::point,
        .m_contour_data         = std::vector {5.0, 6.0, 7.0},
        .m_fiducial_description = core::tools::uuid::generate(),
        .m_fiducial_identifier  = core::tools::uuid::generate(),
        .m_fiducial_uid         = core::tools::uuid::generate()
    },
    data::fiducials_series::query_result {
        .m_fiducial_set_index   = 2,
        .m_fiducial_index       = 1,
        .m_shape_index          = 0,
        .m_group_name           = "group_2",
        .m_visible              = true,
        .m_size                 = 16.F,
        .m_private_shape        = data::fiducials_series::private_shape::cube,
        .m_color                = std::array {0.1F, 0.2F, 0.3F, 0.4F},
        .m_shape                = data::fiducials_series::shape::plane,
        .m_contour_data         = std::vector {8.0, 9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0},
        .m_fiducial_description = core::tools::uuid::generate(),
        .m_fiducial_identifier  = core::tools::uuid::generate(),
        .m_fiducial_uid         = core::tools::uuid::generate()
    },
    data::fiducials_series::query_result {
        .m_fiducial_set_index   = 2,
        .m_fiducial_index       = 2,
        .m_shape_index          = 0,
        .m_group_name           = "group_2",
        .m_visible              = true,
        .m_size                 = 16.F,
        .m_private_shape        = data::fiducials_series::private_shape::cube,
        .m_color                = std::array {0.1F, 0.2F, 0.3F, 0.4F},
        .m_shape                = data::fiducials_series::shape::shape,
        .m_contour_data         = std::vector {17.0, 18.0, 19.0, 20.0, 21.0, 22.0},
        .m_fiducial_description = core::tools::uuid::generate(),
        .m_fiducial_identifier  = core::tools::uuid::generate(),
        .m_fiducial_uid         = core::tools::uuid::generate()
    },
    data::fiducials_series::query_result {
        .m_fiducial_set_index   = 2,
        .m_fiducial_index       = 3,
        .m_shape_index          = 1,
        .m_group_name           = "group_2",
        .m_visible              = true,
        .m_size                 = 16.F,
        .m_private_shape        = data::fiducials_series::private_shape::cube,
        .m_color                = std::array {0.1F, 0.2F, 0.3F, 0.4F},
        .m_shape                = data::fiducials_series::shape::point,
        .m_contour_data         = std::vector {8.0, 9.0, 10.0},
        .m_fiducial_description = core::tools::uuid::generate(),
        .m_fiducial_identifier  = core::tools::uuid::generate(),
        .m_fiducial_uid         = core::tools::uuid::generate()
    },
};

//------------------------------------------------------------------------------

void check_fiducial_with_gdcm(
    const gdcm::DataSet& _fiducial_set_dataset,
    const gdcm::DataSet& _fiducial_dataset,
    const std::optional<std::string>& _group_name,
    const std::optional<std::array<float, 4> >& _color,
    const std::optional<float>& _size,
    const std::optional<data::fiducials_series::private_shape>& _private_shape,
    const std::optional<bool>& _visible,
    const std::optional<std::vector<double> >& _contour_data,
    const std::optional<std::string>& _fiducial_description,
    const std::optional<std::string>& _fiducial_identifier,
    const std::optional<std::string>& _fiducial_uid
)
{
    // Check attributes values in the fiducial set dataset
    static constexpr std::uint16_t s_PRIVATE_GROUP {0x0099};
    static constexpr std::uint16_t s_PRIVATE_DATA_ELEMENT {0x9910};

    static constexpr std::uint16_t s_GROUP_NAME_ELEMENT {0};
    static constexpr std::uint16_t s_COLOR_ELEMENT {1};
    static constexpr std::uint16_t s_SIZE_ELEMENT {2};
    static constexpr std::uint16_t s_PRIVATE_SHAPE_ELEMENT {3};
    static constexpr std::uint16_t s_VISIBLE_ELEMENT {4};

    // Group name
    if(_group_name)
    {
        const auto* group_name_value = _fiducial_set_dataset.GetDataElement(
            gdcm::Tag(s_PRIVATE_GROUP, s_PRIVATE_DATA_ELEMENT + s_GROUP_NAME_ELEMENT)
        ).GetByteValue();

        const auto& actual_group_name = gdcm::String<>(
            group_name_value->GetPointer(),
            group_name_value->GetLength()
        ).Trim();

        CPPUNIT_ASSERT_EQUAL(*_group_name, actual_group_name);
    }

    // Color
    if(_color)
    {
        const auto* color_value = _fiducial_set_dataset.GetDataElement(
            gdcm::Tag(s_PRIVATE_GROUP, s_PRIVATE_DATA_ELEMENT + s_COLOR_ELEMENT)
        ).GetByteValue();

        const auto& actual_color = gdcm::String<>(
            color_value->GetPointer(),
            color_value->GetLength()
        ).Trim();

        std::stringstream expected_stream;
        expected_stream << (*_color)[0] << "," << (*_color)[1] << "," << (*_color)[2] << "," << (*_color)[3];

        CPPUNIT_ASSERT_EQUAL(expected_stream.str(), actual_color);
    }

    // Size
    if(_size)
    {
        const auto* size_value = _fiducial_set_dataset.GetDataElement(
            gdcm::Tag(s_PRIVATE_GROUP, s_PRIVATE_DATA_ELEMENT + s_SIZE_ELEMENT)
        ).GetByteValue();

        const auto& actual_size = gdcm::String<>(
            size_value->GetPointer(),
            size_value->GetLength()
        ).Trim();

        CPPUNIT_ASSERT_EQUAL(*_size, std::stof(actual_size));
    }

    // Private shape
    if(_private_shape)
    {
        const auto* private_shape_value = _fiducial_set_dataset.GetDataElement(
            gdcm::Tag(s_PRIVATE_GROUP, s_PRIVATE_DATA_ELEMENT + s_PRIVATE_SHAPE_ELEMENT)
        ).GetByteValue();

        const auto& actual_private_shape = gdcm::String<>(
            private_shape_value->GetPointer(),
            private_shape_value->GetLength()
        ).Trim();

        CPPUNIT_ASSERT_EQUAL(
            std::string(*_private_shape == data::fiducials_series::private_shape::cube ? "CUBE" : "SPHERE"),
            actual_private_shape
        );
    }

    // Visible
    if(_visible)
    {
        const auto* visible_value = _fiducial_set_dataset.GetDataElement(
            gdcm::Tag(s_PRIVATE_GROUP, s_PRIVATE_DATA_ELEMENT + s_VISIBLE_ELEMENT)
        ).GetByteValue();

        const auto& actual_visible = gdcm::String<>(
            visible_value->GetPointer(),
            visible_value->GetLength()
        ).Trim();

        CPPUNIT_ASSERT_EQUAL(std::string(*_visible ? "true" : "false"), actual_visible);
    }

    // Check attributes values in the fiducial dataset
    // contour_data
    if(_contour_data)
    {
        gdcm::Keywords::ContourData contour_data_attribute;
        contour_data_attribute.SetFromDataSet(_fiducial_dataset);

        const std::vector<double> actual_contour_data(
            contour_data_attribute.GetValues(),
            contour_data_attribute.GetValues() + contour_data_attribute.GetNumberOfValues()
        );

        std::stringstream expected_stream;
        expected_stream << (*_contour_data)[0] << "," << (*_contour_data)[1] << "," << (*_contour_data)[2];

        std::stringstream actual_stream;
        actual_stream << actual_contour_data[0] << "," << actual_contour_data[1] << "," << actual_contour_data[2];

        CPPUNIT_ASSERT_EQUAL(expected_stream.str(), actual_stream.str());
    }

    // fiducial_description
    if(_fiducial_description)
    {
        gdcm::Keywords::FiducialDescription fiducial_description_attribute;
        fiducial_description_attribute.SetFromDataSet(_fiducial_dataset);
        const auto actual_fiducial_description = fiducial_description_attribute.GetValue().Trim();
        CPPUNIT_ASSERT_EQUAL(*_fiducial_description, actual_fiducial_description);
    }

    // fiducial_identifier
    if(_fiducial_identifier)
    {
        gdcm::Keywords::FiducialIdentifier fiducial_identifier_attribute;
        fiducial_identifier_attribute.SetFromDataSet(_fiducial_dataset);
        const auto actual_fiducial_identifier = fiducial_identifier_attribute.GetValue().Trim();
        CPPUNIT_ASSERT_EQUAL(*_fiducial_identifier, actual_fiducial_identifier);
    }

    // fiducial_uid
    if(_fiducial_uid)
    {
        gdcm::Keywords::FiducialUID fiducial_uid_attribute;
        fiducial_uid_attribute.SetFromDataSet(_fiducial_dataset);
        const auto actual_fiducial_uid = fiducial_uid_attribute.GetValue().Trim();
        CPPUNIT_ASSERT_EQUAL(*_fiducial_uid, actual_fiducial_uid);
    }
}

//------------------------------------------------------------------------------

void compare_queries(
    const data::fiducials_series::query_result& _expected,
    const data::fiducials_series::query_result& _actual
)
{
    CPPUNIT_ASSERT_EQUAL(_expected.m_fiducial_set_index, _actual.m_fiducial_set_index);
    CPPUNIT_ASSERT_EQUAL(_expected.m_fiducial_index, _actual.m_fiducial_index);
    CPPUNIT_ASSERT_EQUAL(_expected.m_shape_index, _actual.m_shape_index);
    CPPUNIT_ASSERT_EQUAL(*_expected.m_group_name, *_actual.m_group_name);
    CPPUNIT_ASSERT_EQUAL(*_expected.m_visible, *_actual.m_visible);
    CPPUNIT_ASSERT_EQUAL(*_expected.m_size, *_actual.m_size);
    CPPUNIT_ASSERT_EQUAL(*_expected.m_private_shape, *_actual.m_private_shape);
    CPPUNIT_ASSERT(core::is_equal(*_expected.m_color, *_actual.m_color));
    CPPUNIT_ASSERT_EQUAL(*_expected.m_shape, *_actual.m_shape);
    CPPUNIT_ASSERT(core::is_equal(*_expected.m_contour_data, *_actual.m_contour_data));
    CPPUNIT_ASSERT_EQUAL(*_expected.m_fiducial_description, *_actual.m_fiducial_description);
    CPPUNIT_ASSERT_EQUAL(*_expected.m_fiducial_identifier, *_actual.m_fiducial_identifier);
    CPPUNIT_ASSERT_EQUAL(*_expected.m_fiducial_uid, *_actual.m_fiducial_uid);
}

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

} // namespace

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

void fiducials_series_test::static_test()
{
    CPPUNIT_ASSERT_EQUAL(
        sight::data::fiducials_series::shape_to_string(
            sight::data::fiducials_series::shape::point
        ),
        std::string("POINT")
    );
    CPPUNIT_ASSERT_EQUAL(
        sight::data::fiducials_series::shape_to_string(
            sight::data::fiducials_series::shape::line
        ),
        std::string("LINE")
    );
    CPPUNIT_ASSERT_EQUAL(
        sight::data::fiducials_series::shape_to_string(
            sight::data::fiducials_series::shape::surface
        ),
        std::string("SURFACE")
    );
    CPPUNIT_ASSERT_EQUAL(
        sight::data::fiducials_series::shape_to_string(
            sight::data::fiducials_series::shape::l_shape
        ),
        std::string("L_SHAPE")
    );
    CPPUNIT_ASSERT_EQUAL(
        sight::data::fiducials_series::shape_to_string(
            sight::data::fiducials_series::shape::t_shape
        ),
        std::string("T_SHAPE")
    );
    CPPUNIT_ASSERT(
        sight::data::fiducials_series::shape_to_string(
            sight::data::fiducials_series::shape::invalid
        ).empty()
    );
}

//------------------------------------------------------------------------------

void fiducials_series_test::bounding_box_test()
{
    sight::data::fiducials_series::fiducial fiducial;

    {
        auto [min, max] = fiducial.contour_data_bounding_box();
        CPPUNIT_ASSERT_DOUBLES_EQUAL(min.x, std::numeric_limits<double>::max(), 1e-3);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(min.y, std::numeric_limits<double>::max(), 1e-3);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(min.z, std::numeric_limits<double>::max(), 1e-3);

        CPPUNIT_ASSERT_DOUBLES_EQUAL(max.x, std::numeric_limits<double>::min(), 1e-3);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(max.y, std::numeric_limits<double>::min(), 1e-3);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(max.z, std::numeric_limits<double>::min(), 1e-3);
    }

    {
        auto bb = fiducial.graphic_coordinates_data_bounding_box(0);
        CPPUNIT_ASSERT(!bb.has_value());
    }

    // Copy the current contour in the current fiducial data
    fiducial.contour_data.emplace_back(0., 0., 0.);
    fiducial.contour_data.emplace_back(16., 16., 16.);
    {
        auto [min, max] = fiducial.contour_data_bounding_box();

        CPPUNIT_ASSERT_DOUBLES_EQUAL(min.x, 0., 1.e-3);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(min.y, 0., 1.e-3);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(min.z, 0., 1.e-3);

        CPPUNIT_ASSERT_DOUBLES_EQUAL(max.x, 16., 1.e-3);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(max.y, 16., 1.e-3);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(max.z, 16., 1.e-3);
    }

    fiducial.graphic_coordinates_data_sequence = std::vector<sight::data::fiducials_series::graphic_coordinates_data>();
    // We only need 1 element as the contour only spans the current frame
    fiducial.graphic_coordinates_data_sequence->emplace_back();

    auto& gcds0 = fiducial.graphic_coordinates_data_sequence.value().at(0);

    gcds0.graphic_data.emplace_back(0., 0.);
    gcds0.graphic_data.emplace_back(8., 8.);

    {
        auto bb = fiducial.graphic_coordinates_data_bounding_box(0);
        CPPUNIT_ASSERT(bb.has_value());

        CPPUNIT_ASSERT_DOUBLES_EQUAL(std::get<0>(*bb).x, 0., 1.e-3);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(std::get<0>(*bb).y, 0., 1.e-3);

        CPPUNIT_ASSERT_DOUBLES_EQUAL(std::get<1>(*bb).x, 8., 1.e-3);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(std::get<1>(*bb).y, 8., 1.e-3);
    }
}

//------------------------------------------------------------------------------

void fiducials_series_test::add_fiducial_test()
{
    auto fiducials_series = std::make_shared<data::fiducials_series>();

    const std::string group_name {core::tools::uuid::generate()};

    const std::array<float, 4> color {0.1F, 0.2F, 0.3F, 0.4F};
    const float size {32.F};
    const data::fiducials_series::private_shape private_shape {data::fiducials_series::private_shape::cube};
    const bool visible {false};

    {
        const std::vector<double> contour_data {1.F, 2.F, 3.F};
        const std::string fiducial_description {core::id::join(group_name, 0)};
        const std::string fiducial_identifier {core::id::join(group_name, 0)};
        const std::string fiducial_uid {core::tools::uuid::generate()};

        const auto& fiducial_add_predicate =
            [&](data::fiducials_series::query_result& _result) -> bool
            {
                // Fiducial set part
                _result.m_group_name    = group_name;
                _result.m_color         = color;
                _result.m_size          = size;
                _result.m_private_shape = private_shape;
                _result.m_visible       = visible;

                // Fiducial part
                _result.m_contour_data         = contour_data;
                _result.m_fiducial_description = fiducial_description;
                _result.m_fiducial_identifier  = fiducial_identifier;
                _result.m_fiducial_uid         = fiducial_uid;

                return true;
            };

        fiducials_series->add_fiducial(fiducial_add_predicate, data::fiducials_series::shape::point, group_name);

        // Check the result using GDCM
        const auto& fiducial_set_sequence_tag = gdcm::Keywords::FiducialSetSequence::GetTag();
        const auto& fiducials_dataset         = fiducials_series->get_data_set();

        // Get fiducial set dataset
        CPPUNIT_ASSERT(fiducials_dataset.FindDataElement(fiducial_set_sequence_tag));
        const auto& fiducial_set_sequence = fiducials_dataset.GetDataElement(fiducial_set_sequence_tag).GetValueAsSQ();
        CPPUNIT_ASSERT_EQUAL(fiducial_set_sequence->GetNumberOfItems(), gdcm::SequenceOfItems::SizeType(1));
        const auto& fiducial_set_dataset = fiducial_set_sequence->GetItem(1).GetNestedDataSet();

        // Get fiducial dataset
        const auto& fiducial_sequence_tag = gdcm::Keywords::FiducialSequence::GetTag();
        CPPUNIT_ASSERT(fiducial_set_dataset.FindDataElement(fiducial_sequence_tag));
        const auto& fiducial_sequence = fiducial_set_dataset.GetDataElement(fiducial_sequence_tag).GetValueAsSQ();
        CPPUNIT_ASSERT_EQUAL(fiducial_sequence->GetNumberOfItems(), gdcm::SequenceOfItems::SizeType(1));
        const auto& fiducial_dataset = fiducial_sequence->GetItem(1).GetNestedDataSet();

        check_fiducial_with_gdcm(
            fiducial_set_dataset,
            fiducial_dataset,
            group_name,
            color,
            size,
            private_shape,
            visible,
            contour_data,
            fiducial_description,
            fiducial_identifier,
            fiducial_uid
        );
    }

    // Add a plane fiducial in the same group
    {
        const std::vector<double> contour_data {4.F, 5.F, 6.F, 7.F, 8.F, 9.F, 10.F, 11.F, 12.F};
        const std::string fiducial_description {core::id::join(group_name, 1)};
        const std::string fiducial_identifier {core::id::join(group_name, 1)};
        const std::string fiducial_uid {core::tools::uuid::generate()};

        const auto& fiducial_add_predicate =
            [&](data::fiducials_series::query_result& _result) -> bool
            {
                // Fiducial set part
                _result.m_group_name    = group_name;
                _result.m_color         = color;
                _result.m_size          = size;
                _result.m_private_shape = private_shape;
                _result.m_visible       = visible;

                // Fiducial part
                _result.m_shape                = data::fiducials_series::shape::plane;
                _result.m_contour_data         = contour_data;
                _result.m_fiducial_description = fiducial_description;
                _result.m_fiducial_identifier  = fiducial_identifier;
                _result.m_fiducial_uid         = fiducial_uid;

                return true;
            };

        fiducials_series->add_fiducial(fiducial_add_predicate, data::fiducials_series::shape::plane, group_name);

        // Check the result using GDCM
        const auto& fiducial_set_sequence_tag = gdcm::Keywords::FiducialSetSequence::GetTag();
        const auto& fiducials_dataset         = fiducials_series->get_data_set();

        // Get fiducial set dataset
        CPPUNIT_ASSERT(fiducials_dataset.FindDataElement(fiducial_set_sequence_tag));
        const auto& fiducial_set_sequence = fiducials_dataset.GetDataElement(fiducial_set_sequence_tag).GetValueAsSQ();
        CPPUNIT_ASSERT_EQUAL(fiducial_set_sequence->GetNumberOfItems(), gdcm::SequenceOfItems::SizeType(1));
        const auto& fiducial_set_dataset = fiducial_set_sequence->GetItem(1).GetNestedDataSet();

        // Get fiducial dataset
        const auto& fiducial_sequence_tag = gdcm::Keywords::FiducialSequence::GetTag();
        CPPUNIT_ASSERT(fiducial_set_dataset.FindDataElement(fiducial_sequence_tag));
        const auto& fiducial_sequence = fiducial_set_dataset.GetDataElement(fiducial_sequence_tag).GetValueAsSQ();
        CPPUNIT_ASSERT_EQUAL(fiducial_sequence->GetNumberOfItems(), gdcm::SequenceOfItems::SizeType(2));
        const auto& fiducial_dataset = fiducial_sequence->GetItem(2).GetNestedDataSet();

        check_fiducial_with_gdcm(
            fiducial_set_dataset,
            fiducial_dataset,
            group_name,
            color,
            size,
            private_shape,
            visible,
            contour_data,
            fiducial_description,
            fiducial_identifier,
            fiducial_uid
        );
    }

    // Add a point fiducial in a second group
    {
        const std::string group_name_2 {core::tools::uuid::generate()};

        const std::array<float, 4> color_2 {0.9F, 0.10F, 0.12F, 0.13F};
        const float size_2 {16.F};
        const data::fiducials_series::private_shape private_shape_2 {data::fiducials_series::private_shape::sphere};
        const bool visible_2 {true};

        const std::vector<double> contour_data {7.F, 8.F, 9.F};
        const std::string fiducial_description {core::id::join(group_name_2, 0)};
        const std::string fiducial_identifier {core::id::join(group_name_2, 0)};
        const std::string fiducial_uid {core::tools::uuid::generate()};

        const auto& fiducial_add_predicate =
            [&](data::fiducials_series::query_result& _result) -> bool
            {
                // Fiducial set part
                _result.m_group_name    = group_name_2;
                _result.m_color         = color_2;
                _result.m_size          = size_2;
                _result.m_private_shape = private_shape_2;
                _result.m_visible       = visible_2;

                // Fiducial part
                _result.m_contour_data         = contour_data;
                _result.m_fiducial_description = fiducial_description;
                _result.m_fiducial_identifier  = fiducial_identifier;
                _result.m_fiducial_uid         = fiducial_uid;

                return true;
            };

        fiducials_series->add_fiducial(fiducial_add_predicate, data::fiducials_series::shape::point, group_name_2);

        // Check the result using GDCM
        const auto& fiducial_set_sequence_tag = gdcm::Keywords::FiducialSetSequence::GetTag();
        const auto& fiducials_dataset         = fiducials_series->get_data_set();

        // Get fiducial set dataset
        CPPUNIT_ASSERT(fiducials_dataset.FindDataElement(fiducial_set_sequence_tag));
        const auto& fiducial_set_sequence = fiducials_dataset.GetDataElement(fiducial_set_sequence_tag).GetValueAsSQ();
        CPPUNIT_ASSERT_EQUAL(fiducial_set_sequence->GetNumberOfItems(), gdcm::SequenceOfItems::SizeType(2));
        const auto& fiducial_set_dataset = fiducial_set_sequence->GetItem(2).GetNestedDataSet();

        // Get fiducial dataset
        const auto& fiducial_sequence_tag = gdcm::Keywords::FiducialSequence::GetTag();
        CPPUNIT_ASSERT(fiducial_set_dataset.FindDataElement(fiducial_sequence_tag));
        const auto& fiducial_sequence = fiducial_set_dataset.GetDataElement(fiducial_sequence_tag).GetValueAsSQ();
        CPPUNIT_ASSERT_EQUAL(fiducial_sequence->GetNumberOfItems(), gdcm::SequenceOfItems::SizeType(1));
        const auto& fiducial_dataset = fiducial_sequence->GetItem(1).GetNestedDataSet();

        check_fiducial_with_gdcm(
            fiducial_set_dataset,
            fiducial_dataset,
            group_name_2,
            color_2,
            size_2,
            private_shape_2,
            visible_2,
            contour_data,
            fiducial_description,
            fiducial_identifier,
            fiducial_uid
        );
    }
}

//------------------------------------------------------------------------------

void fiducials_series_test::query_fiducials_test()
{
    auto fiducials_series = std::make_shared<data::fiducials_series>();

    // Fill the created fiducials series with some fiducials
    for(const auto& query_result : TEST_VECTOR)
    {
        fiducials_series->add_fiducial(
            [&query_result](data::fiducials_series::query_result& _result) -> bool
            {
                _result = query_result;
                return true;
            },
            *query_result.m_shape,
            *query_result.m_group_name
        );
    }

    // Basic checks using GDCM (this is somewhat redundant with add_fiducial_test)
    {
        const auto& fiducial_set_sequence_tag = gdcm::Keywords::FiducialSetSequence::GetTag();
        const auto& fiducials_dataset         = fiducials_series->get_data_set();

        // Get fiducial set dataset
        CPPUNIT_ASSERT(fiducials_dataset.FindDataElement(fiducial_set_sequence_tag));
        const auto& fiducial_set_sequence = fiducials_dataset.GetDataElement(fiducial_set_sequence_tag).GetValueAsSQ();

        /// 3 "groups" should have been created
        CPPUNIT_ASSERT_EQUAL(fiducial_set_sequence->GetNumberOfItems(), gdcm::SequenceOfItems::SizeType(3));

        for(std::size_t fiducial_set_index = 1 ; fiducial_set_index <= 3 ; ++fiducial_set_index)
        {
            const auto& fiducial_set_dataset  = fiducial_set_sequence->GetItem(fiducial_set_index).GetNestedDataSet();
            const auto& fiducial_sequence_tag = gdcm::Keywords::FiducialSequence::GetTag();
            CPPUNIT_ASSERT(fiducial_set_dataset.FindDataElement(fiducial_sequence_tag));
            const auto& fiducial_sequence = fiducial_set_dataset.GetDataElement(fiducial_sequence_tag).GetValueAsSQ();

            switch(fiducial_set_index)
            {
                case 1:
                    CPPUNIT_ASSERT_EQUAL(
                        fiducial_sequence->GetNumberOfItems(),
                        gdcm::SequenceOfItems::SizeType(1)
                    );
                    break;

                case 2:
                    CPPUNIT_ASSERT_EQUAL(
                        fiducial_sequence->GetNumberOfItems(),
                        gdcm::SequenceOfItems::SizeType(2)
                    );
                    break;

                case 3:
                    CPPUNIT_ASSERT_EQUAL(
                        fiducial_sequence->GetNumberOfItems(),
                        gdcm::SequenceOfItems::SizeType(4)
                    );
                    break;

                default:
                    break;
            }
        }
    }

    // Query all fiducials, without any filtering
    {
        const auto& query_results = fiducials_series->query_fiducials();
        CPPUNIT_ASSERT_EQUAL(TEST_VECTOR.size(), query_results.size());

        for(std::size_t index = 0 ; index < TEST_VECTOR.size() ; ++index)
        {
            compare_queries(TEST_VECTOR[index], query_results[index]);
        }
    }

    // Query all fiducials from group 1
    {
        const auto& query_results = fiducials_series->query_fiducials(std::nullopt, std::nullopt, "group_1");
        CPPUNIT_ASSERT_EQUAL(query_results.size(), std::size_t(2));

        compare_queries(TEST_VECTOR[1], query_results[0]);
        compare_queries(TEST_VECTOR[2], query_results[1]);
    }

    // Query all point fiducials from group 2
    {
        const auto& query_results = fiducials_series->query_fiducials(
            std::nullopt,
            data::fiducials_series::shape::point,
            "group_2"
        );

        CPPUNIT_ASSERT_EQUAL(query_results.size(), std::size_t(2));

        compare_queries(TEST_VECTOR[3], query_results[0]);
        compare_queries(TEST_VECTOR[6], query_results[1]);
    }

    // Query all fiducials with predicate
    {
        const auto& query_results = fiducials_series->query_fiducials(
            [](const data::fiducials_series::query_result& _result) -> bool
            {
                return _result.m_visible.value_or(true);
            });

        CPPUNIT_ASSERT_EQUAL(query_results.size(), std::size_t(5));

        compare_queries(TEST_VECTOR[0], query_results[0]);
        compare_queries(TEST_VECTOR[3], query_results[1]);
        compare_queries(TEST_VECTOR[4], query_results[2]);
        compare_queries(TEST_VECTOR[5], query_results[3]);
        compare_queries(TEST_VECTOR[6], query_results[4]);
    }

    // Query on fiducial
    {
        const auto& query_results = fiducials_series->query_fiducials(
            std::nullopt,
            data::fiducials_series::shape::point,
            "group_2",
            1
        );

        CPPUNIT_ASSERT_EQUAL(std::size_t(1), query_results.size());
        compare_queries(TEST_VECTOR[6], query_results[0]);
    }
}

//------------------------------------------------------------------------------

void fiducials_series_test::remove_fiducials_test()
{
    // Test removing all fiducials
    {
        auto fiducials_series = std::make_shared<data::fiducials_series>();

        // Fill the created fiducials series with some fiducials
        for(const auto& query_result : TEST_VECTOR)
        {
            fiducials_series->add_fiducial(
                [&query_result](data::fiducials_series::query_result& _result) -> bool
                {
                    _result = query_result;
                    return true;
                },
                *query_result.m_shape,
                *query_result.m_group_name
            );
        }

        // Remove all fiducials
        const auto& removed_results = fiducials_series->remove_fiducials();
        CPPUNIT_ASSERT_EQUAL(TEST_VECTOR.size(), removed_results.first.size());

        for(std::size_t index = 0 ; index < TEST_VECTOR.size() ; ++index)
        {
            compare_queries(TEST_VECTOR[index], removed_results.first[index]);
        }

        CPPUNIT_ASSERT(core::is_equal(std::set<std::string> {"group_0", "group_1", "group_2"}, removed_results.second));

        // Check the result using GDCM
        const auto& fiducial_set_sequence_tag = gdcm::Keywords::FiducialSetSequence::GetTag();
        const auto& fiducials_dataset         = fiducials_series->get_data_set();

        // Get fiducial set dataset
        CPPUNIT_ASSERT(fiducials_dataset.FindDataElement(fiducial_set_sequence_tag));
        const auto& fiducial_set_sequence = fiducials_dataset.GetDataElement(fiducial_set_sequence_tag).GetValueAsSQ();
        CPPUNIT_ASSERT_EQUAL(gdcm::SequenceOfItems::SizeType(0), fiducial_set_sequence->GetNumberOfItems());
    }

    // Test removing all fiducials from group 1
    {
        auto fiducials_series = std::make_shared<data::fiducials_series>();

        // Fill the created fiducials series with some fiducials
        for(const auto& query_result : TEST_VECTOR)
        {
            fiducials_series->add_fiducial(
                [&query_result](data::fiducials_series::query_result& _result) -> bool
                {
                    _result = query_result;
                    return true;
                },
                *query_result.m_shape,
                *query_result.m_group_name
            );
        }

        // Remove all group_1 fiducials and group_1 fiducial set
        const auto& removed_results = fiducials_series->remove_fiducials(std::nullopt, std::nullopt, "group_1");
        CPPUNIT_ASSERT_EQUAL(std::size_t(2), removed_results.first.size());

        compare_queries(TEST_VECTOR[1], removed_results.first[0]);
        compare_queries(TEST_VECTOR[2], removed_results.first[1]);

        CPPUNIT_ASSERT(core::is_equal(std::set<std::string> {"group_1"}, removed_results.second));

        // Check the result using GDCM
        const auto& fiducial_set_sequence_tag = gdcm::Keywords::FiducialSetSequence::GetTag();
        const auto& fiducials_dataset         = fiducials_series->get_data_set();

        // Get fiducial set dataset
        CPPUNIT_ASSERT(fiducials_dataset.FindDataElement(fiducial_set_sequence_tag));
        const auto& fiducial_set_sequence = fiducials_dataset.GetDataElement(fiducial_set_sequence_tag).GetValueAsSQ();
        CPPUNIT_ASSERT_EQUAL(gdcm::SequenceOfItems::SizeType(2), fiducial_set_sequence->GetNumberOfItems());

        // Query again and check the remaining fiducials
        std::array adjusted_test_vector {
            TEST_VECTOR[0], TEST_VECTOR[3], TEST_VECTOR[4], TEST_VECTOR[5], TEST_VECTOR[6]
        };

        // Since the previous fiducial set at index 1 has been removed, the fiducial set at index 2 is now at index 1
        adjusted_test_vector[1].m_fiducial_set_index = 1;
        adjusted_test_vector[2].m_fiducial_set_index = 1;
        adjusted_test_vector[3].m_fiducial_set_index = 1;
        adjusted_test_vector[4].m_fiducial_set_index = 1;

        const auto& query_results = fiducials_series->query_fiducials();
        CPPUNIT_ASSERT_EQUAL(adjusted_test_vector.size(), query_results.size());

        for(std::size_t index = 0 ; index < query_results.size() ; ++index)
        {
            compare_queries(adjusted_test_vector[index], query_results[index]);
        }
    }

    // Test removing all point fiducials (only group 2 should be kept)
    {
        auto fiducials_series = std::make_shared<data::fiducials_series>();

        // Fill the created fiducials series with some fiducials
        for(const auto& query_result : TEST_VECTOR)
        {
            fiducials_series->add_fiducial(
                [&query_result](data::fiducials_series::query_result& _result) -> bool
                {
                    _result = query_result;
                    return true;
                },
                *query_result.m_shape,
                *query_result.m_group_name
            );
        }

        // Remove all point fiducials
        const auto& removed_results = fiducials_series->remove_fiducials(
            std::nullopt,
            data::fiducials_series::shape::point
        );

        CPPUNIT_ASSERT_EQUAL(std::size_t(5), removed_results.first.size());

        std::array adjusted_test_vector {
            TEST_VECTOR[0], TEST_VECTOR[1], TEST_VECTOR[2], TEST_VECTOR[3], TEST_VECTOR[6]
        };

        for(std::size_t index = 0 ; index < removed_results.first.size() ; ++index)
        {
            compare_queries(adjusted_test_vector[index], removed_results.first[index]);
        }

        CPPUNIT_ASSERT(core::is_equal(std::set<std::string> {"group_0", "group_1"}, removed_results.second));

        // Query again and check the remaining fiducials
        std::array remaining_test_vector {
            TEST_VECTOR[4], TEST_VECTOR[5]
        };

        // Since the previous fiducial has been removed, set the fiducial set at index 0
        remaining_test_vector[0].m_fiducial_set_index = 0;
        remaining_test_vector[0].m_fiducial_index     = 0;
        remaining_test_vector[1].m_fiducial_set_index = 0;
        remaining_test_vector[1].m_fiducial_index     = 1;

        const auto& query_results = fiducials_series->query_fiducials();
        CPPUNIT_ASSERT_EQUAL(remaining_test_vector.size(), query_results.size());

        for(std::size_t index = 0 ; index < query_results.size() ; ++index)
        {
            compare_queries(remaining_test_vector[index], query_results[index]);
        }
    }

    // Test executing the predicate on all point fiducials of group 2
    {
        auto fiducials_series = std::make_shared<data::fiducials_series>();

        // Fill the created fiducials series with some fiducials
        for(const auto& query_result : TEST_VECTOR)
        {
            fiducials_series->add_fiducial(
                [&query_result](data::fiducials_series::query_result& _result) -> bool
                {
                    _result = query_result;
                    return true;
                },
                *query_result.m_shape,
                *query_result.m_group_name
            );
        }

        // Remove the point fiducial from group 2 with the given fiducial_uid
        const auto& removed_results = fiducials_series->remove_fiducials(
            [](const data::fiducials_series::query_result& _result) -> bool
            {
                return _result.m_fiducial_uid == TEST_VECTOR[6].m_fiducial_uid;
            },
            data::fiducials_series::shape::point,
            "group_2"
        );

        CPPUNIT_ASSERT_EQUAL(std::size_t(1), removed_results.first.size());

        compare_queries(TEST_VECTOR[6], removed_results.first[0]);

        CPPUNIT_ASSERT(core::is_equal(std::set<std::string> {}, removed_results.second));

        // Query again and check the remaining fiducials
        const auto& query_results = fiducials_series->query_fiducials();
        CPPUNIT_ASSERT_EQUAL(TEST_VECTOR.size() - 1, query_results.size());

        for(std::size_t index = 0 ; index < query_results.size() ; ++index)
        {
            compare_queries(TEST_VECTOR[index], query_results[index]);
        }
    }
}

//------------------------------------------------------------------------------

void fiducials_series_test::modify_fiducials_test()
{
    auto fiducials_series = std::make_shared<data::fiducials_series>();

    // Fill the created fiducials series with some fiducials
    for(const auto& query_result : TEST_VECTOR)
    {
        fiducials_series->add_fiducial(
            [&query_result](data::fiducials_series::query_result& _result) -> bool
            {
                _result = query_result;
                return true;
            },
            *query_result.m_shape,
            *query_result.m_group_name
        );
    }

    // Modify the contour data of the point fiducial from group 2 at index 1
    fiducials_series->modify_fiducials(
        [](data::fiducials_series::query_result& _result) -> bool
        {
            _result.m_contour_data = {6.F, 6.F, 6.F};
            return true;
        },
        data::fiducials_series::shape::point,
        "group_2",
        1
    );

    // Check the fiducials
    {
        const auto& query_results = fiducials_series->query_fiducials();
        CPPUNIT_ASSERT_EQUAL(TEST_VECTOR.size(), query_results.size());

        auto adjusted_test_vector = TEST_VECTOR;
        adjusted_test_vector[6].m_contour_data = {6.F, 6.F, 6.F};

        for(std::size_t index = 0 ; index < query_results.size() ; ++index)
        {
            compare_queries(adjusted_test_vector[index], query_results[index]);
        }
    }
}

//------------------------------------------------------------------------------

void fiducials_series_test::point_test()
{
    auto fiducials_series = std::make_shared<data::fiducials_series>();

    const std::string group_name {core::tools::uuid::generate()};

    // Should fail, because the group does not exists
    CPPUNIT_ASSERT(!fiducials_series->add_point(group_name, {0., 0., 0.}));

    // Should now succeed
    fiducials_series->add_group(group_name, {1.0, 1.0, 1.0, 1.0}, 1.F);
    CPPUNIT_ASSERT(fiducials_series->add_point(group_name, {0., 0., 0.}));
    CPPUNIT_ASSERT(fiducials_series->add_point(group_name, {1., 1., 1.}));

    // Count the number of points
    std::vector<sight::data::fiducials_series::query_result> query_results;
    query_results = fiducials_series->query_fiducials(
        std::nullopt,
        data::fiducials_series::shape::point
    );
    CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(2), query_results.size());

    // Remove a point and count again
    fiducials_series->remove_point(group_name, 1);
    query_results = fiducials_series->query_fiducials(
        std::nullopt,
        data::fiducials_series::shape::point
    );
    CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(1), query_results.size());
}

} // namespace sight::data::ut
