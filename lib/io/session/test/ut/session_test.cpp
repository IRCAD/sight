/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
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

#include "session_test.hpp"

#include <core/crypto/aes256.hpp>
#include <core/crypto/base64.hpp>
#include <core/os/temp_path.hpp>
#include <core/tools/uuid.hpp>

#include <data/activity.hpp>
#include <data/activity_set.hpp>
#include <data/array.hpp>
#include <data/boolean.hpp>
#include <data/calibration_info.hpp>
#include <data/camera.hpp>
#include <data/camera_set.hpp>
#include <data/color.hpp>
#include <data/dicom_series.hpp>
#include <data/dvec2.hpp>
#include <data/dvec3.hpp>
#include <data/dvec4.hpp>
#include <data/fiducials_series.hpp>
#include <data/image.hpp>
#include <data/image_series.hpp>
#include <data/integer.hpp>
#include <data/ivec2.hpp>
#include <data/ivec3.hpp>
#include <data/ivec4.hpp>
#include <data/landmarks.hpp>
#include <data/line.hpp>
#include <data/map.hpp>
#include <data/material.hpp>
#include <data/matrix4.hpp>
#include <data/model_series.hpp>
#include <data/mt/locked_ptr.hpp>
#include <data/plane.hpp>
#include <data/plane_list.hpp>
#include <data/point.hpp>
#include <data/point_list.hpp>
#include <data/real.hpp>
#include <data/reconstruction.hpp>
#include <data/resection.hpp>
#include <data/resection_db.hpp>
#include <data/series.hpp>
#include <data/series_set.hpp>
#include <data/set.hpp>
#include <data/string.hpp>
#include <data/structure_traits.hpp>
#include <data/structure_traits_dictionary.hpp>
#include <data/transfer_function.hpp>
#include <data/vector.hpp>

#include <geometry/data/mesh.hpp>

#include <io/dicom/reader/series_set.hpp>
#include <io/session/detail/core/session_deserializer.hpp>
#include <io/session/detail/core/session_serializer.hpp>
#include <io/session/helper.hpp>
#include <io/session/session_reader.hpp>
#include <io/session/session_writer.hpp>
#include <io/zip/exception/read.hpp>
#include <io/zip/exception/write.hpp>

#include <utest/filter.hpp>

#include <utest_data/data.hpp>
#include <utest_data/generator/image.hpp>
#include <utest_data/generator/mesh.hpp>

#include <random>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::session::ut::session_test);

