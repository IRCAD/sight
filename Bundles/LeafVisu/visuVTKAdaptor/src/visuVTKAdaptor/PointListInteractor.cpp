/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKAdaptor/PointListInteractor.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>
#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/Material.hpp>
#include <fwData/PointList.hpp>
#include <fwData/Reconstruction.hpp>

#include <fwRenderVTK/vtk/Helpers.hpp>

#include <fwServices/macros.hpp>

#include <vtkAbstractPropPicker.h>
#include <vtkActor.h>
#include <vtkCommand.h>
#include <vtkCubeSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindowInteractor.h>

#define START_INTERACTION_EVENT vtkCommand::LeftButtonPressEvent
#define STOP_INTERACTION_EVENT  vtkCommand::LeftButtonReleaseEvent

fwServicesRegisterMacro( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::PointListInteractor,
                         ::fwData::PointList );

namespace visuVTKAdaptor
{

class PointListInteractorCallback : public vtkCommand
{
public:
    static PointListInteractorCallback *New()
    {
        return new PointListInteractorCallback();
    }

    PointListInteractorCallback() :
        m_picker(nullptr),
        m_priority(-1.f),
        m_mouseMoveObserved(false)
    {
        this->PassiveObserverOff();
    }

    ~PointListInteractorCallback()
    {
    }

    virtual void Execute( vtkObject *caller, unsigned long eventId, void *)
    {
        assert(m_priority>=0);
        SLM_ASSERT("m_adaptor not instanced", m_adaptor);
        SLM_ASSERT("m_picker not instanced", m_picker);
        if ( m_mouseMoveObserved ||
             (m_adaptor->getInteractor()->GetControlKey() && m_adaptor->getInteractor()->GetShiftKey()) )
        {
            if ( eventId == vtkCommand::MouseMoveEvent )
            {
                SLM_ASSERT("m_mouseMoveObserved not instanced", m_mouseMoveObserved);
                if(pickSomething())
                {
                    process();
                }
            }
            else if ( eventId == START_INTERACTION_EVENT)
            {
                assert(m_mouseMoveObserved == false );
                if ( pickSomething() )
                {
                    m_mouseMoveObserved = true;
                    SetAbortFlag(1);
                    m_adaptor->resetPointList();
                    process();
                    m_adaptor->getInteractor()->AddObserver(vtkCommand::MouseMoveEvent, this, m_priority);
                }
            }
            else if ( eventId == STOP_INTERACTION_EVENT && m_mouseMoveObserved)
            {
                SLM_ASSERT("m_mouseMoveObserved not instanced", m_mouseMoveObserved);
                m_adaptor->getInteractor()->RemoveObservers(vtkCommand::MouseMoveEvent, this);
                m_mouseMoveObserved = false;
            }
        }
    }

    bool pickSomething()
    {
        int x,y;
        double display[3];

        m_adaptor->getInteractor()->GetEventPosition(x, y);
        display[0] = x;
        display[1] = y;
        display[2] = 0;

        return m_picker->Pick( display, m_adaptor->getRenderer() );
    }


    void process() // from
    {
        double world[3] = {-1, 0, 0};
        ::fwRenderVTK::vtk::getNearestPickedPosition(m_picker, m_adaptor->getRenderer(), world);
        OSLM_TRACE("PICK" << world[0] << " ," << world[1] << " ," << world[2] );
        m_adaptor->addPoint( world[0], world[1], world[2] );
    }

    void setAdaptor( PointListInteractor::sptr adaptor)
    {
        m_adaptor = adaptor;
    }

    void setPicker( vtkAbstractPropPicker *adaptor)
    {
        m_picker = adaptor;
    }

    void setPriority( float priority )
    {
        m_priority = priority;
    }

protected:
    PointListInteractor::sptr m_adaptor;
    vtkAbstractPropPicker *m_picker;
    float m_priority;

    bool m_mouseMoveObserved;

};

//------------------------------------------------------------------------------

PointListInteractor::PointListInteractor() throw() :
    m_interactionCommand(nullptr),
    m_priority(0.999f)
{
}

//------------------------------------------------------------------------------

PointListInteractor::~PointListInteractor() throw()
{
}

//------------------------------------------------------------------------------

void PointListInteractor::doConfigure() throw(fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void PointListInteractor::doStart() throw(fwTools::Failed)
{
    PointListInteractorCallback *observer = PointListInteractorCallback::New();
    observer->setAdaptor( PointListInteractor::dynamicCast(this->getSptr()) );
    observer->setPicker(this->getPicker());
    observer->setPriority(  m_priority );

    m_interactionCommand = observer;

    this->getInteractor()->AddObserver(START_INTERACTION_EVENT, m_interactionCommand, m_priority);
    this->getInteractor()->AddObserver(STOP_INTERACTION_EVENT, m_interactionCommand, m_priority);
}

//------------------------------------------------------------------------------

void PointListInteractor::doUpdate() throw(fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void PointListInteractor::doSwap() throw(fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void PointListInteractor::doStop() throw(fwTools::Failed)
{
    this->getInteractor()->RemoveObservers(START_INTERACTION_EVENT, m_interactionCommand);
    this->getInteractor()->RemoveObservers(STOP_INTERACTION_EVENT, m_interactionCommand);
    m_interactionCommand->Delete();
    m_interactionCommand = NULL;
    this->unregisterServices();
}

//------------------------------------------------------------------------------

void PointListInteractor::resetPointList()
{
    ::fwData::PointList::sptr list = this->getObject< ::fwData::PointList >();

    for (auto point : list->getPoints())
    {
        auto sig =
            list->signal< ::fwData::PointList::PointRemovedSignalType >(::fwData::PointList::s_POINT_REMOVED_SIG);
        sig->asyncEmit(point);
    }

    list->getRefPoints().clear();
}

//------------------------------------------------------------------------------

void PointListInteractor::addPoint(const double &x, const double &y, const double &z)
{
    ::fwData::PointList::sptr list             = this->getObject< ::fwData::PointList >();
    ::fwData::Point::PointCoordArrayType coord = {{ x, y, z }};
    ::fwData::Point::sptr p                    = ::fwData::Point::New();
    p->getRefCoord()                           = coord;

    list->getRefPoints().push_back(p);

    auto sig = list->signal< ::fwData::PointList::PointAddedSignalType >(::fwData::PointList::s_POINT_ADDED_SIG);
    sig->asyncEmit(p);
}

//------------------------------------------------------------------------------

} //namespace visuVTKAdaptor
