/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/UUID.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/IEditionService.hpp>
#include "opKinect/KinectSrv.hpp"
#include <fwData/Acquisition.hpp>


namespace opKinect
{

fwServicesRegisterMacro( ::fwGui::IActionSrv , ::opKinect::KinectSrv, ::fwData::Acquisition ) ;

KinectSrv::KinectSrv() throw()
{
}

KinectSrv::~KinectSrv() throw()
{
}

void KinectSrv::configuring() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

void KinectSrv::starting() throw ( ::fwTools::Failed )
{
    // Get acquisition
    ::fwData::Acquisition::sptr acq = this->getObject< ::fwData::Acquisition >();
    SLM_ASSERT("Associated object is not an acquisition", acq);

    // Start Kinect thread
    kinect = new Kinect(this->getSptr(), acq);
    kinect->start();
}

void KinectSrv::stopping() throw ( ::fwTools::Failed )
{
    // Stop Kinect thread
    kinect->stop();
}

void KinectSrv::receiving( fwServices::ObjectMsg::csptr msg ) throw ( ::fwTools::Failed )
{
}

void KinectSrv::updating() throw ( ::fwTools::Failed )
{
}

void KinectSrv::info ( std::ostream &_sstream )
{
}

}
