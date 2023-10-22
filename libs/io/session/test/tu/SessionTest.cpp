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

#include "SessionTest.hpp"

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
#include <data/composite.hpp>
#include <data/dicom_series.hpp>
#include <data/image.hpp>
#include <data/image_series.hpp>
#include <data/integer.hpp>
#include <data/landmarks.hpp>
#include <data/line.hpp>
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
#include <io/session/SessionReader.hpp>
#include <io/session/SessionWriter.hpp>
#include <io/zip/exception/Read.hpp>
#include <io/zip/exception/Write.hpp>

#include <utest/Filter.hpp>

#include <utest_data/Data.hpp>
#include <utest_data/generator/image.hpp>
#include <utest_data/generator/mesh.hpp>

#include <random>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::session::ut::SessionTest);

namespace sight::io::session::ut
{

// For UUID::generate();
using core::tools::UUID;

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

void SessionTest::setUp()
{
}

//------------------------------------------------------------------------------

void SessionTest::tearDown()
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
        object->setDescription(UUID::generate());

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
    CPPUNIT_ASSERT_EQUAL(get_expected<T>(_variant)->getValue(), _actual->getValue());
}

//------------------------------------------------------------------------------

template<typename T>
inline void test(const bool _encrypt, const bool _raw, const bool _empty_obj = false)
{
    static constexpr auto password = "password";

    const auto& test_id = T::leaf_classname() + "_" + std::to_string(_encrypt) + "_" + std::to_string(_raw);

    // Create a temporary directory
    core::os::temp_dir tmp_dir;
    const auto test_path = tmp_dir / (test_id + (_raw ? ".json" : ".zip"));

    static constexpr auto fieldName = "field";

    // Test serialization
    {
        // Create the data object
        auto object = _empty_obj ? std::make_shared<T>() : create<T>(0);

        // Add a field
        object->set_field(fieldName, create<T>(1));

        // Create the session writer
        auto session_writer = std::make_shared<io::session::SessionWriter>();
        CPPUNIT_ASSERT(session_writer);

        // Configure the session writer
        session_writer->set_object(object);
        session_writer->set_file(test_path);

        if(_raw)
        {
            session_writer->setArchiveFormat(io::zip::Archive::ArchiveFormat::FILESYSTEM);
        }
        else if(_encrypt)
        {
            session_writer->setPassword(password);
        }

        // Write the session
        CPPUNIT_ASSERT_NO_THROW(session_writer->write());

        CPPUNIT_ASSERT(std::filesystem::exists(test_path));
    }

    // Test deserialization
    {
        auto session_reader = std::make_shared<io::session::SessionReader>();
        CPPUNIT_ASSERT(session_reader);

        // Configure the session reader
        session_reader->set_file(test_path);

        if(_raw)
        {
            session_reader->setArchiveFormat(io::zip::Archive::ArchiveFormat::FILESYSTEM);
        }
        else if(_encrypt)
        {
            session_reader->setPassword(password);
        }

        // Read the session
        CPPUNIT_ASSERT_NO_THROW(session_reader->read());

        // Test value
        auto actual_object = std::dynamic_pointer_cast<T>(session_reader->getObject());

        // Create the data object
        auto expected_object = _empty_obj ? std::make_shared<T>() : create<T>(0);

        // Add a field
        expected_object->set_field(fieldName, create<T>(1));

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

void SessionTest::booleanTest()
{
    test_combine<data::boolean>();
}

//------------------------------------------------------------------------------

void SessionTest::integerTest()
{
    test_combine<data::integer>();
}

//------------------------------------------------------------------------------

void SessionTest::floatTest()
{
    test_combine<data::real>();
}

//------------------------------------------------------------------------------

template<>
inline data::string::sptr generate<data::string>(const std::size_t /*unused*/)
{
    return std::make_shared<data::string>(UUID::generate());
}

//------------------------------------------------------------------------------

void SessionTest::stringTest()
{
    test_combine<data::string>();
}

//------------------------------------------------------------------------------

template<>
inline data::composite::sptr generate<data::composite>(const std::size_t _variant)
{
    auto object = std::make_shared<data::composite>();
    (*object)[data::boolean::classname()] = create<data::boolean>(_variant);
    (*object)[data::integer::classname()] = create<data::integer>(_variant);
    (*object)[data::real::classname()]    = create<data::real>(_variant);
    (*object)[data::string::classname()]  = create<data::string>(_variant);

    return object;
}

//------------------------------------------------------------------------------

void SessionTest::compositeTest()
{
    test_combine<data::composite>();
}

//------------------------------------------------------------------------------

template<>
inline data::mesh::sptr create<data::mesh>(const std::size_t _variant)
{
    const auto& object = std::make_shared<data::mesh>();
    object->deep_copy(get_expected<data::mesh>(_variant));
    object->shrinkToFit();
    return object;
}

//------------------------------------------------------------------------------

template<>
inline data::mesh::sptr generate<data::mesh>(const std::size_t /*unused*/)
{
    auto object = std::make_shared<data::mesh>();

    utest_data::generator::mesh::generateTriangleQuadMesh(object);
    geometry::data::mesh::shakePoint(object);
    geometry::data::mesh::colorizeMeshPoints(object);
    geometry::data::mesh::colorizeMeshCells(object);
    geometry::data::mesh::generatePointNormals(object);
    geometry::data::mesh::generateCellNormals(object);
    object->shrinkToFit();

    return object;
}

//------------------------------------------------------------------------------

void SessionTest::meshTest()
{
    test_combine<data::mesh>();
}

//------------------------------------------------------------------------------

template<>
inline data::series::sptr generate<data::series>(const std::size_t _variant)
{
    auto object = std::make_shared<data::series>();

    // Fill trivial attributes
    object->setSOPKeyword(sight::data::dicom::sop::Keyword::CTImageStorage);
    object->setModality(UUID::generate());
    object->setSeriesDescription(UUID::generate());
    object->setSeriesInstanceUID(UUID::generate());
    object->setSeriesNumber(std::int32_t(_variant));
    object->setLaterality(UUID::generate());
    object->setSeriesDate(generate_da(_variant));
    object->setSeriesTime(generate_tm(_variant));
    object->setPerformingPhysicianName(UUID::generate() + "\\" + UUID::generate());
    object->setProtocolName(UUID::generate());
    object->setBodyPartExamined(UUID::generate());
    object->setPatientPosition(UUID::generate());
    object->set_anatomical_orientation_type(UUID::generate());
    object->setPerformedProcedureStepID(UUID::generate());
    object->setPerformedProcedureStepStartDate(generate_da(_variant));
    object->setPerformedProcedureStepStartTime(generate_tm(_variant));
    object->setPerformedProcedureStepEndDate(generate_da(_variant));
    object->setPerformedProcedureStepEndTime(generate_tm(_variant));
    object->setPerformedProcedureStepDescription(UUID::generate());
    object->setCommentsOnThePerformedProcedureStep(UUID::generate());

    // Equipment Module
    object->setInstitutionName(UUID::generate());

    // Patient Module
    object->setPatientName(UUID::generate());
    object->setPatientID(UUID::generate());
    object->setPatientBirthDate(generate_da(_variant));
    object->setPatientSex(UUID::generate());

    // Study Module
    object->setStudyDescription(UUID::generate());
    object->setStudyInstanceUID(UUID::generate());
    object->setStudyID(UUID::generate());
    object->setStudyDate(generate_da(_variant));
    object->setStudyTime(generate_tm(_variant));
    object->setReferringPhysicianName(UUID::generate());

    // Patient Study Module
    object->setPatientAge(UUID::generate());
    object->setPatientSize(double(_variant));
    object->setPatientWeight(double(_variant));

    // Generate specific instance data
    for(std::size_t i = 0 ; i < _variant + 3 ; ++i)
    {
        object->setImagePositionPatient(
            {
                double(_variant + i),
                double(_variant + i + 1),
                double(_variant + i + 2)
            },
            i
        );

        object->setImageOrientationPatient(
            {
                double(_variant + i), double(_variant + i + 1), double(_variant + i + 2),
                double(_variant + i + 3), double(_variant + i + 4), double(_variant + i + 5)
            },
            i
        );

        object->setFrameAcquisitionDateTime(generate_dt(_variant + i), i);
        object->setFrameComments(UUID::generate(), i);
        object->setFrameLabel(UUID::generate(), i);
    }

    // Test private tag...
    object->setPrivateValue(UUID::generate(), 0x10);
    object->setMultiFramePrivateValue(UUID::generate(), 0x15, 0);

    return object;
}

//------------------------------------------------------------------------------

void SessionTest::seriesTest()
{
    test_combine<data::series>();
}

//------------------------------------------------------------------------------

template<>
inline data::activity::sptr generate<data::activity>(const std::size_t _variant)
{
    auto object = std::make_shared<data::activity>();

    (*object)[data::boolean::classname()] = create<data::boolean>(_variant);
    (*object)[data::integer::classname()] = create<data::integer>(_variant);
    (*object)[data::real::classname()]    = create<data::real>(_variant);
    (*object)[data::string::classname()]  = create<data::string>(_variant);

    return object;
}

//------------------------------------------------------------------------------

void SessionTest::activityTest()
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
            using T = decltype(_type);

            object->resize(
                {_variant + 2, _variant + 2},
                std::is_same_v<T, double>
                ? core::type::DOUBLE
                : std::is_same_v<T, float>
                ? core::type::FLOAT
                : std::is_same_v<T, std::uint8_t>
                ? core::type::UINT8
                : std::is_same_v<T, std::uint16_t>
                ? core::type::UINT16
                : std::is_same_v<T, std::uint32_t>
                ? core::type::UINT32
                : std::is_same_v<T, std::uint64_t>
                ? core::type::UINT64
                : std::is_same_v<T, std::int8_t>
                ? core::type::INT8
                : std::is_same_v<T, std::int16_t>
                ? core::type::INT16
                : std::is_same_v<T, std::int32_t>
                ? core::type::INT32
                : std::is_same_v<T, std::int64_t>
                ? core::type::INT64
                : core::type::NONE,
                true
            );

            T counter = static_cast<T>(0);
            for(auto it = object->begin<T>(),
                end = object->end<T>() ;
                it != end ;
                ++it)
            {
                *it = static_cast<T>(_variant) + counter++;
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

void SessionTest::arrayTest()
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
            using T = decltype(_type);

            // Warning: generateImage use reflection that cannot deal with double value (truncated to float precision),
            // thus the 0.1 + static_cast<double>(variant)
            utest_data::generator::image::generateImage(
                object,
                {_variant + 5, _variant + 5, _variant + 5},
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
                std::is_same_v<T, double>
                ? core::type::DOUBLE
                : std::is_same_v<T, float>
                ? core::type::FLOAT
                : std::is_same_v<T, std::uint8_t>
                ? core::type::UINT8
                : std::is_same_v<T, std::uint16_t>
                ? core::type::UINT16
                : std::is_same_v<T, std::uint32_t>
                ? core::type::UINT32
                : std::is_same_v<T, std::uint64_t>
                ? core::type::UINT64
                : std::is_same_v<T, std::int8_t>
                ? core::type::INT8
                : std::is_same_v<T, std::int16_t>
                ? core::type::INT16
                : std::is_same_v<T, std::int32_t>
                ? core::type::INT32
                : std::is_same_v<T, std::int64_t>
                ? core::type::INT64
                : core::type::NONE,

                std::is_same_v<T, double>
                ? data::image::PixelFormat::GRAY_SCALE
                : std::is_same_v<T, float>
                ? data::image::PixelFormat::GRAY_SCALE
                : std::is_same_v<T, std::uint8_t>
                ? data::image::PixelFormat::BGR
                : std::is_same_v<T, std::uint16_t>
                ? data::image::PixelFormat::BGRA
                : std::is_same_v<T, std::uint32_t>
                ? data::image::PixelFormat::RGB
                : std::is_same_v<T, std::uint64_t>
                ? data::image::PixelFormat::RGBA
                : std::is_same_v<T, std::int8_t>
                ? data::image::PixelFormat::GRAY_SCALE
                : std::is_same_v<T, std::int16_t>
                ? data::image::PixelFormat::BGR
                : std::is_same_v<T, std::int32_t>
                ? data::image::PixelFormat::BGRA
                : std::is_same_v<T, std::int64_t>
                ? data::image::PixelFormat::RGB
                : data::image::PixelFormat::UNDEFINED,
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

void SessionTest::imageTest()
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

void SessionTest::vectorTest()
{
    test_combine<data::vector>();
}

//------------------------------------------------------------------------------

template<>
inline data::point::sptr generate<data::point>(const std::size_t /*unused*/)
{
    auto object = std::make_shared<data::point>();

    object->setCoord({random<double>(), random<double>(), random<double>()});

    return object;
}

//------------------------------------------------------------------------------

void SessionTest::pointTest()
{
    test_combine<data::point>();
}

//------------------------------------------------------------------------------

template<>
inline data::point_list::sptr generate<data::point_list>(const std::size_t _variant)
{
    auto object = std::make_shared<data::point_list>();

    auto& points = object->getPoints();
    for(std::size_t i = 0, end = _variant + 3 ; i < end ; ++i)
    {
        points.push_back(create<data::point>(i));
    }

    return object;
}

//------------------------------------------------------------------------------

void SessionTest::pointListTest()
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

        object->addRecord(image, point_list);
    }

    return object;
}

//------------------------------------------------------------------------------

void SessionTest::calibrationInfoTest()
{
    test_combine<data::calibration_info>();
}

//------------------------------------------------------------------------------

template<>
inline data::camera::sptr generate<data::camera>(const std::size_t _variant)
{
    auto object = std::make_shared<data::camera>();

    object->setWidth(random<std::size_t>());
    object->setHeight(random<std::size_t>());
    object->setFx(random<double>());
    object->setFy(random<double>());
    object->setCx(random<double>());
    object->setCy(random<double>());
    object->setDistortionCoefficient(
        random<double>(),
        random<double>(),
        random<double>(),
        random<double>(),
        random<double>()
    );
    object->setSkew(random<double>());
    object->setIsCalibrated(_variant % 2 == 0);
    object->setCameraID(UUID::generate());
    object->setMaximumFrameRate(random<float>());
    constexpr std::array pixel_formats {
        data::camera::PixelFormat::ADOBEDNG,
        data::camera::PixelFormat::ARGB32,
        data::camera::PixelFormat::ARGB32_PREMULTIPLIED,
        data::camera::PixelFormat::RGB32,
        data::camera::PixelFormat::RGB24,
        data::camera::PixelFormat::RGB565,
        data::camera::PixelFormat::RGB555,
        data::camera::PixelFormat::ARGB8565_PREMULTIPLIED,
        data::camera::PixelFormat::BGRA32,
        data::camera::PixelFormat::BGRA32_PREMULTIPLIED,
        data::camera::PixelFormat::BGR32,
        data::camera::PixelFormat::BGR24,
        data::camera::PixelFormat::BGR565,
        data::camera::PixelFormat::BGR555,
        data::camera::PixelFormat::BGRA5658_PREMULTIPLIED,
        data::camera::PixelFormat::AYUV444,
        data::camera::PixelFormat::AYUV444_PREMULTIPLIED,
        data::camera::PixelFormat::YUV444,
        data::camera::PixelFormat::YUV420P,
        data::camera::PixelFormat::YV12,
        data::camera::PixelFormat::UYVY,
        data::camera::PixelFormat::YUYV,
        data::camera::PixelFormat::NV12,
        data::camera::PixelFormat::NV21,
        data::camera::PixelFormat::IMC1,
        data::camera::PixelFormat::IMC2,
        data::camera::PixelFormat::IMC3,
        data::camera::PixelFormat::IMC4,
        data::camera::PixelFormat::Y8,
        data::camera::PixelFormat::Y16,
        data::camera::PixelFormat::JPEG,
        data::camera::PixelFormat::CAMERARAW,
        data::camera::PixelFormat::ADOBEDNG,
        data::camera::PixelFormat::RGBA32,
        data::camera::PixelFormat::USER,
        data::camera::PixelFormat::INVALID
    };
    object->setPixelFormat(pixel_formats[_variant % 35]);
    object->setVideoFile("/" + UUID::generate());
    object->setStreamUrl(UUID::generate());
    object->setCameraSource(
        _variant % 3 == 0
        ? data::camera::source_t::DEVICE
        : _variant % 3 == 1
        ? data::camera::source_t::FILE
        : _variant % 3 == 2
        ? data::camera::source_t::STREAM
        : data::camera::source_t::UNKNOWN
    );
    object->setScale(random<double>());

    return object;
}

//------------------------------------------------------------------------------

void SessionTest::cameraTest()
{
    test_combine<data::camera>();
}

//------------------------------------------------------------------------------

template<>
inline data::color::sptr generate<data::color>(const std::size_t /*unused*/)
{
    auto object = std::make_shared<data::color>();

    object->setRGBA(random<float>(), random<float>(), random<float>(), random<float>());

    return object;
}

//------------------------------------------------------------------------------

void SessionTest::colorTest()
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
        const std::string name = UUID::generate();

        object->addGroup(
            name,
            {random<float>(), random<float>(), random<float>(), random<float>()},
            random<float>(),
            _variant % 2 == 0
            ? data::landmarks::Shape::CUBE
            : data::landmarks::Shape::SPHERE,
            _variant % 3 == 0
        );

        for(std::size_t j = 0, j_end = _variant + 2 ; j < j_end ; ++j)
        {
            object->addPoint(name, {random<double>(), random<double>(), random<double>()});
        }
    }

    return object;
}