namespace sight::io::session::ut
{

// For uuid::generate();
using core::tools::uuid;

//------------------------------------------------------------------------------

template<typename T>
inline T random()
{
    using uniform_distribution = std::conditional_t<
        std::is_floating_point_v<T>,
        std::uniform_real_distribution<T>,
        std::uniform_int_distribution<T>
    >;

    static uniform_distribution distributor(std::numeric_limits<T>::min(), std::numeric_limits<T>::max());
    static std::mt19937 generator {std::random_device {}()};

    return distributor(generator);
}

//------------------------------------------------------------------------------

inline static std::string generate_tm(std::size_t _variant)
{
    std::stringstream hh;
    hh << std::setfill('0') << std::setw(2) << (_variant % 24);

    std::stringstream mm;
    mm << std::setfill('0') << std::setw(2) << ((_variant + 1) % 60);

    std::stringstream ss;
    ss << std::setfill('0') << std::setw(2) << ((_variant + 2) % 60);

    std::stringstream ffffff;
    ffffff << std::setfill('0') << std::setw(6) << ((_variant + 3) % 1000000);

    return hh.str() + mm.str() + ss.str() + "." + ffffff.str();
}

//------------------------------------------------------------------------------

inline static std::string generate_da(std::size_t _variant)
{
    std::stringstream yyyy;
    yyyy << std::setfill('0') << std::setw(4) << (_variant % 10000);

    std::stringstream mm;
    mm << std::setfill('0') << std::setw(2) << ((_variant + 1) % 12);

    std::stringstream dd;
    dd << std::setfill('0') << std::setw(2) << ((_variant + 2) % 32);

    return yyyy.str() + mm.str() + dd.str();
}

//------------------------------------------------------------------------------

inline static std::string generate_dt(std::size_t _variant)
{
    return generate_da(_variant) + generate_tm(_variant);
}

//------------------------------------------------------------------------------

void session_test::setUp()
{
}

//------------------------------------------------------------------------------

void session_test::tearDown()
{
}

//------------------------------------------------------------------------------

template<typename T>
inline typename T::sptr generate(const std::size_t /*unused*/)
{
    return std::make_shared<T>(static_cast<typename T::value_t>(random<typename T::value_t>()));
}

//------------------------------------------------------------------------------

template<typename T>
inline const typename T::csptr& get_expected(const std::size_t _variant)
{
    static std::map<std::size_t, typename T::csptr> map;
    const auto& it = map.find(_variant);

    if(it == map.cend())
    {
        const auto& object = generate<T>(_variant);
        object->set_description(uuid::generate());

        return map.insert_or_assign(_variant, object).first->second;
    }

    return it->second;
}

//------------------------------------------------------------------------------

template<typename T>
inline typename T::sptr create(const std::size_t _variant)
{
    const auto& object = std::make_shared<T>();
    object->deep_copy(get_expected<T>(_variant));
    return object;
}

//------------------------------------------------------------------------------

template<typename T>
inline void compare(const typename T::csptr& _actual, const std::size_t _variant)
{
    CPPUNIT_ASSERT(_actual);
    CPPUNIT_ASSERT_EQUAL(get_expected<T>(_variant)->get_value(), _actual->get_value());
}

//------------------------------------------------------------------------------

template<typename T>
inline void test(const bool _encrypt, const bool _raw, const bool _empty_obj = false)
{
    static constexpr auto s_PASSWORD = "password";

    const auto& test_id = T::leaf_classname() + "_" + std::to_string(_encrypt) + "_" + std::to_string(_raw);

    // Create a temporary directory
    core::os::temp_dir tmp_dir;
    const auto test_path = tmp_dir / (test_id + (_raw ? ".json" : ".zip"));

    static constexpr auto s_FIELD_NAME = "field";

    // Test serialization
    {
        // Create the data object
        auto object = _empty_obj ? std::make_shared<T>() : create<T>(0);

        // Add a field
        object->set_field(s_FIELD_NAME, create<T>(1));

        // Create the session writer
        auto session_writer = std::make_shared<io::session::session_writer>();
        CPPUNIT_ASSERT(session_writer);

        // Configure the session writer
        session_writer->set_object(object);
        session_writer->set_file(test_path);

        if(_raw)
        {
            session_writer->set_archive_format(io::zip::archive::archive_format::filesystem);
        }
        else if(_encrypt)
        {
            session_writer->set_password(s_PASSWORD);
        }

        // Write the session
        CPPUNIT_ASSERT_NO_THROW(session_writer->write());

        CPPUNIT_ASSERT(std::filesystem::exists(test_path));
    }

    // Test deserialization
    {
        auto session_reader = std::make_shared<io::session::session_reader>();
        CPPUNIT_ASSERT(session_reader);

        // Configure the session reader
        session_reader->set_file(test_path);

        if(_raw)
        {
            session_reader->set_archive_format(io::zip::archive::archive_format::filesystem);
        }
        else if(_encrypt)
        {
            session_reader->set_password(s_PASSWORD);
        }

        // Read the session
        CPPUNIT_ASSERT_NO_THROW(session_reader->read());

        // Test value
        auto actual_object = std::dynamic_pointer_cast<T>(session_reader->get_object());

        // Create the data object
        auto expected_object = _empty_obj ? std::make_shared<T>() : create<T>(0);

        // Add a field
        expected_object->set_field(s_FIELD_NAME, create<T>(1));

        CPPUNIT_ASSERT(*expected_object == *actual_object);
    }
}

//------------------------------------------------------------------------------

template<typename T>
inline void test_combine()
{
    test<T>(false, false, false);
    test<T>(false, false, true);
    test<T>(false, true, false);
    test<T>(false, true, true);
    test<T>(true, false, false);
    test<T>(true, false, true);
}

//------------------------------------------------------------------------------

template<>
inline data::boolean::sptr generate<data::boolean>(const std::size_t _variant)
{
    return std::make_shared<data::boolean>(_variant % 2 == 0);
}

//------------------------------------------------------------------------------

void session_test::boolean_test()
{
    test_combine<data::boolean>();
}

//------------------------------------------------------------------------------

void session_test::integer_test()
{
    test_combine<data::integer>();
}

//------------------------------------------------------------------------------

void session_test::float_test()
{
    test_combine<data::real>();
}

//------------------------------------------------------------------------------

template<>
inline data::dvec2::sptr generate<data::dvec2>(const std::size_t _variant)
{
    auto object = std::make_shared<data::dvec2>();
    (*object)[0] = static_cast<data::dvec2::value_t::value_type>(_variant);
    (*object)[1] = static_cast<data::dvec2::value_t::value_type>(_variant + 1);
    return object;
}

//------------------------------------------------------------------------------

template<>
inline data::dvec3::sptr generate<data::dvec3>(const std::size_t _variant)
{
    auto object = std::make_shared<data::dvec3>();
    (*object)[0] = static_cast<data::dvec3::value_t::value_type>(_variant);
    (*object)[1] = static_cast<data::dvec3::value_t::value_type>(_variant + 1);
    (*object)[2] = static_cast<data::dvec3::value_t::value_type>(_variant + 2);
    return object;
}

//------------------------------------------------------------------------------

template<>
inline data::dvec4::sptr generate<data::dvec4>(const std::size_t _variant)
{
    auto object = std::make_shared<data::dvec4>();
    (*object)[0] = static_cast<data::dvec4::value_t::value_type>(_variant);
    (*object)[1] = static_cast<data::dvec4::value_t::value_type>(_variant + 1);
    (*object)[2] = static_cast<data::dvec4::value_t::value_type>(_variant + 2);
    (*object)[3] = static_cast<data::dvec4::value_t::value_type>(_variant + 3);
    return object;
}

//------------------------------------------------------------------------------

template<>
inline data::ivec2::sptr generate<data::ivec2>(const std::size_t _variant)
{
    auto object = std::make_shared<data::ivec2>();
    (*object)[0] = static_cast<data::ivec2::value_t::value_type>(_variant);
    (*object)[1] = static_cast<data::ivec2::value_t::value_type>(_variant + 1);
    return object;
}

//------------------------------------------------------------------------------

template<>
inline data::ivec3::sptr generate<data::ivec3>(const std::size_t _variant)
{
    auto object = std::make_shared<data::ivec3>();
    (*object)[0] = static_cast<data::ivec3::value_t::value_type>(_variant);
    (*object)[1] = static_cast<data::ivec3::value_t::value_type>(_variant + 1);
    (*object)[2] = static_cast<data::ivec3::value_t::value_type>(_variant + 2);
    return object;
}

//------------------------------------------------------------------------------

template<>
inline data::ivec4::sptr generate<data::ivec4>(const std::size_t _variant)
{
    auto object = std::make_shared<data::ivec4>();
    (*object)[0] = static_cast<data::ivec4::value_t::value_type>(_variant);
    (*object)[1] = static_cast<data::ivec4::value_t::value_type>(_variant + 1);
    (*object)[2] = static_cast<data::ivec4::value_t::value_type>(_variant + 2);
    (*object)[3] = static_cast<data::ivec4::value_t::value_type>(_variant + 3);
    return object;
}

//------------------------------------------------------------------------------

void session_test::vec_test()
{
    test_combine<data::dvec2>();
    test_combine<data::dvec3>();
    test_combine<data::dvec4>();
    test_combine<data::ivec2>();
    test_combine<data::ivec3>();
    test_combine<data::ivec4>();
}

//------------------------------------------------------------------------------

template<>
inline data::string::sptr generate<data::string>(const std::size_t /*unused*/)
{
    return std::make_shared<data::string>(uuid::generate());
}

//------------------------------------------------------------------------------

void session_test::string_test()
{
    test_combine<data::string>();
}

//------------------------------------------------------------------------------

template<>
inline data::map::sptr generate<data::map>(const std::size_t _variant)
{
    auto object = std::make_shared<data::map>();
    (*object)[data::boolean::classname()] = create<data::boolean>(_variant);
    (*object)[data::integer::classname()] = create<data::integer>(_variant);
    (*object)[data::real::classname()]    = create<data::real>(_variant);
    (*object)[data::string::classname()]  = create<data::string>(_variant);

    return object;
}

//------------------------------------------------------------------------------

void session_test::map_test()
{
    test_combine<data::map>();
}

//------------------------------------------------------------------------------

template<>
inline data::mesh::sptr create<data::mesh>(const std::size_t _variant)
{
    const auto& object = std::make_shared<data::mesh>();
    object->deep_copy(get_expected<data::mesh>(_variant));
    object->shrink_to_fit();
    return object;
}

//------------------------------------------------------------------------------

template<>
inline data::mesh::sptr generate<data::mesh>(const std::size_t /*unused*/)
{
    auto object = std::make_shared<data::mesh>();

    utest_data::generator::mesh::generate_triangle_quad_mesh(object);
    geometry::data::mesh::shake_point(object);
    geometry::data::mesh::colorize_mesh_points(object);
    geometry::data::mesh::colorize_mesh_cells(object);
    geometry::data::mesh::generate_point_normals(object);
    geometry::data::mesh::generate_cell_normals(object);
    object->shrink_to_fit();

    return object;
}

//------------------------------------------------------------------------------

void session_test::mesh_test()
{
    test_combine<data::mesh>();
}

//------------------------------------------------------------------------------

template<>
inline data::series::sptr generate<data::series>(const std::size_t _variant)
{
    auto object = std::make_shared<data::series>();

    // Fill trivial attributes
    object->set_sop_keyword(sight::data::dicom::sop::Keyword::EnhancedUSVolumeStorage);
    object->set_modality(sight::data::dicom::modality_t::us);
    object->set_series_description(uuid::generate());
    object->set_series_instance_uid(uuid::generate());
    object->set_series_number(std::int32_t(_variant));
    object->set_laterality(uuid::generate());
    object->set_series_date(generate_da(_variant));
    object->set_series_time(generate_tm(_variant));
    object->set_performing_physician_name(uuid::generate() + "\\" + uuid::generate());
    object->set_protocol_name(uuid::generate());
    object->set_body_part_examined(uuid::generate());
    object->set_patient_position(uuid::generate());
    object->set_anatomical_orientation_type(uuid::generate());
    object->set_performed_procedure_step_id(uuid::generate());
    object->set_performed_procedure_step_start_date(generate_da(_variant));
    object->set_performed_procedure_step_start_time(generate_tm(_variant));
    object->set_performed_procedure_step_end_date(generate_da(_variant));
    object->set_performed_procedure_step_end_time(generate_tm(_variant));
    object->set_performed_procedure_step_description(uuid::generate());
    object->set_comments_on_the_performed_procedure_step(uuid::generate());

    // Equipment Module
    object->set_institution_name(uuid::generate());

    // Patient Module
    object->set_patient_name(uuid::generate());
    object->set_patient_id(uuid::generate());
    object->set_patient_birth_date(generate_da(_variant));
    object->set_patient_sex(uuid::generate());

    // Study Module
    object->set_study_description(uuid::generate());
    object->set_study_instance_uid(uuid::generate());
    object->set_study_id(uuid::generate());
    object->set_study_date(generate_da(_variant));
    object->set_study_time(generate_tm(_variant));
    object->set_referring_physician_name(uuid::generate());

    // Patient Study Module
    object->set_patient_age(uuid::generate());
    object->set_patient_size(double(_variant));
    object->set_patient_weight(double(_variant));

    // Other Modules
    object->set_slice_thickness(double(_variant));
    object->set_frame_acquisition_date_time("57", 0);
    object->set_frame_comments("58", 0);
    object->set_frame_label("59", 0);
    object->set_ultrasound_acquisition_geometry(data::dicom::ultrasound_acquisition_geometry_t::patient);
    object->set_patient_frame_of_reference_source(data::dicom::patient_frame_of_reference_source_t::table);
    object->set_dimension_organization_type(data::dicom::dimension_organization_t::volume);

    // Generate specific instance data
    for(std::size_t i = 0 ; i < _variant + 3 ; ++i)
    {
        object->set_image_position_patient(
            {
                double(_variant + i),
                double(_variant + i + 1),
                double(_variant + i + 2)
            },
            i
        );

        object->set_image_orientation_patient(
            {
                double(_variant + i), double(_variant + i + 1), double(_variant + i + 2),
                double(_variant + i + 3), double(_variant + i + 4), double(_variant + i + 5)
            },
            i
        );

        object->set_frame_acquisition_date_time(generate_dt(_variant + i), i);
        object->set_frame_comments(uuid::generate(), i);
        object->set_frame_label(uuid::generate(), i);
    }

    // Test private tag...
    object->set_private_value(uuid::generate(), 0x10);
    object->set_multi_frame_private_value(uuid::generate(), 0x15, 0);

    return object;
}

//------------------------------------------------------------------------------

void session_test::series_test()
{
    test_combine<data::series>();
}

//------------------------------------------------------------------------------

template<>
inline data::activity::sptr generate<data::activity>(const std::size_t _variant)
{
    auto object = std::make_shared<data::activity>();

    (*object)[data::boolean::classname() + std::to_string(_variant)] = create<data::boolean>(_variant);
    (*object)[data::integer::classname() + std::to_string(_variant)] = create<data::integer>(_variant);
    (*object)[data::real::classname() + std::to_string(_variant)]    = create<data::real>(_variant);
    (*object)[data::string::classname() + std::to_string(_variant)]  = create<data::string>(_variant);

    return object;
}

//------------------------------------------------------------------------------

void session_test::activity_test()
{
    test_combine<data::activity>();
}

//------------------------------------------------------------------------------

template<>
inline data::array::sptr generate<data::array>(const std::size_t _variant)
{
    auto object = std::make_shared<data::array>();

    const auto lock = object->dump_lock();

    auto fill =
        [&](auto _type)
        {
            using type = decltype(_type);

            object->resize(
                {_variant + 2, _variant + 2},
                std::is_same_v<type, double>
                ? core::type::DOUBLE
                : std::is_same_v<type, float>
                ? core::type::FLOAT
                : std::is_same_v<type, std::uint8_t>
                ? core::type::UINT8
                : std::is_same_v<type, std::uint16_t>
                ? core::type::UINT16
                : std::is_same_v<type, std::uint32_t>
                ? core::type::UINT32
                : std::is_same_v<type, std::uint64_t>
                ? core::type::UINT64
                : std::is_same_v<type, std::int8_t>
                ? core::type::INT8
                : std::is_same_v<type, std::int16_t>
                ? core::type::INT16
                : std::is_same_v<type, std::int32_t>
                ? core::type::INT32
                : std::is_same_v<type, std::int64_t>
                ? core::type::INT64
                : core::type::NONE,
                true
            );

            auto counter = static_cast<type>(0);
            for(auto it = object->begin<type>(),
                end = object->end<type>() ;
                it != end ;
                ++it)
            {
                *it = static_cast<type>(_variant) + counter++;
            }
        };

    switch(_variant % 5)
    {
        case 0:
            fill(static_cast<std::uint8_t>(0));
            break;

        case 1:
            fill(static_cast<std::uint16_t>(0));
            break;

        case 2:
            fill(static_cast<std::uint32_t>(0));
            break;

        case 3:
            fill(0.0);
            break;

        case 4:
            fill(0.0F);
            break;

        default:
            CPPUNIT_FAIL("Unknown variant.");
            break;
    }

    return object;
}

//------------------------------------------------------------------------------

void session_test::array_test()
{
    test_combine<data::array>();
}

//------------------------------------------------------------------------------

template<>
inline data::image::sptr generate<data::image>(const std::size_t _variant)
{
    auto object = std::make_shared<data::image>();

    const auto lock = object->dump_lock();

    auto fill =
        [&](auto _type)
        {
            using type = decltype(_type);

            // Warning: generateImage use reflection that cannot deal with double value (truncated to float precision),
            // thus the 0.1 + static_cast<double>(variant)
            utest_data::generator::image::generate_image(
                object,
            {
                _variant + 5,
                _variant + 5,
                _variant + 5
            },
            {
                0.1 + static_cast<double>(_variant),
                0.2 + static_cast<double>(_variant),
                0.3 + static_cast<double>(_variant)
            },
            {
                0.4 + static_cast<double>(_variant),
                0.5 + static_cast<double>(_variant),
                0.6 + static_cast<double>(_variant)
            },
            {
                0.36, 0.48, -0.8, -0.8, 0.6, 0.0, 0.48, 0.64, 0.6
            },
                std::is_same_v<type, double>
                ? core::type::DOUBLE
                : std::is_same_v<type, float>
                ? core::type::FLOAT
                : std::is_same_v<type, std::uint8_t>
                ? core::type::UINT8
                : std::is_same_v<type, std::uint16_t>
                ? core::type::UINT16
                : std::is_same_v<type, std::uint32_t>
                ? core::type::UINT32
                : std::is_same_v<type, std::uint64_t>
                ? core::type::UINT64
                : std::is_same_v<type, std::int8_t>
                ? core::type::INT8
                : std::is_same_v<type, std::int16_t>
                ? core::type::INT16
                : std::is_same_v<type, std::int32_t>
                ? core::type::INT32
                : std::is_same_v<type, std::int64_t>
                ? core::type::INT64
                : core::type::NONE,

                std::is_same_v<type, double>
                ? data::image::pixel_format_t::gray_scale
                : std::is_same_v<type, float>
                ? data::image::pixel_format_t::gray_scale
                : std::is_same_v<type, std::uint8_t>
                ? data::image::pixel_format_t::bgr
                : std::is_same_v<type, std::uint16_t>
                ? data::image::pixel_format_t::bgra
                : std::is_same_v<type, std::uint32_t>
                ? data::image::pixel_format_t::rgb
                : std::is_same_v<type, std::uint64_t>
                ? data::image::pixel_format_t::rgba
                : std::is_same_v<type, std::int8_t>
                ? data::image::pixel_format_t::gray_scale
                : std::is_same_v<type, std::int16_t>
                ? data::image::pixel_format_t::bgr
                : std::is_same_v<type, std::int32_t>
                ? data::image::pixel_format_t::bgra
                : std::is_same_v<type, std::int64_t>
                ? data::image::pixel_format_t::rgb
                : data::image::pixel_format_t::undefined,
                std::uint32_t(_variant)
            );
        };

    switch(_variant % 5)
    {
        case 0:
            fill(static_cast<std::uint8_t>(0));
            break;

        case 1:
            fill(static_cast<std::uint16_t>(0));
            break;

        case 2:
            fill(static_cast<std::uint32_t>(0));
            break;

        case 3:
            fill(0.0);
            break;

        case 4:
            fill(0.0F);
            break;

        default:
            CPPUNIT_FAIL("Unknown variant.");
            break;
    }

    return object;
}

//------------------------------------------------------------------------------

void session_test::image_test()
{
    test_combine<data::image>();
}

//------------------------------------------------------------------------------

template<>
inline data::vector::sptr generate<data::vector>(const std::size_t _variant)
{
    auto object = std::make_shared<data::vector>();

    object->push_back(create<data::boolean>(_variant));
    object->push_back(create<data::integer>(_variant));
    object->push_back(create<data::real>(_variant));
    object->push_back(create<data::string>(_variant));
    object->push_back(create<data::activity>(_variant));

    return object;
}

//------------------------------------------------------------------------------

void session_test::vector_test()
{
    test_combine<data::vector>();
}

//------------------------------------------------------------------------------

template<>
inline data::point::sptr generate<data::point>(const std::size_t /*unused*/)
{
    auto object = std::make_shared<data::point>();

    *object = {random<double>(), random<double>(), random<double>()};

    return object;
}

//------------------------------------------------------------------------------

void session_test::point_test()
{
    test_combine<data::point>();
}

//------------------------------------------------------------------------------

template<>
inline data::point_list::sptr generate<data::point_list>(const std::size_t _variant)
{
    auto object = std::make_shared<data::point_list>();

    auto& points = object->get_points();
    for(std::size_t i = 0, end = _variant + 3 ; i < end ; ++i)
    {
        points.push_back(create<data::point>(i));
    }

    return object;
}

//------------------------------------------------------------------------------

void session_test::point_list_test()
{
    test_combine<data::point_list>();
}

//------------------------------------------------------------------------------

template<>
inline data::calibration_info::sptr generate<data::calibration_info>(const std::size_t _variant)
{
    auto object = std::make_shared<data::calibration_info>();

    for(std::size_t i = 0, end = _variant + 2 ; i < end ; ++i)
    {
        // Create the image
        auto image = create<data::image>(_variant + i);

        // Create the PointList
        auto point_list = create<data::point_list>(_variant + i);

        object->add_record(image, point_list);
    }

    return object;
}

//------------------------------------------------------------------------------

void session_test::calibration_info_test()
{
    test_combine<data::calibration_info>();
}

//------------------------------------------------------------------------------

template<>
inline data::camera::sptr generate<data::camera>(const std::size_t _variant)
{
    auto object = std::make_shared<data::camera>();

    object->set_width(random<std::size_t>());
    object->set_height(random<std::size_t>());
    object->set_fx(random<double>());
    object->set_fy(random<double>());
    object->set_cx(random<double>());
    object->set_cy(random<double>());
    object->set_distortion_coefficient(
        random<double>(),
        random<double>(),
        random<double>(),
        random<double>(),
        random<double>()
    );
    object->set_skew(random<double>());
    object->set_is_calibrated(_variant % 2 == 0);
    object->set_camera_id(uuid::generate());
    object->set_maximum_frame_rate(random<float>());
    constexpr std::array pixel_formats {
        data::camera::pixel_format_t::adobedng,
        data::camera::pixel_format_t::argb32,
        data::camera::pixel_format_t::argb32_premultiplied,
        data::camera::pixel_format_t::rgb32,
        data::camera::pixel_format_t::rgb24,
        data::camera::pixel_format_t::rgb565,
        data::camera::pixel_format_t::rgb555,
        data::camera::pixel_format_t::argb8565_premultiplied,
        data::camera::pixel_format_t::bgra32,
        data::camera::pixel_format_t::bgra32_premultiplied,
        data::camera::pixel_format_t::bgr32,
        data::camera::pixel_format_t::bgr24,
        data::camera::pixel_format_t::bgr565,
        data::camera::pixel_format_t::bgr555,
        data::camera::pixel_format_t::bgra5658_premultiplied,
        data::camera::pixel_format_t::ayuv444,
        data::camera::pixel_format_t::ayuv444_premultiplied,
        data::camera::pixel_format_t::yuv444,
        data::camera::pixel_format_t::yuv420_p,
        data::camera::pixel_format_t::yv12,
        data::camera::pixel_format_t::uyvy,
        data::camera::pixel_format_t::yuyv,
        data::camera::pixel_format_t::nv12,
        data::camera::pixel_format_t::nv21,
        data::camera::pixel_format_t::imc1,
        data::camera::pixel_format_t::imc2,
        data::camera::pixel_format_t::imc3,
        data::camera::pixel_format_t::imc4,
        data::camera::pixel_format_t::y8,
        data::camera::pixel_format_t::y16,
        data::camera::pixel_format_t::jpeg,
        data::camera::pixel_format_t::cameraraw,
        data::camera::pixel_format_t::adobedng,
        data::camera::pixel_format_t::rgba32,
        data::camera::pixel_format_t::user,
        data::camera::pixel_format_t::invalid
    };
    object->set_pixel_format(pixel_formats[_variant % 35]);
    object->set_video_file("/" + uuid::generate());
    object->set_stream_url(uuid::generate());
    object->set_camera_source(
        _variant % 3 == 0
        ? data::camera::source_t::device
        : _variant % 3 == 1
        ? data::camera::source_t::file
        : _variant % 3 == 2
        ? data::camera::source_t::stream
        : data::camera::source_t::unknown
    );
    object->set_scale(random<double>());

    return object;
}

//------------------------------------------------------------------------------

void session_test::camera_test()
{
    test_combine<data::camera>();
}

//------------------------------------------------------------------------------

template<>
inline data::color::sptr generate<data::color>(const std::size_t /*unused*/)
{
    auto object = std::make_shared<data::color>();

    object->set_rgba(random<float>(), random<float>(), random<float>(), random<float>());

    return object;
}

//------------------------------------------------------------------------------

void session_test::color_test()
{
    test_combine<data::color>();
}

//------------------------------------------------------------------------------

template<>
inline data::landmarks::sptr generate<data::landmarks>(const std::size_t _variant)
{
    auto object = std::make_shared<data::landmarks>();

    for(std::size_t i = 0, i_end = _variant + 2 ; i < i_end ; ++i)
    {
        const std::string name = uuid::generate();

        object->add_group(
            name,
            {random<float>(), random<float>(), random<float>(), random<float>()},
            random<float>(),
            _variant % 2 == 0
            ? data::landmarks::shape::cube
            : data::landmarks::shape::sphere,
            _variant % 3 == 0
        );

        for(std::size_t j = 0, j_end = _variant + 2 ; j < j_end ; ++j)
        {
            object->add_point(name, {random<double>(), random<double>(), random<double>()});
        }
    }

    return object;
}

//------------------------------------------------------------------------------

void session_test::landmarks_test()
{
    test_combine<data::landmarks>();
}

//------------------------------------------------------------------------------

template<>
inline data::line::sptr generate<data::line>(const std::size_t _variant)
{
    auto object = std::make_shared<data::line>();

    object->set_position(create<data::point>(_variant));
    object->set_direction(create<data::point>(_variant + 1));

    return object;
}

//------------------------------------------------------------------------------

void session_test::line_test()
{
    test_combine<data::line>();
}

//------------------------------------------------------------------------------

template<>
inline data::material::sptr generate<data::material>(const std::size_t _variant)
{
    auto object = std::make_shared<data::material>();

    std::array shading {
        data::material::shading_t::ambient,
        data::material::shading_t::flat,
        data::material::shading_t::phong
    };

    std::array representation {
        data::material::representation_t::edge,
        data::material::representation_t::point,
        data::material::representation_t::surface,
        data::material::representation_t::wireframe
    };

    std::array options {
        data::material::options_t::cells_normals,
        data::material::options_t::normals,
        data::material::options_t::standard
    };

    // Set ambient color
    object->set_ambient(create<data::color>(_variant));

    // Set diffuse color
    object->set_diffuse(create<data::color>(_variant + 1));

    // Set diffuse texture
    object->set_diffuse_texture(create<data::image>(_variant));

    // Others
    object->set_shading_mode(shading[_variant % std::size(shading)]);
    object->set_representation_mode(representation[_variant % std::size(representation)]);
    object->set_options_mode(options[_variant % std::size(options)]);
    object->set_diffuse_texture_filtering(
        _variant % 3 == 0
        ? data::material::filtering_t::linear
        : data::material::filtering_t::nearest
    );
    object->set_diffuse_texture_wrapping(
        _variant % 3 == 0
        ? data::material::wrapping_t::clamp
        : data::material::wrapping_t::repeat
    );

    return object;
}

//------------------------------------------------------------------------------

void session_test::material_test()
{
    test_combine<data::material>();
}

//------------------------------------------------------------------------------

template<>
inline data::matrix4::sptr generate<data::matrix4>(const std::size_t /*unused*/)
{
    auto object = std::make_shared<data::matrix4>();

    for(double& coefficient : *object)
    {
        coefficient = random<double>();
    }

    return object;
}

//------------------------------------------------------------------------------

void session_test::matrix4_test()
{
    test_combine<data::matrix4>();
}

//------------------------------------------------------------------------------

template<>
inline data::plane::sptr generate<data::plane>(const std::size_t _variant)
{
    auto object = std::make_shared<data::plane>();

    auto& points = object->get_points();
    for(std::size_t i = 0, end = points.size() ; i < end ; ++i)
    {
        points[i] = create<data::point>(i + _variant);
    }

    return object;
}

//------------------------------------------------------------------------------

void session_test::plane_test()
{
    test_combine<data::plane>();
}

//------------------------------------------------------------------------------

template<>
inline data::plane_list::sptr generate<data::plane_list>(const std::size_t _variant)
{
    auto object = std::make_shared<data::plane_list>();

    auto& planes = object->get_planes();

    for(std::size_t i = 0, end = _variant + 2 ; i < end ; ++i)
    {
        planes.push_back(create<data::plane>(_variant + i));
    }

    return object;
}

//------------------------------------------------------------------------------

void session_test::plane_list_test()
{
    test_combine<data::plane_list>();
}

//------------------------------------------------------------------------------

template<>
inline data::reconstruction::sptr generate<data::reconstruction>(const std::size_t _variant)
{
    auto object = std::make_shared<data::reconstruction>();

    object->set_is_visible(_variant % 3 == 0);
    object->set_organ_name(uuid::generate());
    object->set_structure_type(uuid::generate());
    object->set_computed_mask_volume(random<double>());
    object->set_label(random<std::uint32_t>());

    // Material
    object->set_material(create<data::material>(_variant));

    // image
    object->set_image(create<data::image>(_variant));

    // Mesh
    object->set_mesh(create<data::mesh>(_variant));

    return object;
}

//------------------------------------------------------------------------------

void session_test::reconstruction_test()
{
    test_combine<data::reconstruction>();
}

//------------------------------------------------------------------------------

template<>
inline data::structure_traits::sptr generate<data::structure_traits>(const std::size_t _variant)
{
    auto object = std::make_shared<data::structure_traits>();

    static constexpr std::array s_CLASSES {
        data::structure_traits::structure_class::environment,
        data::structure_traits::structure_class::functional,
        data::structure_traits::structure_class::lesion,
        data::structure_traits::structure_class::no_constraint,
        data::structure_traits::structure_class::organ,
        data::structure_traits::structure_class::tool,
        data::structure_traits::structure_class::vessel
    };

    static constexpr std::array s_CATEGORIES {
        data::structure_traits::category::abdomen,
        data::structure_traits::category::arm,
        data::structure_traits::category::body,
        data::structure_traits::category::head,
        data::structure_traits::category::leg,
        data::structure_traits::category::liver_segments,
        data::structure_traits::category::neck,
        data::structure_traits::category::other,
        data::structure_traits::category::pelvis,
        data::structure_traits::category::thorax
    };

    object->set_type(uuid::generate());
    object->set_class(s_CLASSES[_variant % std::size(s_CLASSES)]);
    object->set_native_exp(uuid::generate());
    object->set_native_geometric_exp(uuid::generate());
    object->set_attachment_type(uuid::generate());
    object->set_anatomic_region(uuid::generate());
    object->set_property_category(uuid::generate());
    object->set_property_type(uuid::generate());

    // Categories

    // Reset categories.
    data::structure_traits::category_container_t categories;

    for(std::size_t i = 0, end = _variant + 2 ; i < end ; ++i)
    {
        categories.push_back(s_CATEGORIES[(i + _variant) % std::size(s_CATEGORIES)]);
    }

    object->set_categories(categories);

    // Color
    object->set_color(create<data::color>(_variant));

    return object;
}

//------------------------------------------------------------------------------

void session_test::structure_traits_test()
{
    test_combine<data::structure_traits>();
}

//------------------------------------------------------------------------------

template<>
inline data::structure_traits_dictionary::sptr generate<data::structure_traits_dictionary>(const std::size_t _variant)
{
    auto object = std::make_shared<data::structure_traits_dictionary>();

    auto organ = create<data::structure_traits>(_variant);
    organ->set_class(data::structure_traits::organ);
    organ->set_attachment_type("");

    object->add_structure(organ);

    for(std::size_t i = 0, end = _variant + 2 ; i < end ; ++i)
    {
        auto structure             = create<data::structure_traits>(_variant + i + 1);
        const auto structure_class = structure->get_class();

        if(structure_class != data::structure_traits::lesion
           && structure_class != data::structure_traits::functional)
        {
            structure->set_attachment_type("");
        }
        else
        {
            structure->set_attachment_type(organ->type());
        }

        object->add_structure(structure);
    }

    return object;
}

//------------------------------------------------------------------------------

void session_test::structure_traits_dictionary_test()
{
    test_combine<data::structure_traits_dictionary>();
}

//------------------------------------------------------------------------------

template<>
inline data::resection::sptr generate<data::resection>(const std::size_t _variant)
{
    auto object = std::make_shared<data::resection>();

    object->set_name(uuid::generate());
    object->set_is_safe_part(_variant % 2 == 0);
    object->set_is_valid(_variant % 3 == 0);
    object->set_is_visible(_variant % 4 == 0);

    auto& inputs  = object->get_inputs();
    auto& outputs = object->get_outputs();
    for(std::size_t i = 0, end = _variant + 2 ; i < end ; ++i)
    {
        inputs.push_back(create<data::reconstruction>(_variant + i));
        outputs.push_back(create<data::reconstruction>(_variant + i + 1));
    }

    return object;
}

//------------------------------------------------------------------------------

void session_test::resection_test()
{
    test_combine<data::resection>();
}

//------------------------------------------------------------------------------

template<>
inline data::resection_db::sptr generate<data::resection_db>(const std::size_t _variant)
{
    auto object = std::make_shared<data::resection_db>();

    object->set_safe_resection(create<data::resection>(_variant));

    for(std::size_t i = 0, end = _variant + 2 ; i < end ; ++i)
    {
        object->add_resection(create<data::resection>(_variant + i + 1));
    }

    return object;
}

//------------------------------------------------------------------------------

void session_test::resection_db_test()
{
    test_combine<data::resection_db>();
}

//------------------------------------------------------------------------------

template<>
inline data::transfer_function::sptr generate<data::transfer_function>(const std::size_t _variant)
{
    auto object = std::make_shared<data::transfer_function>();

    object->set_level(random<double>());
    object->set_window(random<double>());
    object->set_name(uuid::generate());
    object->set_background_color(
        data::transfer_function::color_t(
            random<double>(),
            random<double>(),
            random<double>(),
            random<double>()
        )
    );

    auto tf_data = object->pieces().emplace_back(std::make_shared<data::transfer_function_piece>());
    tf_data->set_interpolation_mode(
        _variant % 3 == 0
        ? data::transfer_function::interpolation_mode::linear
        : data::transfer_function::interpolation_mode::nearest
    );
    tf_data->set_clamped(_variant % 4 == 0);

    for(std::size_t i = 0, end = _variant + 2 ; i < end ; ++i)
    {
        tf_data->insert(
            {
                random<double>(),
                data::transfer_function::color_t(
                    random<double>(),
                    random<double>(),
                    random<double>(),
                    random<double>()
                )
            });
    }

    return object;
}

//------------------------------------------------------------------------------

void session_test::transfer_function_test()
{
    test_combine<data::transfer_function>();
}

//------------------------------------------------------------------------------

template<>
inline data::dicom_series::sptr generate<data::dicom_series>(const std::size_t _variant)
{
    data::dicom_series::sptr dicom_series;

    // Only load the real dicom once
    if(_variant == 0)
    {
        // Setup the series_set to be able to read
        auto series_set                  = std::make_shared<data::series_set>();
        const std::filesystem::path path = utest_data::dir()
                                           / "sight/Patient/Dicom/DicomDB/86-CT-Skull";

        CPPUNIT_ASSERT_MESSAGE(
            "The dicom directory '" + path.string() + "' does not exist",
            std::filesystem::exists(path)
        );

        // Read source Dicom
        auto reader = std::make_shared<io::dicom::reader::series_set>();
        reader->set_object(series_set);
        reader->set_folder(path);

        CPPUNIT_ASSERT_NO_THROW(reader->read_dicom_series());
        CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(1), series_set->size());

        dicom_series = std::dynamic_pointer_cast<data::dicom_series>(series_set->front());
    }
    else
    {
        // Take the first variant as basis
        dicom_series = std::make_shared<data::dicom_series>();
        dicom_series->shallow_copy(get_expected<data::dicom_series>(0));
    }

