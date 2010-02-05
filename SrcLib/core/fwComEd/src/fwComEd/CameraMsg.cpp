/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwComEd/CameraMsg.hpp"

namespace fwComEd
{

//-----------------------------------------------------------------------------

std::string CameraMsg::NEW_CAMERA="NEW_CAMERA";
std::string CameraMsg::CAMERA_MOVING="CAMERA_MOVING";

//-----------------------------------------------------------------------------

CameraMsg::CameraMsg() throw()
{
	m_positionCamera[0] = 0.f;
	m_positionCamera[1] = 0.f;
	m_positionCamera[2] = 0.f;

	m_focalCamera[0] = 0.f;
	m_focalCamera[1] = 0.f;
	m_focalCamera[2] = 0.f;
	m_focalCamera[3] = 0.f;

	m_viewUpCamera[0] = 0.f;
	m_viewUpCamera[1] = 0.f;
	m_viewUpCamera[2] = 0.f;
}

//-----------------------------------------------------------------------------

CameraMsg::~CameraMsg() throw()
{}

//-----------------------------------------------------------------------------

void CameraMsg::info(std::ostream &_sstream ) const
{
	this->ObjectMsg::info(_sstream) ;
}

//-----------------------------------------------------------------------------

void CameraMsg::setPositionCamera( const double _positionValue[3]  )
{
	m_positionCamera[0] = _positionValue[0];
	m_positionCamera[1] = _positionValue[1];
	m_positionCamera[2] = _positionValue[2];
}

//-----------------------------------------------------------------------------

const double* CameraMsg::getPositionCamera( void )const
{
	return m_positionCamera ;
}

//-----------------------------------------------------------------------------

void CameraMsg::setFocalCamera( const double _focalValue[4] )
{
	m_focalCamera[0] = _focalValue[0];
	m_focalCamera[1] = _focalValue[1];
	m_focalCamera[2] = _focalValue[2];
	m_focalCamera[3] = _focalValue[3];
}

//-----------------------------------------------------------------------------

const double* CameraMsg::getFocalCamera( void )const
{
	return m_focalCamera ;
}

//-----------------------------------------------------------------------------

void CameraMsg::setViewUpCamera( const double _viewUpValue[3] )
{
	m_viewUpCamera[0] = _viewUpValue[0];
	m_viewUpCamera[1] = _viewUpValue[1];
	m_viewUpCamera[2] = _viewUpValue[2];
}

//-----------------------------------------------------------------------------

const double* CameraMsg::getViewUpCamera( void )const
{
	return m_viewUpCamera;
}

//-----------------------------------------------------------------------------

} // namespace fwComEd