//------------------------------------------------------------------------------

void SessionTest::landmarksTest()
{
    test_combine<data::landmarks>();
}

//------------------------------------------------------------------------------

template<>
inline data::line::sptr generate<data::line>(const std::size_t _variant)
{
    auto object = std::make_shared<data::line>();

    object->setPosition(create<data::point>(_variant));
    object->setDirection(create<data::point>(_variant + 1));

    return object;
}

//------------------------------------------------------------------------------

void SessionTest::lineTest()
{
    test_combine<data::line>();
}

//------------------------------------------------------------------------------

template<>
inline data::material::sptr generate<data::material>(const std::size_t _variant)
{
    auto object = std::make_shared<data::material>();

    std::array shading {
        data::material::shading_t::AMBIENT,
        data::material::shading_t::FLAT,
        data::material::shading_t::PHONG
    };

    std::array representation {
        data::material::representation_t::EDGE,
        data::material::representation_t::POINT,
        data::material::representation_t::SURFACE,
        data::material::representation_t::WIREFRAME
    };

    std::array options {
        data::material::options_t::CELLS_NORMALS,
        data::material::options_t::NORMALS,
        data::material::options_t::STANDARD
    };

    // Set ambient color
    object->setAmbient(create<data::color>(_variant));

    // Set diffuse color
    object->setDiffuse(create<data::color>(_variant + 1));

    // Set diffuse texture
    object->setDiffuseTexture(create<data::image>(_variant));

    // Others
    object->setShadingMode(shading[_variant % std::size(shading)]);
    object->setRepresentationMode(representation[_variant % std::size(representation)]);
    object->setOptionsMode(options[_variant % std::size(options)]);
    object->setDiffuseTextureFiltering(
        _variant % 3 == 0
        ? data::material::filtering_t::LINEAR
        : data::material::filtering_t::NEAREST
    );
    object->setDiffuseTextureWrapping(
        _variant % 3 == 0
        ? data::material::wrapping_t::CLAMP
        : data::material::wrapping_t::REPEAT
    );

    return object;
}