    // Randomize a bit the dicomSeries
    for(std::size_t i = 0, end = _variant + 2 ; i < end ; ++i)
    {
        dicom_series->addsop_class_uid(uuid::generate());
        dicom_series->add_computed_tag_value(uuid::generate(), uuid::generate());
    }

    // Inherited attributes
    dicom_series->series::shallow_copy(get_expected<data::series>(_variant));

    return dicom_series;
}

//------------------------------------------------------------------------------

void session_test::dicom_series_test()
{
    if(utest::filter::ignore_slow_tests())
    {
        return;
    }

    test_combine<data::dicom_series>();
}

//------------------------------------------------------------------------------

template<>
inline data::image_series::sptr generate<data::image_series>(const std::size_t _variant)
{
    auto object = std::make_shared<data::image_series>();

    // Inherited attributes
    object->image::shallow_copy(get_expected<data::image>(_variant));
    object->series::shallow_copy(get_expected<data::series>(_variant));

    // Children
    object->get_fiducials()->shallow_copy(get_expected<data::fiducials_series>(_variant));

    object->set_contrast_bolus_agent(uuid::generate());
    object->set_contrast_bolus_route(uuid::generate());
    object->set_contrast_bolus_volume(double(_variant));
    object->set_contrast_bolus_start_time(generate_tm(_variant));
    object->set_contrast_bolus_stop_time(generate_tm(_variant));
    object->set_contrast_bolus_total_dose(double(_variant));

    object->set_contrast_flow_rate(
        std::to_string(_variant)
        + "\\"
        + std::to_string(_variant + 1)
        + "\\"
        + std::to_string(_variant + 2)
    );

    object->set_contrast_flow_duration(
        std::to_string(_variant + 4)
        + "\\"
        + std::to_string(_variant + 5)
        + "\\"
        + std::to_string(_variant + 6)
    );

    object->set_contrast_bolus_ingredient(uuid::generate());
    object->set_contrast_bolus_ingredient_concentration(double(_variant));
    object->set_acquisition_date(generate_da(_variant));
    object->set_acquisition_time(generate_tm(_variant));

    object->set_dicom_reference(create<data::dicom_series>(_variant));

    return object;
}

