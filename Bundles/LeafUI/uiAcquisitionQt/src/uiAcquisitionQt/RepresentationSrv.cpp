#include <fwTools/UUID.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/IEditionService.hpp>
#include "uiAcquisitionQt/RepresentationSrv.hpp"
#include <fwData/Acquisition.hpp>
#include <fwData/String.hpp>
#include <fwComEd/MaterialMsg.hpp>
#include <fwComEd/AcquisitionMsg.hpp>
#include <fwComEd/ReconstructionMsg.hpp>
#include <fwComEd/TriangularMeshMsg.hpp>


namespace uiAcquisition
{

REGISTER_SERVICE( ::fwGui::IActionSrv , ::uiAcquisition::RepresentationSrv, ::fwData::Acquisition ) ;

/**
 * @brief Constructor
 */
RepresentationSrv::RepresentationSrv() throw()
{
}

/**
 * @brief Destructor
 */
RepresentationSrv::~RepresentationSrv() throw()
{
}

/**
 * @brief Used to define the service parameters and analyze its configuration.
 */
void RepresentationSrv::configuring() throw ( ::fwTools::Failed )
{  
}

/**
 * @brief Used to launch the service.
 */
void RepresentationSrv::starting() throw ( ::fwTools::Failed )
{
    // Get acquisition
    ::fwData::Acquisition::sptr acq = this->getObject< ::fwData::Acquisition >();
    SLM_ASSERT("Associated object is not an acquisition", acq);

    // Fill Reconstruction vector
    fillReconstructionVector(acq);
}

/**
 * @brief Used to stop the service.
 */
void RepresentationSrv::stopping() throw ( ::fwTools::Failed )
{
}

/**
 * @brief Called by a data to notify a service.
 *
 * @param msg : Incoming message
 */
void RepresentationSrv::updating( fwServices::ObjectMsg::csptr msg ) throw ( ::fwTools::Failed )
{        
    if (msg->hasEvent("KINECT_SWIPE_RIGHT_EVENT") || msg->hasEvent("KINECT_SWIPE_LEFT_EVENT")) {
        updating();
    }
}

/**
 * @brief Called to do an action on the data associated to the service.
 */
void RepresentationSrv::updating() throw ( ::fwTools::Failed )
{
    int static etat = 0;

    etat++;
    if (etat >= m_reconstructions.size()) {
        etat = 0;
    }

    // change alpha
    ::fwData::Material::sptr material = m_reconstructions[etat]->getMaterial();
    material->ambient()->alpha() = 0.5 ;

    // notify
    ::fwComEd::MaterialMsg::NewSptr msg23;
    msg23->addEvent( ::fwComEd::MaterialMsg::MATERIAL_IS_MODIFIED );
    ::fwServices::IEditionService::notify(this->getSptr(), material, msg23);
}

/**
 * @brief info of the class
 */
void RepresentationSrv::info ( std::ostream &_sstream )
{
}


void RepresentationSrv::fillReconstructionVector(::fwData::Acquisition::sptr acquisition)
{
     std::pair< ::fwData::Acquisition::ReconstructionIterator,
                ::fwData::Acquisition::ReconstructionIterator > 
                reconstructionIters = acquisition->getReconstructions();

    ::fwData::Acquisition::ReconstructionIterator reconstruction = reconstructionIters.first;
    while( reconstruction != reconstructionIters.second )
    {
        // Get reconstruction
        ::fwData::Reconstruction::sptr rec = (*reconstruction);
        m_reconstructions.push_back(rec);
  
        reconstruction++;
    }    
}


}
