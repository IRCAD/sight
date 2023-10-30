/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
 * Copyright (C) 2017-2019 IHU Strasbourg
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

#include "helperTest.hpp"

#include <core/tools/random/generator.hpp>

#include <data/point.hpp>

#include <geometry/data/matrix4.hpp>
#include <geometry/vision/helper.hpp>

#include <utest_data/Data.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::geometry::vision::ut::helper_test);

namespace sight::geometry::vision::ut
{

using core::tools::random::safe_rand;

using expected_chessboard_t = std::vector<sight::data::point::point_coord_array_t>;

//------------------------------------------------------------------------------

static inline cv::Mat read_rgb_image(const std::string _fname)
{
    cv::Mat img = cv::imread(_fname, cv::IMREAD_COLOR);
    cv::cvtColor(img, img, cv::COLOR_BGR2RGB);

    return img;
}

//------------------------------------------------------------------------------

static inline void compare_chessboards(
    const expected_chessboard_t& _expected,
    const sight::data::point_list::csptr& _detected
)
{
    CPPUNIT_ASSERT_EQUAL(_expected.size(), _detected->get_points().size());

    for(std::uint32_t i = 0 ; i < _expected.size() ; ++i)
    {
        const auto& expected_coords = _expected[i];
        const auto& detected_coords = _detected->get_points()[i]->get_coord();

        CPPUNIT_ASSERT_DOUBLES_EQUAL(expected_coords[0], detected_coords[0], 0.01);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(expected_coords[1], detected_coords[1], 0.01);
        CPPUNIT_ASSERT_EQUAL(0.0, detected_coords[2]);
    }
}

//------------------------------------------------------------------------------

void helper_test::setUp()
{
}

//------------------------------------------------------------------------------

void helper_test::tearDown()
{
}

//------------------------------------------------------------------------------

void helper_test::reprojection_identity()
{
    std::vector<cv::Point3f> object_points;
    object_points.emplace_back(-30.F, 30.F, 0.F);
    object_points.emplace_back(30.F, 30.F, 0.F);
    object_points.emplace_back(30.F, -30.F, 0.F);
    object_points.emplace_back(-30.F, -30.F, 0.F);

    std::vector<cv::Point2f> image_points;
    image_points.emplace_back(-30.F, 30.F);
    image_points.emplace_back(30.F, 30.F);
    image_points.emplace_back(30.F, -30.F);
    image_points.emplace_back(-30.F, -30.F);

    cv::Mat rvec;
    cv::Mat tvec;
    cv::Mat camera_matrix;
    cv::Mat distorsion;

    rvec = cv::Mat::zeros(3, 1, CV_64F);
    tvec = cv::Mat::zeros(3, 1, CV_64F);

    camera_matrix = cv::Mat::eye(3, 3, CV_64F);
    distorsion    = cv::Mat::zeros(5, 1, CV_64F);

    geometry::vision::helper::error_and_points_t result =
        geometry::vision::helper::compute_reprojection_error(
            object_points,
            image_points,
            rvec,
            tvec,
            camera_matrix,
            distorsion
        );

    CPPUNIT_ASSERT_DOUBLES_EQUAL(0., result.first, 10e-6);

    for(unsigned int i = 0 ; i < 4 ; ++i)
    {
        cv::Point2f expected = image_points[i];
        cv::Point2f actual   = result.second[i];

        CPPUNIT_ASSERT_DOUBLES_EQUAL(
            static_cast<double>(expected.x),
            static_cast<double>(actual.x),
            10e-6
        );
        CPPUNIT_ASSERT_DOUBLES_EQUAL(
            static_cast<double>(expected.y),
            static_cast<double>(actual.y),
            10e-6
        );
    }
}

//------------------------------------------------------------------------------

void helper_test::reprojection_real_dataset_test1()
{
    std::vector<cv::Point3f> object_points;
    object_points.emplace_back(-30.F, 30.F, 0.F);
    object_points.emplace_back(30.F, 30.F, 0.F);
    object_points.emplace_back(30.F, -30.F, 0.F);
    object_points.emplace_back(-30.F, -30.F, 0.F);

    std::vector<cv::Point2f> image_points;

    cv::Mat rvec;
    cv::Mat tvec;
    cv::Mat camera_matrix;
    cv::Mat distorsion;

    rvec = cv::Mat::zeros(3, 1, CV_64F);
    tvec = cv::Mat::zeros(3, 1, CV_64F);

    camera_matrix = cv::Mat::eye(3, 3, CV_64F);
    distorsion    = cv::Mat::zeros(5, 1, CV_64F);

    //Values from real dataset 1
    image_points.emplace_back(220.92876F, 224.95102F);
    image_points.emplace_back(393.3707F, 216.15828F);
    image_points.emplace_back(416.92828F, 365.78033F);
    image_points.emplace_back(207.39848F, 375.8844F);

    rvec.at<double>(0) = 2.428872194668891;
    rvec.at<double>(1) = -0.04640821030480094;
    rvec.at<double>(2) = -0.05408638946393874;

    tvec.at<double>(0) = 2.276957511901855;
    tvec.at<double>(1) = 2.249284982681274;
    tvec.at<double>(2) = 197.2704315185547;

    camera_matrix.at<double>(0, 0) = 621.8917806423808; //fx
    camera_matrix.at<double>(0, 2) = 302.178791918929;  //cx

    camera_matrix.at<double>(1, 1) = 640.3221665123945; //fy
    camera_matrix.at<double>(1, 2) = 281.4344544571003; //cy

    distorsion.at<double>(0) = 0.06356924027204514;   //k1
    distorsion.at<double>(1) = -0.07869192957878113;  //k2
    distorsion.at<double>(2) = -0.01339129731059074;  //p1
    distorsion.at<double>(3) = -0.007336273789405823; //p2
    distorsion.at<double>(4) = 0.6075565814971924;    //k3

    std::vector<cv::Point2f> reprojected_points_expected;
    reprojected_points_expected.emplace_back(220.89903F, 224.99107F);
    reprojected_points_expected.emplace_back(393.38748F, 216.12437F);
    reprojected_points_expected.emplace_back(416.922F, 365.82419F);
    reprojected_points_expected.emplace_back(207.41541F, 375.83548F);

    const double expected_err = 0.0462637;

    geometry::vision::helper::error_and_points_t result =
        geometry::vision::helper::compute_reprojection_error(
            object_points,
            image_points,
            rvec,
            tvec,
            camera_matrix,
            distorsion
        );

    CPPUNIT_ASSERT_DOUBLES_EQUAL(expected_err, result.first, 10e-6);

    for(unsigned int i = 0 ; i < 4 ; ++i)
    {
        cv::Point2f expected = reprojected_points_expected[i];
        cv::Point2f actual   = result.second[i];

        CPPUNIT_ASSERT_DOUBLES_EQUAL(
            static_cast<double>(expected.x),
            static_cast<double>(actual.x),
            10e-6
        );
        CPPUNIT_ASSERT_DOUBLES_EQUAL(
            static_cast<double>(expected.y),
            static_cast<double>(actual.y),
            10e-6
        );
    }
}

//------------------------------------------------------------------------------

void helper_test::reprojection_real_dataset_test2()
{
    std::vector<cv::Point3f> object_points;
    object_points.emplace_back(-30.F, 30.F, 0.F);
    object_points.emplace_back(30.F, 30.F, 0.F);
    object_points.emplace_back(30.F, -30.F, 0.F);
    object_points.emplace_back(-30.F, -30.F, 0.F);

    std::vector<cv::Point2f> image_points;

    cv::Mat rvec;
    cv::Mat tvec;
    cv::Mat camera_matrix;
    cv::Mat distorsion;

    rvec = cv::Mat::zeros(3, 1, CV_64F);
    tvec = cv::Mat::zeros(3, 1, CV_64F);

    camera_matrix = cv::Mat::eye(3, 3, CV_64F);
    distorsion    = cv::Mat::zeros(5, 1, CV_64F);

    //Values from real dataset 1
    image_points.emplace_back(232.76308F, 227.31705F);
    image_points.emplace_back(410.1149F, 220.90685F);
    image_points.emplace_back(434.63507F, 372.6174F);
    image_points.emplace_back(216.75545F, 379.34464F);

    rvec.at<double>(0) = 2.397315382812963;
    rvec.at<double>(1) = -0.02181686018178059;
    rvec.at<double>(2) = -0.05900882764341142;

    tvec.at<double>(0) = 6.422468185424805;
    tvec.at<double>(1) = 3.302493572235107;
    tvec.at<double>(2) = 191.1024627685547;

    camera_matrix.at<double>(0, 0) = 621.8917806423808; //fx
    camera_matrix.at<double>(0, 2) = 302.178791918929;  //cx

    camera_matrix.at<double>(1, 1) = 640.3221665123945; //fy
    camera_matrix.at<double>(1, 2) = 281.4344544571003; //cy

    distorsion.at<double>(0) = 0.06356924027204514;   //k1
    distorsion.at<double>(1) = -0.07869192957878113;  //k2
    distorsion.at<double>(2) = -0.01339129731059074;  //p1
    distorsion.at<double>(3) = -0.007336273789405823; //p2
    distorsion.at<double>(4) = 0.6075565814971924;    //k3

    std::vector<cv::Point2f> reprojected_points_expected;
    reprojected_points_expected.emplace_back(232.92502F, 227.35114F);
    reprojected_points_expected.emplace_back(409.92566F, 220.82785F);
    reprojected_points_expected.emplace_back(434.73633F, 372.69672F);
    reprojected_points_expected.emplace_back(216.67587F, 379.3006F);

    const double expected_err = 0.153504;

    geometry::vision::helper::error_and_points_t result =
        geometry::vision::helper::compute_reprojection_error(
            object_points,
            image_points,
            rvec,
            tvec,
            camera_matrix,
            distorsion
        );

    CPPUNIT_ASSERT_DOUBLES_EQUAL(expected_err, result.first, 10e-6);

    for(unsigned int i = 0 ; i < 4 ; ++i)
    {
        cv::Point2f expected = reprojected_points_expected[i];
        cv::Point2f actual   = result.second[i];

        CPPUNIT_ASSERT_DOUBLES_EQUAL(
            static_cast<double>(expected.x),
            static_cast<double>(actual.x),
            10e-6
        );
        CPPUNIT_ASSERT_DOUBLES_EQUAL(
            static_cast<double>(expected.y),
            static_cast<double>(actual.y),
            10e-6
        );
    }
}

//------------------------------------------------------------------------------

void helper_test::tool_calibration_basic()
{
    const std::uint8_t nb_matrices = 34;
    auto matrices_vector           = std::make_shared<sight::data::vector>();

    auto res_center_matrix    = std::make_shared<sight::data::matrix4>();
    auto res_transform_matrix = std::make_shared<sight::data::matrix4>();

    const glm::dvec3 center(0., 0., 0.);
    const double radius = 18;
    const glm::dvec3 translation(radius, 0, 0);

    // generates matrices
    for(std::size_t i = 0 ; i < nb_matrices ; ++i)
    {
        const double angle_in_degrees = safe_rand() % 180;
        const double angle            = glm::radians(angle_in_degrees);

        const double x = safe_rand() % 100 / 100.;
        const double y = safe_rand() % 100 / 100.;
        const double z = safe_rand() % 100 / 100.;

        glm::dvec3 axis(x, y, z);
        axis = glm::normalize(axis);

        glm::dmat4 matrix = glm::translate(glm::dmat4(1.), center);

        matrix = glm::rotate(matrix, angle, axis);

        matrix = glm::translate(matrix, translation);

        auto mat = std::make_shared<sight::data::matrix4>();
        geometry::data::from_glm_mat(*mat, matrix);

        matrices_vector->push_back(mat);
    }

    geometry::vision::helper::calibrate_pointing_tool(matrices_vector, res_transform_matrix, res_center_matrix);

    for(std::uint8_t i = 0 ; i < 3 ; ++i)
    {
        for(std::uint8_t j = 0 ; j < 3 ; ++j)
        {
            const double val = (i == j ? 1. : 0.);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(
                "transform[" + std::to_string(i) + "][" + std::to_string(j) + "]",
                val,
                (*res_transform_matrix)(i, j),
                0.00001
            );
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(
                "center[" + std::to_string(i) + "][" + std::to_string(j) + "]",
                val,
                (*res_center_matrix)(i, j),
                0.00001
            );
        }

        CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(
            "transform[" + std::to_string(i) + "][3]",
            -translation[i],
            (*res_transform_matrix)(i, 3),
            0.00001
        );
        CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(
            "center[" + std::to_string(i) + "][3]",
            center[i],
            (*res_center_matrix)(i, 3),
            0.00001
        );
    }
}

//------------------------------------------------------------------------------

void helper_test::tool_calibration()
{
    const std::uint8_t nb_matrices = 46;
    auto matrices_vector           = std::make_shared<sight::data::vector>();

    auto res_center_matrix    = std::make_shared<sight::data::matrix4>();
    auto res_transform_matrix = std::make_shared<sight::data::matrix4>();

    const glm::dvec3 center(36., 52., -530.);
    const glm::dvec3 translation(-459.45, 46.6, -88.54);

    // generates matrices
    for(std::size_t i = 0 ; i < nb_matrices ; ++i)
    {
        const double angle_in_degrees = safe_rand() % 360;
        const double angle            = glm::radians(angle_in_degrees);

        const double x = safe_rand() % 100 / 100.;
        const double y = safe_rand() % 100 / 100.;
        const double z = safe_rand() % 100 / 100.;

        glm::dvec3 axis(x, y, z);
        axis = glm::normalize(axis);

        glm::dmat4 matrix = glm::translate(glm::dmat4(1.), center);
        matrix = glm::rotate(matrix, angle, axis);
        matrix = glm::translate(matrix, translation);

        sight::data::matrix4::sptr mat = std::make_shared<sight::data::matrix4>();
        geometry::data::from_glm_mat(*mat, matrix);

        matrices_vector->push_back(mat);
    }

    geometry::vision::helper::calibrate_pointing_tool(matrices_vector, res_transform_matrix, res_center_matrix);

    for(std::uint8_t i = 0 ; i < 3 ; ++i)
    {
        for(std::uint8_t j = 0 ; j < 3 ; ++j)
        {
            const double val = (i == j ? 1. : 0.);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(
                "transform[" + std::to_string(i) + "][" + std::to_string(j) + "]",
                val,
                (*res_transform_matrix)(i, j),
                0.00001
            );
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(
                "center[" + std::to_string(i) + "][" + std::to_string(j) + "]",
                val,
                (*res_center_matrix)(i, j),
                0.00001
            );
        }

        CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(
            "transform[" + std::to_string(i) + "][3]",
            -translation[i],
            (*res_transform_matrix)(i, 3),
            0.00001
        );
        CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(
            "center[" + std::to_string(i) + "][3]",
            center[i],
            (*res_center_matrix)(i, 3),
            0.00001
        );
    }
}

//------------------------------------------------------------------------------

void helper_test::chessboard_detection_test()
{
    const auto calib_data_dir = utest_data::dir() / "sight" / "calibration";
    {
        const cv::Mat chess_rgb0 = read_rgb_image((calib_data_dir / "chessboardRGB0.tiff").string());

        CPPUNIT_ASSERT_EQUAL(false, chess_rgb0.empty());
        CPPUNIT_ASSERT_EQUAL(3, chess_rgb0.channels());
        CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(1), chess_rgb0.elemSize1());

        const sight::data::point_list::csptr detected_chess = geometry::vision::helper::detect_chessboard(
            chess_rgb0,
            9,
            6,
            0.25F
        );

        const expected_chessboard_t expected_chessboard {
            {743.776, 429.954, 0.0},
            {822.537, 406.108, 0.0},
            {903.759, 383.811, 0.0},
            {984.949, 360.995, 0.0},
            {1065.96, 338.696, 0.0},
            {1146.08, 317.558, 0.0},
            {1225.5, 297.388, 0.0},
            {1302.24, 276.388, 0.0},
            {762.255, 508.037, 0.0},
            {843.201, 484.556, 0.0},
            {925.183, 459.933, 0.0},
            {1007.52, 436.709, 0.0},
            {1090.01, 413.705, 0.0},
            {1171.69, 392.962, 0.0},
            {1251.22, 370.878, 0.0},
            {1328.73, 350.631, 0.0},
            {781.533, 587.441, 0.0},
            {863.607, 563.586, 0.0},
            {946.907, 539.981, 0.0},
            {1030.25, 515.842, 0.0},
            {1114.75, 492.551, 0.0},
            {1197.15, 468.854, 0.0},
            {1277.41, 446.939, 0.0},
            {1355.75, 424.917, 0.0},
            {802.255, 668.659, 0.0},
            {885.733, 644.733, 0.0},
            {970.318, 621.618, 0.0},
            {1055.95, 597.213, 0.0},
            {1139.18, 573.509, 0.0},
            {1222.72, 548.964, 0.0},
            {1304.79, 525.518, 0.0},
            {1383.7, 501.95, 0.0},
            {824.759, 751.571, 0.0},
            {909.364, 728.133, 0.0},
            {994.358, 704.498, 0.0},
            {1080.42, 680.497, 0.0},
            {1165.74, 656.074, 0.0},
            {1249.06, 630.149, 0.0},
            {1331.57, 605.354, 0.0},
            {1411.5, 581.015, 0.0}
        };

        compare_chessboards(expected_chessboard, detected_chess);
    }

