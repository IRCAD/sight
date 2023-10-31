/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
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

#include <core/com/signal.hxx>
#include <core/location/single_file.hpp>
#include <core/location/single_folder.hpp>

#include <data/camera_set.hpp>

#include <io/__/service/io_types.hpp>

#include <ui/__/dialog/location.hpp>

#include <opencv2/core.hpp>

#include <cmath>
#include <sstream>

namespace sight::module::io::vision
{

// ----------------------------------------------------------------------------

open_cv_reader::open_cv_reader()
= default;

// ----------------------------------------------------------------------------

open_cv_reader::~open_cv_reader()
= default;

// ----------------------------------------------------------------------------

void open_cv_reader::configuring()
{
    sight::io::service::reader::configuring();
}

// ----------------------------------------------------------------------------

void open_cv_reader::open_location_dialog()
{
    this->define_location_gui();
}

//----------------------------------------------------------------------------

bool open_cv_reader::define_location_gui()
{
    bool ok = false;

    // Ask user for the file path
    static auto default_directory = std::make_shared<core::location::single_folder>();

    sight::ui::dialog::location dialog_file;
    dialog_file.set_title(m_window_title.empty() ? "Enter file name" : m_window_title);
    dialog_file.set_default_location(default_directory);
    dialog_file.set_option(ui::dialog::location::read);
    dialog_file.set_type(ui::dialog::location::single_file);
    dialog_file.add_filter("XML or YAML file", "*.xml *.yml *.yaml");

    auto result = std::dynamic_pointer_cast<core::location::single_file>(dialog_file.show());

    if(result)
    {
        this->set_file(result->get_file());
        default_directory->set_folder(result->get_file().parent_path());
        dialog_file.save_default_location(default_directory);
        ok = true;
    }
    else
    {
        this->clear_locations();
    }

    return ok;
}

//----------------------------------------------------------------------------

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
        use_dialog = this->define_location_gui();
        if(!use_dialog)
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

        auto sig = camera_set->signal<data::camera_set::removed_camera_signal_t>
                       (data::camera_set::REMOVED_CAMERA_SIG);
        sig->async_emit(std::const_pointer_cast<data::camera>(cam));
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

        auto sig = camera_set->signal<data::camera_set::added_camera_signal_t>(data::camera_set::ADDED_CAMERA_SIG);
        sig->async_emit(cam);

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
            auto sig_extrinsic = camera_set->signal<data::camera_set::extrinsic_calibrated_signal_t>
                                     (data::camera_set::EXTRINSIC_CALIBRATED_SIG);
            sig_extrinsic->async_emit();
        }
    }

    fs.release(); // close file

    auto sig = camera_set->signal<data::camera_set::modified_signal_t>(data::camera_set::MODIFIED_SIG);
    sig->async_emit();

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
