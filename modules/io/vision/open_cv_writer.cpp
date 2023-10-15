/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
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

#include <core/location/single_file.hpp>
#include <core/location/single_folder.hpp>

#include <data/camera_set.hpp>

#include <ui/__/dialog/location.hpp>

#include <opencv2/core.hpp>

#include <sstream>

namespace sight::module::io::vision
{

// ----------------------------------------------------------------------------

open_cv_writer::open_cv_writer()
= default;

// ----------------------------------------------------------------------------

open_cv_writer::~open_cv_writer()
= default;

// ----------------------------------------------------------------------------

void open_cv_writer::configuring()
{
    sight::io::service::writer::configuring();
}

//----------------------------------------------------------------------------

void open_cv_writer::openLocationDialog()
{
    this->defineLocationGUI();
}

//----------------------------------------------------------------------------

bool open_cv_writer::defineLocationGUI()
{
    bool ok = false;

    // Ask user for the file path
    static auto defaultDirectory = std::make_shared<core::location::single_folder>();

    sight::ui::dialog::location dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Enter file name" : m_windowTitle);
    dialogFile.setDefaultLocation(defaultDirectory);
    dialogFile.setOption(ui::dialog::location::WRITE);
    dialogFile.setType(ui::dialog::location::SINGLE_FILE);
    dialogFile.addFilter("XML file", "*.xml");
    dialogFile.addFilter("YAML file", "*.yaml *.yml");

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
    if(!this->hasLocationDefined())
    {
        use_dialog = this->defineLocationGUI();
        if(!use_dialog)
        {
            m_writeFailed = true;
            return;
        }
    }

    auto data       = m_data.lock();
    auto camera_set = std::dynamic_pointer_cast<const data::camera_set>(data.get_shared());

    if(!camera_set)
    {
        m_writeFailed = true;
    }

    SIGHT_ASSERT("CameraSet is null", camera_set);

    std::size_t numberOfCameras = camera_set->size();

    std::vector<data::camera::csptr> cameras;
    std::vector<cv::Mat> cameraMatrices;
    std::vector<cv::Mat> cameraDistCoefs;

    // Set the cameras
    cv::Mat extrinsic = cv::Mat::eye(4, 4, CV_64F);

    for(std::size_t i = 0 ; i < numberOfCameras ; ++i)
    {
        cameras.push_back(camera_set->get_camera(i));
        cameraMatrices.push_back(cv::Mat::eye(3, 3, CV_64F));
        cameraDistCoefs.push_back(cv::Mat::eye(5, 1, CV_64F));

        cameraMatrices[i].at<double>(0, 0) = cameras[i]->getFx();
        cameraMatrices[i].at<double>(1, 1) = cameras[i]->getFy();
        cameraMatrices[i].at<double>(0, 2) = cameras[i]->getCx();
        cameraMatrices[i].at<double>(1, 2) = cameras[i]->getCy();

        for(std::uint8_t c = 0 ; c < 5 ; ++c)
        {
            cameraDistCoefs[i].at<double>(c, 0) = cameras[i]->getDistortionCoefficient()[c];
        }
    }

    cv::FileStorage fs(this->get_file().string(), cv::FileStorage::WRITE);

    fs << "nbCameras" << static_cast<int>(numberOfCameras);

    for(std::size_t c = 0 ; c < numberOfCameras ; ++c)
    {
        const auto camera = camera_set->get_camera(c);
        std::stringstream camNum;
        camNum << "camera_" << c;

        fs << camNum.str() << "{";
        fs << "id" << camera->getCameraID().c_str();
        fs << "description" << camera->getDescription().c_str();
        fs << "imageWidth" << static_cast<int>(camera->getWidth());
        fs << "imageHeight" << static_cast<int>(camera->getHeight());
        fs << "matrix" << cameraMatrices[c];
        fs << "distortion" << cameraDistCoefs[c];
        fs << "scale" << camera->getScale();

        const auto extrinsicMatrix = camera_set->get_extrinsic_matrix(c);
        if(extrinsicMatrix)
        {
            for(std::uint8_t i = 0 ; i < 4 ; ++i)
            {
                for(std::uint8_t j = 0 ; j < 4 ; ++j)
                {
                    extrinsic.at<double>(i, j) = (*extrinsicMatrix)(i, j);
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
        this->clearLocations();
    }
}

// ----------------------------------------------------------------------------

sight::io::service::IOPathType open_cv_writer::getIOPathType() const
{
    return sight::io::service::FILE;
}

// ----------------------------------------------------------------------------

} //namespace sight::module::io::vision
