/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
 * Copyright (C) 2017-2021 IHU Strasbourg
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

#include "helper.hpp"

#include "geometry/vision/detail/ReprojectionError.hpp"

#include <core/spy_log.hpp>

#include <geometry/eigen/helper.hpp>

#include <cmath>
#include <thread>

namespace sight::geometry::vision::helper
{

//-----------------------------------------------------------------------------

error_and_points_t compute_reprojection_error(
    const std::vector<cv::Point3f>& _object_points,
    const std::vector<cv::Point2f>& _image_points,
    const cv::Mat& _rvecs,
    const cv::Mat& _tvecs,
    const cv::Mat& _camera_matrix,
    const cv::Mat& _dist_coeffs
)
{
    error_and_points_t error_and_projected_points;
    std::vector<cv::Point2f> image_points2;
    int total_points = 0;
    double total_err = 0;
    double err       = NAN;

    //projection
    cv::projectPoints(
        cv::Mat(_object_points),
        _rvecs,
        _tvecs,
        _camera_matrix,
        _dist_coeffs,
        image_points2
    );

    error_and_projected_points.second = image_points2;

    //difference
    err = cv::norm(cv::Mat(_image_points), cv::Mat(image_points2), cv::NORM_L2);

    int n = static_cast<int>(_object_points.size());
    total_err    += err * err;
    total_points += n;

    error_and_projected_points.first = std::sqrt(total_err / total_points);

    return error_and_projected_points;
}

//-----------------------------------------------------------------------------

cv::Matx44f camera_pose_monocular(
    const std::vector<cv::Point3f>& _object_points,
    const std::vector<cv::Point2f>& _image_points,
    const cv::Mat _camera_matrix,
    const cv::Mat& _dist_coeffs,
    const int _flag
)
{
    SIGHT_ASSERT(
        "There should be the same number of 3d points than 2d points",
        _object_points.size() == _image_points.size()
    );

    cv::Mat rvec;
    cv::Mat tvec;
    cv::Mat r;
    cv::Mat t;
    t = cv::Mat::eye(4, 4, CV_64F);

    //solvePnP
    cv::solvePnP(_object_points, _image_points, _camera_matrix, _dist_coeffs, rvec, tvec, false, _flag);

    // to matrix
    cv::Rodrigues(rvec, r); // R is 3x3

    t(cv::Range(0, 3), cv::Range(0, 3)) = r * 1;    // copies R into T
    t(cv::Range(0, 3), cv::Range(3, 4)) = tvec * 1; // copies tvec into T

    return cv::Matx44f(t);
}

//-----------------------------------------------------------------------------

cv::Matx44f camera_pose_stereo(
    const std::vector<cv::Point3f>& _object_points,
    const cv::Mat& _camera_matrix1,
    const cv::Mat& _dist_coeffs1,
    const cv::Mat& _camera_matrix2,
    const cv::Mat& _dist_coeffs2,
    const std::vector<cv::Point2f>& _img_points1,
    const std::vector<cv::Point2f>& _img_points2,
    const cv::Mat& _r,
    const cv::Mat& _t
)
{
    //1. initialize solution with solvePnP
    cv::Mat rvec;
    cv::Mat tvec;
    cv::Mat r;
    cv::Mat t;
    t = cv::Mat::eye(4, 4, CV_64F);

    cv::Mat extrinsic = cv::Mat::eye(4, 4, CV_64F);

    extrinsic(cv::Range(0, 3), cv::Range(0, 3)) = _r * 1;
    extrinsic(cv::Range(0, 3), cv::Range(3, 4)) = _t * 1;

    cv::solvePnP(
        _object_points,
        _img_points1,
        _camera_matrix1,
        _dist_coeffs1,
        rvec,
        tvec,
        false,
        cv::SOLVEPNP_ITERATIVE
    );

    std::vector<double> optim_vector = {{
        rvec.at<double>(0), rvec.at<double>(1), rvec.at<double>(2),
        tvec.at<double>(0), tvec.at<double>(1), tvec.at<double>(2)
    }
    };

    //2. Creation of ceres problem
    //Minimization of sum of reprojection error for each points in each images

    ::ceres::Problem problem;

    //Cost function for image 1
    for(std::size_t i = 0 ; i < _img_points1.size() ; ++i)
    {
        ::ceres::CostFunction* cost_function = detail::reprojection_error::create(
            _camera_matrix1,
            _dist_coeffs1,
            _img_points1[i],
            _object_points[i],
            cv::Mat::eye(4, 4, CV_64F)
        );
        problem.AddResidualBlock(
            cost_function,
            nullptr,
            optim_vector.data()
        );
    }

    //image 2
    for(std::size_t i = 0 ; i < _img_points2.size() ; ++i)
    {
        ::ceres::CostFunction* cost_function = detail::reprojection_error::create(
            _camera_matrix2,
            _dist_coeffs2,
            _img_points2[i],
            _object_points[i],
            extrinsic
        );
        problem.AddResidualBlock(
            cost_function,
            nullptr,
            optim_vector.data()
        );
    }

    ::ceres::Solver::Options options;
    options.linear_solver_type           = ::ceres::SPARSE_NORMAL_CHOLESKY;
    options.trust_region_strategy_type   = ::ceres::LEVENBERG_MARQUARDT;
    options.minimizer_progress_to_stdout = false;
    options.gradient_tolerance           = 1e-8;
    options.function_tolerance           = 1e-8;
    options.max_num_iterations           = 100;

    int numthreads = static_cast<int>(std::thread::hardware_concurrency() / 2);

    options.num_threads = numthreads;

    ::ceres::Solver::Summary summary;
    ::ceres::Solve(options, &problem, &summary);

    SIGHT_DEBUG("Ceres report : " + summary.FullReport());

    cv::Mat final_r_vec = (cv::Mat_<double>(3, 1) << optim_vector[0], optim_vector[1], optim_vector[2]);
    cv::Mat final_t_vec = (cv::Mat_<double>(3, 1) << optim_vector[3], optim_vector[4], optim_vector[5]);

    cv::Rodrigues(final_r_vec, r); //Rotation vec. to matrix

    t(cv::Range(0, 3), cv::Range(0, 3)) = r * 1;           // copies R into T
    t(cv::Range(0, 3), cv::Range(3, 4)) = final_t_vec * 1; // copies tvec into T

    return cv::Matx44f(t);
}

//-----------------------------------------------------------------------------

void calibrate_pointing_tool(
    const data::vector::csptr _matrices_vector,
    data::matrix4::sptr _calibration_matrix,
    data::matrix4::sptr _center_matrix
)
{
    if(_matrices_vector->size() < 4)
    {
        SIGHT_WARN("Number of points when computing the tool calibration should be more than 5.");
        return;
    }

    geometry::eigen::helper::EigenMatrix matrix_sum;
    matrix_sum.fill(0.);
    Eigen::Vector4d vector_sum;
    vector_sum.fill(0);

    for(const auto& i : *_matrices_vector)
    {
        data::matrix4::csptr m1 = std::dynamic_pointer_cast<data::matrix4>(i);
        SIGHT_ASSERT("This element of the vector is not a data::matrix4", m1);
        geometry::eigen::helper::EigenMatrix xyz1;
        xyz1.fill(0.);
        xyz1(0, 0) = (*m1)(0, 3);
        xyz1(0, 1) = (*m1)(1, 3);
        xyz1(0, 2) = (*m1)(2, 3);
        xyz1(0, 3) = 1.0;

        matrix_sum = matrix_sum + xyz1.transpose() * xyz1;
        vector_sum = vector_sum + xyz1.squaredNorm() * Eigen::Vector4d(xyz1(0, 0), xyz1(0, 1), xyz1(0, 2), xyz1(0, 3));
    }

    geometry::eigen::helper::EigenMatrix temp_matrix;
    temp_matrix.fill(0.);
    temp_matrix(0, 0) = vector_sum[0];
    temp_matrix(0, 1) = vector_sum[1];
    temp_matrix(0, 2) = vector_sum[2];
    temp_matrix(0, 3) = vector_sum[3];
    temp_matrix       = -temp_matrix* matrix_sum.inverse();

    const double a = -1. * temp_matrix(0, 0) / 2.;
    const double b = -1. * temp_matrix(0, 1) / 2.;
    const double c = -1. * temp_matrix(0, 2) / 2.;

    Eigen::Vector3d translation;
    translation.fill(0);
    for(const auto& i : *_matrices_vector)
    {
        data::matrix4::csptr m1 = std::dynamic_pointer_cast<data::matrix4>(i);
        SIGHT_ASSERT("This element of the vector is not a data::matrix4", m1);
        const geometry::eigen::helper::EigenMatrix point_matrix = geometry::eigen::helper::to_eigen(m1);
        geometry::eigen::helper::EigenMatrix center_matrix(point_matrix);
        const geometry::eigen::helper::EigenMatrix point_matrix_inverse = point_matrix.inverse();

        center_matrix(0, 3) = a;
        center_matrix(1, 3) = b;
        center_matrix(2, 3) = c;

        const geometry::eigen::helper::EigenMatrix calibration_matrix = point_matrix_inverse * center_matrix;
        translation(0) += calibration_matrix(0, 3);
        translation(1) += calibration_matrix(1, 3);
        translation(2) += calibration_matrix(2, 3);
    }

    translation /= static_cast<double>(_matrices_vector->size());

    (*_calibration_matrix)(0, 3) = translation(0);
    (*_calibration_matrix)(1, 3) = translation(1);
    (*_calibration_matrix)(2, 3) = translation(2);

    (*_center_matrix)(0, 3) = a;
    (*_center_matrix)(1, 3) = b;
    (*_center_matrix)(2, 3) = c;
}

//-----------------------------------------------------------------------------

data::point_list::sptr detect_chessboard(
    const cv::Mat& _img,
    std::size_t _x_dim,
    std::size_t _y_dim,
    float _scale
)
{
    data::point_list::sptr pointlist;

    SIGHT_ASSERT("Expected 8bit pixel components, this image has: " << 8 * _img.elemSize1(), _img.elemSize1() == 1);

    // Ensure that we have a true depth-less 2D image.
    const cv::Mat img2d = _img.dims == 3 ? _img.reshape(0, 2, _img.size + 1) : _img;

    cv::Mat gray_img;
    if(_img.channels() == 1)
    {
        gray_img = img2d;
    }
    else
    {
        const auto cvt_method = _img.channels() == 3 ? cv::COLOR_RGB2GRAY : cv::COLOR_RGBA2GRAY;

        cv::cvtColor(img2d, gray_img, cvt_method);
    }

    const cv::Size board_size(static_cast<int>(_x_dim) - 1, static_cast<int>(_y_dim) - 1);
    std::vector<cv::Point2f> corners;

    const int flags = cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_NORMALIZE_IMAGE | cv::CALIB_CB_FILTER_QUADS
                      | cv::CALIB_CB_FAST_CHECK;

    cv::Mat detection_image;

    if(_scale < 1.F)
    {
        cv::resize(gray_img, detection_image, cv::Size(), _scale, _scale);
    }
    else
    {
        detection_image = gray_img;
    }

    const bool pattern_was_found = cv::findChessboardCorners(detection_image, board_size, corners, flags);

    if(pattern_was_found)
    {
        // Rescale points to get their coordinates in the full scale image.
        const auto rescale = [_scale](cv::Point2f& _pt){_pt = _pt / _scale;};
        std::ranges::for_each(corners, rescale);

        // Refine points coordinates in the full scale image.
        cv::TermCriteria term(cv::TermCriteria::MAX_ITER + cv::TermCriteria::EPS, 30, 0.1);
        cv::cornerSubPix(gray_img, corners, cv::Size(5, 5), cv::Size(-1, -1), term);

        pointlist = std::make_shared<data::point_list>();
        data::point_list::container_t& points = pointlist->get_points();
        points.reserve(corners.size());

        const auto cv2_sight_pt = [](const cv::Point2f& _p){return std::make_shared<data::point>(_p.x, _p.y);};
        std::ranges::transform(corners, std::back_inserter(points), cv2_sight_pt);
    }

    return pointlist;
}

// ----------------------------------------------------------------------------

} // namespace sight::geometry::vision::helper
