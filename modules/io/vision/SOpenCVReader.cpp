/************************************************************************
 *
 * Copyright (C) 2017-2021 IRCAD France
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

#include "SOpenCVReader.hpp"

#include <core/com/Signal.hxx>
#include <core/location/SingleFile.hpp>
#include <core/location/SingleFolder.hpp>

#include <data/CameraSeries.hpp>

#include <io/base/service/ioTypes.hpp>

#include <ui/base/dialog/LocationDialog.hpp>

#include <opencv2/core.hpp>

#include <sstream>

namespace sight::module::io::vision
{

// ----------------------------------------------------------------------------

SOpenCVReader::SOpenCVReader()
{
}

// ----------------------------------------------------------------------------

SOpenCVReader::~SOpenCVReader()
{
}

// ----------------------------------------------------------------------------

void SOpenCVReader::configuring()
{
    sight::io::base::service::IReader::configuring();
}

// ----------------------------------------------------------------------------

void SOpenCVReader::openLocationDialog()
{
    this->defineLocationGUI();
}

//----------------------------------------------------------------------------

bool SOpenCVReader::defineLocationGUI()
{
    bool ok = false;

    // Ask user for the file path
    static auto defaultDirectory = std::make_shared<core::location::SingleFolder>();

    sight::ui::base::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Enter file name" : m_windowTitle);
    dialogFile.setDefaultLocation(defaultDirectory);
    dialogFile.setOption(ui::base::dialog::ILocationDialog::READ);
    dialogFile.setType(ui::base::dialog::ILocationDialog::SINGLE_FILE);
    dialogFile.addFilter("XML or YAML file", "*.xml *.yml *.yaml");

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

void SOpenCVReader::starting()
{
}

// ----------------------------------------------------------------------------

void SOpenCVReader::stopping()
{
}

// ----------------------------------------------------------------------------

void SOpenCVReader::updating()
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

    cv::FileStorage fs(this->getFile().string().c_str(), cv::FileStorage::READ); // Read the settings
    if(!fs.isOpened())
    {
        this->m_readFailed = true;
        SIGHT_ERROR("The file " + this->getFile().string() + " cannot be opened.");
    }

    // Remove all CameraSeries
    const auto data      = m_data.lock();
    const auto camSeries = std::dynamic_pointer_cast<data::CameraSeries>(data.get_shared());
    const size_t cams    = camSeries->getNumberOfCameras();

    for(size_t c = 0 ; c < cams ; ++c)
    {
        data::Camera::sptr cam = camSeries->getCamera(0);
        camSeries->removeCamera(cam);

        auto sig = camSeries->signal<data::CameraSeries::RemovedCameraSignalType>
                       (data::CameraSeries::s_REMOVED_CAMERA_SIG);
        sig->asyncEmit(cam);
    }

    int nbCameras;
    fs["nbCameras"] >> nbCameras;

    for(int c = 0 ; c < nbCameras ; ++c)
    {
        std::stringstream camNum;
        camNum << "camera_" << c;

        cv::FileNode n = fs[camNum.str()];

        std::string id, desc;
        int width, height;
        cv::Mat matrix, dist;
        double scale;
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

        data::Camera::sptr cam = data::Camera::New();
        cam->setFx(matrix.at<double>(0, 0));
        cam->setFy(matrix.at<double>(1, 1));
        cam->setCx(matrix.at<double>(0, 2));
        cam->setCy(matrix.at<double>(1, 2));

        cam->setCameraID(id);
        cam->setDescription(desc);
        cam->setWidth(static_cast<size_t>(width));
        cam->setHeight(static_cast<size_t>(height));

        cam->setDistortionCoefficient(
            dist.at<double>(0),
            dist.at<double>(1),
            dist.at<double>(2),
            dist.at<double>(3),
            dist.at<double>(4)
        );

        cam->setScale(scale);
        cam->setIsCalibrated(true);

        camSeries->addCamera(cam);

        auto sig = camSeries->signal<data::CameraSeries::AddedCameraSignalType>(data::CameraSeries::s_ADDED_CAMERA_SIG);
        sig->asyncEmit(cam);

        cv::Mat extrinsic;
        n["extrinsic"] >> extrinsic;

        if(!extrinsic.empty())
        {
            data::Matrix4::sptr extMat = data::Matrix4::New();

            for(size_t i = 0 ; i < 4 ; ++i)
            {
                for(size_t j = 0 ; j < 4 ; ++j)
                {
                    extMat->setCoefficient(
                        i,
                        j,
                        extrinsic.at<double>(
                            static_cast<int>(i),
                            static_cast<int>(j)
                        )
                    );
                }
            }

            camSeries->setExtrinsicMatrix(static_cast<size_t>(c), extMat);
            auto sigExtrinsic = camSeries->signal<data::CameraSeries::ExtrinsicCalibratedSignalType>
                                    (data::CameraSeries::s_EXTRINSIC_CALIBRATED_SIG);
            sigExtrinsic->asyncEmit();
        }
    }

    fs.release(); // close file

    auto sig = camSeries->signal<data::CameraSeries::ModifiedSignalType>(data::CameraSeries::s_MODIFIED_SIG);
    sig->asyncEmit();

    //clear locations only if it was configured through GUI.
    if(use_dialog)
    {
        this->clearLocations();
    }
}

// ----------------------------------------------------------------------------

sight::io::base::service::IOPathType SOpenCVReader::getIOPathType() const
{
    return sight::io::base::service::FILE;
}

// ----------------------------------------------------------------------------

} //namespace sight::module::io::vision
