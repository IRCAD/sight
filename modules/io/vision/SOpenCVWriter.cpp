/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
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

#include "SOpenCVWriter.hpp"

#include <core/location/SingleFile.hpp>
#include <core/location/SingleFolder.hpp>

#include <data/CameraSeries.hpp>
#include <data/mt/ObjectReadLock.hpp>

#include <service/macros.hpp>

#include <ui/base/dialog/LocationDialog.hpp>

#include <opencv2/core.hpp>

#include <sstream>

namespace sight::module::io::vision
{

// ----------------------------------------------------------------------------

SOpenCVWriter::SOpenCVWriter()
{
}

// ----------------------------------------------------------------------------

SOpenCVWriter::~SOpenCVWriter()
{
}

// ----------------------------------------------------------------------------

void SOpenCVWriter::configuring()
{
    sight::io::base::service::IWriter::configuring();
}

// ----------------------------------------------------------------------------

void SOpenCVWriter::configureWithIHM()
{
    this->openLocationDialog();
}

//----------------------------------------------------------------------------

void SOpenCVWriter::openLocationDialog()
{
    this->defineLocationGUI();
}

//----------------------------------------------------------------------------

bool SOpenCVWriter::defineLocationGUI()
{
    bool ok = false;

    // Ask user for the file path
    static auto defaultDirectory = std::make_shared<core::location::SingleFolder>();

    sight::ui::base::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Enter file name" : m_windowTitle);
    dialogFile.setDefaultLocation(defaultDirectory);
    dialogFile.setOption(ui::base::dialog::ILocationDialog::WRITE);
    dialogFile.setType(ui::base::dialog::ILocationDialog::SINGLE_FILE);
    dialogFile.addFilter("XML file", "*.xml");
    dialogFile.addFilter("YAML file", "*.yaml *.yml");

    auto result = core::location::SingleFile::dynamicCast(dialogFile.show());

    if(result)
    {
        this->setFile(result->getFile());
        defaultDirectory->setFolder(result->getFile().parent_path());
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

void SOpenCVWriter::starting()
{
}

// ----------------------------------------------------------------------------

void SOpenCVWriter::stopping()
{
}

// ----------------------------------------------------------------------------

void SOpenCVWriter::updating()
{
    data::CameraSeries::csptr camSeries = this->getInput<data::CameraSeries>(sight::io::base::service::s_DATA_KEY);

    if(!camSeries)
    {
        m_writeFailed = true;
    }

    SIGHT_ASSERT("CameraSeries is null", camSeries);

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

    data::mt::ObjectReadLock lock(camSeries);
    size_t numberOfCameras = camSeries->getNumberOfCameras();

    std::vector<data::Camera::sptr> cameras;
    std::vector< ::cv::Mat> cameraMatrices;
    std::vector< ::cv::Mat> cameraDistCoefs;

    // Set the cameras
    data::Matrix4::sptr extrinsicMatrix;
    ::cv::Mat extrinsic = ::cv::Mat::eye(4, 4, CV_64F);

    data::mt::ObjectReadLock camSeriesLock(camSeries);

    for(size_t i = 0 ; i < numberOfCameras ; ++i)
    {
        cameras.push_back(camSeries->getCamera(i));
        cameraMatrices.push_back(::cv::Mat::eye(3, 3, CV_64F));
        cameraDistCoefs.push_back(::cv::Mat::eye(5, 1, CV_64F));

        cameraMatrices[i].at<double>(0, 0) = cameras[i]->getFx();
        cameraMatrices[i].at<double>(1, 1) = cameras[i]->getFy();
        cameraMatrices[i].at<double>(0, 2) = cameras[i]->getCx();
        cameraMatrices[i].at<double>(1, 2) = cameras[i]->getCy();

        for(std::uint8_t c = 0 ; c < 5 ; ++c)
        {
            cameraDistCoefs[i].at<double>(c, 0) = cameras[i]->getDistortionCoefficient()[c];
        }
    }

    ::cv::FileStorage fs(this->getFile().string().c_str(), ::cv::FileStorage::WRITE);

    fs << "nbCameras" << static_cast<int>(numberOfCameras);

    for(size_t c = 0 ; c < numberOfCameras ; ++c)
    {
        std::stringstream camNum;
        camNum << "camera_" << c;

        fs << camNum.str() << "{";
        fs << "id" << camSeries->getCamera(c)->getCameraID().c_str();
        fs << "description" << camSeries->getCamera(c)->getDescription().c_str();
        fs << "imageWidth" << static_cast<int>(camSeries->getCamera(c)->getWidth());
        fs << "imageHeight" << static_cast<int>(camSeries->getCamera(c)->getHeight());
        fs << "matrix" << cameraMatrices[c];
        fs << "distortion" << cameraDistCoefs[c];
        fs << "scale" << camSeries->getCamera(c)->getScale();

        extrinsicMatrix = camSeries->getExtrinsicMatrix(c);
        if(extrinsicMatrix)
        {
            for(std::uint8_t i = 0 ; i < 4 ; ++i)
            {
                for(std::uint8_t j = 0 ; j < 4 ; ++j)
                {
                    extrinsic.at<double>(i, j) = extrinsicMatrix->getCoefficient(i, j);
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

sight::io::base::service::IOPathType SOpenCVWriter::getIOPathType() const
{
    return sight::io::base::service::FILE;
}

// ----------------------------------------------------------------------------

} //namespace sight::module::io::vision
