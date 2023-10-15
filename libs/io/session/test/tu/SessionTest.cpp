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

inline static std::string generateTM(std::size_t variant)
{
    std::stringstream hh;
    hh << std::setfill('0') << std::setw(2) << (variant % 24);

    std::stringstream mm;
    mm << std::setfill('0') << std::setw(2) << ((variant + 1) % 60);

    std::stringstream ss;
    ss << std::setfill('0') << std::setw(2) << ((variant + 2) % 60);

    std::stringstream ffffff;
    ffffff << std::setfill('0') << std::setw(6) << ((variant + 3) % 1000000);

    return hh.str() + mm.str() + ss.str() + "." + ffffff.str();
}

//------------------------------------------------------------------------------

inline static std::string generateDA(std::size_t variant)
{
    std::stringstream yyyy;
    yyyy << std::setfill('0') << std::setw(4) << (variant % 10000);

    std::stringstream mm;
    mm << std::setfill('0') << std::setw(2) << ((variant + 1) % 12);

    std::stringstream dd;
    dd << std::setfill('0') << std::setw(2) << ((variant + 2) % 32);

    return yyyy.str() + mm.str() + dd.str();
}

//------------------------------------------------------------------------------

inline static std::string generateDT(std::size_t variant)
{
    return generateDA(variant) + generateTM(variant);
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
    return std::make_shared<T>(static_cast<typename T::ValueType>(random<typename T::ValueType>()));
}

//------------------------------------------------------------------------------

template<typename T>
inline const typename T::csptr& getExpected(const std::size_t variant)
{
    static std::map<std::size_t, typename T::csptr> MAP;
    const auto& it = MAP.find(variant);

    if(it == MAP.cend())
    {
        const auto& object = generate<T>(variant);
        object->setDescription(UUID::generate());

        return MAP.insert_or_assign(variant, object).first->second;
    }

    return it->second;
}

//------------------------------------------------------------------------------

template<typename T>
inline typename T::sptr create(const std::size_t variant)
{
    const auto& object = std::make_shared<T>();
    object->deep_copy(getExpected<T>(variant));
    return object;
}

//------------------------------------------------------------------------------

template<typename T>
inline void compare(const typename T::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);
    CPPUNIT_ASSERT_EQUAL(getExpected<T>(variant)->getValue(), actual->getValue());
}

//------------------------------------------------------------------------------

template<typename T>
inline void test(const bool encrypt, const bool raw, const bool empty_obj = false)
{
    static constexpr auto password = "password";

    const auto& test_id = T::leaf_classname() + "_" + std::to_string(encrypt) + "_" + std::to_string(raw);

    // Create a temporary directory
    core::os::temp_dir tmpDir;
    const auto testPath = tmpDir / (test_id + (raw ? ".json" : ".zip"));

    static constexpr auto fieldName = "field";

    // Test serialization
    {
        // Create the data object
        auto object = empty_obj ? std::make_shared<T>() : create<T>(0);

        // Add a field
        object->setField(fieldName, create<T>(1));

        // Create the session writer
        auto sessionWriter = std::make_shared<io::session::SessionWriter>();
        CPPUNIT_ASSERT(sessionWriter);

        // Configure the session writer
        sessionWriter->set_object(object);
        sessionWriter->set_file(testPath);

        if(raw)
        {
            sessionWriter->setArchiveFormat(io::zip::Archive::ArchiveFormat::FILESYSTEM);
        }
        else if(encrypt)
        {
            sessionWriter->setPassword(password);
        }

        // Write the session
        CPPUNIT_ASSERT_NO_THROW(sessionWriter->write());

        CPPUNIT_ASSERT(std::filesystem::exists(testPath));
    }

    // Test deserialization
    {
        auto sessionReader = std::make_shared<io::session::SessionReader>();
        CPPUNIT_ASSERT(sessionReader);

        // Configure the session reader
        sessionReader->set_file(testPath);

        if(raw)
        {
            sessionReader->setArchiveFormat(io::zip::Archive::ArchiveFormat::FILESYSTEM);
        }
        else if(encrypt)
        {
            sessionReader->setPassword(password);
        }

        // Read the session
        CPPUNIT_ASSERT_NO_THROW(sessionReader->read());

        // Test value
        auto actual_object = std::dynamic_pointer_cast<T>(sessionReader->getObject());

        // Create the data object
        auto expected_object = empty_obj ? std::make_shared<T>() : create<T>(0);

        // Add a field
        expected_object->setField(fieldName, create<T>(1));

        CPPUNIT_ASSERT(*expected_object == *actual_object);
    }
}

