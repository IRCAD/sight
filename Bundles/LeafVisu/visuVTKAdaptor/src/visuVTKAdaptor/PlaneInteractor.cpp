/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef ANDROID

#include "visuVTKAdaptor/PlaneInteractor.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>

#include <fwData/Plane.hpp>

#include <fwMath/PlaneFunctions.hpp>

#include <fwServices/macros.hpp>


#include <vtkRenderWindowInteractor.h>
#include <vtkCommand.h>

fwServicesRegisterMacro( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::PlaneInteractor, ::fwData::Object );

namespace visuVTKAdaptor
{

class PlaneInteractorCallback : public vtkCommand
{
public:
    static PlaneInteractorCallback *New()
    {
        return new PlaneInteractorCallback();
    }

    PlaneInteractorCallback()
    {
    }

    ~PlaneInteractorCallback()
    {
    }

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

protected:
    PlaneInteractor::sptr m_adaptor;

};

//------------------------------------------------------------------------------

PlaneInteractor::PlaneInteractor() noexcept :
    m_vtkObserver(nullptr),
    m_priority(1.)
{
}

//------------------------------------------------------------------------------

PlaneInteractor::~PlaneInteractor() noexcept
{
}

//------------------------------------------------------------------------------

void PlaneInteractor::doConfigure()
{
}

//------------------------------------------------------------------------------

void PlaneInteractor::doStart()
{
    if (::fwData::Plane::dynamicCast(this->getObject()))
    {
        PlaneInteractorCallback *observer = PlaneInteractorCallback::New();
        observer->setAdaptor( PlaneInteractor::dynamicCast(this->getSptr()) );

        m_vtkObserver = observer;

        this->getInteractor()->AddObserver(vtkCommand::KeyPressEvent, m_vtkObserver, m_priority);
        this->getInteractor()->AddObserver(vtkCommand::KeyReleaseEvent, m_vtkObserver, m_priority);
        this->getInteractor()->AddObserver(vtkCommand::MouseWheelForwardEvent, m_vtkObserver, m_priority);
        this->getInteractor()->AddObserver(vtkCommand::MouseWheelBackwardEvent, m_vtkObserver, m_priority);
    }
}

//------------------------------------------------------------------------------

void PlaneInteractor::doUpdate()
{
}

//------------------------------------------------------------------------------

void PlaneInteractor::doSwap()
{
    SLM_TRACE_FUNC();
    this->doStop();
    this->doStart();
}

//------------------------------------------------------------------------------

void PlaneInteractor::doStop()
{
    if(m_vtkObserver)
    {
        this->getInteractor()->RemoveObservers(vtkCommand::KeyPressEvent, m_vtkObserver);
        this->getInteractor()->RemoveObservers(vtkCommand::KeyReleaseEvent, m_vtkObserver);
        this->getInteractor()->RemoveObservers(vtkCommand::MouseWheelForwardEvent, m_vtkObserver);
        this->getInteractor()->RemoveObservers(vtkCommand::MouseWheelBackwardEvent, m_vtkObserver);
        m_vtkObserver->Delete();
        m_vtkObserver = nullptr;
    }
}

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

            auto sig = plane->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
            sig->asyncEmit();
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

        fwPlane planeDesc;
        ::fwMath::setValues(planeDesc, pt0->getCoord(), pt1->getCoord(), pt2->getCoord());

        fwVec3d normal = ::fwMath::getNormal(planeDesc);
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
            ::fwMath::setValues(planeDesc, pt0->getCoord(), pt1->getCoord(), pt2->getCoord());
            normal = ::fwMath::getNormal(planeDesc);

            ::fwData::Object::ModifiedSignalType::sptr sig;
            sig = pt0->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
            sig->asyncEmit();

            sig = pt1->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
            sig->asyncEmit();

            sig = pt2->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
            sig->asyncEmit();

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
        auto sig = plane->signal< ::fwData::Plane::SelectedSignalType >(::fwData::Plane::s_SELECTED_SIG);
        sig->asyncEmit(false);
    }
}


} //namespace visuVTKAdaptor

#endif // ANDROID
