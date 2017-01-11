/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKAdaptor/PickerInteractor.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>

#include <fwCore/HiResClock.hpp>

#include <fwData/Composite.hpp>
#include <fwData/Material.hpp>
#include <fwData/Reconstruction.hpp>

#include <fwRenderVTK/vtk/Helpers.hpp>

#include <fwServices/macros.hpp>

#include <vtkAbstractPropPicker.h>
#include <vtkActor.h>
#include <vtkCellPicker.h>
#include <vtkCommand.h>
#include <vtkCubeSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindowInteractor.h>

#define START_INTERACTION_EVENT vtkCommand::LeftButtonPressEvent
#define STOP_INTERACTION_EVENT  vtkCommand::LeftButtonReleaseEvent

fwServicesRegisterMacro( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::PickerInteractor, ::fwData::Composite );

namespace visuVTKAdaptor
{

static const std::map< unsigned long, ::fwDataTools::PickingInfo::Event > s_vtkEventIDConversion
{
    { vtkCommand::LeftButtonReleaseEvent, ::fwDataTools::PickingInfo::Event::MOUSE_LEFT_UP },
    { vtkCommand::RightButtonReleaseEvent, ::fwDataTools::PickingInfo::Event::MOUSE_RIGHT_UP },
    { vtkCommand::MiddleButtonReleaseEvent, ::fwDataTools::PickingInfo::Event::MOUSE_MIDDLE_UP },
    { vtkCommand::MouseWheelForwardEvent, ::fwDataTools::PickingInfo::Event::MOUSE_WHEELFORWARD },
    { vtkCommand::LeftButtonPressEvent, ::fwDataTools::PickingInfo::Event::MOUSE_LEFT_DOWN },
    { vtkCommand::RightButtonPressEvent, ::fwDataTools::PickingInfo::Event::MOUSE_RIGHT_DOWN },
    { vtkCommand::MiddleButtonPressEvent, ::fwDataTools::PickingInfo::Event::MOUSE_MIDDLE_DOWN },
    { vtkCommand::MouseWheelBackwardEvent, ::fwDataTools::PickingInfo::Event::MOUSE_WHEELBACKWARD },
    { vtkCommand::MouseMoveEvent, ::fwDataTools::PickingInfo::Event::MOUSE_MOVE },
    { vtkCommand::KeyPressEvent, ::fwDataTools::PickingInfo::Event::KEY_PRESS }
};

PickerInteractor::MapEventIdType PickerInteractor::m_eventIdConversion
{
    { std::string("MOUSE_LEFT_UP"), MOUSE_LEFT_UP },
    { std::string("MOUSE_RIGHT_UP"), MOUSE_RIGHT_UP },
    { std::string("MOUSE_MIDDLE_UP"), MOUSE_MIDDLE_UP },
    { std::string("MOUSE_WHEELBACKWARD"), MOUSE_WHEELBACKWARD },
    { std::string("MOUSE_LEFT_DOWN"), MOUSE_LEFT_DOWN },
    { std::string("MOUSE_RIGHT_DOWN"), MOUSE_RIGHT_DOWN },
    { std::string("MOUSE_MIDDLE_DOWN"), MOUSE_MIDDLE_DOWN },
    { std::string("MOUSE_WHEELBACKWARD"), MOUSE_WHEELBACKWARD },
    { std::string("MOUSE_MOVE"), MOUSE_MOVE },
    { std::string("KEY_PRESS"), KEY_PRESS }
};

const ::fwCom::Signals::SignalKeyType PickerInteractor::s_PICKED_SIGNAL = "picked";

//------------------------------------------------------------------------------


class PickerInteractorCallback : public vtkCommand
{
public:
    static PickerInteractorCallback* New()
    {
        return new PickerInteractorCallback();
    }

    PickerInteractorCallback() : m_picker(nullptr), m_eventId(nullptr)
    {
        this->PassiveObserverOn();
    }

    ~PickerInteractorCallback()
    {
    }