//------------------------------------------------------------------------------

void SessionTest::materialTest()
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

void SessionTest::matrix4_test()
{
    test_combine<data::matrix4>();
}

//------------------------------------------------------------------------------

template<>
inline data::plane::sptr generate<data::plane>(const std::size_t _variant)
{
    auto object = std::make_shared<data::plane>();

    auto& points = object->getPoints();
    for(std::size_t i = 0, end = points.size() ; i < end ; ++i)
    {
        points[i] = create<data::point>(i + _variant);
    }

    return object;
}

//------------------------------------------------------------------------------

void SessionTest::planeTest()
{
    test_combine<data::plane>();
}

//------------------------------------------------------------------------------

template<>
inline data::plane_list::sptr generate<data::plane_list>(const std::size_t _variant)
{
    auto object = std::make_shared<data::plane_list>();

    auto& planes = object->getPlanes();

    for(std::size_t i = 0, end = _variant + 2 ; i < end ; ++i)
    {
        planes.push_back(create<data::plane>(_variant + i));
    }

    return object;
}

//------------------------------------------------------------------------------

void SessionTest::planeListTest()
{
    test_combine<data::plane_list>();
}

//------------------------------------------------------------------------------

template<>
inline data::reconstruction::sptr generate<data::reconstruction>(const std::size_t _variant)
{
    auto object = std::make_shared<data::reconstruction>();

    object->setIsVisible(_variant % 3 == 0);
    object->setOrganName(UUID::generate());
    object->set_structure_type(UUID::generate());
    object->setComputedMaskVolume(random<double>());

    // Material
    object->setMaterial(create<data::material>(_variant));

    // image
    object->setImage(create<data::image>(_variant));

    // Mesh
    object->setMesh(create<data::mesh>(_variant));

    return object;
}

