#include <fwTools/UUID.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/IEditionService.hpp>
#include "opSofa/SofaCoreSrv.hpp"
#include <fwData/Acquisition.hpp>
#include <fwData/String.hpp>


namespace opSofa
{

REGISTER_SERVICE( ::fwGui::IActionSrv , ::opSofa::SofaCoreSrv, ::fwData::Acquisition ) ;

/**
 * @brief Constructor
 */
SofaCoreSrv::SofaCoreSrv() throw()
{
    sofa = NULL;
}

/**
 * @brief Destructor
 */
SofaCoreSrv::~SofaCoreSrv() throw()
{
    delete sofa;
}

/**
 * @brief Used to define the service parameters and analyze its configuration.
 */
void SofaCoreSrv::configuring() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();    
}

/**
 * @brief Used to launch the service.
 */
void SofaCoreSrv::starting() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

/**
 * @brief Used to stop the service.
 */
void SofaCoreSrv::stopping() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

/**
 * @brief Called by a data to notify a service.
 *
 * @param _pMsg : Incoming message
 */
void SofaCoreSrv::updating( fwServices::ObjectMsg::csptr msg ) throw ( ::fwTools::Failed )
{
    if (msg->hasEvent("NEW_SOFA_SCENE")) {
        // Delete object sofa
        delete sofa;

        // Get Path Scn
        ::fwData::String::csptr pathScn = ::fwData::String::dynamicConstCast(msg->getDataInfo("NEW_SOFA_SCENE"));

        // Get acquisition
        ::fwData::Acquisition::sptr acq = this->getObject< ::fwData::Acquisition >();

        // Create object sofa
        sofa = new SofaBusiness();
        sofa->loadScn(pathScn->value(), acq, this->getSptr());

        // Apply at sofa the number of image by second
        sofa->setTimeStepAnimation(1000/50);

        // Start thread sofa
        sofa->startThread();
    }
}

/**
 * @brief Called to do an action on the data associated to the service.
 */
void SofaCoreSrv::updating() throw ( ::fwTools::Failed )
{
}

/**
 * @brief info of the class
 */
void SofaCoreSrv::info ( std::ostream &_sstream )
{
}

}
