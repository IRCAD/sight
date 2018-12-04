/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "visuVTKAdaptor/SPlaneInteractor.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>

#include <fwData/Plane.hpp>

#include <fwMath/PlaneFunctions.hpp>

#include <fwServices/macros.hpp>

#include <vtkCommand.h>
#include <vtkRenderWindowInteractor.h>

fwServicesRegisterMacro( ::fwRenderVTK::IAdaptor, ::visuVTKAdaptor::SPlaneInteractor, ::fwData::Object );

namespace visuVTKAdaptor
{

static const ::fwServices::IService::KeyType s_PLANE_INOUT = "plane";

class SPlaneInteractorCallback : public vtkCommand
{
public:
    //------------------------------------------------------------------------------

    static SPlaneInteractorCallback* New()
    {
        return new SPlaneInteractorCallback();
    }

    SPlaneInteractorCallback()
    {
    }

    ~SPlaneInteractorCallback()
    {
    }

    //------------------------------------------------------------------------------

    virtual void Execute( vtkObject* caller, unsigned long eventId, void*)
    {
        if ( eventId == vtkCommand::KeyPressEvent)
        {
            vtkRenderWindowInteractor* rwi = vtkRenderWindowInteractor::SafeDownCast(caller);
            SLM_ASSERT("invalid vtkRenderWindowInteractor", rwi);
            SLM_ASSERT("invalid m_adaptor", m_adaptor);

            char* keySym = rwi->GetKeySym();

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

    //------------------------------------------------------------------------------

    void setAdaptor( SPlaneInteractor::sptr adaptor)
    {
        m_adaptor = adaptor;
    }

protected:
    SPlaneInteractor::sptr m_adaptor;

};

//------------------------------------------------------------------------------

SPlaneInteractor::SPlaneInteractor() noexcept :
    m_vtkObserver(nullptr),
    m_priority(1.f)
{
}

//------------------------------------------------------------------------------

SPlaneInteractor::~SPlaneInteractor() noexcept
{
}

//------------------------------------------------------------------------------

void SPlaneInteractor::configuring()
{
    this->configureParams();
}

//------------------------------------------------------------------------------

void SPlaneInteractor::starting()
{
    this->initialize();

    if (this->getInOut< ::fwData::Plane >(s_PLANE_INOUT))
    {
        SPlaneInteractorCallback* observer = SPlaneInteractorCallback::New();
        observer->setAdaptor( SPlaneInteractor::dynamicCast(this->getSptr()) );

        m_vtkObserver = observer;

        this->getInteractor()->AddObserver(vtkCommand::KeyPressEvent, m_vtkObserver, m_priority);
        this->getInteractor()->AddObserver(vtkCommand::KeyReleaseEvent, m_vtkObserver, m_priority);
        this->getInteractor()->AddObserver(vtkCommand::MouseWheelForwardEvent, m_vtkObserver, m_priority);
        this->getInteractor()->AddObserver(vtkCommand::MouseWheelBackwardEvent, m_vtkObserver, m_priority);
    }
}

//------------------------------------------------------------------------------

void SPlaneInteractor::updating()
{
}

//------------------------------------------------------------------------------

void SPlaneInteractor::stopping()
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

void SPlaneInteractor::switchPlaneNormal()
{
    ::fwData::Plane::sptr plane = this->getInOut< ::fwData::Plane >(s_PLANE_INOUT);

    if (plane)
    {
        ::fwData::Point::sptr pt0 = plane->getPoints()[0];
        ::fwData::Point::sptr pt1 = plane->getPoints()[1];
        ::fwData::Point::sptr pt2 = plane->getPoints()[2];
        if ( pt0 && pt1 && pt2 )
        {
            plane->setValue(pt0, pt2, pt1);

            auto sig = plane->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
            sig->asyncEmit();
        }
        this->setVtkPipelineModified();
    }
}

//------------------------------------------------------------------------------

void SPlaneInteractor::pushPlane(double factor)
{
    ::fwData::Plane::sptr plane = this->getInOut< ::fwData::Plane >(s_PLANE_INOUT);
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

            vec0 = vec0 + normal*factor;
            vec1 = vec1 + normal*factor;
            vec2 = vec2 + normal*factor;

            pt0->setCoord(vec0);
            pt1->setCoord(vec1);
            pt2->setCoord(vec2);

            plane->setValue(pt0, pt1, pt2);
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

void SPlaneInteractor::deselectPlane()
{
    ::fwData::Plane::csptr plane = this->getInOut< ::fwData::Plane >(s_PLANE_INOUT);
    if (plane)
    {
        auto sig = plane->signal< ::fwData::Plane::SelectedSignalType >(::fwData::Plane::s_SELECTED_SIG);
        sig->asyncEmit(false);
    }
}

} //namespace visuVTKAdaptor