//------------------------------------------------------------------------------

void SessionTest::reconstructionTest()
{
    test_combine<data::reconstruction>();
}

//------------------------------------------------------------------------------

template<>
inline data::structure_traits::sptr generate<data::structure_traits>(const std::size_t _variant)
{
    auto object = std::make_shared<data::structure_traits>();

    static constexpr std::array CLASSES {
        data::structure_traits::StructureClass::ENVIRONMENT,
        data::structure_traits::StructureClass::FUNCTIONAL,
        data::structure_traits::StructureClass::LESION,
        data::structure_traits::StructureClass::NO_CONSTRAINT,
        data::structure_traits::StructureClass::ORGAN,
        data::structure_traits::StructureClass::TOOL,
        data::structure_traits::StructureClass::VESSEL
    };

    static constexpr std::array CATEGORIES {
        data::structure_traits::Category::ABDOMEN,
        data::structure_traits::Category::ARM,
        data::structure_traits::Category::BODY,
        data::structure_traits::Category::HEAD,
        data::structure_traits::Category::LEG,
        data::structure_traits::Category::LIVER_SEGMENTS,
        data::structure_traits::Category::NECK,
        data::structure_traits::Category::OTHER,
        data::structure_traits::Category::PELVIS,
        data::structure_traits::Category::THORAX
    };

    object->setType(UUID::generate());
    object->setClass(CLASSES[_variant % std::size(CLASSES)]);
    object->setNativeExp(UUID::generate());
    object->setNativeGeometricExp(UUID::generate());
    object->set_attachment_type(UUID::generate());
    object->setAnatomicRegion(UUID::generate());
    object->setPropertyCategory(UUID::generate());
    object->set_property_type(UUID::generate());

    // Categories

    // Reset categories.
    data::structure_traits::CategoryContainer categories;

    for(std::size_t i = 0, end = _variant + 2 ; i < end ; ++i)
    {
        categories.push_back(CATEGORIES[(i + _variant) % std::size(CATEGORIES)]);
    }

    object->setCategories(categories);

    // Color
    object->setColor(create<data::color>(_variant));

    return object;
}

