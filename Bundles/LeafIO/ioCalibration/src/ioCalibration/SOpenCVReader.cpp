/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ioCalibration/SOpenCVReader.hpp"

#include <arData/CameraSeries.hpp>

#include <fwCom/Signal.hxx>

#include <fwData/location/Folder.hpp>
#include <fwData/location/SingleFile.hpp>
#include <fwData/mt/ObjectReadLock.hpp>

#include <fwGui/dialog/LocationDialog.hpp>

#include <fwServices/macros.hpp>

#include <opencv2/core.hpp>

#include <sstream>

namespace ioCalibration
{
fwServicesRegisterMacro(::io::IReader, ::ioCalibration::SOpenCVReader);
// ----------------------------------------------------------------------------

SOpenCVReader::SOpenCVReader()
{
}

// ----------------------------------------------------------------------------

SOpenCVReader::~SOpenCVReader()
{
}

// ----------------------------------------------------------------------------

void SOpenCVReader::configuring() throw (::fwTools::Failed)
{
    ::io::IReader::configuring();
}

// ----------------------------------------------------------------------------

void SOpenCVReader::configureWithIHM() throw (::fwTools::Failed)
{
    this->defineLocationGUI();
}

//----------------------------------------------------------------------------

bool SOpenCVReader::defineLocationGUI()
{
    bool ok = false;

    // Ask user for the file path
    static ::boost::filesystem::path _sDefaultPath;

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle("Enter file name");
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::READ);
    dialogFile.setType(::fwGui::dialog::ILocationDialog::SINGLE_FILE);
    dialogFile.addFilter("XML file", "*.xml");
    dialogFile.addFilter("yaml file", "*.yaml");

    ::fwData::location::SingleFile::sptr result
        = ::fwData::location::SingleFile::dynamicCast( dialogFile.show() );

    if (result)
    {
        _sDefaultPath = result->getPath();
        this->setFile( _sDefaultPath );
        dialogFile.saveDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath.parent_path()) );
        ok = true;
    }
    else
    {
        this->clearLocations();
    }

    return ok;
}

//----------------------------------------------------------------------------

void SOpenCVReader::starting() throw (::fwTools::Failed)
{

}

// ----------------------------------------------------------------------------

void SOpenCVReader::stopping() throw (::fwTools::Failed)
{

}

// ----------------------------------------------------------------------------

void SOpenCVReader::updating() throw (::fwTools::Failed)
{

    ::arData::CameraSeries::sptr camSeries = this->getInOut< ::arData::CameraSeries >("target");

    bool use_dialog = false;
    //use dialog only if no file was configured
    if(!this->hasLocationDefined())
    {
        use_dialog = this->defineLocationGUI();
        if(!use_dialog)
        {
            return;
        }
    }

    ::cv::FileStorage fs(this->getFile().string().c_str(), ::cv::FileStorage::READ); // Read the settings
    if (!fs.isOpened())
    {
        SLM_ERROR("The file "+ this->getFile().string() + " cannot be opened.");
    }

    //Clean CameraSeries
    if(camSeries->getNumberOfCameras() > 0)
    {
        camSeries->clear();
    }

    int nbCameras;
    fs["nbCameras"] >> nbCameras;

    for(int c = 0; c < nbCameras; ++c)
    {
        std::stringstream camNum;
        camNum<<"camera_"<<c;

        ::cv::FileNode n = fs[camNum.str()];

        std::string id, desc;
        int width, height;
        ::cv::Mat matrix, dist;
        n["id"] >> id;
        n["description"] >> desc;
        n["imageWidth"] >> width;
        n["imageHeight"] >> height;
        n["matrix"] >> matrix;
        n["distortion"] >> dist;

        ::arData::Camera::sptr cam = ::arData::Camera::New();
        cam->setFx(matrix.at<double>(0, 0));
        cam->setFy(matrix.at<double>(1, 1));
        cam->setCx(matrix.at<double>(0, 2));
        cam->setCy(matrix.at<double>(1, 2));

        cam->setCameraID(id);
        cam->setDescription(desc);
        cam->setWidth(static_cast<size_t>(width));
        cam->setHeight(static_cast<size_t>(height));

        cam->setDistortionCoefficient(dist.at<double>(0),
                                      dist.at<double>(1),
                                      dist.at<double>(2),
                                      dist.at<double>(3),
                                      dist.at<double>(4));

        camSeries->addCamera(cam);

        auto sig = camSeries->signal< ::arData::CameraSeries::AddedCameraSignalType >(
            ::arData::CameraSeries::s_ADDED_CAMERA_SIG);
        sig->asyncEmit(cam);

        ::cv::Mat extrinsic;
        fs["extrinsic"] >> extrinsic;

        if(!extrinsic.empty())
        {
            ::fwData::TransformationMatrix3D::sptr extMat;

            for(size_t i = 0; i < 4; ++i)
            {
                for(size_t j = 0; j < 4; ++j)
                {
                    extMat->setCoefficient(i, j, extrinsic.at< double >(static_cast<int>(i),
                                                                        static_cast<int>(j)));
                }
            }

            camSeries->setExtrinsicMatrix(static_cast<size_t>(c), extMat);
        }
    }

    fs.release(); // close file

    auto sig = camSeries->signal< ::arData::CameraSeries::ModifiedSignalType >(::arData::CameraSeries::s_MODIFIED_SIG);
    sig->asyncEmit();

    //clear locations only if it was configured through GUI.
    if(use_dialog)
    {
        this->clearLocations();
    }
}

// ----------------------------------------------------------------------------

void SOpenCVReader::swapping() throw (::fwTools::Failed)
{
    this->stop();
    this->start();
}

// ----------------------------------------------------------------------------

::io::IOPathType SOpenCVReader::getIOPathType() const
{
    return ::io::FILE;
}

// ----------------------------------------------------------------------------
} //namespace ioCalibration
