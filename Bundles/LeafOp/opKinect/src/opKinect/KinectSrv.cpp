/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwMedData/ModelSeries.hpp>

 #include <boost/make_shared.hpp>

#include <fwTools/UUID.hpp>

#include <fwServices/macros.hpp>



#include "opKinect/KinectSrv.hpp"


namespace opKinect
{

fwServicesRegisterMacro(::fwGui::IActionSrv, ::opKinect::KinectSrv, ::fwMedData::ModelSeries);

//---------------------------------------------------------------------------------------

KinectSrv::KinectSrv() throw()
{
}

//---------------------------------------------------------------------------------------

KinectSrv::~KinectSrv() throw()
{
}

//---------------------------------------------------------------------------------------

void KinectSrv::configuring() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

//---------------------------------------------------------------------------------------

void KinectSrv::starting() throw ( ::fwTools::Failed )
{
    ::fwMedData::ModelSeries::sptr ms = this->getObject< ::fwMedData::ModelSeries >();
    SLM_ASSERT("Invalid object", ms);

    // Start Kinect thread
    m_kinect = ::boost::make_shared<Kinect>(this->getSptr(), ms);
    m_kinect->start();
}

//---------------------------------------------------------------------------------------

void KinectSrv::stopping() throw ( ::fwTools::Failed )
{
    // Stop Kinect thread
    m_kinect->stop();
}

//---------------------------------------------------------------------------------------

void KinectSrv::receiving( fwServices::ObjectMsg::csptr msg ) throw ( ::fwTools::Failed )
{
}

//---------------------------------------------------------------------------------------

void KinectSrv::updating() throw ( ::fwTools::Failed )
{
}

//---------------------------------------------------------------------------------------

void KinectSrv::info ( std::ostream &_sstream )
{
}

//---------------------------------------------------------------------------------------

}

