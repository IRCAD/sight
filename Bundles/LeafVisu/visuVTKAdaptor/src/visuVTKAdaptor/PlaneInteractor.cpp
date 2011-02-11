/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>


#include <fwData/Plane.hpp>

#include <fwMath/PlaneFunctions.hpp>

#include <fwComEd/PlaneMsg.hpp>
#include <fwComEd/PointMsg.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/Factory.hpp>

#include <fwServices/ObjectServiceRegistry.hpp>

#include <vtkRenderWindowInteractor.h>
#include <vtkCommand.h>

#include "visuVTKAdaptor/PlaneInteractor.hpp"
#include <fwServices/IEditionService.hpp>

REGISTER_SERVICE( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::PlaneInteractor, ::fwTools::Object ) ;

namespace visuVTKAdaptor
{

class PlaneInteractorCallback : public vtkCommand
{
public:
    static PlaneInteractorCallback *New()
    { return new PlaneInteractorCallback(); }

    PlaneInteractorCallback()
    {}

    ~PlaneInteractorCallback()
    {}

    virtual void Execute( vtkObject *caller, unsigned long eventId, void *)
    {
        if ( eventId == vtkCommand::KeyPressEvent)
        {
            vtkRenderWindowInteractor *rwi = vtkRenderWindowInteractor::SafeDownCast(caller);
            SLM_ASSERT("invalid vtkRenderWindowInteractor", rwi);
            SLM_ASSERT("invalid m_adaptor", m_adaptor);

            char *keySym = rwi->GetKeySym();

            if (std::string(keySym) == "space")
            {
                m_adaptor->switchPlaneNormal();
            }
            else if (std::string(keySym) == "Escape" || std::string(keySym) == "Tab")
            {
                m_adaptor->deselectPlane();
            }
        }
        else if (eventId == vtkCommand::MouseWheelForwardEvent)
        {
            SetAbortFlag(1);
            m_adaptor->pushPlane(1);
        }
        else if (eventId == vtkCommand::MouseWheelBackwardEvent)
        {
            SetAbortFlag(1);
            m_adaptor->pushPlane(-1);
        }
    }

    void setAdaptor( PlaneInteractor::sptr adaptor)
    {
        m_adaptor = adaptor;
    }

protected :
    PlaneInteractor::sptr m_adaptor;

};

//------------------------------------------------------------------------------

PlaneInteractor::PlaneInteractor() throw()
{
    m_priority = 1;
    handlingEventOff();
}

//------------------------------------------------------------------------------

PlaneInteractor::~PlaneInteractor() throw()
{}

//------------------------------------------------------------------------------

void PlaneInteractor::configuring() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();

    SLM_ASSERT("Tag config is required", m_configuration->getName() == "config");
    //this->setRenderId( m_configuration->getAttributeValue("renderer") );
    //this->setPickerId( m_configuration->getAttributeValue("picker") );
}

//------------------------------------------------------------------------------

void PlaneInteractor::doStart() throw(fwTools::Failed)
{
    if (::fwData::Plane::dynamicCast(this->getObject()))
    {
        PlaneInteractorCallback *observer = PlaneInteractorCallback::New();
        observer->setAdaptor( PlaneInteractor::dynamicCast(this->getSptr()) );

        m_vtkObserver = observer;

        this->getInteractor()->AddObserver(vtkCommand::KeyPressEvent  , m_vtkObserver, m_priority);
        this->getInteractor()->AddObserver(vtkCommand::KeyReleaseEvent, m_vtkObserver, m_priority);
        this->getInteractor()->AddObserver(vtkCommand::MouseWheelForwardEvent, m_vtkObserver, m_priority);
        this->getInteractor()->AddObserver(vtkCommand::MouseWheelBackwardEvent, m_vtkObserver, m_priority);
    }
}

//------------------------------------------------------------------------------

void PlaneInteractor::doUpdate() throw(fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void PlaneInteractor::doSwap() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->doStop();
    this->doStart();
}

//------------------------------------------------------------------------------