    {
        const cv::Mat chess_rgb1 = read_rgb_image((calib_data_dir / "chessboardRGB1.tiff").string());

        CPPUNIT_ASSERT_EQUAL(false, chess_rgb1.empty());
        CPPUNIT_ASSERT_EQUAL(3, chess_rgb1.channels());
        CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(1), chess_rgb1.elemSize1());

        const sight::data::point_list::csptr detected_chess = geometry::vision::helper::detect_chessboard(
            chess_rgb1,
            9,
            6,
            0.25F
        );

        const expected_chessboard_t expected_chessboard {
            {739.341, 394.44, 0.0},
            {822.388, 400.651, 0.0},
            {907.42, 406.712, 0.0},
            {994.231, 413.023, 0.0},
            {1083.19, 420.158, 0.0},
            {1172.81, 426.635, 0.0},
            {1262.29, 434.602, 0.0},
            {1350.71, 442.513, 0.0},
            {715.38, 444.954, 0.0},
            {801.192, 451.001, 0.0},
            {890.202, 457.584, 0.0},
            {980.726, 464.706, 0.0},
            {1073.71, 471.96, 0.0},
            {1167.41, 480.591, 0.0},
            {1261.17, 487.394, 0.0},
            {1353.41, 496.41, 0.0},
            {688.791, 498.563, 0.0},
            {777.528, 506.308, 0.0},
            {870.225, 514.509, 0.0},
            {965.688, 522.361, 0.0},
            {1061.79, 530.537, 0.0},
            {1160.71, 538.446, 0.0},
            {1258.89, 546.978, 0.0},
            {1356.25, 555.507, 0.0},
            {660.464, 558.027, 0.0},
            {752.304, 566.913, 0.0},
            {849.368, 576.214, 0.0},
            {948.725, 585.055, 0.0},
            {1049.9, 593.711, 0.0},
            {1153.29, 603.112, 0.0},
            {1256.79, 611.605, 0.0},
            {1359.36, 620.074, 0.0},
            {629.889, 622.737, 0.0},
            {726.49, 633.109, 0.0},
            {825.885, 643.74, 0.0},
            {930.803, 653.878, 0.0},
            {1037.3, 664.525, 0.0},
            {1145.55, 674.339, 0.0},
            {1254.45, 683.401, 0.0},
            {1361.35, 691.41, 0.0}
        };

