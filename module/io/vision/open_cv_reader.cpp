/************************************************************************
 *
 * Copyright (C) 2017-2026 IRCAD France
 * Copyright (C) 2017-2020 IHU Strasbourg
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

#include "open_cv_reader.hpp"

#include <data/camera_set.hpp>

#include <io/__/service/io_types.hpp>

#include <opencv2/core/persistence.hpp>

#include <cmath>
#include <sstream>

namespace sight::module::io::vision
{

// ----------------------------------------------------------------------------

open_cv_reader::open_cv_reader() :
    reader("Enter file name")
{
}

// ----------------------------------------------------------------------------

open_cv_reader::~open_cv_reader()
= default;

// ----------------------------------------------------------------------------

void open_cv_reader::configuring()
{
    sight::io::service::reader::configuring();
}

//------------------------------------------------------------------------------

void open_cv_reader::starting()
{
}

// ----------------------------------------------------------------------------

void open_cv_reader::stopping()
{
}

// ----------------------------------------------------------------------------

void open_cv_reader::updating()
{
    bool use_dialog = false;

    //use dialog only if no file was configured
    if(!this->has_location_defined())
    {
        this->open_location_dialog();
        use_dialog = true;
        if(!this->has_location_defined())
        {
            this->m_read_failed = true;
            return;
        }
    }

    cv::FileStorage fs(this->get_file().string(), cv::FileStorage::READ); // Read the settings
    if(!fs.isOpened())
    {
        this->m_read_failed = true;
        SIGHT_ERROR("The file " + this->get_file().string() + " cannot be opened.");
    }

    // Remove all CameraSet
    const auto data        = m_data.lock();
    const auto camera_set  = std::dynamic_pointer_cast<data::camera_set>(data.get_shared());
    const std::size_t cams = camera_set->size();

    for(std::size_t c = 0 ; c < cams ; ++c)
    {
        data::camera::csptr cam = camera_set->get_camera(0);
        camera_set->remove_camera(std::const_pointer_cast<data::camera>(cam));
        camera_set->async_emit(
            data::camera_set::signals::REMOVED_CAMERA,
            std::const_pointer_cast<data::camera>(cam)
        );
    }

    int nb_cameras = 0;
    fs["nbCameras"] >> nb_cameras;

    for(int c = 0 ; c < nb_cameras ; ++c)
    {
        std::stringstream cam_num;
        cam_num << "camera_" << c;

        cv::FileNode n = fs[cam_num.str()];

        std::string id;
        std::string desc;
        int width  = 0;
        int height = 0;
        cv::Mat matrix;
        cv::Mat dist;
        double scale = NAN;
        n["id"] >> id;
        n["description"] >> desc;
        n["imageWidth"] >> width;
        n["imageHeight"] >> height;
        n["matrix"] >> matrix;
        n["distortion"] >> dist;

        if(!n["scale"].empty())
        {
            n["scale"] >> scale;
        }
        else
        {
            scale = 1.;
        }

        data::camera::sptr cam = std::make_shared<data::camera>();
        cam->set_fx(matrix.at<double>(0, 0));
        cam->set_fy(matrix.at<double>(1, 1));
        cam->set_cx(matrix.at<double>(0, 2));
        cam->set_cy(matrix.at<double>(1, 2));

        cam->set_camera_id(id);
        cam->set_description(desc);
        cam->set_width(static_cast<std::size_t>(width));
        cam->set_height(static_cast<std::size_t>(height));

        cam->set_distortion_coefficient(
            dist.at<double>(0),
            dist.at<double>(1),
            dist.at<double>(2),
            dist.at<double>(3),
            dist.at<double>(4)
        );

        cam->set_scale(scale);
        cam->set_is_calibrated(true);

        camera_set->add_camera(cam);
        camera_set->async_emit(data::camera_set::signals::ADDED_CAMERA, cam);

        cv::Mat extrinsic;
        n["extrinsic"] >> extrinsic;

        if(!extrinsic.empty())
        {
            data::matrix4::sptr ext_mat = std::make_shared<data::matrix4>();

            for(std::size_t i = 0 ; i < 4 ; ++i)
            {
                for(std::size_t j = 0 ; j < 4 ; ++j)
                {
                    (*ext_mat)(i, j) = extrinsic.at<double>(static_cast<int>(i), static_cast<int>(j));
                }
            }

            camera_set->set_extrinsic_matrix(static_cast<std::size_t>(c), ext_mat);
            auto sig_extrinsic = camera_set->signal<data::camera_set::signals::extrinsic_calibrated_t>
                                     (data::camera_set::signals::EXTRINSIC_CALIBRATED);
            sig_extrinsic->async_emit();
        }
    }

    fs.release(); // close file

    camera_set->async_emit(data::signals::MODIFIED);

    //clear locations only if it was configured through GUI.
    if(use_dialog)
    {
        this->clear_locations();
    }
}

// ----------------------------------------------------------------------------

sight::io::service::path_type_t open_cv_reader::get_path_type() const
{
    return sight::io::service::file;
}

// ----------------------------------------------------------------------------

} //namespace sight::module::io::vision
