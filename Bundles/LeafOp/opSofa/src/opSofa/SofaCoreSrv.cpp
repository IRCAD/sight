#include <fwTools/UUID.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/IEditionService.hpp>
#include "opSofa/SofaCoreSrv.hpp"
#include <fwData/Acquisition.hpp>
#include <fwData/String.hpp>
#include <fwData/Integer.hpp>
#include <fwData/Float.hpp>
#include <fwData/Vector.hpp>
#include <QMessageBox>


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
    }

    else if (msg->hasEvent("START_STOP_SOFA")) {
        if (sofa) {
            // if animation is running
            if (sofa->isAnimate()) {
                // Stop animation
                sofa->stopThread();
            } else {
                // Start animation
                sofa->startThread();
            }
        } else {
            QMessageBox::warning(0, "Warning", "For launch animation you must first load scene file !");
        }
    }

    else if (msg->hasEvent("EDITOR_MESH_SOFA")) {
        if (sofa) {
            ::fwData::Vector::csptr data = ::fwData::Vector::dynamicConstCast(msg->getDataInfo("EDITOR_MESH_SOFA"));
            ::fwData::String::csptr idMesh = ::fwData::String::dynamicConstCast(data->at(0));
            ::fwData::Integer::csptr strength = ::fwData::Integer::dynamicConstCast(data->at(1));
            sofa->shakeMesh(idMesh->value(), strength->value());
        }
    }

    else if (msg->hasEvent("MOVE_MESH_SOFA")) {
        if (sofa) {
            ::fwData::Vector::csptr data = ::fwData::Vector::dynamicConstCast(msg->getDataInfo("MOVE_MESH_SOFA"));
            ::fwData::String::csptr idMesh = ::fwData::String::dynamicConstCast(data->at(0));
            ::fwData::Integer::csptr x = ::fwData::Integer::dynamicConstCast(data->at(1));
            ::fwData::Integer::csptr y = ::fwData::Integer::dynamicConstCast(data->at(2));
            ::fwData::Integer::csptr z = ::fwData::Integer::dynamicConstCast(data->at(3));
            ::fwData::Float::csptr rx = ::fwData::Float::dynamicConstCast(data->at(4));
            ::fwData::Float::csptr ry = ::fwData::Float::dynamicConstCast(data->at(5));
            ::fwData::Float::csptr rz = ::fwData::Float::dynamicConstCast(data->at(6));
            sofa->moveMesh(idMesh->value(), x->value(), y->value(), z->value(), rx->value(), ry->value(), rz->value());
        }
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