        compare_chessboards(expected_chessboard, detected_chess);
    }

    {
        const cv::Mat chess_gray = cv::imread(
            (calib_data_dir / "chessboardGray.tiff").string(),
            cv::IMREAD_GRAYSCALE
        );

        CPPUNIT_ASSERT_EQUAL(false, chess_gray.empty());
        CPPUNIT_ASSERT_EQUAL(1, chess_gray.channels());
        CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(1), chess_gray.elemSize1());

        const sight::data::point_list::csptr detected_chess = geometry::vision::helper::detect_chessboard(
            chess_gray,
            9,
            6,
            0.25F
        );

        const expected_chessboard_t expected_chessboard {
            {933.376, 504.662, 0.0},
            {980.189, 463.743, 0.0},
            {1033.96, 427.51, 0.0},
            {1081.23, 392.296, 0.0},
            {1126.85, 357.625, 0.0},
            {1169.53, 325.472, 0.0},
            {1209.91, 295.648, 0.0},
            {1248.9, 266.503, 0.0},
            {984.026, 557.646, 0.0},
            {1034.66, 516.435, 0.0},
            {1083.63, 477.243, 0.0},
            {1129.72, 440.497, 0.0},
            {1174.63, 404.916, 0.0},
            {1215.77, 371.629, 0.0},
            {1255.68, 339.41, 0.0},
            {1293.53, 309.586, 0.0},
            {1034.53, 610.859, 0.0},
            {1084.45, 568.443, 0.0},
            {1132.77, 527.856, 0.0},
            {1179.05, 490.443, 0.0},
            {1221.37, 452.432, 0.0},
            {1262.46, 417.321, 0.0},
            {1300.72, 384.633, 0.0},
            {1337.88, 353.154, 0.0},
            {1086.83, 664.58, 0.0},
            {1135.66, 621.173, 0.0},
            {1182.46, 578.872, 0.0},
            {1227.32, 539.044, 0.0},
            {1268.66, 501.042, 0.0},
            {1309.61, 464.375, 0.0},
            {1346.99, 429.715, 0.0},
            {1382.89, 397.553, 0.0},
            {1138.79, 718.459, 0.0},
            {1186.3, 673.672, 0.0},
            {1232.29, 629.863, 0.0},
            {1275.78, 588.117, 0.0},
            {1316.88, 549.465, 0.0},
            {1355.55, 511.947, 0.0},
            {1392.14, 476.584, 0.0},
            {1426.73, 442.136, 0.0}
        };

        compare_chessboards(expected_chessboard, detected_chess);

        const sight::data::point_list::csptr detected_chess2 = geometry::vision::helper::detect_chessboard(
            chess_gray,
            8,
            5,
            0.25F
        );
        CPPUNIT_ASSERT(!detected_chess2);
    }
}

