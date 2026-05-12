/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
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
#include <data/line.hpp>
#include <data/map.hpp>
#include <data/material.hpp>
#include <data/matrix4.hpp>
#include <data/model_series.hpp>
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

#include <io/session/helper.hpp>
#include <io/session/session_reader.hpp>
#include <io/session/session_writer.hpp>

#include <utest/filter.hpp>

#include <utest_data/generator/image.hpp>
#include <utest_data/generator/mesh.hpp>

#include <doctest/doctest.h>

#include <random>

// For uuid::generate();
using sight::core::tools::uuid;

//------------------------------------------------------------------------------

template<typename T>
static inline T random()
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

template<typename T>
static inline T::sptr generate(const std::size_t /*unused*/)
{
    return std::make_shared<T>(static_cast<T::value_t>(random<typename T::value_t>()));
}

//------------------------------------------------------------------------------

template<typename T>
static inline const T::csptr& get_expected(const std::size_t _variant)
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
static inline T::sptr create(const std::size_t _variant)
{
    const auto& object = std::make_shared<T>();
    object->deep_copy(get_expected<T>(_variant));
    return object;
}

//------------------------------------------------------------------------------

template<typename T>
static inline void compare(const typename T::csptr& _actual, const std::size_t _variant)
{
    CHECK(_actual);
    CHECK_EQ(get_expected<T>(_variant)->get_value(), _actual->get_value());
}

//------------------------------------------------------------------------------

template<typename T>
static inline void test(const bool _encrypt, const bool _raw, const bool _empty_obj, const std::size_t _variant = 0)
{
    static constexpr auto s_PASSWORD = "password";

    const auto& test_id = T::leaf_classname() + "_" + std::to_string(_encrypt) + "_" + std::to_string(_raw);

    // Create a temporary directory
    sight::core::os::temp_dir tmp_dir;
    const auto test_path = tmp_dir / (test_id + (_raw ? ".json" : ".zip"));

    static constexpr auto s_FIELD_NAME = "field";

    // Test serialization
    {
        // Create the data object
        auto object = _empty_obj ? std::make_shared<T>() : create<T>(_variant);

        // Add a field
        object->set_field(s_FIELD_NAME, create<T>(_variant + 1));

        // Create the session writer
        auto session_writer = std::make_shared<sight::io::session::session_writer>();
        CHECK(session_writer);

        // Configure the session writer
        session_writer->set_object(object);
        session_writer->set_file(test_path);

        if(_raw)
        {
            session_writer->set_archive_format(sight::io::zip::archive::archive_format::filesystem);
        }
        else if(_encrypt)
        {
            session_writer->set_password(s_PASSWORD);
        }

        // Write the session
        auto observer = std::make_shared<sight::core::progress::observer>("Writing session... ");
        CHECK_NOTHROW(session_writer->write(observer));

        CHECK(std::filesystem::exists(test_path));
    }

    // Test deserialization
    {
        auto session_reader = std::make_shared<sight::io::session::session_reader>();
        CHECK(session_reader);

        // Configure the session reader
        session_reader->set_file(test_path);

        if(_raw)
        {
            session_reader->set_archive_format(sight::io::zip::archive::archive_format::filesystem);
        }
        else if(_encrypt)
        {
            session_reader->set_password(s_PASSWORD);
        }

        // Read the session
        auto observer = std::make_shared<sight::core::progress::observer>("Reading session... ");
        CHECK_NOTHROW(session_reader->read(observer));

        // Test value
        auto actual_object = std::dynamic_pointer_cast<T>(session_reader->get_object());

        // Create the data object
        auto expected_object = _empty_obj ? std::make_shared<T>() : create<T>(_variant);

        // Add a field
        expected_object->set_field(s_FIELD_NAME, create<T>(_variant + 1));

        CHECK(*expected_object == *actual_object);
    }
}

//------------------------------------------------------------------------------

template<typename T>
static inline void test_combine(const std::size_t _variant = 0)
{
    test<T>(false, false, false, _variant);
    test<T>(false, false, true, _variant);
    test<T>(false, true, false, _variant);
    test<T>(false, true, true, _variant);
    test<T>(true, false, false, _variant);
    test<T>(true, false, true, _variant);
}

//------------------------------------------------------------------------------

template<>
inline sight::data::boolean::sptr generate<sight::data::boolean>(const std::size_t _variant)
{
    return std::make_shared<sight::data::boolean>(_variant % 2 == 0);
}

//------------------------------------------------------------------------------

template<>
inline sight::data::dvec2::sptr generate<sight::data::dvec2>(const std::size_t _variant)
{
    auto object = std::make_shared<sight::data::dvec2>();
    (*object)[0] = static_cast<sight::data::dvec2::value_t::value_type>(_variant);
    (*object)[1] = static_cast<sight::data::dvec2::value_t::value_type>(_variant + 1);
    return object;
}

//------------------------------------------------------------------------------

template<>
inline sight::data::dvec3::sptr generate<sight::data::dvec3>(const std::size_t _variant)
{
    auto object = std::make_shared<sight::data::dvec3>();
    (*object)[0] = static_cast<sight::data::dvec3::value_t::value_type>(_variant);
    (*object)[1] = static_cast<sight::data::dvec3::value_t::value_type>(_variant + 1);
    (*object)[2] = static_cast<sight::data::dvec3::value_t::value_type>(_variant + 2);
    return object;
}

//------------------------------------------------------------------------------

template<>
inline sight::data::dvec4::sptr generate<sight::data::dvec4>(const std::size_t _variant)
{
    auto object = std::make_shared<sight::data::dvec4>();
    (*object)[0] = static_cast<sight::data::dvec4::value_t::value_type>(_variant);
    (*object)[1] = static_cast<sight::data::dvec4::value_t::value_type>(_variant + 1);
    (*object)[2] = static_cast<sight::data::dvec4::value_t::value_type>(_variant + 2);
    (*object)[3] = static_cast<sight::data::dvec4::value_t::value_type>(_variant + 3);
    return object;
}