void PlaneInteractor::doStop() throw(fwTools::Failed)
{
    if(m_vtkObserver)
    {
        this->getInteractor()->RemoveObservers(vtkCommand::KeyPressEvent  , m_vtkObserver);
        this->getInteractor()->RemoveObservers(vtkCommand::KeyReleaseEvent, m_vtkObserver);
        this->getInteractor()->RemoveObservers(vtkCommand::MouseWheelForwardEvent, m_vtkObserver);
        this->getInteractor()->RemoveObservers(vtkCommand::MouseWheelBackwardEvent, m_vtkObserver);

        m_vtkObserver = NULL;
    }
}

//------------------------------------------------------------------------------

void PlaneInteractor::doUpdate( ::fwServices::ObjectMsg::csptr msg) throw(fwTools::Failed)
{}

//------------------------------------------------------------------------------

void PlaneInteractor::switchPlaneNormal()
{
    ::fwData::Plane::sptr plane ( ::fwData::Plane::dynamicCast( this->getObject() ) );

    if (plane)
    {
        ::fwData::Point::sptr pt0 = plane->getPoints()[0];
        ::fwData::Point::sptr pt1 = plane->getPoints()[1];
        ::fwData::Point::sptr pt2 = plane->getPoints()[2];
        if ( pt0 && pt1 && pt2 )
        {
            plane->setValue(pt0,pt2,pt1);

            ::fwComEd::PlaneMsg::NewSptr modifiedMsg;
            modifiedMsg->addEvent( ::fwComEd::PlaneMsg::PLANE_MODIFIED );
            ::fwServices::IEditionService::notify( this->getSptr(), plane, modifiedMsg);
        }
        this->setVtkPipelineModified();
    }
}

//------------------------------------------------------------------------------

void PlaneInteractor::pushPlane(double factor)
{
    ::fwData::Plane::sptr plane ( ::fwData::Plane::dynamicCast( this->getObject() ) );
    if (plane)
    {
        ::fwData::Point::sptr pt0 = plane->getPoints()[0];
        ::fwData::Point::sptr pt1 = plane->getPoints()[1];
        ::fwData::Point::sptr pt2 = plane->getPoints()[2];

        fwVec3d normal = ::fwMath::getNormal(plane->getPlane());
        if ( pt0 && pt1 && pt2 )
        {
            fwVec3d vec0 = pt0->getCoord();
            fwVec3d vec1 = pt1->getCoord();
            fwVec3d vec2 = pt2->getCoord();

            fwVec3d vector = normal*factor;

            vec0 = vec0 + normal*factor;
            vec1 = vec1 + normal*factor;
            vec2 = vec2 + normal*factor;

            pt0->setCoord(vec0);
            pt1->setCoord(vec1);
            pt2->setCoord(vec2);

            plane->setValue(pt0,pt1,pt2);

            normal = ::fwMath::getNormal(plane->getPlane());

            ::fwComEd::PointMsg::NewSptr modifiedMsg;
            modifiedMsg->addEvent( ::fwComEd::PointMsg::POINT_IS_MODIFIED );
            ::fwServices::IEditionService::notify( this->getSptr(), pt0, modifiedMsg);
            ::fwComEd::PointMsg::NewSptr modifiedMsg2;
            modifiedMsg2->addEvent( ::fwComEd::PointMsg::POINT_IS_MODIFIED );
            ::fwServices::IEditionService::notify( this->getSptr(), pt1, modifiedMsg2);
            ::fwComEd::PointMsg::NewSptr modifiedMsg3;
            modifiedMsg3->addEvent( ::fwComEd::PointMsg::POINT_IS_MODIFIED );
            ::fwServices::IEditionService::notify( this->getSptr(), pt2, modifiedMsg3);
            this->setVtkPipelineModified();
        }
    }
}

//------------------------------------------------------------------------------

void PlaneInteractor::deselectPlane()
{
    ::fwData::Plane::sptr plane ( ::fwData::Plane::dynamicCast( this->getObject() ) );
    if (plane)
    {
        ::fwComEd::PlaneMsg::NewSptr deselectMsg;
        deselectMsg->addEvent( ::fwComEd::PlaneMsg::DESELECT_PLANE );
        ::fwServices::IEditionService::notify( this->getSptr(), plane, deselectMsg);
    }
}


} //namespace visuVTKAdaptor
