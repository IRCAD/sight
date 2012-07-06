/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <iostream>
#include <fstream>

#include <fwServices/Base.hpp>
#include <fwServices/ObjectMsg.hpp>
#include <fwServices/IEditionService.hpp>
#include <fwData/TriangularMesh.hpp>
#include <fwData/Camera.hpp>
#include <fwCore/base.hpp>
#include <fwServices/macros.hpp>
#include <fwComEd/CameraMsg.hpp>

#include "ioData/CameraReaderService.hpp"

REGISTER_SERVICE( ::io::IReader , ::ioData::CameraReaderService , ::fwData::Camera ) ;

namespace ioData
{

//-----------------------------------------------------------------------------

CameraReaderService::CameraReaderService()
{
}

//-----------------------------------------------------------------------------

void CameraReaderService::info(std::ostream &_sstream )
{
    this->SuperClass::info( _sstream ) ;
    _sstream << std::endl << " camera calibration file reader" ;
}

//-----------------------------------------------------------------------------

std::vector< std::string > CameraReaderService::getSupportedExtensions()
{
    std::vector< std::string > extensions ;
    extensions.push_back(".cal");
    return extensions ;
}

//-----------------------------------------------------------------------------

CameraReaderService::~CameraReaderService() throw()
{}

//------------------------------------------------------------------------------

::io::IOPathType CameraReaderService::getIOPathType() const
{
    return ::io::FILE;
}

//-----------------------------------------------------------------------------

void CameraReaderService::updating() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    if( this->hasLocationDefined() )
    {
        // Retrieve object
        ::fwData::Camera::sptr cam = this->getObject< ::fwData::Camera>( );
        SLM_ASSERT("cam not instanced", cam);

        OSLM_INFO("path: " << this->getFile().string());
        this->loadCalibration(this->getFile().string(), cam);

        // Notify reading
        ::fwComEd::CameraMsg::NewSptr msg;
        msg->addEvent( ::fwComEd::CameraMsg::NEW_CAMERA ) ;

        ::fwServices::IEditionService::notify(this->getSptr(), cam, msg);
    }
}

//-----------------------------------------------------------------------------

bool CameraReaderService::loadCalibration( const std::string &fileName, ::fwData::Camera::sptr cam )
{
    std::ifstream f;
    f.open (fileName.c_str(), std::fstream::in);
    if (!f.is_open())
        return false;
    double M[5];
    unsigned int i, j;
    ::fwData::TransformationMatrix3D::NewSptr m_extrinsicMatrix;
    for( i=0 ; i<4 ; ++i )
    {
        for( j=0; j<4; ++j )
        {
            f>>M[j];
            m_extrinsicMatrix->setCoefficient(i,j,M[j]);
        }
    }
    cam->setExtrinsicCameraTransformation(m_extrinsicMatrix);
    // alpha values : Focale en X ; Focale en Y
    for( i=0; i<5; ++i )
    {
        f>>M[i];
    }

    cam->setFx(M[0]); // AlphaU
    cam->setFy(M[1]); // AlphaV
    // Principal point : Position du centre optique u0 ; v0
    cam->setCx(M[2]); // u0
    cam->setCy(M[3]); // v0
    /*
    //TODO
    // ajouter coeff de distortion et skew � ::fwData::Video
    cam->setAlphaC(M[4]); // skew (Orthogonalit� de la plaque CCD)
    // Radial 1er ordre ; 2�me ordre ; Tangentiel 1er ordre ; 2�me ordre ; Radial 3�me ordre
    for( i=0 ; i<5 ; ++i )
    {
        f>>M[i];
        setkc(i,M[i]);
    }
    */
    f.close();

    return true;
}

//-----------------------------------------------------------------------------

}