//------------------------------------------------------------------------------

void session_test::image_series_test()
{
    if(utest::filter::ignore_slow_tests())
    {
        return;
    }

    test_combine<data::image_series>();
}

//------------------------------------------------------------------------------

template<>
inline data::model_series::sptr generate<data::model_series>(const std::size_t _variant)
{
    auto object = std::make_shared<data::model_series>();

    object->set_dicom_reference(create<data::dicom_series>(_variant));

    std::vector<data::reconstruction::sptr> reconstruction_db;
    for(std::size_t i = 0, end = _variant + 2 ; i < end ; ++i)
    {
        reconstruction_db.push_back(create<data::reconstruction>(_variant + i));
    }

    object->set_reconstruction_db(reconstruction_db);

    // Inherited attributes
    object->series::shallow_copy(get_expected<data::series>(_variant));

    return object;
}

//------------------------------------------------------------------------------

void session_test::model_series_test()
{
    if(utest::filter::ignore_slow_tests())
    {
        return;
    }

    test_combine<data::model_series>();
}

//------------------------------------------------------------------------------

template<>
inline data::fiducials_series::sptr generate<data::fiducials_series>(const std::size_t _variant)
{
    auto fs = std::make_shared<sight::data::fiducials_series>();
    fs->set_content_label(std::to_string(_variant));
    fs->set_content_description("FS");
    fs->set_content_creator_name("John Doe");

    // Adds a dummy fiducial set
    data::fiducials_series::fiducial_set fiducial_set;

    data::fiducials_series::referenced_image referenced_image;
    referenced_image.referenced_sop_class_uid    = "1";
    referenced_image.referenced_sop_instance_uid = "2";
    referenced_image.referenced_frame_number     = {3};
    referenced_image.referenced_segment_number   = {4};
    fiducial_set.referenced_image_sequence       = {referenced_image};

    fiducial_set.frame_of_reference_uid = "5";

    data::fiducials_series::fiducial fiducial;
    fiducial.shape_type           = data::fiducials_series::shape::point;
    fiducial.fiducial_description = "6";
    fiducial.fiducial_identifier  = "7";

    data::fiducials_series::graphic_coordinates_data graphic_coordinates_data;
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
    fiducial_set.shape      = data::fiducials_series::private_shape::cube;
    fiducial_set.visibility = true;

    // Test setFiducialSets method
    fs->set_fiducial_sets({fiducial_set});

    return fs;
}