//------------------------------------------------------------------------------

template<>
inline sight::data::ivec2::sptr generate<sight::data::ivec2>(const std::size_t _variant)
{
    auto object = std::make_shared<sight::data::ivec2>();
    (*object)[0] = static_cast<sight::data::ivec2::value_t::value_type>(_variant);
    (*object)[1] = static_cast<sight::data::ivec2::value_t::value_type>(_variant + 1);
    return object;
}

//------------------------------------------------------------------------------

template<>
inline sight::data::ivec3::sptr generate<sight::data::ivec3>(const std::size_t _variant)
{
    auto object = std::make_shared<sight::data::ivec3>();
    (*object)[0] = static_cast<sight::data::ivec3::value_t::value_type>(_variant);
    (*object)[1] = static_cast<sight::data::ivec3::value_t::value_type>(_variant + 1);
    (*object)[2] = static_cast<sight::data::ivec3::value_t::value_type>(_variant + 2);
    return object;
}

//------------------------------------------------------------------------------

template<>
inline sight::data::ivec4::sptr generate<sight::data::ivec4>(const std::size_t _variant)
{
    auto object = std::make_shared<sight::data::ivec4>();
    (*object)[0] = static_cast<sight::data::ivec4::value_t::value_type>(_variant);
    (*object)[1] = static_cast<sight::data::ivec4::value_t::value_type>(_variant + 1);
    (*object)[2] = static_cast<sight::data::ivec4::value_t::value_type>(_variant + 2);
    (*object)[3] = static_cast<sight::data::ivec4::value_t::value_type>(_variant + 3);
    return object;
}

//------------------------------------------------------------------------------

template<>
inline sight::data::string::sptr generate<sight::data::string>(const std::size_t /*unused*/)
{
    return std::make_shared<sight::data::string>(uuid::generate());
}

//------------------------------------------------------------------------------

template<>
inline sight::data::map::sptr generate<sight::data::map>(const std::size_t _variant)
{
    auto object = std::make_shared<sight::data::map>();
    (*object)[sight::data::boolean::classname()] = create<sight::data::boolean>(_variant);
    (*object)[sight::data::integer::classname()] = create<sight::data::integer>(_variant);
    (*object)[sight::data::real::classname()]    = create<sight::data::real>(_variant);
    (*object)[sight::data::string::classname()]  = create<sight::data::string>(_variant);

    return object;
}

//------------------------------------------------------------------------------

template<>
inline sight::data::mesh::sptr create<sight::data::mesh>(const std::size_t _variant)
{
    const auto& object = std::make_shared<sight::data::mesh>();
    object->deep_copy(get_expected<sight::data::mesh>(_variant));
    object->shrink_to_fit();
    return object;
}

//------------------------------------------------------------------------------

template<>
inline sight::data::mesh::sptr generate<sight::data::mesh>(const std::size_t _variant)
{
    auto object = std::make_shared<sight::data::mesh>();

    if(_variant < 10)
    {
        sight::utest_data::generator::mesh::generate_triangle_quad_mesh(object);
        sight::geometry::data::mesh::shake_point(object);
        sight::geometry::data::mesh::colorize_mesh_points(object);
        sight::geometry::data::mesh::colorize_mesh_cells(object);
        sight::geometry::data::mesh::generate_point_normals(object);
        sight::geometry::data::mesh::generate_cell_normals(object);
    }
    else if(_variant < 20)
    {
        const auto attributes = sight::data::mesh::attribute::point_normals
                                | sight::data::mesh::attribute::point_colors
                                | sight::data::mesh::attribute::point_tex_coords
                                | sight::data::mesh::attribute::cell_normals
                                | sight::data::mesh::attribute::cell_colors
                                | sight::data::mesh::attribute::cell_tex_coords;

        sight::utest_data::generator::mesh::generate_triangle_mesh(object, attributes);
        sight::geometry::data::mesh::shake_point(object);
        sight::geometry::data::mesh::colorize_mesh_points(object);
        sight::geometry::data::mesh::colorize_mesh_cells(object);
        sight::geometry::data::mesh::generate_point_normals(object);
        sight::geometry::data::mesh::generate_cell_normals(object);
    }
    else if(_variant < 30)
    {
        const auto attributes = sight::data::mesh::attribute::point_normals
                                | sight::data::mesh::attribute::point_tex_coords
                                | sight::data::mesh::attribute::cell_colors
                                | sight::data::mesh::attribute::cell_tex_coords;

        sight::utest_data::generator::mesh::generate_triangle_mesh(object, attributes);
        sight::geometry::data::mesh::shake_point(object);
        sight::geometry::data::mesh::colorize_mesh_cells(object);
        sight::geometry::data::mesh::generate_point_normals(object);
    }
    else if(_variant < 40)
    {
        const auto attributes = sight::data::mesh::attribute::point_colors
                                | sight::data::mesh::attribute::point_tex_coords
                                | sight::data::mesh::attribute::cell_colors
                                | sight::data::mesh::attribute::cell_tex_coords;

        sight::utest_data::generator::mesh::generate_triangle_mesh(object, attributes);
        sight::geometry::data::mesh::shake_point(object);
        sight::geometry::data::mesh::colorize_mesh_points(object);
        sight::geometry::data::mesh::colorize_mesh_cells(object);
    }
    else if(_variant < 50)
    {
        const auto attributes = sight::data::mesh::attribute::point_colors
                                | sight::data::mesh::attribute::cell_colors;

        sight::utest_data::generator::mesh::generate_triangle_mesh(object, attributes);
        sight::geometry::data::mesh::shake_point(object);
        sight::geometry::data::mesh::colorize_mesh_points(object);
        sight::geometry::data::mesh::colorize_mesh_cells(object);
    }
    else if(_variant < 60)
    {
        const auto attributes = sight::data::mesh::attribute::point_normals
                                | sight::data::mesh::attribute::cell_normals;

        sight::utest_data::generator::mesh::generate_triangle_mesh(object, attributes);
        sight::geometry::data::mesh::shake_point(object);
        sight::geometry::data::mesh::generate_point_normals(object);
        sight::geometry::data::mesh::generate_cell_normals(object);
    }
    else if(_variant < 70)
    {
        const auto attributes = sight::data::mesh::attribute::point_tex_coords
                                | sight::data::mesh::attribute::cell_tex_coords;

        sight::utest_data::generator::mesh::generate_triangle_mesh(object, attributes);
        sight::geometry::data::mesh::shake_point(object);
    }
    else if(_variant < 80)
    {
        const auto attributes = sight::data::mesh::attribute::point_normals
                                | sight::data::mesh::attribute::point_tex_coords
                                | sight::data::mesh::attribute::cell_normals
                                | sight::data::mesh::attribute::cell_tex_coords;

        sight::utest_data::generator::mesh::generate_triangle_mesh(object, attributes);
        sight::geometry::data::mesh::shake_point(object);
        sight::geometry::data::mesh::generate_point_normals(object);
        sight::geometry::data::mesh::generate_cell_normals(object);
    }
    else if(_variant < 90)
    {
        sight::utest_data::generator::mesh::generate_triangle_mesh(object);
        sight::geometry::data::mesh::shake_point(object);
    }

    object->shrink_to_fit();

    return object;
}

