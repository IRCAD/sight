/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */



#include <boost/foreach.hpp>

#include <vtkAbstractPropPicker.h>
#include <vtkActor.h>
#include <vtkCommand.h>
#include <vtkCubeSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindowInteractor.h>

#include <fwComEd/PointListMsg.hpp>

#include <fwData/Material.hpp>
#include <fwData/PointList.hpp>
#include <fwData/Reconstruction.hpp>

#include <fwRenderVTK/vtk/Helpers.hpp>

#include <fwServices/Factory.hpp>
#include <fwServices/ObjectServiceRegistry.hpp>
#include <fwServices/macros.hpp>

#include "visuVTKAdaptor/PointListInteractor.hpp"


#define START_INTERACTION_EVENT vtkCommand::LeftButtonPressEvent
#define STOP_INTERACTION_EVENT  vtkCommand::LeftButtonReleaseEvent

REGISTER_SERVICE( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::PointListInteractor, ::fwData::PointList ) ;

namespace visuVTKAdaptor
{

class PointListInteractorCallback : public vtkCommand
{
public:
    static PointListInteractorCallback *New()
    { return new PointListInteractorCallback(); }

    PointListInteractorCallback() : m_mouseMoveObserved(false),
                        m_priority(-1)
    {
        m_picker = NULL;
        this->PassiveObserverOff();
    }

    ~PointListInteractorCallback()
    {

    }

    virtual void Execute( vtkObject *caller, unsigned long eventId, void *)
    {
        assert(m_priority>=0);
        assert(m_adaptor);
        assert(m_picker);
        if ( m_mouseMoveObserved || (m_adaptor->getInteractor()->GetControlKey() && m_adaptor->getInteractor()->GetShiftKey()) )
        {
            if ( eventId == vtkCommand::MouseMoveEvent )
            {
                assert(m_mouseMoveObserved);
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
                assert(m_mouseMoveObserved);
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

        return  m_picker->Pick( display , m_adaptor->getRenderer() );
    }


    void process() // from
    {
        double world[3] = {-1,0,0};
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

protected :
    PointListInteractor::sptr m_adaptor;
    vtkAbstractPropPicker *m_picker;
    float    m_priority;

    bool m_mouseMoveObserved;

};

//------------------------------------------------------------------------------

PointListInteractor::PointListInteractor() throw()
    : m_priority(0.999)
{
    handlingEventOff();
}

//------------------------------------------------------------------------------

PointListInteractor::~PointListInteractor() throw()
{
}

//------------------------------------------------------------------------------

void PointListInteractor::configuring() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();

    assert(m_configuration->getName() == "config");
    this->setPickerId( m_configuration->getAttributeValue("picker") );
    this->setRenderId( m_configuration->getAttributeValue("renderer") );
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
    list->getRefPoints().clear();

    ::fwComEd::PointListMsg::NewSptr msg;
    msg->addEvent(::fwComEd::PointListMsg::ELEMENT_REMOVED);
    ::fwServices::IEditionService::notify(this->getSptr(), list, msg);
}

//------------------------------------------------------------------------------

void PointListInteractor::addPoint(const double &x, const double &y, const double &z)
{
    ::fwData::PointList::sptr list = this->getObject< ::fwData::PointList >();
    ::fwData::Point::PointCoordArrayType coord = {{ x, y, z }};
    ::fwData::Point::NewSptr p;
    p->getRefCoord() = coord;

    list->getRefPoints().push_back(p);

    ::fwComEd::PointListMsg::NewSptr msg;
    msg->addEvent(::fwComEd::PointListMsg::ELEMENT_ADDED);
    ::fwServices::IEditionService::notify(this->getSptr(), list, msg);
}

//------------------------------------------------------------------------------

} //namespace visuVTKAdaptor
