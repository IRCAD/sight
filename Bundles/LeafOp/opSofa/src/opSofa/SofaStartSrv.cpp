#include <fwTools/UUID.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/IEditionService.hpp>
#include "opSofa/SofaStartSrv.hpp"
#include <fwData/Acquisition.hpp>
#include <fwData/String.hpp>


namespace opSofa
{

REGISTER_SERVICE( ::fwGui::IActionSrv , ::opSofa::SofaStartSrv, ::fwData::Acquisition ) ;

/**
 * @brief Constructor
 */
SofaStartSrv::SofaStartSrv() throw()
{
}

/**
 * @brief Destructor
 */
SofaStartSrv::~SofaStartSrv() throw()
{
}

/**
 * @brief Used to define the service parameters and analyze its configuration.
 */
void SofaStartSrv::configuring() throw ( ::fwTools::Failed )
{   
}

/**
 * @brief Used to launch the service.
 */
void SofaStartSrv::starting() throw ( ::fwTools::Failed )
{
}

/**
 * @brief Used to stop the service.
 */
void SofaStartSrv::stopping() throw ( ::fwTools::Failed )
{
}

/**
 * @brief Called by a data to notify a service.
 *
 * @param _pMsg : Incoming message
 */
void SofaStartSrv::updating( fwServices::ObjectMsg::csptr msg ) throw ( ::fwTools::Failed )
{
}

/**
 * @brief Called to do an action on the data associated to the service.
 */
void SofaStartSrv::updating() throw ( ::fwTools::Failed )
{
    // Get acquisition
    ::fwData::Acquisition::sptr acq = this->getObject< ::fwData::Acquisition >();

    // Create message start or stop Sofa, half the time
    ::fwServices::ObjectMsg::NewSptr msg;
    msg->addEvent("START_STOP_SOFA");

    // Send message
    ::fwServices::IEditionService::notify(this->getSptr(), acq, msg);
}

/**
 * @brief info of the class
 */
void SofaStartSrv::info ( std::ostream &_sstream )
{
}

}