//------------------------------------------------------------------------------

void SessionTest::structureTraitsTest()
{
    test_combine<data::structure_traits>();
}

//------------------------------------------------------------------------------

template<>
inline data::structure_traits_dictionary::sptr generate<data::structure_traits_dictionary>(const std::size_t _variant)
{
    auto object = std::make_shared<data::structure_traits_dictionary>();

    auto organ = create<data::structure_traits>(_variant);
    organ->setClass(data::structure_traits::ORGAN);
    organ->set_attachment_type("");

    object->addStructure(organ);

    for(std::size_t i = 0, end = _variant + 2 ; i < end ; ++i)
    {
        auto structure             = create<data::structure_traits>(_variant + i + 1);
        const auto structure_class = structure->getClass();

        if(structure_class != data::structure_traits::LESION
           && structure_class != data::structure_traits::FUNCTIONAL)
        {
            structure->set_attachment_type("");
        }
        else
        {
            structure->set_attachment_type(organ->getType());
        }

        object->addStructure(structure);
    }

    return object;
}

//------------------------------------------------------------------------------

void SessionTest::structureTraitsDictionaryTest()
{
    test_combine<data::structure_traits_dictionary>();
}

//------------------------------------------------------------------------------

template<>
inline data::resection::sptr generate<data::resection>(const std::size_t _variant)
{
    auto object = std::make_shared<data::resection>();

    object->setName(UUID::generate());
    object->setIsSafePart(_variant % 2 == 0);
    object->setIsValid(_variant % 3 == 0);
    object->setIsVisible(_variant % 4 == 0);

    auto& inputs  = object->getInputs();
    auto& outputs = object->getOutputs();
    for(std::size_t i = 0, end = _variant + 2 ; i < end ; ++i)
    {
        inputs.push_back(create<data::reconstruction>(_variant + i));
        outputs.push_back(create<data::reconstruction>(_variant + i + 1));
    }

    return object;
}