//------------------------------------------------------------------------------

void helper_test::chessboard_detection_scale_test()
{
    const auto calib_data_dir = utest_data::dir() / "sight" / "calibration";

    const cv::Mat chess_rgb0 = read_rgb_image((calib_data_dir / "chessboardRGB0.tiff").string());

    const sight::data::point_list::csptr detected_chess_full_scale =
        geometry::vision::helper::detect_chessboard(chess_rgb0, 9, 6, 1.F);

    const sight::data::point_list::csptr detected_chess_quarter_scale =
        geometry::vision::helper::detect_chessboard(chess_rgb0, 9, 6, 0.25F);

    CPPUNIT_ASSERT_EQUAL(
        detected_chess_full_scale->get_points().size(),
        detected_chess_quarter_scale->get_points().size()
    );

    for(std::uint32_t i = 0 ; i < detected_chess_full_scale->get_points().size() ; ++i)
    {
        const auto& full_scale_coords    = detected_chess_full_scale->get_points()[i]->get_coord();
        const auto& quarter_scale_coords = detected_chess_quarter_scale->get_points()[i]->get_coord();

        for(std::uint8_t j = 0 ; j < 3 ; ++j)
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL(full_scale_coords[j], quarter_scale_coords[j], 0.5);
        }
    }
}

} // namespace sight::geometry::vision::ut
