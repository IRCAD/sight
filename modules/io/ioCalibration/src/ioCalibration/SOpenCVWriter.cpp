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

#include "ioCalibration/SOpenCVWriter.hpp"

#include <data/CameraSeries.hpp>
#include <data/location/Folder.hpp>
#include <data/location/SingleFile.hpp>
#include <data/mt/ObjectReadLock.hpp>

#include <services/macros.hpp>

#include <opencv2/core.hpp>

#include <ui/base/dialog/LocationDialog.hpp>

#include <sstream>

namespace ioCalibration
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
    sight::io::base::services::IWriter::configuring();
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
    static std::filesystem::path _sDefaultPath;

    sight::ui::base::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Enter file name" : m_windowTitle);
    dialogFile.setDefaultLocation( data::location::Folder::New(_sDefaultPath) );
    dialogFile.setOption(ui::base::dialog::ILocationDialog::WRITE);
    dialogFile.setType(ui::base::dialog::ILocationDialog::SINGLE_FILE);
    dialogFile.addFilter("XML file", "*.xml");
    dialogFile.addFilter("YAML file", "*.yaml *.yml");

    data::location::SingleFile::sptr result
        = data::location::SingleFile::dynamicCast( dialogFile.show() );

    if (result)
    {
        _sDefaultPath = result->getPath();
        this->setFile( _sDefaultPath );
        dialogFile.saveDefaultLocation( data::location::Folder::New(_sDefaultPath.parent_path()) );
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

    data::CameraSeries::csptr camSeries = this->getInput< data::CameraSeries >(sight::io::base::services::s_DATA_KEY);

    if(!camSeries)
    {
        m_writeFailed = true;
    }

    SLM_ASSERT("CameraSeries is null", camSeries);

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

    std::vector< data::Camera::sptr > cameras;
    std::vector< ::cv::Mat > cameraMatrices;
    std::vector< ::cv::Mat > cameraDistCoefs;

    // Set the cameras
    data::TransformationMatrix3D::sptr extrinsicMatrix;
    ::cv::Mat extrinsic = ::cv::Mat::eye(4, 4, CV_64F);

    data::mt::ObjectReadLock camSeriesLock(camSeries);

    for(size_t i = 0; i < numberOfCameras; ++i)
    {
        cameras.push_back(camSeries->getCamera(i));
        cameraMatrices.push_back(::cv::Mat::eye(3, 3, CV_64F));
        cameraDistCoefs.push_back(::cv::Mat::eye(5, 1, CV_64F));

        cameraMatrices[i].at<double>(0, 0) = cameras[i]->getFx();
        cameraMatrices[i].at<double>(1, 1) = cameras[i]->getFy();
        cameraMatrices[i].at<double>(0, 2) = cameras[i]->getCx();
        cameraMatrices[i].at<double>(1, 2) = cameras[i]->getCy();

        for(std::uint8_t c = 0; c < 5; ++c)
        {
            cameraDistCoefs[i].at<double>(c, 0) = cameras[i]->getDistortionCoefficient()[c];
        }
    }

    ::cv::FileStorage fs(this->getFile().string().c_str(), ::cv::FileStorage::WRITE);

    fs << "nbCameras"<< static_cast<int>(numberOfCameras);

    for( size_t c = 0; c < numberOfCameras; ++c)
    {
        std::stringstream camNum;
        camNum << "camera_"<< c;

        fs << camNum.str() << "{";
        fs << "id"<<camSeries->getCamera(c)->getCameraID().c_str();
        fs << "description" << camSeries->getCamera(c)->getDescription().c_str();
        fs << "imageWidth" << static_cast< int> (camSeries->getCamera(c)->getWidth());
        fs << "imageHeight" << static_cast< int >(camSeries->getCamera(c)->getHeight());
        fs << "matrix" << cameraMatrices[c];
        fs << "distortion" << cameraDistCoefs[c];
        fs << "scale" << camSeries->getCamera(c)->getScale();

        extrinsicMatrix = camSeries->getExtrinsicMatrix(c);
        if(extrinsicMatrix)
        {
            for(std::uint8_t i = 0; i < 4; ++i)
            {
                for(std::uint8_t j = 0; j < 4; ++j)
                {
                    extrinsic.at< double >(i, j) = extrinsicMatrix->getCoefficient(i, j);
                }
            }
            fs << "extrinsic"<< extrinsic;
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

void SOpenCVWriter::swapping()
{
    this->stop();
    this->start();
}

// ----------------------------------------------------------------------------

sight::io::base::services::IOPathType SOpenCVWriter::getIOPathType() const
{
    return sight::io::base::services::FILE;
}

// ----------------------------------------------------------------------------
} //namespace ioCalibration