    virtual void Execute( vtkObject* caller, unsigned long eventId, void*)
    {
        SLM_ASSERT("m_adaptor not instanced", m_adaptor);
        SLM_ASSERT("m_picker not instanced", m_picker);

        this->process(vtkRenderWindowInteractor::SafeDownCast(caller), eventId);
    }

    bool pickSomething()
    {
        int x,y;
        double display[3];

        m_adaptor->getInteractor()->GetEventPosition(x, y);
        display[0] = x;
        display[1] = y;
        display[2] = 0;

        return (m_picker->Pick( display, m_adaptor->getRenderer() ) != 0);
    }


    void process(vtkRenderWindowInteractor* caller, unsigned long eventId) // from
    {
        SLM_ASSERT("bad vtk caller", caller);
        if( m_eventId->find( static_cast< PickerInteractor::EventID>(eventId) ) != m_eventId->end() )
        {
#ifdef __linux
            /// We receive way too many MOUSE_MOVE events on Linux
            /// HACK_FB: Skip some of them...
            if(eventId == PickerInteractor::MOUSE_MOVE)
            {
                m_skipMove++;
                if( m_skipMove % 10 )
                {
                    return;
                }
            }
#endif
            if(this->pickSomething())
            {
                ::fwDataTools::PickingInfo info;
                ::fwRenderVTK::vtk::getNearestPickedPosition(m_picker, m_adaptor->getRenderer(), info.m_worldPos);
                OSLM_TRACE("PICK" << info.m_worldPos[0] << " ," << info.m_worldPos[1] << " ," << info.m_worldPos[2] );

                info.m_modifierMask =
                    caller->GetControlKey() ? ::fwDataTools::PickingInfo::CTRL : ::fwDataTools::PickingInfo::NONE;
                info.m_modifierMask |=
                    caller->GetShiftKey() ? ::fwDataTools::PickingInfo::SHIFT : ::fwDataTools::PickingInfo::NONE;

                vtkCellPicker* picker = vtkCellPicker::SafeDownCast ( m_picker );
                if (picker)
                {
                    info.m_cellId         = picker->GetCellId();
                    info.m_closestPointId = picker->GetPointId();
                }

                const auto iter = s_vtkEventIDConversion.find(eventId);
                SLM_ASSERT("Unknown eventId", iter != s_vtkEventIDConversion.end());
                info.m_eventId = iter->second;

                info.m_keyPressed = caller->GetKeyCode();
                OSLM_TRACE("EVENT" << static_cast<int>(info.m_eventId) );
                OSLM_TRACE("KEY" << info.m_keyPressed << " - MASK " << static_cast<int>(info.m_modifierMask) );

                info.m_timestamp = ::fwCore::HiResClock::getTimeInMilliSec();

                auto sig = m_adaptor->signal<PickerInteractor::PickedSignalType>(
                    PickerInteractor::s_PICKED_SIGNAL);
                sig->asyncEmit(info);
            }
        }
    }

    void setAdaptor( PickerInteractor::sptr adaptor)
    {
        m_adaptor = adaptor;
    }

    void setPicker( vtkAbstractPropPicker* picker)
    {
        m_picker = picker;
    }