//------------------------------------------------------------------------------

template<typename T>
inline void testCombine()
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
inline data::boolean::sptr generate<data::boolean>(const std::size_t variant)
{
    return std::make_shared<data::boolean>(variant % 2 == 0);
}

//------------------------------------------------------------------------------

void SessionTest::booleanTest()
{
    testCombine<data::boolean>();
}

//------------------------------------------------------------------------------

void SessionTest::integerTest()
{
    testCombine<data::integer>();
}

//------------------------------------------------------------------------------

void SessionTest::floatTest()
{
    testCombine<data::real>();
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
    testCombine<data::string>();
}

//------------------------------------------------------------------------------

template<>
inline data::composite::sptr generate<data::composite>(const std::size_t variant)
{
    auto object = std::make_shared<data::composite>();
    (*object)[data::boolean::classname()] = create<data::boolean>(variant);
    (*object)[data::integer::classname()] = create<data::integer>(variant);
    (*object)[data::real::classname()]    = create<data::real>(variant);
    (*object)[data::string::classname()]  = create<data::string>(variant);

    return object;
}

//------------------------------------------------------------------------------

void SessionTest::compositeTest()
{
    testCombine<data::composite>();
}

//------------------------------------------------------------------------------

template<>
inline data::mesh::sptr create<data::mesh>(const std::size_t variant)
{
    const auto& object = std::make_shared<data::mesh>();
    object->deep_copy(getExpected<data::mesh>(variant));
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
    testCombine<data::mesh>();
}

//------------------------------------------------------------------------------