//------------------------------------------------------------------------------

template<>
inline sight::data::series::sptr generate<sight::data::series>(const std::size_t _variant)
{
    auto object = std::make_shared<sight::data::series>();

    // Fill trivial attributes
    object->set_sop_keyword(sight::data::dicom::sop::Keyword::EnhancedUSVolumeStorage);
    object->set_modality(sight::data::dicom::modality_t::us);
    object->set_series_description(uuid::generate());
    object->set_series_instance_uid(uuid::generate());
    object->set_series_number(static_cast<std::int32_t>(_variant));
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
    object->set_patient_size(static_cast<double>(_variant));
    object->set_patient_weight(static_cast<double>(_variant));

    // Other Modules
    object->set_slice_thickness(static_cast<double>(_variant));
    object->set_frame_acquisition_date_time("57", 0);
    object->set_frame_comments("58", 0);
    object->set_frame_label("59", 0);
    object->set_ultrasound_acquisition_geometry(sight::data::dicom::ultrasound_acquisition_geometry_t::patient);
    object->set_patient_frame_of_reference_source(sight::data::dicom::patient_frame_of_reference_source_t::table);
    object->set_dimension_organization_type(sight::data::dicom::dimension_organization_t::volume);

    // Generate specific instance data
    for(std::size_t i = 0 ; i < _variant + 3 ; ++i)
    {
        object->set_image_position_patient(
        {
            static_cast<double>(_variant + i),
            static_cast<double>(_variant + i + 1),
            static_cast<double>(_variant + i + 2)
        },
            i
        );

        object->set_image_orientation_patient(
        {
            static_cast<double>(_variant + i), static_cast<double>(_variant + i + 1),
            static_cast<double>(_variant + i + 2),
            static_cast<double>(_variant + i + 3), static_cast<double>(_variant + i + 4),
            static_cast<double>(_variant + i + 5)
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

template<>
inline sight::data::activity::sptr generate<sight::data::activity>(const std::size_t _variant)
{
    auto object = std::make_shared<sight::data::activity>();

    (*object)[sight::data::boolean::classname() + std::to_string(_variant)] = create<sight::data::boolean>(_variant);
    (*object)[sight::data::integer::classname() + std::to_string(_variant)] = create<sight::data::integer>(_variant);
    (*object)[sight::data::real::classname() + std::to_string(_variant)]    = create<sight::data::real>(_variant);
    (*object)[sight::data::string::classname() + std::to_string(_variant)]  = create<sight::data::string>(_variant);

    return object;
}

//------------------------------------------------------------------------------

template<>
inline sight::data::array::sptr generate<sight::data::array>(const std::size_t _variant)
{
    auto object = std::make_shared<sight::data::array>();

    const auto lock = object->dump_lock();

    auto fill =
        [&](auto _type)
        {
            using type = decltype(_type);

            object->resize(
                {_variant + 2, _variant + 2},
                std::is_same_v<type, double>
                ? sight::core::type::FLOAT64
                : std::is_same_v<type, float>
                ? sight::core::type::FLOAT32
                : std::is_same_v<type, std::uint8_t>
                ? sight::core::type::UINT8
                : std::is_same_v<type, std::uint16_t>
                ? sight::core::type::UINT16
                : std::is_same_v<type, std::uint32_t>
                ? sight::core::type::UINT32
                : std::is_same_v<type, std::uint64_t>
                ? sight::core::type::UINT64
                : std::is_same_v<type, std::int8_t>
                ? sight::core::type::INT8
                : std::is_same_v<type, std::int16_t>
                ? sight::core::type::INT16
                : std::is_same_v<type, std::int32_t>
                ? sight::core::type::INT32
                : std::is_same_v<type, std::int64_t>
                ? sight::core::type::INT64
                : sight::core::type::NONE,
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
            CHECK_MESSAGE(false, "Unknown variant.");
            break;
    }

    return object;
}

//------------------------------------------------------------------------------

template<>
inline sight::data::image::sptr generate<sight::data::image>(const std::size_t _variant)
{
    auto object = std::make_shared<sight::data::image>();

    const auto lock = object->dump_lock();

    auto fill =
        [&](auto _type)
        {
            using type = decltype(_type);

            // Warning: generateImage use reflection that cannot deal with double value (truncated to float precision),
            // thus the 0.1 + static_cast<double>(variant)
            sight::utest_data::generator::image::generate_image(
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
                ? sight::core::type::FLOAT64
                : std::is_same_v<type, float>
                ? sight::core::type::FLOAT32
                : std::is_same_v<type, std::uint8_t>
                ? sight::core::type::UINT8
                : std::is_same_v<type, std::uint16_t>
                ? sight::core::type::UINT16
                : std::is_same_v<type, std::uint32_t>
                ? sight::core::type::UINT32
                : std::is_same_v<type, std::uint64_t>
                ? sight::core::type::UINT64
                : std::is_same_v<type, std::int8_t>
                ? sight::core::type::INT8
                : std::is_same_v<type, std::int16_t>
                ? sight::core::type::INT16
                : std::is_same_v<type, std::int32_t>
                ? sight::core::type::INT32
                : std::is_same_v<type, std::int64_t>
                ? sight::core::type::INT64
                : sight::core::type::NONE,

                std::is_same_v<type, double>
                ? sight::data::image::pixel_format_t::gray_scale
                : std::is_same_v<type, float>
                ? sight::data::image::pixel_format_t::gray_scale
                : std::is_same_v<type, std::uint8_t>
                ? sight::data::image::pixel_format_t::bgr
                : std::is_same_v<type, std::uint16_t>
                ? sight::data::image::pixel_format_t::bgra
                : std::is_same_v<type, std::uint32_t>
                ? sight::data::image::pixel_format_t::rgb
                : std::is_same_v<type, std::uint64_t>
                ? sight::data::image::pixel_format_t::rgba
                : std::is_same_v<type, std::int8_t>
                ? sight::data::image::pixel_format_t::gray_scale
                : std::is_same_v<type, std::int16_t>
                ? sight::data::image::pixel_format_t::bgr
                : std::is_same_v<type, std::int32_t>
                ? sight::data::image::pixel_format_t::bgra
                : std::is_same_v<type, std::int64_t>
                ? sight::data::image::pixel_format_t::rgb
                : sight::data::image::pixel_format_t::undefined,
                static_cast<std::uint32_t>(_variant)
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
            CHECK_MESSAGE(false, "Unknown variant.");
            break;
    }

    return object;
}

//------------------------------------------------------------------------------

template<>
inline sight::data::vector::sptr generate<sight::data::vector>(const std::size_t _variant)
{
    auto object = std::make_shared<sight::data::vector>();

    object->push_back(create<sight::data::boolean>(_variant));
    object->push_back(create<sight::data::integer>(_variant));
    object->push_back(create<sight::data::real>(_variant));
    object->push_back(create<sight::data::string>(_variant));
    object->push_back(create<sight::data::activity>(_variant));

    return object;
}

//------------------------------------------------------------------------------

template<>
inline sight::data::point::sptr generate<sight::data::point>(const std::size_t /*unused*/)
{
    auto object = std::make_shared<sight::data::point>();

    *object = {random<double>(), random<double>(), random<double>()};

    return object;
}

//------------------------------------------------------------------------------

template<>
inline sight::data::point_list::sptr generate<sight::data::point_list>(const std::size_t _variant)
{
    auto points = std::make_shared<sight::data::point_list>();

    for(std::size_t i = 0, end = _variant + 3 ; i < end ; ++i)
    {
        points->push_back(create<sight::data::point>(i));
    }

    return points;
}

//------------------------------------------------------------------------------

template<>
inline sight::data::calibration_info::sptr generate<sight::data::calibration_info>(const std::size_t _variant)
{
    auto object = std::make_shared<sight::data::calibration_info>();

    for(std::size_t i = 0, end = _variant + 2 ; i < end ; ++i)
    {
        // Create the image
        auto image = create<sight::data::image>(_variant + i);

        // Create the PointList
        auto point_list = create<sight::data::point_list>(_variant + i);

        object->add_record(image, point_list);
    }

    return object;
}

//------------------------------------------------------------------------------

template<>
inline sight::data::camera::sptr generate<sight::data::camera>(const std::size_t _variant)
{
    auto object = std::make_shared<sight::data::camera>();

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
        sight::data::camera::pixel_format_t::adobedng,
        sight::data::camera::pixel_format_t::argb32,
        sight::data::camera::pixel_format_t::argb32_premultiplied,
        sight::data::camera::pixel_format_t::rgb32,
        sight::data::camera::pixel_format_t::rgb24,
        sight::data::camera::pixel_format_t::rgb565,
        sight::data::camera::pixel_format_t::rgb555,
        sight::data::camera::pixel_format_t::argb8565_premultiplied,
        sight::data::camera::pixel_format_t::bgra32,
        sight::data::camera::pixel_format_t::bgra32_premultiplied,
        sight::data::camera::pixel_format_t::bgr32,
        sight::data::camera::pixel_format_t::bgr24,
        sight::data::camera::pixel_format_t::bgr565,
        sight::data::camera::pixel_format_t::bgr555,
        sight::data::camera::pixel_format_t::bgra5658_premultiplied,
        sight::data::camera::pixel_format_t::ayuv444,
        sight::data::camera::pixel_format_t::ayuv444_premultiplied,
        sight::data::camera::pixel_format_t::yuv444,
        sight::data::camera::pixel_format_t::yuv420_p,
        sight::data::camera::pixel_format_t::yv12,
        sight::data::camera::pixel_format_t::uyvy,
        sight::data::camera::pixel_format_t::yuyv,
        sight::data::camera::pixel_format_t::nv12,
        sight::data::camera::pixel_format_t::nv21,
        sight::data::camera::pixel_format_t::imc1,
        sight::data::camera::pixel_format_t::imc2,
        sight::data::camera::pixel_format_t::imc3,
        sight::data::camera::pixel_format_t::imc4,
        sight::data::camera::pixel_format_t::y8,
        sight::data::camera::pixel_format_t::y16,
        sight::data::camera::pixel_format_t::jpeg,
        sight::data::camera::pixel_format_t::cameraraw,
        sight::data::camera::pixel_format_t::adobedng,
        sight::data::camera::pixel_format_t::rgba32,
        sight::data::camera::pixel_format_t::user,
        sight::data::camera::pixel_format_t::invalid
    };
    object->set_pixel_format(pixel_formats[_variant % 35]);
    object->set_video_file("/" + uuid::generate());
    object->set_stream_url(uuid::generate());
    object->set_camera_source(
        _variant % 3 == 0
        ? sight::data::camera::source_t::device
        : _variant % 3 == 1
        ? sight::data::camera::source_t::file
        : _variant % 3 == 2
        ? sight::data::camera::source_t::stream
        : sight::data::camera::source_t::unknown
    );
    object->set_scale(random<double>());

    return object;
}

//------------------------------------------------------------------------------

template<>
inline sight::data::color::sptr generate<sight::data::color>(const std::size_t /*unused*/)
{
    auto object = std::make_shared<sight::data::color>();

    object->set_rgba(random<float>(), random<float>(), random<float>(), random<float>());

    return object;
}

//------------------------------------------------------------------------------

template<>
inline sight::data::line::sptr generate<sight::data::line>(const std::size_t _variant)
{
    auto object = std::make_shared<sight::data::line>();

    object->set_position(create<sight::data::point>(_variant));
    object->set_direction(create<sight::data::point>(_variant + 1));

    return object;
}

//------------------------------------------------------------------------------

template<>
inline sight::data::material::sptr generate<sight::data::material>(const std::size_t _variant)
{
    auto object = std::make_shared<sight::data::material>();

    std::array shading {
        sight::data::material::shading_t::ambient,
        sight::data::material::shading_t::flat,
        sight::data::material::shading_t::phong
    };

    std::array representation {
        sight::data::material::representation_t::edge,
        sight::data::material::representation_t::point,
        sight::data::material::representation_t::surface,
        sight::data::material::representation_t::wireframe
    };

    std::array options {
        sight::data::material::options_t::cells_normals,
        sight::data::material::options_t::normals,
        sight::data::material::options_t::standard
    };

    // Set ambient color
    object->set_ambient(create<sight::data::color>(_variant));

    // Set diffuse color
    object->set_diffuse(create<sight::data::color>(_variant + 1));

    // Set diffuse texture
    object->set_diffuse_texture(create<sight::data::image>(_variant));

    // Others
    object->set_shading_mode(shading[_variant % std::size(shading)]);
    object->set_representation_mode(representation[_variant % std::size(representation)]);
    object->set_options_mode(options[_variant % std::size(options)]);
    object->set_diffuse_texture_filtering(
        _variant % 3 == 0
        ? sight::data::material::filtering_t::linear
        : sight::data::material::filtering_t::nearest
    );
    object->set_diffuse_texture_wrapping(
        _variant % 3 == 0
        ? sight::data::material::wrapping_t::clamp
        : sight::data::material::wrapping_t::repeat
    );

    return object;
}

//------------------------------------------------------------------------------

template<>
inline sight::data::matrix4::sptr generate<sight::data::matrix4>(const std::size_t /*unused*/)
{
    auto object = std::make_shared<sight::data::matrix4>();

    for(double& coefficient : *object)
    {
        coefficient = random<double>();
    }

    return object;
}

//------------------------------------------------------------------------------

template<>
inline sight::data::plane::sptr generate<sight::data::plane>(const std::size_t _variant)
{
    auto object = std::make_shared<sight::data::plane>();

    auto& points = object->get_points();
    for(std::size_t i = 0, end = points.size() ; i < end ; ++i)
    {
        points[i] = create<sight::data::point>(i + _variant);
    }

    return object;
}

//------------------------------------------------------------------------------

template<>
inline sight::data::plane_list::sptr generate<sight::data::plane_list>(const std::size_t _variant)
{
    auto object = std::make_shared<sight::data::plane_list>();

    auto& planes = object->get_planes();

    for(std::size_t i = 0, end = _variant + 2 ; i < end ; ++i)
    {
        planes.push_back(create<sight::data::plane>(_variant + i));
    }

    return object;
}

//------------------------------------------------------------------------------

template<>
inline sight::data::reconstruction::sptr generate<sight::data::reconstruction>(const std::size_t _variant)
{
    auto object = std::make_shared<sight::data::reconstruction>();

    object->set_is_visible(_variant % 3 == 0);
    object->set_organ_name(uuid::generate());
    object->set_structure_type(uuid::generate());
    object->set_computed_mask_volume(random<double>());
    object->set_label(random<std::uint32_t>());

    // Material
    object->set_material(create<sight::data::material>(_variant));

    // image
    object->set_image(create<sight::data::image>(_variant));

    // Mesh
    object->set_mesh(create<sight::data::mesh>(_variant));

    return object;
}

//------------------------------------------------------------------------------

template<>
inline sight::data::structure_traits::sptr generate<sight::data::structure_traits>(const std::size_t _variant)
{
    auto object = std::make_shared<sight::data::structure_traits>();

    static constexpr std::array s_CLASSES {
        sight::data::structure_traits::structure_class::environment,
        sight::data::structure_traits::structure_class::functional,
        sight::data::structure_traits::structure_class::lesion,
        sight::data::structure_traits::structure_class::no_constraint,
        sight::data::structure_traits::structure_class::organ,
        sight::data::structure_traits::structure_class::tool,
        sight::data::structure_traits::structure_class::vessel
    };

    static constexpr std::array s_CATEGORIES {
        sight::data::structure_traits::category::abdomen,
        sight::data::structure_traits::category::arm,
        sight::data::structure_traits::category::body,
        sight::data::structure_traits::category::head,
        sight::data::structure_traits::category::leg,
        sight::data::structure_traits::category::liver_segments,
        sight::data::structure_traits::category::neck,
        sight::data::structure_traits::category::other,
        sight::data::structure_traits::category::pelvis,
        sight::data::structure_traits::category::thorax
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
    sight::data::structure_traits::category_container_t categories;

    for(std::size_t i = 0, end = _variant + 2 ; i < end ; ++i)
    {
        categories.push_back(s_CATEGORIES[(i + _variant) % std::size(s_CATEGORIES)]);
    }

    object->set_categories(categories);

    // Color
    object->set_color(create<sight::data::color>(_variant));

    return object;
}

//------------------------------------------------------------------------------

template<>
inline sight::data::structure_traits_dictionary::sptr generate<sight::data::structure_traits_dictionary>(
    const std::size_t _variant
)
{
    auto object = std::make_shared<sight::data::structure_traits_dictionary>();

    auto organ = create<sight::data::structure_traits>(_variant);
    organ->set_class(sight::data::structure_traits::organ);
    organ->set_attachment_type("");

    object->add_structure(organ);

    for(std::size_t i = 0, end = _variant + 2 ; i < end ; ++i)
    {
        auto structure             = create<sight::data::structure_traits>(_variant + i + 1);
        const auto structure_class = structure->get_class();

        if(structure_class != sight::data::structure_traits::lesion
           && structure_class != sight::data::structure_traits::functional)
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

template<>
inline sight::data::resection::sptr generate<sight::data::resection>(const std::size_t _variant)
{
    auto object = std::make_shared<sight::data::resection>();

    object->set_name(uuid::generate());
    object->set_is_safe_part(_variant % 2 == 0);
    object->set_is_valid(_variant % 3 == 0);
    object->set_is_visible(_variant % 4 == 0);

    auto& inputs  = object->get_inputs();
    auto& outputs = object->get_outputs();
    for(std::size_t i = 0, end = _variant + 2 ; i < end ; ++i)
    {
        inputs.push_back(create<sight::data::reconstruction>(_variant + i));
        outputs.push_back(create<sight::data::reconstruction>(_variant + i + 1));
    }

    return object;
}

//------------------------------------------------------------------------------

template<>
inline sight::data::resection_db::sptr generate<sight::data::resection_db>(const std::size_t _variant)
{
    auto object = std::make_shared<sight::data::resection_db>();

    object->set_safe_resection(create<sight::data::resection>(_variant));

    for(std::size_t i = 0, end = _variant + 2 ; i < end ; ++i)
    {
        object->add_resection(create<sight::data::resection>(_variant + i + 1));
    }

    return object;
}

//------------------------------------------------------------------------------

template<>
inline sight::data::transfer_function::sptr generate<sight::data::transfer_function>(const std::size_t _variant)
{
    auto object = std::make_shared<sight::data::transfer_function>();

    object->set_level(random<double>());
    object->set_window(random<double>());
    object->set_name(uuid::generate());
    object->set_background_color(
        sight::data::transfer_function::color_t(
            random<double>(),
            random<double>(),
            random<double>(),
            random<double>()
        )
    );

    auto tf_data = object->pieces().emplace_back(std::make_shared<sight::data::transfer_function_piece>());
    tf_data->set_interpolation_mode(
        _variant % 3 == 0
        ? sight::data::transfer_function::interpolation_mode::linear
        : sight::data::transfer_function::interpolation_mode::nearest
    );
    tf_data->set_clamped(_variant % 4 == 0);

    for(std::size_t i = 0, end = _variant + 2 ; i < end ; ++i)
    {
        tf_data->insert(
        {
            random<double>(),
            sight::data::transfer_function::color_t(
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

template<>
inline sight::data::image_series::sptr generate<sight::data::image_series>(const std::size_t _variant)
{
    auto object = std::make_shared<sight::data::image_series>();

    // Inherited attributes
    object->image::shallow_copy(get_expected<sight::data::image>(_variant));
    object->series::shallow_copy(get_expected<sight::data::series>(_variant));

    // Children
    object->get_fiducials()->shallow_copy(get_expected<sight::data::fiducials_series>(_variant));

    object->set_contrast_bolus_agent(uuid::generate());
    object->set_contrast_bolus_route(uuid::generate());
    object->set_contrast_bolus_volume(static_cast<double>(_variant));
    object->set_contrast_bolus_start_time(generate_tm(_variant));
    object->set_contrast_bolus_stop_time(generate_tm(_variant));
    object->set_contrast_bolus_total_dose(static_cast<double>(_variant));

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
    object->set_contrast_bolus_ingredient_concentration(static_cast<double>(_variant));
    object->set_acquisition_date(generate_da(_variant));
    object->set_acquisition_time(generate_tm(_variant));

    return object;
}

//------------------------------------------------------------------------------

template<>
inline sight::data::model_series::sptr generate<sight::data::model_series>(const std::size_t _variant)
{
    auto object = std::make_shared<sight::data::model_series>();

    std::vector<sight::data::reconstruction::sptr> reconstruction_db;
    for(std::size_t i = 0, end = _variant + 2 ; i < end ; ++i)
    {
        reconstruction_db.push_back(create<sight::data::reconstruction>(_variant + i));
    }

    object->set_reconstruction_db(reconstruction_db);

    // Inherited attributes
    object->series::shallow_copy(get_expected<sight::data::series>(_variant));

    return object;
}

//------------------------------------------------------------------------------

template<>
inline sight::data::fiducials_series::sptr generate<sight::data::fiducials_series>(const std::size_t _variant)
{
    auto fs = std::make_shared<sight::data::fiducials_series>();
    fs->set_content_label(std::to_string(_variant));
    fs->set_content_description("FS");
    fs->set_content_creator_name("John Doe");

    // Adds a dummy fiducial set
    sight::data::fiducials_series::fiducial_set fiducial_set;

    sight::data::fiducials_series::referenced_image referenced_image;
    referenced_image.referenced_sop_class_uid    = "1";
    referenced_image.referenced_sop_instance_uid = "2";
    referenced_image.referenced_frame_number     = {3};
    referenced_image.referenced_segment_number   = {4};
    fiducial_set.referenced_image_sequence       = {referenced_image};

    fiducial_set.frame_of_reference_uid = "5";

    sight::data::fiducials_series::fiducial fiducial;
    fiducial.shape_type           = sight::data::fiducials_series::shape::point;
    fiducial.fiducial_description = "6";
    fiducial.fiducial_identifier  = "7";

    sight::data::fiducials_series::graphic_coordinates_data graphic_coordinates_data;
    graphic_coordinates_data.referenced_image_sequence.referenced_sop_class_uid    = "8";
    graphic_coordinates_data.referenced_image_sequence.referenced_sop_instance_uid = "9";
    graphic_coordinates_data.referenced_image_sequence.referenced_frame_number     = {10};
    graphic_coordinates_data.referenced_image_sequence.referenced_segment_number   = {11};
    graphic_coordinates_data.graphic_data                                          = {{.x = 12, .y = 13}};
    fiducial.graphic_coordinates_data_sequence                                     = {graphic_coordinates_data};

    fiducial.fiducial_uid = "14";
    fiducial.contour_data = {{.x = 15, .y = 16, .z = 17}};
    fiducial_set.fiducial_sequence.push_back(fiducial);

    fiducial_set.group_name = "18";
    fiducial_set.color      = {{19, 20, 21, 22}};
    fiducial_set.size       = 23.F;
    fiducial_set.shape      = sight::data::fiducials_series::private_shape::cube;
    fiducial_set.visibility = true;

    // Test setFiducialSets method
    fs->set_fiducial_sets({fiducial_set});

    return fs;
}

//------------------------------------------------------------------------------

template<>
inline sight::data::activity_set::sptr generate<sight::data::activity_set>(const std::size_t _variant)
{
    auto object = std::make_shared<sight::data::activity_set>();

    for(std::size_t i = 0, end = _variant + 2 ; i < end ; ++i)
    {
        object->push_back(create<sight::data::activity>(_variant + i));
    }

    return object;
}

//------------------------------------------------------------------------------

template<>
inline sight::data::camera_set::sptr generate<sight::data::camera_set>(const std::size_t _variant)
{
    auto object = std::make_shared<sight::data::camera_set>();

    for(std::size_t i = 0, end = _variant + 2 ; i < end ; ++i)
    {
        auto camera = create<sight::data::camera>(_variant + i);
        auto matrix = create<sight::data::matrix4>(_variant + i);
        object->push_back(std::make_pair(camera, matrix));
    }

    return object;
}

//------------------------------------------------------------------------------

template<>
inline sight::data::series_set::sptr generate<sight::data::series_set>(const std::size_t _variant)
{
    auto object = std::make_shared<sight::data::series_set>();

    for(std::size_t i = 0, end = _variant + 2 ; i < end ; ++i)
    {
        object->push_back(create<sight::data::series>(_variant + i));
    }

    return object;
}

//------------------------------------------------------------------------------

template<>
inline sight::data::set::sptr generate<sight::data::set>(const std::size_t _variant)
{
    auto object = std::make_shared<sight::data::set>();

    for(std::size_t i = 0, end = _variant + 2 ; i < end ; ++i)
    {
        object->push_back(create<sight::data::series>(_variant + i));
    }

    return object;
}

TEST_SUITE("sight::io::session")
{
    TEST_CASE("boolean")
    {
        test_combine<sight::data::boolean>();
    }

//------------------------------------------------------------------------------

    TEST_CASE("integer")
    {
        test_combine<sight::data::integer>();
    }

//------------------------------------------------------------------------------

    TEST_CASE("float")
    {
        test_combine<sight::data::real>();
    }

//------------------------------------------------------------------------------

    TEST_CASE("vec")
    {
        test_combine<sight::data::dvec2>();
        test_combine<sight::data::dvec3>();
        test_combine<sight::data::dvec4>();
        test_combine<sight::data::ivec2>();
        test_combine<sight::data::ivec3>();
        test_combine<sight::data::ivec4>();
    }

//------------------------------------------------------------------------------

    TEST_CASE("string")
    {
        test_combine<sight::data::string>();
    }

//------------------------------------------------------------------------------

    TEST_CASE("map")
    {
        test_combine<sight::data::map>();
    }

//------------------------------------------------------------------------------

    TEST_CASE("mesh")
    {
        test_combine<sight::data::mesh>();
        test_combine<sight::data::mesh>(10);
        test_combine<sight::data::mesh>(20);
        test_combine<sight::data::mesh>(30);
        test_combine<sight::data::mesh>(40);
        test_combine<sight::data::mesh>(50);
        test_combine<sight::data::mesh>(60);
        test_combine<sight::data::mesh>(70);
        test_combine<sight::data::mesh>(80);
    }

//------------------------------------------------------------------------------

    TEST_CASE("series")
    {
        test_combine<sight::data::series>();
    }

//------------------------------------------------------------------------------

    TEST_CASE("activity")
    {
        test_combine<sight::data::activity>();
    }

//------------------------------------------------------------------------------

    TEST_CASE("array")
    {
        test_combine<sight::data::array>();
    }

//------------------------------------------------------------------------------

    TEST_CASE("image")
    {
        test_combine<sight::data::image>();
    }

//------------------------------------------------------------------------------

    TEST_CASE("vector")
    {
        test_combine<sight::data::vector>();
    }
//------------------------------------------------------------------------------

    TEST_CASE("point")
    {
        test_combine<sight::data::point>();
    }

//------------------------------------------------------------------------------

    TEST_CASE("point_list")
    {
        test_combine<sight::data::point_list>();
    }

//------------------------------------------------------------------------------

    TEST_CASE("calibration_info")
    {
        test_combine<sight::data::calibration_info>();
    }

//------------------------------------------------------------------------------

    TEST_CASE("camera")
    {
        test_combine<sight::data::camera>();
    }

//------------------------------------------------------------------------------

    TEST_CASE("color")
    {
        test_combine<sight::data::color>();
    }

//------------------------------------------------------------------------------

    TEST_CASE("line")
    {
        test_combine<sight::data::line>();
    }

//------------------------------------------------------------------------------

    TEST_CASE("material")
    {
        test_combine<sight::data::material>();
    }

//------------------------------------------------------------------------------

    TEST_CASE("matrix4")
    {
        test_combine<sight::data::matrix4>();
    }

//------------------------------------------------------------------------------

    TEST_CASE("plane")
    {
        test_combine<sight::data::plane>();
    }

//------------------------------------------------------------------------------

    TEST_CASE("plane_list")
    {
        test_combine<sight::data::plane_list>();
    }

//------------------------------------------------------------------------------

    TEST_CASE("reconstruction")
    {
        test_combine<sight::data::reconstruction>();
    }

//------------------------------------------------------------------------------

    TEST_CASE("structure_traits")
    {
        test_combine<sight::data::structure_traits>();
    }

//------------------------------------------------------------------------------

    TEST_CASE("structure_traits_dictionary")
    {
        test_combine<sight::data::structure_traits_dictionary>();
    }

//------------------------------------------------------------------------------

    TEST_CASE("resection")
    {
        test_combine<sight::data::resection>();
    }

//------------------------------------------------------------------------------

    TEST_CASE("resection_db")
    {
        test_combine<sight::data::resection_db>();
    }

//------------------------------------------------------------------------------

    TEST_CASE("transfer_function")
    {
        test_combine<sight::data::transfer_function>();
    }

//------------------------------------------------------------------------------

    TEST_CASE("image_series")
    {
        if(sight::utest::filter::ignore_slow_tests())
        {
            return;
        }

        test_combine<sight::data::image_series>();
    }

//------------------------------------------------------------------------------

    TEST_CASE("model_series")
    {
        if(sight::utest::filter::ignore_slow_tests())
        {
            return;
        }

        test_combine<sight::data::model_series>();
    }

//------------------------------------------------------------------------------

    TEST_CASE("fiducials_series")
    {
        if(sight::utest::filter::ignore_slow_tests())
        {
            return;
        }

        test_combine<sight::data::fiducials_series>();
    }

//------------------------------------------------------------------------------

    TEST_CASE("activity_set")
    {
        test_combine<sight::data::activity_set>();
    }

//------------------------------------------------------------------------------

    TEST_CASE("camera_set")
    {
        test_combine<sight::data::camera_set>();
    }

//------------------------------------------------------------------------------

    TEST_CASE("series_set")
    {
        test_combine<sight::data::series_set>();
    }

//------------------------------------------------------------------------------

    TEST_CASE("set")
    {
        test_combine<sight::data::set>();
    }

//------------------------------------------------------------------------------

    inline static void custom_serialize(
        sight::io::zip::archive_writer& /*unused*/,
        boost::property_tree::ptree& _tree,
        sight::data::object::csptr _object,
        std::map<std::string, sight::data::object::csptr>& /*unused*/,
        const sight::core::crypto::secure_string& /*unused*/ = ""
)
    {
        // Cast to the right type
        const auto string = sight::io::session::helper::safe_cast<sight::data::string>(_object);

        // Add a version number. Not mandatory, but could help for future release
        sight::io::session::helper::write_version<sight::data::string>(_tree, 666);

        sight::io::session::helper::write_string(_tree, "custom", string->get_value());
    }

//------------------------------------------------------------------------------

    inline static sight::data::string::sptr custom_deserialize(
        sight::io::zip::archive_reader& /*unused*/,
        const boost::property_tree::ptree& _tree,
        const std::map<std::string, sight::data::object::sptr>& /*unused*/,
        sight::data::object::sptr _object,
        const sight::core::crypto::secure_string& /*unused*/ = ""
)
    {
        // Create or reuse the object
        auto string = sight::io::session::helper::cast_or_create<sight::data::string>(_object);

        // Check version number. Not mandatory, but could help for future release
        sight::io::session::helper::read_version<sight::data::string>(_tree, 0, 666);

        // Assign the value
        string->set_value(sight::io::session::helper::read_string(_tree, "custom"));

        return string;
    }

//------------------------------------------------------------------------------

    TEST_CASE("custom_serializer")
    {
        // Create a temporary directory
        sight::core::os::temp_dir tmp_dir;
        const auto test_path = tmp_dir / "customSerializerTest.zip";

        // Test serialization
        {
            // Create the data object
            auto object = create<sight::data::string>(0);

            // Create the session writer
            auto session_writer = std::make_shared<sight::io::session::session_writer>();
            CHECK(session_writer);

            // Configure the session writer
            session_writer->set_object(object);
            session_writer->set_file(test_path);

            // Test serializer getter
            CHECK(session_writer->serializer(sight::data::string::classname()));

            // Change the session serializer by setting a new one using setCustomSerializer
            session_writer->set_custom_serializer(sight::data::string::classname(), custom_serialize);

            // Write the new session
            const auto observer = std::make_shared<sight::core::progress::observer>("Session Writer Test");
            CHECK_NOTHROW(session_writer->write(observer));

            CHECK(std::filesystem::exists(test_path));
        }

        // Test deserialization
        {
            auto session_reader = std::make_shared<sight::io::session::session_reader>();
            CHECK(session_reader);

            // Configure the session reader
            session_reader->set_file(test_path);

            // Read the session: it should fail since the serializer has been modified by a custom one
            const auto observer = std::make_shared<sight::core::progress::observer>("Session Reader Test");
            CHECK_THROWS_AS(session_reader->read(observer), sight::core::exception);

            // Test deserializer getter
            CHECK(session_reader->deserializer(sight::data::string::classname()));

            // Set the new customDeserializer
            session_reader->set_custom_deserializer(sight::data::string::classname(), custom_deserialize);

            CHECK_NOTHROW(session_reader->read(observer));

            // Test value
            auto object = std::dynamic_pointer_cast<sight::data::string>(session_reader->get_object());
            compare<sight::data::string>(object, 0);
        }
    }
}
