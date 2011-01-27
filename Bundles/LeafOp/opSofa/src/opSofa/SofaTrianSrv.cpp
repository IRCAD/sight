#include <fwTools/UUID.hpp>
#include <fwData/TriangularMesh.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/IEditionService.hpp>
#include <fwComEd/TriangularMeshMsg.hpp>
#include <vtkIO/vtk.hpp>

#include <QInputDialog>

#include "opSofa/SofaTrianSrv.hpp"


namespace opSofa
{

REGISTER_SERVICE( ::fwGui::IActionSrv , ::opSofa::SofaTrianSrv, ::fwTools::Object ) ;

/**
 * Constructor
 */
SofaTrianSrv::SofaTrianSrv() throw() : m_meshUID("")
{
}

/**
 * Destructor
 */
SofaTrianSrv::~SofaTrianSrv() throw()
{
}

/**
 * configuring() method is used to define the service parameters and analyze its configuration.
 */
void SofaTrianSrv::configuring() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC() ;
    this->initialize();

    SLM_ASSERT( "Mesh UID andImage UID must be defined in the service configuration", m_configuration->findConfigurationElement("mesh") );
    m_meshUID = m_configuration->findConfigurationElement("mesh")->getExistingAttributeValue("uid");

    OSLM_INFO( "Mesh UID = " << m_meshUID);
}

/**
 * starting() method is used to launch the service.
 */
void SofaTrianSrv::starting() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    this->actionServiceStarting();
}

/**
 * stopping() method is used to stop the service.
 */
void SofaTrianSrv::stopping() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    this->actionServiceStopping();
}

/**
 * updating(_pMsg) is called by a data to notify a service.
 * @param _pMsg : Incoming message
 */
void SofaTrianSrv::updating( fwServices::ObjectMsg::csptr _pMsg ) throw ( ::fwTools::Failed )
{
}

/**
 * updating() method is called to do an action on the data associated to the service.
 */
void SofaTrianSrv::updating() throw ( ::fwTools::Failed )
{
    int static etat = 1;

    if (etat == 1) {
        etat++;
        SLM_TRACE_FUNC();

        // Retreive object
        OSLM_ASSERT("Not found the mesh defined by uid : " << m_meshUID, ::fwTools::fwID::exist(m_meshUID)) ;
        ::fwData::TriangularMesh::sptr pMesh = ::fwData::TriangularMesh::dynamicCast(::fwTools::fwID::getObject( m_meshUID )) ;

        int ips = QInputDialog::getDouble(0, "Nombre d'image par seconde", "FPS :", 10, 1, 1000, 0);

        sofa = new SofaBusiness();
        sofa->loadMesh(pMesh, this->getSptr());

        if (ips!=0)
        {    sofa->setTimeStepAnimation(1000/ips); }
        else
        {    sofa->setTimeStepAnimation(1000/10); }

        sofa->startThread();

    } else if (etat == 2) {
        sofa->reset();
    }
}

}
