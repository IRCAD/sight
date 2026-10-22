/************************************************************************
 *
 * Copyright (C) 2014-2026 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include "open_cv_writer.hpp"

#include <data/camera_set.hpp>

#include <opencv2/core/mat.hpp>
#include <opencv2/core/persistence.hpp>
#include <sstream>

namespace sight::module::io::vision
{

// ----------------------------------------------------------------------------

open_cv_writer::open_cv_writer() :
    writer("Enter file name")
{
}

// ----------------------------------------------------------------------------

void open_cv_writer::configuring()
{
    sight::io::service::writer::configuring();
}

//------------------------------------------------------------------------------

void open_cv_writer::starting()
{
}

// ----------------------------------------------------------------------------

void open_cv_writer::stopping()
{
}

// ----------------------------------------------------------------------------

void open_cv_writer::updating()
{
    bool use_dialog = false;
    if(!this->has_location_defined())
    {
        this->open_location_dialog();
        use_dialog = true;
        if(!this->has_location_defined())
        {
            m_write_failed = true;
            return;
        }
    }

    auto data       = m_data.lock();
    auto camera_set = std::dynamic_pointer_cast<const data::camera_set>(data.get_shared());

    if(!camera_set)
    {
        m_write_failed = true;
    }

    SIGHT_ASSERT("CameraSet is null", camera_set);

    std::size_t number_of_cameras = camera_set->size();

    std::vector<data::camera::csptr> cameras;
    std::vector<cv::Mat> camera_matrices;
    std::vector<cv::Mat> camera_dist_coefs;

    // Set the cameras
    cv::Mat extrinsic = cv::Mat::eye(4, 4, CV_64F);

    for(std::size_t i = 0 ; i < number_of_cameras ; ++i)
    {
        cameras.push_back(camera_set->get_camera(i));
        camera_matrices.push_back(cv::Mat::eye(3, 3, CV_64F));
        camera_dist_coefs.push_back(cv::Mat::eye(5, 1, CV_64F));

        camera_matrices[i].at<double>(0, 0) = cameras[i]->get_fx();
        camera_matrices[i].at<double>(1, 1) = cameras[i]->get_fy();
        camera_matrices[i].at<double>(0, 2) = cameras[i]->get_cx();
        camera_matrices[i].at<double>(1, 2) = cameras[i]->get_cy();

        for(std::uint8_t c = 0 ; c < 5 ; ++c)
        {
            camera_dist_coefs[i].at<double>(c, 0) = cameras[i]->get_distortion_coefficient()[c];
        }
    }

    cv::FileStorage fs(this->get_file().string(), cv::FileStorage::WRITE);

    fs << "nbCameras" << static_cast<int>(number_of_cameras);

    for(std::size_t c = 0 ; c < number_of_cameras ; ++c)
    {
        const auto camera = camera_set->get_camera(c);
        std::stringstream cam_num;
        cam_num << "camera_" << c;

        fs << cam_num.str() << "{";
        fs << "id" << camera->get_camera_id().c_str();
        fs << "description" << camera->get_description().c_str();
        fs << "imageWidth" << static_cast<int>(camera->get_width());
        fs << "imageHeight" << static_cast<int>(camera->get_height());
        fs << "matrix" << camera_matrices[c];
        fs << "distortion" << camera_dist_coefs[c];
        fs << "scale" << camera->get_scale();

        const auto extrinsic_matrix = camera_set->get_extrinsic_matrix(c);
        if(extrinsic_matrix)
        {
            for(std::uint8_t i = 0 ; i < 4 ; ++i)
            {
                for(std::uint8_t j = 0 ; j < 4 ; ++j)
                {
                    extrinsic.at<double>(i, j) = (*extrinsic_matrix)(i, j);
                }
            }

            fs << "extrinsic" << extrinsic;
        }

        fs << "}";
    }

    fs.release();

    //clear locations only if it was configured through GUI.
    if(use_dialog)
    {
        this->clear_locations();
    }
}

// ----------------------------------------------------------------------------

sight::io::service::path_type_t open_cv_writer::get_path_type() const
{
    return sight::io::service::file;
}

// ----------------------------------------------------------------------------

} //namespace sight::module::io::vision