//------------------------------------------------------------------------------

void session_test::fiducials_series_test()
{
    if(utest::filter::ignore_slow_tests())
    {
        return;
    }

    test_combine<data::fiducials_series>();
}

//------------------------------------------------------------------------------

template<>
inline data::activity_set::sptr generate<data::activity_set>(const std::size_t _variant)
{
    auto object = std::make_shared<data::activity_set>();

    for(std::size_t i = 0, end = _variant + 2 ; i < end ; ++i)
    {
        object->push_back(create<data::activity>(_variant + i));
    }

    return object;
}

//------------------------------------------------------------------------------

void session_test::activity_set_test()
{
    test_combine<data::activity_set>();
}

//------------------------------------------------------------------------------

template<>
inline data::camera_set::sptr generate<data::camera_set>(const std::size_t _variant)
{
    auto object = std::make_shared<data::camera_set>();

    for(std::size_t i = 0, end = _variant + 2 ; i < end ; ++i)
    {
        auto camera = create<data::camera>(_variant + i);
        auto matrix = create<data::matrix4>(_variant + i);
        object->push_back(std::make_pair(camera, matrix));
    }

    return object;
}

//------------------------------------------------------------------------------

void session_test::camera_set_test()
{
    test_combine<data::camera_set>();
}

//------------------------------------------------------------------------------