template<>
inline data::series::sptr generate<data::series>(const std::size_t variant)
{
    auto object = std::make_shared<data::series>();

    // Fill trivial attributes
    object->setSOPKeyword(sight::data::dicom::sop::Keyword::CTImageStorage);
    object->setModality(UUID::generate());
    object->setSeriesDescription(UUID::generate());
    object->setSeriesInstanceUID(UUID::generate());
    object->setSeriesNumber(std::int32_t(variant));
    object->setLaterality(UUID::generate());
    object->setSeriesDate(generateDA(variant));
    object->setSeriesTime(generateTM(variant));
    object->setPerformingPhysicianName(UUID::generate() + "\\" + UUID::generate());
    object->setProtocolName(UUID::generate());
    object->setBodyPartExamined(UUID::generate());
    object->setPatientPosition(UUID::generate());
    object->setAnatomicalOrientationType(UUID::generate());
    object->setPerformedProcedureStepID(UUID::generate());
    object->setPerformedProcedureStepStartDate(generateDA(variant));
    object->setPerformedProcedureStepStartTime(generateTM(variant));
    object->setPerformedProcedureStepEndDate(generateDA(variant));
    object->setPerformedProcedureStepEndTime(generateTM(variant));
    object->setPerformedProcedureStepDescription(UUID::generate());
    object->setCommentsOnThePerformedProcedureStep(UUID::generate());

    // Equipment Module
    object->setInstitutionName(UUID::generate());

    // Patient Module
    object->setPatientName(UUID::generate());
    object->setPatientID(UUID::generate());
    object->setPatientBirthDate(generateDA(variant));
    object->setPatientSex(UUID::generate());

    // Study Module
    object->setStudyDescription(UUID::generate());
    object->setStudyInstanceUID(UUID::generate());
    object->setStudyID(UUID::generate());
    object->setStudyDate(generateDA(variant));
    object->setStudyTime(generateTM(variant));
    object->setReferringPhysicianName(UUID::generate());

    // Patient Study Module
    object->setPatientAge(UUID::generate());
    object->setPatientSize(double(variant));
    object->setPatientWeight(double(variant));

    // Generate specific instance data
    for(std::size_t i = 0 ; i < variant + 3 ; ++i)
    {
        object->setImagePositionPatient(
            {
                double(variant + i),
                double(variant + i + 1),
                double(variant + i + 2)
            },
            i
        );

        object->setImageOrientationPatient(
            {
                double(variant + i), double(variant + i + 1), double(variant + i + 2),
                double(variant + i + 3), double(variant + i + 4), double(variant + i + 5)
            },
            i
        );

        object->setFrameAcquisitionDateTime(generateDT(variant + i), i);
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
    testCombine<data::series>();
}

//------------------------------------------------------------------------------

template<>
inline data::activity::sptr generate<data::activity>(const std::size_t variant)
{
    auto object = std::make_shared<data::activity>();

    (*object)[data::boolean::classname()] = create<data::boolean>(variant);
    (*object)[data::integer::classname()] = create<data::integer>(variant);
    (*object)[data::real::classname()]    = create<data::real>(variant);
    (*object)[data::string::classname()]  = create<data::string>(variant);

    return object;
}

//------------------------------------------------------------------------------

void SessionTest::activityTest()
{
    testCombine<data::activity>();
}

//------------------------------------------------------------------------------

template<>
inline data::array::sptr generate<data::array>(const std::size_t variant)
{
    auto object = std::make_shared<data::array>();

    const auto lock = object->dump_lock();

    auto fill =
        [&](auto type)
        {
            using T = decltype(type);

            object->resize(
                {variant + 2, variant + 2},
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
                *it = static_cast<T>(variant) + counter++;
            }
        };

    switch(variant % 5)
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
    testCombine<data::array>();
}

//------------------------------------------------------------------------------

template<>
inline data::image::sptr generate<data::image>(const std::size_t variant)
{
    auto object = std::make_shared<data::image>();

    const auto lock = object->dump_lock();

    auto fill =
        [&](auto type)
        {
            using T = decltype(type);

            // Warning: generateImage use reflection that cannot deal with double value (truncated to float precision),
            // thus the 0.1 + static_cast<double>(variant)
            utest_data::generator::image::generateImage(
                object,
                {variant + 5, variant + 5, variant + 5},
            {
                0.1 + static_cast<double>(variant),
                0.2 + static_cast<double>(variant),
                0.3 + static_cast<double>(variant)
            },
            {
                0.4 + static_cast<double>(variant),
                0.5 + static_cast<double>(variant),
                0.6 + static_cast<double>(variant)
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
                std::uint32_t(variant)
            );
        };

    switch(variant % 5)
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
    testCombine<data::image>();
}

//------------------------------------------------------------------------------

template<>
inline data::vector::sptr generate<data::vector>(const std::size_t variant)
{
    auto object = std::make_shared<data::vector>();

    object->push_back(create<data::boolean>(variant));
    object->push_back(create<data::integer>(variant));
    object->push_back(create<data::real>(variant));
    object->push_back(create<data::string>(variant));
    object->push_back(create<data::activity>(variant));

    return object;
}

//------------------------------------------------------------------------------

void SessionTest::vectorTest()
{
    testCombine<data::vector>();
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
    testCombine<data::point>();
}

//------------------------------------------------------------------------------

template<>
inline data::point_list::sptr generate<data::point_list>(const std::size_t variant)
{
    auto object = std::make_shared<data::point_list>();

    auto& points = object->getPoints();
    for(std::size_t i = 0, end = variant + 3 ; i < end ; ++i)
    {
        points.push_back(create<data::point>(i));
    }

    return object;
}

//------------------------------------------------------------------------------

void SessionTest::pointListTest()
{
    testCombine<data::point_list>();
}

//------------------------------------------------------------------------------

template<>
inline data::calibration_info::sptr generate<data::calibration_info>(const std::size_t variant)
{
    auto object = std::make_shared<data::calibration_info>();

    for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
    {
        // Create the image
        auto image = create<data::image>(variant + i);

        // Create the PointList
        auto pointList = create<data::point_list>(variant + i);

        object->addRecord(image, pointList);
    }

    return object;
}

//------------------------------------------------------------------------------

void SessionTest::calibrationInfoTest()
{
    testCombine<data::calibration_info>();
}

//------------------------------------------------------------------------------

template<>
inline data::camera::sptr generate<data::camera>(const std::size_t variant)
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
    object->setIsCalibrated(variant % 2 == 0);
    object->setCameraID(UUID::generate());
    object->setMaximumFrameRate(random<float>());
    constexpr std::array pixelFormats {
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
    object->setPixelFormat(pixelFormats[variant % 35]);
    object->setVideoFile("/" + UUID::generate());
    object->setStreamUrl(UUID::generate());
    object->setCameraSource(
        variant % 3 == 0
        ? data::camera::SourceType::DEVICE
        : variant % 3 == 1
        ? data::camera::SourceType::FILE
        : variant % 3 == 2
        ? data::camera::SourceType::STREAM
        : data::camera::SourceType::UNKNOWN
    );
    object->setScale(random<double>());

    return object;
}

//------------------------------------------------------------------------------

void SessionTest::cameraTest()
{
    testCombine<data::camera>();
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
    testCombine<data::color>();
}

//------------------------------------------------------------------------------

template<>
inline data::landmarks::sptr generate<data::landmarks>(const std::size_t variant)
{
    auto object = std::make_shared<data::landmarks>();

    for(std::size_t i = 0, i_end = variant + 2 ; i < i_end ; ++i)
    {
        const std::string name = UUID::generate();

        object->addGroup(
            name,
            {random<float>(), random<float>(), random<float>(), random<float>()},
            random<float>(),
            variant % 2 == 0
            ? data::landmarks::Shape::CUBE
            : data::landmarks::Shape::SPHERE,
            variant % 3 == 0
        );

        for(std::size_t j = 0, j_end = variant + 2 ; j < j_end ; ++j)
        {
            object->addPoint(name, {random<double>(), random<double>(), random<double>()});
        }
    }

    return object;
}

//------------------------------------------------------------------------------

void SessionTest::landmarksTest()
{
    testCombine<data::landmarks>();
}

//------------------------------------------------------------------------------

template<>
inline data::line::sptr generate<data::line>(const std::size_t variant)
{
    auto object = std::make_shared<data::line>();

    object->setPosition(create<data::point>(variant));
    object->setDirection(create<data::point>(variant + 1));

    return object;
}

//------------------------------------------------------------------------------

void SessionTest::lineTest()
{
    testCombine<data::line>();
}

//------------------------------------------------------------------------------

template<>
inline data::material::sptr generate<data::material>(const std::size_t variant)
{
    auto object = std::make_shared<data::material>();

    std::array shading {
        data::material::ShadingType::AMBIENT,
        data::material::ShadingType::FLAT,
        data::material::ShadingType::PHONG
    };

    std::array representation {
        data::material::RepresentationType::EDGE,
        data::material::RepresentationType::POINT,
        data::material::RepresentationType::SURFACE,
        data::material::RepresentationType::WIREFRAME
    };

    std::array options {
        data::material::OptionsType::CELLS_NORMALS,
        data::material::OptionsType::NORMALS,
        data::material::OptionsType::STANDARD
    };

    // Set ambient color
    object->setAmbient(create<data::color>(variant));

    // Set diffuse color
    object->setDiffuse(create<data::color>(variant + 1));

    // Set diffuse texture
    object->setDiffuseTexture(create<data::image>(variant));

    // Others
    object->setShadingMode(shading[variant % std::size(shading)]);
    object->setRepresentationMode(representation[variant % std::size(representation)]);
    object->setOptionsMode(options[variant % std::size(options)]);
    object->setDiffuseTextureFiltering(
        variant % 3 == 0
        ? data::material::FilteringType::LINEAR
        : data::material::FilteringType::NEAREST
    );
    object->setDiffuseTextureWrapping(
        variant % 3 == 0
        ? data::material::WrappingType::CLAMP
        : data::material::WrappingType::REPEAT
    );

    return object;
}

//------------------------------------------------------------------------------

void SessionTest::materialTest()
{
    testCombine<data::material>();
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
    testCombine<data::matrix4>();
}

//------------------------------------------------------------------------------

template<>
inline data::plane::sptr generate<data::plane>(const std::size_t variant)
{
    auto object = std::make_shared<data::plane>();

    auto& points = object->getPoints();
    for(std::size_t i = 0, end = points.size() ; i < end ; ++i)
    {
        points[i] = create<data::point>(i + variant);
    }

    return object;
}

//------------------------------------------------------------------------------

void SessionTest::planeTest()
{
    testCombine<data::plane>();
}

//------------------------------------------------------------------------------

template<>
inline data::plane_list::sptr generate<data::plane_list>(const std::size_t variant)
{
    auto object = std::make_shared<data::plane_list>();

    auto& planes = object->getPlanes();

    for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
    {
        planes.push_back(create<data::plane>(variant + i));
    }

    return object;
}

//------------------------------------------------------------------------------

void SessionTest::planeListTest()
{
    testCombine<data::plane_list>();
}

//------------------------------------------------------------------------------

template<>
inline data::reconstruction::sptr generate<data::reconstruction>(const std::size_t variant)
{
    auto object = std::make_shared<data::reconstruction>();

    object->setIsVisible(variant % 3 == 0);
    object->setOrganName(UUID::generate());
    object->setStructureType(UUID::generate());
    object->setComputedMaskVolume(random<double>());

    // Material
    object->setMaterial(create<data::material>(variant));

    // image
    object->setImage(create<data::image>(variant));

    // Mesh
    object->setMesh(create<data::mesh>(variant));

    return object;
}

//------------------------------------------------------------------------------

void SessionTest::reconstructionTest()
{
    testCombine<data::reconstruction>();
}

//------------------------------------------------------------------------------

template<>
inline data::structure_traits::sptr generate<data::structure_traits>(const std::size_t variant)
{
    auto object = std::make_shared<data::structure_traits>();

    const std::array CLASSES {
        data::structure_traits::StructureClass::ENVIRONMENT,
        data::structure_traits::StructureClass::FUNCTIONAL,
        data::structure_traits::StructureClass::LESION,
        data::structure_traits::StructureClass::NO_CONSTRAINT,
        data::structure_traits::StructureClass::ORGAN,
        data::structure_traits::StructureClass::TOOL,
        data::structure_traits::StructureClass::VESSEL
    };

    const std::array CATEGORIES {
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
    object->setClass(CLASSES[variant % std::size(CLASSES)]);
    object->setNativeExp(UUID::generate());
    object->setNativeGeometricExp(UUID::generate());
    object->setAttachmentType(UUID::generate());
    object->setAnatomicRegion(UUID::generate());
    object->setPropertyCategory(UUID::generate());
    object->setPropertyType(UUID::generate());

    // Categories

    // Reset categories.
    data::structure_traits::CategoryContainer categories;

    for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
    {
        categories.push_back(CATEGORIES[(i + variant) % std::size(CATEGORIES)]);
    }

    object->setCategories(categories);

    // Color
    object->setColor(create<data::color>(variant));

    return object;
}

//------------------------------------------------------------------------------

void SessionTest::structureTraitsTest()
{
    testCombine<data::structure_traits>();
}

//------------------------------------------------------------------------------

template<>
inline data::structure_traits_dictionary::sptr generate<data::structure_traits_dictionary>(const std::size_t variant)
{
    auto object = std::make_shared<data::structure_traits_dictionary>();

    auto organ = create<data::structure_traits>(variant);
    organ->setClass(data::structure_traits::ORGAN);
    organ->setAttachmentType("");

    object->addStructure(organ);

    for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
    {
        auto structure            = create<data::structure_traits>(variant + i + 1);
        const auto structureClass = structure->getClass();

        if(structureClass != data::structure_traits::LESION
           && structureClass != data::structure_traits::FUNCTIONAL)
        {
            structure->setAttachmentType("");
        }
        else
        {
            structure->setAttachmentType(organ->getType());
        }

        object->addStructure(structure);
    }

    return object;
}

//------------------------------------------------------------------------------

void SessionTest::structureTraitsDictionaryTest()
{
    testCombine<data::structure_traits_dictionary>();
}

//------------------------------------------------------------------------------

template<>
inline data::resection::sptr generate<data::resection>(const std::size_t variant)
{
    auto object = std::make_shared<data::resection>();

    object->setName(UUID::generate());
    object->setIsSafePart(variant % 2 == 0);
    object->setIsValid(variant % 3 == 0);
    object->setIsVisible(variant % 4 == 0);

    auto& inputs  = object->getInputs();
    auto& outputs = object->getOutputs();
    for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
    {
        inputs.push_back(create<data::reconstruction>(variant + i));
        outputs.push_back(create<data::reconstruction>(variant + i + 1));
    }

    return object;
}

//------------------------------------------------------------------------------

void SessionTest::resectionTest()
{
    testCombine<data::resection>();
}

//------------------------------------------------------------------------------

template<>
inline data::resection_db::sptr generate<data::resection_db>(const std::size_t variant)
{
    auto object = std::make_shared<data::resection_db>();

    object->setSafeResection(create<data::resection>(variant));

    for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
    {
        object->addResection(create<data::resection>(variant + i + 1));
    }

    return object;
}

//------------------------------------------------------------------------------

void SessionTest::resectionDBTest()
{
    testCombine<data::resection_db>();
}

//------------------------------------------------------------------------------

template<>
inline data::transfer_function::sptr generate<data::transfer_function>(const std::size_t variant)
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

    auto tfData = object->pieces().emplace_back(std::make_shared<data::transfer_function_piece>());
    tfData->setInterpolationMode(
        variant % 3 == 0
        ? data::transfer_function::InterpolationMode::LINEAR
        : data::transfer_function::InterpolationMode::NEAREST
    );
    tfData->setClamped(variant % 4 == 0);

    for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
    {
        tfData->insert(
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
    testCombine<data::transfer_function>();
}

//------------------------------------------------------------------------------

template<>
inline data::dicom_series::sptr generate<data::dicom_series>(const std::size_t variant)
{
    data::dicom_series::sptr dicomSeries;

    // Only load the real dicom once
    if(variant == 0)
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

        dicomSeries = std::dynamic_pointer_cast<data::dicom_series>(series_set->front());
    }
    else
    {
        // Take the first variant as basis
        dicomSeries = std::make_shared<data::dicom_series>();
        dicomSeries->shallow_copy(getExpected<data::dicom_series>(0));
    }

    // Randomize a bit the dicomSeries
    for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
    {
        dicomSeries->addsop_classUID(UUID::generate());
        dicomSeries->addComputedTagValue(UUID::generate(), UUID::generate());
    }

    // Inherited attributes
    dicomSeries->series::shallow_copy(getExpected<data::series>(variant));

    return dicomSeries;
}

//------------------------------------------------------------------------------

void SessionTest::dicomSeriesTest()
{
    if(utest::Filter::ignoreSlowTests())
    {
        return;
    }

    testCombine<data::dicom_series>();
}

//------------------------------------------------------------------------------

template<>
inline data::image_series::sptr generate<data::image_series>(const std::size_t variant)
{
    auto object = std::make_shared<data::image_series>();

    // Inherited attributes
    object->image::shallow_copy(getExpected<data::image>(variant));
    object->series::shallow_copy(getExpected<data::series>(variant));

    object->setContrastBolusAgent(UUID::generate());
    object->setContrastBolusRoute(UUID::generate());
    object->setContrastBolusVolume(double(variant));
    object->setContrastBolusStartTime(generateTM(variant));
    object->setContrastBolusStopTime(generateTM(variant));
    object->setContrastBolusTotalDose(double(variant));

    object->setContrastFlowRate(
        std::to_string(variant)
        + "\\"
        + std::to_string(variant + 1)
        + "\\"
        + std::to_string(variant + 2)
    );

    object->setContrastFlowDuration(
        std::to_string(variant + 4)
        + "\\"
        + std::to_string(variant + 5)
        + "\\"
        + std::to_string(variant + 6)
    );

    object->setContrastBolusIngredient(UUID::generate());
    object->setContrastBolusIngredientConcentration(double(variant));
    object->setAcquisitionDate(generateDA(variant));
    object->setAcquisitionTime(generateTM(variant));

    object->setDicomReference(create<data::dicom_series>(variant));

    return object;
}

//------------------------------------------------------------------------------

void SessionTest::imageSeriesTest()
{
    if(utest::Filter::ignoreSlowTests())
    {
        return;
    }

    testCombine<data::image_series>();
}

//------------------------------------------------------------------------------

template<>
inline data::model_series::sptr generate<data::model_series>(const std::size_t variant)
{
    auto object = std::make_shared<data::model_series>();

    object->setDicomReference(create<data::dicom_series>(variant));

    std::vector<data::reconstruction::sptr> reconstructionDB;
    for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
    {
        reconstructionDB.push_back(create<data::reconstruction>(variant + i));
    }

    object->setReconstructionDB(reconstructionDB);

    // Inherited attributes
    object->series::shallow_copy(getExpected<data::series>(variant));

    return object;
}

//------------------------------------------------------------------------------

void SessionTest::modelSeriesTest()
{
    if(utest::Filter::ignoreSlowTests())
    {
        return;
    }

    testCombine<data::model_series>();
}

//------------------------------------------------------------------------------

template<>
inline data::activity_set::sptr generate<data::activity_set>(const std::size_t variant)
{
    auto object = std::make_shared<data::activity_set>();

    for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
    {
        object->push_back(create<data::activity>(variant + i));
    }

    return object;
}

//------------------------------------------------------------------------------

void SessionTest::activitySetTest()
{
    testCombine<data::activity_set>();
}

//------------------------------------------------------------------------------

template<>
inline data::camera_set::sptr generate<data::camera_set>(const std::size_t variant)
{
    auto object = std::make_shared<data::camera_set>();

    for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
    {
        auto camera = create<data::camera>(variant + i);
        auto matrix = create<data::matrix4>(variant + i);
        object->push_back(std::make_pair(camera, matrix));
    }

    return object;
}

//------------------------------------------------------------------------------

void SessionTest::cameraSetTest()
{
    testCombine<data::camera_set>();
}

//------------------------------------------------------------------------------

template<>
inline data::series_set::sptr generate<data::series_set>(const std::size_t variant)
{
    auto object = std::make_shared<data::series_set>();

    for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
    {
        object->push_back(create<data::series>(variant + i));
    }

    return object;
}

//------------------------------------------------------------------------------

void SessionTest::seriesSetTest()
{
    testCombine<data::series_set>();
}

//------------------------------------------------------------------------------

template<>
inline data::set::sptr generate<data::set>(const std::size_t variant)
{
    auto object = std::make_shared<data::set>();

    for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
    {
        object->push_back(create<data::series>(variant + i));
    }

    return object;
}

//------------------------------------------------------------------------------

void SessionTest::setTest()
{
    testCombine<data::set>();
}

//------------------------------------------------------------------------------

inline static void customSerialize(
    zip::ArchiveWriter& /*unused*/,
    boost::property_tree::ptree& tree,
    data::object::csptr object,
    std::map<std::string, data::object::csptr>& /*unused*/,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    // Cast to the right type
    const auto string = helper::safe_cast<data::string>(object);

    // Add a version number. Not mandatory, but could help for future release
    helper::write_version<data::string>(tree, 666);

    helper::write_string(tree, "custom", string->getValue());
}

//------------------------------------------------------------------------------

inline static data::string::sptr customDeserialize(
    zip::ArchiveReader& /*unused*/,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::object::sptr>& /*unused*/,
    data::object::sptr object,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    // Create or reuse the object
    auto string = helper::cast_or_create<data::string>(object);

    // Check version number. Not mandatory, but could help for future release
    helper::read_version<data::string>(tree, 0, 666);

    // Assign the value
    string->setValue(helper::read_string(tree, "custom"));

    return string;
}

//------------------------------------------------------------------------------

void SessionTest::customSerializerTest()
{
    // Create a temporary directory
    core::os::temp_dir tmpDir;
    const auto testPath = tmpDir / "customSerializerTest.zip";

    // Test serialization
    {
        // Create the data object
        auto object = create<data::string>(0);

        // Create the session writer
        auto sessionWriter = std::make_shared<io::session::SessionWriter>();
        CPPUNIT_ASSERT(sessionWriter);

        // Configure the session writer
        sessionWriter->set_object(object);
        sessionWriter->set_file(testPath);

        // Test serializer getter
        CPPUNIT_ASSERT(sessionWriter->serializer(data::string::classname()));

        // Change the session serializer by setting a new one using setCustomSerializer
        sessionWriter->setCustomSerializer(data::string::classname(), customSerialize);

        // Write the new session
        CPPUNIT_ASSERT_NO_THROW(sessionWriter->write());

        CPPUNIT_ASSERT(std::filesystem::exists(testPath));
    }

    // Test deserialization
    {
        auto sessionReader = std::make_shared<io::session::SessionReader>();
        CPPUNIT_ASSERT(sessionReader);

        // Configure the session reader
        sessionReader->set_file(testPath);

        // Read the session: it should fail since the serializer has been modified by a custom one
        CPPUNIT_ASSERT_THROW(sessionReader->read(), sight::core::exception);

        // Test deserializer getter
        CPPUNIT_ASSERT(sessionReader->deserializer(data::string::classname()));

        // Set the new customDeserializer
        sessionReader->setCustomDeserializer(data::string::classname(), customDeserialize);

        CPPUNIT_ASSERT_NO_THROW(sessionReader->read());

        // Test value
        auto object = std::dynamic_pointer_cast<data::string>(sessionReader->getObject());
        compare<data::string>(object, 0);
    }
}

} // namespace sight::io::session::ut