    void setEventId(PickerInteractor::SetEventIdType* eventId)
    {
        m_eventId = eventId;
    }

protected:
    PickerInteractor::SetEventIdType* m_eventId;
    PickerInteractor::sptr m_adaptor;
    vtkAbstractPropPicker* m_picker;
#ifdef __linux
    unsigned int m_skipMove = 0u;
#endif // __linux

};

//------------------------------------------------------------------------------

PickerInteractor::PickerInteractor() throw() : m_interactionCommand(nullptr)
{
    newSignal<PickedSignalType>(s_PICKED_SIGNAL);
}

//------------------------------------------------------------------------------

PickerInteractor::~PickerInteractor() throw()
{
}

//------------------------------------------------------------------------------

void PickerInteractor::doConfigure() throw(fwTools::Failed)
{
    SLM_ASSERT("Required element 'config' is missing.", m_configuration->getName() == "config");

    if (m_configuration->hasAttribute("event"))
    {
        const std::string eventTxt = m_configuration->getAttributeValue("event");

        ::boost::char_separator<char> sep(", ;");
        ::boost::tokenizer< ::boost::char_separator<char> > tok(eventTxt, sep);
        for( const auto it : tok)
        {
            const auto iter = m_eventIdConversion.find(it);
            SLM_ASSERT("Unknown eventId '"+ it+"'.", iter != m_eventIdConversion.end());
            m_eventId.insert(iter->second);
        }
    }
    else
    {
        for(auto elt : m_eventIdConversion)
        {
            m_eventId.insert(elt.second);
        }
    }
}

//------------------------------------------------------------------------------

void PickerInteractor::doStart() throw(fwTools::Failed)
{
    PickerInteractorCallback* observer = PickerInteractorCallback::New();
    observer->setAdaptor( PickerInteractor::dynamicCast(this->getSptr()) );
    observer->setPicker(this->getPicker());
    observer->setEventId(&m_eventId);

    m_interactionCommand = observer;

    vtkRenderWindowInteractor* interactor = this->getInteractor();
    const float priority                  = 0.999f;
    interactor->AddObserver(vtkCommand::LeftButtonPressEvent, m_interactionCommand, priority);
    interactor->AddObserver(vtkCommand::LeftButtonReleaseEvent, m_interactionCommand, priority);
    interactor->AddObserver(vtkCommand::MiddleButtonPressEvent, m_interactionCommand, priority);
    interactor->AddObserver(vtkCommand::MiddleButtonReleaseEvent, m_interactionCommand, priority);
    interactor->AddObserver(vtkCommand::RightButtonPressEvent, m_interactionCommand, priority);
    interactor->AddObserver(vtkCommand::RightButtonReleaseEvent, m_interactionCommand, priority);
    interactor->AddObserver(vtkCommand::MouseMoveEvent, m_interactionCommand, priority);
    interactor->AddObserver(vtkCommand::MouseWheelForwardEvent, m_interactionCommand, priority);
    interactor->AddObserver(vtkCommand::MouseWheelBackwardEvent, m_interactionCommand, priority);
    interactor->AddObserver(vtkCommand::KeyPressEvent, m_interactionCommand, priority);

}

//------------------------------------------------------------------------------

void PickerInteractor::doUpdate() throw(fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void PickerInteractor::doSwap() throw(fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void PickerInteractor::doStop() throw(fwTools::Failed)
{
    vtkRenderWindowInteractor* interactor = this->getInteractor();
    interactor->RemoveObservers(vtkCommand::LeftButtonPressEvent, m_interactionCommand);
    interactor->RemoveObservers(vtkCommand::LeftButtonReleaseEvent, m_interactionCommand);
    interactor->RemoveObservers(vtkCommand::MiddleButtonPressEvent, m_interactionCommand);
    interactor->RemoveObservers(vtkCommand::MiddleButtonReleaseEvent, m_interactionCommand);
    interactor->RemoveObservers(vtkCommand::RightButtonPressEvent, m_interactionCommand);
    interactor->RemoveObservers(vtkCommand::RightButtonReleaseEvent, m_interactionCommand);
    interactor->RemoveObservers(vtkCommand::MouseMoveEvent, m_interactionCommand);
    interactor->RemoveObservers(vtkCommand::MouseWheelForwardEvent, m_interactionCommand);
    interactor->RemoveObservers(vtkCommand::MouseWheelBackwardEvent, m_interactionCommand);
    interactor->RemoveObservers(vtkCommand::KeyPressEvent, m_interactionCommand);

    m_interactionCommand->Delete();
    m_interactionCommand = nullptr;
    this->unregisterServices();
}

//------------------------------------------------------------------------------

} //namespace visuVTKAdaptor