template<>
inline data::series_set::sptr generate<data::series_set>(const std::size_t _variant)
{
    auto object = std::make_shared<data::series_set>();

    for(std::size_t i = 0, end = _variant + 2 ; i < end ; ++i)
    {
        object->push_back(create<data::series>(_variant + i));
    }

    return object;
}

//------------------------------------------------------------------------------

void session_test::series_set_test()
{
    test_combine<data::series_set>();
}

//------------------------------------------------------------------------------

template<>
inline data::set::sptr generate<data::set>(const std::size_t _variant)
{
    auto object = std::make_shared<data::set>();

    for(std::size_t i = 0, end = _variant + 2 ; i < end ; ++i)
    {
        object->push_back(create<data::series>(_variant + i));
    }

    return object;
}

//------------------------------------------------------------------------------

void session_test::set_test()
{
    test_combine<data::set>();
}

//------------------------------------------------------------------------------

inline static void custom_serialize(
    zip::archive_writer& /*unused*/,
    boost::property_tree::ptree& _tree,
    data::object::csptr _object,
    std::map<std::string, data::object::csptr>& /*unused*/,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    // Cast to the right type
    const auto string = helper::safe_cast<data::string>(_object);

    // Add a version number. Not mandatory, but could help for future release
    helper::write_version<data::string>(_tree, 666);

    helper::write_string(_tree, "custom", string->get_value());
}