//------------------------------------------------------------------------------

void SessionTest::resectionTest()
{
    test_combine<data::resection>();
}

//------------------------------------------------------------------------------

template<>
inline data::resection_db::sptr generate<data::resection_db>(const std::size_t _variant)
{
    auto object = std::make_shared<data::resection_db>();

    object->setSafeResection(create<data::resection>(_variant));

    for(std::size_t i = 0, end = _variant + 2 ; i < end ; ++i)
    {
        object->addResection(create<data::resection>(_variant + i + 1));
    }

    return object;
}

//------------------------------------------------------------------------------

void SessionTest::resectionDBTest()
{
    test_combine<data::resection_db>();
}

//------------------------------------------------------------------------------

template<>
inline data::transfer_function::sptr generate<data::transfer_function>(const std::size_t _variant)
{
    auto object = std::make_shared<data::transfer_function>();

    object->setLevel(random<double>());
    object->setWindow(random<double>());
    object->setName(UUID::generate());
    object->setBackgroundColor(
        data::transfer_function::color_t(
            random<double>(),
            random<double>(),
            random<double>(),
            random<double>()
        )
    );

    auto tf_data = object->pieces().emplace_back(std::make_shared<data::transfer_function_piece>());
    tf_data->setInterpolationMode(
        _variant % 3 == 0
        ? data::transfer_function::InterpolationMode::LINEAR
        : data::transfer_function::InterpolationMode::NEAREST
    );
    tf_data->setClamped(_variant % 4 == 0);

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

void SessionTest::transferFunctionTest()
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
        const std::filesystem::path path = utest_data::Data::dir()
                                           / "sight/Patient/Dicom/DicomDB/86-CT-Skull";

        CPPUNIT_ASSERT_MESSAGE(
            "The dicom directory '" + path.string() + "' does not exist",
            std::filesystem::exists(path)
        );

        // Read source Dicom
        auto reader = std::make_shared<io::dicom::reader::series_set>();
        reader->set_object(series_set);
        reader->set_folder(path);

        CPPUNIT_ASSERT_NO_THROW(reader->readDicomSeries());
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
        dicom_series->addsop_classUID(UUID::generate());
        dicom_series->addComputedTagValue(UUID::generate(), UUID::generate());
    }

    // Inherited attributes
    dicom_series->series::shallow_copy(get_expected<data::series>(_variant));

    return dicom_series;
}

