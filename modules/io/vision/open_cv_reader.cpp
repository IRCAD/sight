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

#include <io/__/service/ioTypes.hpp>

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

void open_cv_reader::openLocationDialog()
{
    this->defineLocationGUI();
}

//----------------------------------------------------------------------------

bool open_cv_reader::defineLocationGUI()
{
    bool ok = false;

    // Ask user for the file path
    static auto defaultDirectory = std::make_shared<core::location::single_folder>();

    sight::ui::dialog::location dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Enter file name" : m_windowTitle);
    dialogFile.setDefaultLocation(defaultDirectory);
    dialogFile.setOption(ui::dialog::location::READ);
    dialogFile.setType(ui::dialog::location::SINGLE_FILE);
    dialogFile.addFilter("XML or YAML file", "*.xml *.yml *.yaml");

    auto result = std::dynamic_pointer_cast<core::location::single_file>(dialogFile.show());

    if(result)
    {
        this->set_file(result->get_file());
        defaultDirectory->set_folder(result->get_file().parent_path());
        dialogFile.saveDefaultLocation(defaultDirectory);
        ok = true;
    }
    else
    {
        this->clearLocations();
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
    if(!this->hasLocationDefined())
    {
        use_dialog = this->defineLocationGUI();
        if(!use_dialog)
        {
            this->m_readFailed = true;
            return;
        }
    }

    cv::FileStorage fs(this->get_file().string(), cv::FileStorage::READ); // Read the settings
    if(!fs.isOpened())
    {
        this->m_readFailed = true;
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

    int nbCameras = 0;
    fs["nbCameras"] >> nbCameras;

    for(int c = 0 ; c < nbCameras ; ++c)
    {
        std::stringstream camNum;
        camNum << "camera_" << c;

        cv::FileNode n = fs[camNum.str()];

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
        cam->setFx(matrix.at<double>(0, 0));
        cam->setFy(matrix.at<double>(1, 1));
        cam->setCx(matrix.at<double>(0, 2));
        cam->setCy(matrix.at<double>(1, 2));

        cam->setCameraID(id);
        cam->setDescription(desc);
        cam->setWidth(static_cast<std::size_t>(width));
        cam->setHeight(static_cast<std::size_t>(height));

        cam->setDistortionCoefficient(
            dist.at<double>(0),
            dist.at<double>(1),
            dist.at<double>(2),
            dist.at<double>(3),
            dist.at<double>(4)
        );

        cam->setScale(scale);
        cam->setIsCalibrated(true);

        camera_set->add_camera(cam);

        auto sig = camera_set->signal<data::camera_set::added_camera_signal_t>(data::camera_set::ADDED_CAMERA_SIG);
        sig->async_emit(cam);

        cv::Mat extrinsic;
        n["extrinsic"] >> extrinsic;

        if(!extrinsic.empty())
        {
            data::matrix4::sptr extMat = std::make_shared<data::matrix4>();

            for(std::size_t i = 0 ; i < 4 ; ++i)
            {
                for(std::size_t j = 0 ; j < 4 ; ++j)
                {
                    (*extMat)(i, j) = extrinsic.at<double>(static_cast<int>(i), static_cast<int>(j));
                }
            }

            camera_set->set_extrinsic_matrix(static_cast<std::size_t>(c), extMat);
            auto sigExtrinsic = camera_set->signal<data::camera_set::extrinsic_calibrated_signal_t>
                                    (data::camera_set::EXTRINSIC_CALIBRATED_SIG);
            sigExtrinsic->async_emit();
        }
    }

    fs.release(); // close file

    auto sig = camera_set->signal<data::camera_set::ModifiedSignalType>(data::camera_set::MODIFIED_SIG);
    sig->async_emit();

    //clear locations only if it was configured through GUI.
    if(use_dialog)
    {
        this->clearLocations();
    }
}

// ----------------------------------------------------------------------------

sight::io::service::IOPathType open_cv_reader::getIOPathType() const
{
    return sight::io::service::FILE;
}

// ----------------------------------------------------------------------------

} //namespace sight::module::io::vision
