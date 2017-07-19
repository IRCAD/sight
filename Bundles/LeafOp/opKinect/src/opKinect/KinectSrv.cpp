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

KinectSrv::KinectSrv() noexcept
{
}

//---------------------------------------------------------------------------------------

KinectSrv::~KinectSrv() noexcept
{
}

//---------------------------------------------------------------------------------------

void KinectSrv::configuring()
{
    SLM_TRACE_FUNC();
}

//---------------------------------------------------------------------------------------

void KinectSrv::starting()
{
    ::fwMedData::ModelSeries::sptr ms = this->getObject< ::fwMedData::ModelSeries >();
    SLM_ASSERT("Invalid object", ms);

    // Start Kinect thread
    m_kinect = ::boost::make_shared<Kinect>(this->getSptr(), ms);
    m_kinect->start();
}

//---------------------------------------------------------------------------------------

void KinectSrv::stopping()
{
    // Stop Kinect thread
    m_kinect->stop();
}

//---------------------------------------------------------------------------------------

void KinectSrv::receiving( fwServices::ObjectMsg::csptr msg )
{
}

//---------------------------------------------------------------------------------------

void KinectSrv::updating()
{
}

//---------------------------------------------------------------------------------------

void KinectSrv::info ( std::ostream &_sstream )
{
}

//---------------------------------------------------------------------------------------

}