//------------------------------------------------------------------------------

void SessionTest::dicomSeriesTest()
{
    if(utest::Filter::ignoreSlowTests())
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

    object->setContrastBolusAgent(UUID::generate());
    object->setContrastBolusRoute(UUID::generate());
    object->setContrastBolusVolume(double(_variant));
    object->setContrastBolusStartTime(generate_tm(_variant));
    object->setContrastBolusStopTime(generate_tm(_variant));
    object->setContrastBolusTotalDose(double(_variant));

    object->setContrastFlowRate(
        std::to_string(_variant)
        + "\\"
        + std::to_string(_variant + 1)
        + "\\"
        + std::to_string(_variant + 2)
    );

    object->setContrastFlowDuration(
        std::to_string(_variant + 4)
        + "\\"
        + std::to_string(_variant + 5)
        + "\\"
        + std::to_string(_variant + 6)
    );

    object->setContrastBolusIngredient(UUID::generate());
    object->setContrastBolusIngredientConcentration(double(_variant));
    object->setAcquisitionDate(generate_da(_variant));
    object->setAcquisitionTime(generate_tm(_variant));

    object->setDicomReference(create<data::dicom_series>(_variant));

    return object;
}

//------------------------------------------------------------------------------

void SessionTest::imageSeriesTest()
{
    if(utest::Filter::ignoreSlowTests())
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

    object->setDicomReference(create<data::dicom_series>(_variant));

    std::vector<data::reconstruction::sptr> reconstruction_db;
    for(std::size_t i = 0, end = _variant + 2 ; i < end ; ++i)
    {
        reconstruction_db.push_back(create<data::reconstruction>(_variant + i));
    }

    object->setReconstructionDB(reconstruction_db);

    // Inherited attributes
    object->series::shallow_copy(get_expected<data::series>(_variant));

    return object;
}

