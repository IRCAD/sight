/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <arlcore/CameraList.h>

arlCore::CameraList::CameraList( PlaneSystem &universe ):
Particle( universe, "Camera list" )
{}

arlCore::CameraList::~CameraList( void )
{}

void arlCore::CameraList::push_back( const uint32HL &GUID, unsigned int width, unsigned int heigth )
{
    m_widths.push_back(width);
    m_heigths.push_back(heigth);
    m_cameras.push_back( arlCore::Camera( getPlaneSystem(), GUID) );
}

void arlCore::CameraList::addCamera( const uint32HL &GUID, unsigned int width, unsigned int heigth )
{
    push_back( GUID, width, heigth );
}

unsigned int arlCore::CameraList::size( void ) const
{
    return (unsigned int)m_cameras.size();
}

const arlCore::Camera& arlCore::CameraList::operator[]( unsigned int i ) const
{
    return m_cameras[i];
}

const arlCore::Camera& arlCore::CameraList::back( void ) const
{
    return m_cameras.back();
}

arlCore::Camera& arlCore::CameraList::operator[]( unsigned int i )
{
    return m_cameras[i];
}

arlCore::Camera& arlCore::CameraList::back( void )
{
    return m_cameras.back();
}

const std::vector<arlCore::Camera>& arlCore::CameraList::getList( void ) const
{
    return m_cameras;
}

unsigned int arlCore::CameraList::calibrate( const std::string &calibFile, long int date )
{
    if(calibFile=="") return 0;
    const unsigned int NbCameras = this->size();
    unsigned int i, n=0;
    std::vector<long int>calibrationNo( NbCameras );
    for( i=0 ; i<NbCameras ; ++i )
    {
        if((*this)[i].loadParameters( calibFile, date, m_widths[i], m_heigths[i] )) ++n;
        calibrationNo[i] = (*this)[i].getCalibrationID();
        if( i==0 || ( calibrationNo[0]>0 && calibrationNo[i]>0 && calibrationNo[i]==calibrationNo[0] ))
        {   // CommonPlane on first camera
            // TODO Create a common plane by calibration ID ?
            getPlaneSystem().setTrf( getPlane(), (*this)[i].getPlane(), (*this)[i].getExtrinsic() );
        }
    }
    return n;
}
