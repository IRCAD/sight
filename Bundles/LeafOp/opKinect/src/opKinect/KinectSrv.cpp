#include <fwTools/UUID.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/IEditionService.hpp>
#include "opKinect/KinectSrv.hpp"
#include <fwData/Acquisition.hpp>


namespace opKinect
{

REGISTER_SERVICE( ::fwGui::IActionSrv , ::opKinect::KinectSrv, ::fwData::Acquisition ) ;

/**
 * @brief Constructor
 */
KinectSrv::KinectSrv() throw()
{
}

/**
 * @brief Destructor
 */
KinectSrv::~KinectSrv() throw()
{
}

/**
 * @brief Used to define the service parameters and analyze its configuration.
 */
void KinectSrv::configuring() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();    
}

/**
 * @brief Used to launch the service.
 */
void KinectSrv::starting() throw ( ::fwTools::Failed )
{
    // Get acquisition
    ::fwData::Acquisition::sptr acq = this->getObject< ::fwData::Acquisition >();
    SLM_ASSERT("Associated object is not an acquisition", acq);

    // Start Kinect thread
    kinect = new Kinect(this->getSptr(), acq);
    kinect->start();
}

/**
 * @brief Used to stop the service.
 */
void KinectSrv::stopping() throw ( ::fwTools::Failed )
{
    // Stop Kinect thread
    kinect->stop();
}

/**
 * @brief Called by a data to notify a service.
 *
 * @param msg : Incoming message
 */
void KinectSrv::updating( fwServices::ObjectMsg::csptr msg ) throw ( ::fwTools::Failed )
{
}

/**
 * @brief Called to do an action on the data associated to the service.
 */
void KinectSrv::updating() throw ( ::fwTools::Failed )
{
}

/**
 * @brief info of the class
 */
void KinectSrv::info ( std::ostream &_sstream )
{
}

}