//------------------------------------------------------------------------------

void SessionTest::modelSeriesTest()
{
    if(utest::Filter::ignoreSlowTests())
    {
        return;
    }

    test_combine<data::model_series>();
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

void SessionTest::activitySetTest()
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

void SessionTest::cameraSetTest()
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

void SessionTest::seriesSetTest()
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

void SessionTest::setTest()
{
    test_combine<data::set>();
}

//------------------------------------------------------------------------------

inline static void custom_serialize(
    zip::ArchiveWriter& /*unused*/,
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

    helper::write_string(_tree, "custom", string->getValue());
}

//------------------------------------------------------------------------------

inline static data::string::sptr custom_deserialize(
    zip::ArchiveReader& /*unused*/,
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
    string->setValue(helper::read_string(_tree, "custom"));

    return string;
}

//------------------------------------------------------------------------------

void SessionTest::customSerializerTest()
{
    // Create a temporary directory
    core::os::temp_dir tmp_dir;
    const auto test_path = tmp_dir / "customSerializerTest.zip";

    // Test serialization
    {
        // Create the data object
        auto object = create<data::string>(0);

        // Create the session writer
        auto session_writer = std::make_shared<io::session::SessionWriter>();
        CPPUNIT_ASSERT(session_writer);

        // Configure the session writer
        session_writer->set_object(object);
        session_writer->set_file(test_path);

        // Test serializer getter
        CPPUNIT_ASSERT(session_writer->serializer(data::string::classname()));

        // Change the session serializer by setting a new one using setCustomSerializer
        session_writer->setCustomSerializer(data::string::classname(), custom_serialize);

        // Write the new session
        CPPUNIT_ASSERT_NO_THROW(session_writer->write());

        CPPUNIT_ASSERT(std::filesystem::exists(test_path));
    }

    // Test deserialization
    {
        auto session_reader = std::make_shared<io::session::SessionReader>();
        CPPUNIT_ASSERT(session_reader);

        // Configure the session reader
        session_reader->set_file(test_path);

        // Read the session: it should fail since the serializer has been modified by a custom one
        CPPUNIT_ASSERT_THROW(session_reader->read(), sight::core::exception);

        // Test deserializer getter
        CPPUNIT_ASSERT(session_reader->deserializer(data::string::classname()));

        // Set the new customDeserializer
        session_reader->setCustomDeserializer(data::string::classname(), custom_deserialize);

        CPPUNIT_ASSERT_NO_THROW(session_reader->read());

        // Test value
        auto object = std::dynamic_pointer_cast<data::string>(session_reader->getObject());
        compare<data::string>(object, 0);
    }
}

} // namespace sight::io::session::ut
