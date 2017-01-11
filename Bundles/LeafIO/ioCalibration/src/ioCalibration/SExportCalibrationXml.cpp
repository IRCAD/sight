/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <opencv2/core.hpp>
#include <opencv2/calib3d.hpp>

#include <fwServices/IService.hpp>
#include <fwServices/macros.hpp>

#include <fwGui/Cursor.hpp>
#include <fwGui/dialog/LocationDialog.hpp>

#include <fwData/mt/ObjectWriteLock.hpp>
#include <fwData/mt/ObjectReadLock.hpp>
#include <fwData/TransformationMatrix3D.hpp>
#include <fwData/location/Folder.hpp>
#include <fwData/location/SingleFile.hpp>

#include <arData/CameraSeries.hpp>

#include <sstream>

#include "ioCalibration/SExportCalibrationXml.hpp"


namespace ioCalibration
{
fwServicesRegisterMacro(::io::IWriter, ::ioCalibration::SExportCalibrationXml, ::arData::CameraSeries);
// ----------------------------------------------------------------------------

SExportCalibrationXml::SExportCalibrationXml()
{

}

// ----------------------------------------------------------------------------

SExportCalibrationXml::~SExportCalibrationXml()
{

}

// ----------------------------------------------------------------------------

void SExportCalibrationXml::configuring() throw (fwTools::Failed)
{
    ::io::IWriter::configuring();
}

// ----------------------------------------------------------------------------

void SExportCalibrationXml::configureWithIHM() throw (fwTools::Failed)
{
    // Ask user for the file path
    static ::boost::filesystem::path _sDefaultPath;

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle("Enter file name");
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::WRITE);
    dialogFile.setType(::fwGui::dialog::ILocationDialog::SINGLE_FILE);
    dialogFile.addFilter("XML file", "*.xml");

    ::fwData::location::SingleFile::sptr result
        = ::fwData::location::SingleFile::dynamicCast( dialogFile.show() );

    if (result)
    {
        _sDefaultPath = result->getPath();
        this->setFile( _sDefaultPath );
        dialogFile.saveDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath.parent_path()) );
    }
    else
    {
        this->clearLocations();
    }
}

// ----------------------------------------------------------------------------

void SExportCalibrationXml::starting() throw (fwTools::Failed)
{

}

// ----------------------------------------------------------------------------

void SExportCalibrationXml::stopping() throw (fwTools::Failed)
{

}

// ----------------------------------------------------------------------------

void SExportCalibrationXml::updating() throw (fwTools::Failed)
{
    ::arData::CameraSeries::sptr camSeries = this->getObject< ::arData::CameraSeries >();

    size_t numberOfCameras = camSeries->getNumberOfCameras();

    std::vector< ::arData::Camera::sptr > cameras;
    std::vector< ::cv::Mat > cameraMatrices;
    std::vector< ::cv::Mat > cameraDistCoefs;

    // Set the cameras
    ::fwData::TransformationMatrix3D::sptr extrinsicMatrix;
    ::cv::Mat extrinsic = ::cv::Mat::eye(4, 4, CV_64F);

    ::fwData::mt::ObjectReadLock camSeriesLock(camSeries);

    for(size_t i = 0; i< numberOfCameras; ++i)
    {
        cameras.push_back(camSeries->getCamera(i));
        cameraMatrices.push_back(::cv::Mat::eye(3, 3, CV_64F));
        cameraDistCoefs.push_back(::cv::Mat::eye(5,1, CV_64F));

        cameraMatrices[i].at<double>(0,0) = cameras[i]->getFx();
        cameraMatrices[i].at<double>(1,1) = cameras[i]->getFy();
        cameraMatrices[i].at<double>(0,2) = cameras[i]->getCx();
        cameraMatrices[i].at<double>(1,2) = cameras[i]->getCy();

        for(int c = 0; c < 5; ++c)
        {
            cameraDistCoefs[i].at<double>(c,0) = cameras[i]->getDistortionCoefficient()[c];
        }
    }

    if( this->hasLocationDefined() )
    {
        ::cv::FileStorage fs(this->getFile().string().c_str(), ::cv::FileStorage::WRITE);

        fs<<"nbCameras"<<(int)numberOfCameras;

        for( int i = 0; i< numberOfCameras; ++i)
        {
            std::stringstream camNum;
            camNum << "camera_"<<i;

            fs << camNum.str() << "{";
            fs << "id"<<camSeries->getCamera(i)->getCameraID().c_str();
            fs << "description" << camSeries->getCamera(i)->getDescription().c_str();
            fs << "imageWidth" << static_cast< int> (camSeries->getCamera(i)->getWidth());
            fs << "imageHeight" << static_cast< int >(camSeries->getCamera(i)->getHeight());
            fs << "matrix" << cameraMatrices[i];
            fs << "distortion" << cameraDistCoefs[i];

            extrinsicMatrix = camSeries->getExtrinsicMatrix(i);
            if(extrinsicMatrix)
            {
                for(unsigned int i = 0; i < 4; ++i)
                {
                    for(unsigned int j = 0; j < 4; ++j)
                    {
                        extrinsic.at< double >(i,j) = extrinsicMatrix->getCoefficient(i,j);
                    }
                }
                fs<<"extrinsic"<<extrinsic;
            }
            fs << "}";
        }

        fs.release();
    }
}

// ----------------------------------------------------------------------------

void SExportCalibrationXml::swapping() throw (fwTools::Failed)
{
    this->stop();
    this->start();
}

// ----------------------------------------------------------------------------

::io::IOPathType SExportCalibrationXml::getIOPathType() const
{
    return ::io::FILE;
}

// ----------------------------------------------------------------------------
} //namespace ioCalibration
