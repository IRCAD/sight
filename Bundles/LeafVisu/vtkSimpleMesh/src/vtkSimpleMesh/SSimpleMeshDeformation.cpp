/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include <fwData/Object.hpp>
#include <fwData/Mesh.hpp>
#include <fwData/mt/ObjectReadToWriteLock.hpp>


#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwServices/Base.hpp>
#include <fwServices/registry/ActiveWorkers.hpp>

#include <fwComEd/MeshMsg.hpp>

#include <fwDataTools/MeshGenerator.hpp>

#include "vtkSimpleMesh/SSimpleMeshDeformation.hpp"

fwServicesRegisterMacro( ::fwServices::IController , ::vtkSimpleMesh::SSimpleMeshDeformation , ::fwData::Mesh );



namespace vtkSimpleMesh
{

const ::fwCom::Slots::SlotKeyType SSimpleMeshDeformation::s_START_DEFORMATION_SLOT = "startDeformation";
const ::fwCom::Slots::SlotKeyType SSimpleMeshDeformation::s_STOP_DEFORMATION_SLOT = "stopDeformation";

SSimpleMeshDeformation::SSimpleMeshDeformation() throw()
{
    m_slotStartDeformation = ::fwCom::newSlot( &SSimpleMeshDeformation::startDeformation, this ) ;
    m_slotStopDeformation = ::fwCom::newSlot( &SSimpleMeshDeformation::stopDeformation, this ) ;
    ::fwCom::HasSlots::m_slots( s_START_DEFORMATION_SLOT   , m_slotStartDeformation )
                              ( s_STOP_DEFORMATION_SLOT   , m_slotStopDeformation );

#ifdef COM_LOG
    m_slotStartDeformation->setID( s_START_DEFORMATION_SLOT );
    m_slotStopDeformation->setID( s_STOP_DEFORMATION_SLOT );
#endif

    this->setWorker( ::fwServices::registry::ActiveWorkers::getDefault()->
                                 getWorker( ::fwServices::registry::ActiveWorkers::s_DEFAULT_WORKER ) );
}

//-----------------------------------------------------------------------------

SSimpleMeshDeformation::~SSimpleMeshDeformation() throw()
{
}

//-----------------------------------------------------------------------------

void SSimpleMeshDeformation::starting() throw(fwTools::Failed)
{
    SLM_ASSERT("No valid worker for mesh deformation", m_associatedWorker);
    m_timer = m_associatedWorker->createTimer();

    ::boost::posix_time::time_duration duration = ::boost::posix_time::seconds(5) ;

    m_timer->setFunction(  ::boost::bind( &SSimpleMeshDeformation::updating, this)  );
    m_timer->setDuration(duration);
}

//-----------------------------------------------------------------------------

void SSimpleMeshDeformation::configuring() throw(::fwTools::Failed)
{

}

//-----------------------------------------------------------------------------

void SSimpleMeshDeformation::stopping() throw(fwTools::Failed)
{
    this->stopDeformation();
}

//-----------------------------------------------------------------------------

void SSimpleMeshDeformation::updating() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    ::fwData::Mesh::sptr mesh = this->getObject< ::fwData::Mesh >();

    ::fwData::mt::ObjectReadToWriteLock lock(mesh);
    if (mesh->getNumberOfPoints() > 0)
    {

        lock.upgrade();
        ::fwDataTools::MeshGenerator::shakePoint(mesh);
        lock.downgrade();

        ::fwComEd::MeshMsg::NewSptr msg;;
        msg->addEvent( ::fwComEd::MeshMsg::VERTEX_MODIFIED ) ;

        ::fwData::Object::ObjectModifiedSignalType::sptr sig;
        sig = mesh->signal< ::fwData::Object::ObjectModifiedSignalType >( ::fwData::Object::s_OBJECT_MODIFIED_SIG );

        fwServicesBlockAndNotifyMsgMacro( this->getLightID(), sig, msg, m_slotReceive );
    }
}

//-----------------------------------------------------------------------------

void SSimpleMeshDeformation::receiving( ::fwServices::ObjectMsg::csptr _msg ) throw(fwTools::Failed)
{}

//-----------------------------------------------------------------------------

void SSimpleMeshDeformation::startDeformation()
{
    if (!m_timer->isRunning())
    {
        m_timer->start();
    }
}

//-----------------------------------------------------------------------------

void SSimpleMeshDeformation::stopDeformation()
{
    if (m_timer->isRunning())
    {
        m_timer->stop();
    }
}

//-----------------------------------------------------------------------------

}