//------------------------------------------------------------------------------

inline static data::string::sptr custom_deserialize(
    zip::archive_reader& /*unused*/,
    const boost::property_tree::ptree& _tree,
    const std::map<std::string, data::object::sptr>& /*unused*/,
    data::object::sptr _object,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    // Create or reuse the object
    auto string = helper::cast_or_create<data::string>(_object);

    // Check version number. Not mandatory, but could help for future release
    helper::read_version<data::string>(_tree, 0, 666);

    // Assign the value
    string->set_value(helper::read_string(_tree, "custom"));

    return string;
}

//------------------------------------------------------------------------------

void session_test::custom_serializer_test()
{
    // Create a temporary directory
    core::os::temp_dir tmp_dir;
    const auto test_path = tmp_dir / "customSerializerTest.zip";

    // Test serialization
    {
        // Create the data object
        auto object = create<data::string>(0);

        // Create the session writer
        auto session_writer = std::make_shared<io::session::session_writer>();
        CPPUNIT_ASSERT(session_writer);

        // Configure the session writer
        session_writer->set_object(object);
        session_writer->set_file(test_path);

        // Test serializer getter
        CPPUNIT_ASSERT(session_writer->serializer(data::string::classname()));

        // Change the session serializer by setting a new one using setCustomSerializer
        session_writer->set_custom_serializer(data::string::classname(), custom_serialize);

        // Write the new session
        CPPUNIT_ASSERT_NO_THROW(session_writer->write());

        CPPUNIT_ASSERT(std::filesystem::exists(test_path));
    }

    // Test deserialization
    {
        auto session_reader = std::make_shared<io::session::session_reader>();
        CPPUNIT_ASSERT(session_reader);

        // Configure the session reader
        session_reader->set_file(test_path);

        // Read the session: it should fail since the serializer has been modified by a custom one
        CPPUNIT_ASSERT_THROW(session_reader->read(), sight::core::exception);

        // Test deserializer getter
        CPPUNIT_ASSERT(session_reader->deserializer(data::string::classname()));

        // Set the new customDeserializer
        session_reader->set_custom_deserializer(data::string::classname(), custom_deserialize);

        CPPUNIT_ASSERT_NO_THROW(session_reader->read());

        // Test value
        auto object = std::dynamic_pointer_cast<data::string>(session_reader->get_object());
        compare<data::string>(object, 0);
    }
}

} // namespace sight::io::session::ut
