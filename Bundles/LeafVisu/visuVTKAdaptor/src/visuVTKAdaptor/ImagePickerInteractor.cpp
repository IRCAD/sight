/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKAdaptor/ImagePickerInteractor.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/Composite.hpp>
#include <fwData/Material.hpp>
#include <fwData/Reconstruction.hpp>

#include <fwRenderVTK/vtk/Helpers.hpp>

#include <fwServices/macros.hpp>

#include <vtkAbstractPropPicker.h>
#include <vtkActor.h>
#include <vtkCommand.h>
#include <vtkCubeSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindowInteractor.h>

#include <boost/tokenizer.hpp>

#define START_INTERACTION_EVENT vtkCommand::LeftButtonPressEvent
#define STOP_INTERACTION_EVENT  vtkCommand::LeftButtonReleaseEvent

fwServicesRegisterMacro( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::ImagePickerInteractor, ::fwData::Image );

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

ImagePickerInteractor::MapEventIdType ImagePickerInteractor::m_eventIdConversion
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

class ImagePickerInteractorCallback : public vtkCommand
{
public:
    //------------------------------------------------------------------------------

    static ImagePickerInteractorCallback* New()
    {
        return new ImagePickerInteractorCallback();
    }

    //------------------------------------------------------------------------------

    ImagePickerInteractorCallback() :
        m_picker(nullptr),
        m_eventId(nullptr)
    {
        m_picker = NULL;
        this->PassiveObserverOn();
    }

    //------------------------------------------------------------------------------

    virtual ~ImagePickerInteractorCallback()
    {
    }

    //------------------------------------------------------------------------------

    virtual void Execute( vtkObject* caller, unsigned long eventId, void*)
    {
        SLM_ASSERT("m_adaptor not instanced", m_adaptor);
        SLM_ASSERT("m_picker not instanced", m_picker);

        this->process(vtkRenderWindowInteractor::SafeDownCast(caller), eventId);
    }

    //------------------------------------------------------------------------------

    bool pickSomething()
    {
        int x, y;
        double display[3];

        m_adaptor->getInteractor()->GetEventPosition(x, y);
        display[0] = x;
        display[1] = y;
        display[2] = 0;

        return (m_picker->Pick( display, m_adaptor->getRenderer() ) != 0);
    }

    //------------------------------------------------------------------------------

    void process(vtkRenderWindowInteractor* caller, unsigned long eventId) // from
    {
        if( m_eventId->find( static_cast< ImagePickerInteractor::EventID>(eventId) ) != m_eventId->end() )
        {
#ifdef __linux
            /// We receive way too many MOUSE_MOVE events on Linux
            /// HACK_FB: Skip some of them...
            if(eventId == ImagePickerInteractor::MOUSE_MOVE)
            {
                m_skipMove++;
                if( m_skipMove % 10 )
                {
                    return;
                }
            }
#endif
            SLM_ASSERT("bad vtk caller", caller);

            if ( this->pickSomething() )
            {
                double world[3] = {-1, 0, 0};
                ::fwRenderVTK::vtk::getNearestPickedPosition(m_picker, m_adaptor->getRenderer(), world);
                OSLM_TRACE("PICK" << world[0] << " ," << world[1] << " ," << world[2] );

                ::fwDataTools::PickingInfo info;

                m_adaptor->worldToImageSliceIndex(world, info.m_worldPos);

                const auto iter = s_vtkEventIDConversion.find(eventId);
                SLM_ASSERT("Unknown eventId", iter != s_vtkEventIDConversion.end());
                info.m_eventId = iter->second;

                info.m_keyPressed = caller->GetKeyCode();

                info.m_modifierMask =
                    caller->GetControlKey() ? ::fwDataTools::PickingInfo::CTRL : ::fwDataTools::PickingInfo::NONE;
                info.m_modifierMask |=
                    caller->GetShiftKey() ? ::fwDataTools::PickingInfo::SHIFT : ::fwDataTools::PickingInfo::NONE;

                auto sig = m_adaptor->signal<ImagePickerInteractor::PickedSignalType>(
                    ImagePickerInteractor::s_PICKED_SIGNAL);
                sig->asyncEmit(info);
            }
        }

    }

    //------------------------------------------------------------------------------

    void setAdaptor( ImagePickerInteractor::sptr adaptor)
    {
        m_adaptor = adaptor;
    }

    //------------------------------------------------------------------------------

    void setPicker( vtkAbstractPropPicker* picker)
    {
        m_picker = picker;
    }

    //------------------------------------------------------------------------------

    void setEventId(ImagePickerInteractor::SetEventIdType* eventId)
    {
        m_eventId = eventId;
    }

protected:
    ImagePickerInteractor::sptr m_adaptor;
    vtkAbstractPropPicker* m_picker;
    ImagePickerInteractor::SetEventIdType* m_eventId;
    ImagePickerInteractor::PickedSignalType::sptr m_sigPickedCell; ///< signal to emit
#ifdef __linux
    unsigned int m_skipMove = 0u;
#endif // __linux
};

static const ::fwCom::Slots::SlotKeyType s_UPDATE_SLICE_INDEX_SLOT = "updateSliceIndex";

const ::fwCom::Signals::SignalKeyType ImagePickerInteractor::s_PICKED_SIGNAL = "picked";

//------------------------------------------------------------------------------

ImagePickerInteractor::ImagePickerInteractor() throw() :
    m_interactionCommand(nullptr)
{
    newSignal<PickedSignalType>(s_PICKED_SIGNAL);
    newSlot(s_UPDATE_SLICE_INDEX_SLOT, &ImagePickerInteractor::updateSliceIndex, this);
}

//------------------------------------------------------------------------------

ImagePickerInteractor::~ImagePickerInteractor() throw()
{
}

//------------------------------------------------------------------------------

void ImagePickerInteractor::doConfigure() throw(fwTools::Failed)
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

void ImagePickerInteractor::doStart() throw(fwTools::Failed)
{
    ImagePickerInteractorCallback* observer = ImagePickerInteractorCallback::New();
    observer->setAdaptor( ImagePickerInteractor::dynamicCast(this->getSptr()) );
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

    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    this->updateImageInfos(image);
}

//------------------------------------------------------------------------------

void ImagePickerInteractor::doUpdate() throw(fwTools::Failed)
{
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    this->updateImageInfos(image);
}

//-----------------------------------------------------------------------------

void ImagePickerInteractor::updateSliceIndex(int axial, int frontal, int sagittal)
{
    m_axialIndex->value()    = axial;
    m_frontalIndex->value()  = frontal;
    m_sagittalIndex->value() = sagittal;
}

//------------------------------------------------------------------------------

void ImagePickerInteractor::doSwap() throw(fwTools::Failed)
{
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    this->updateImageInfos(image);
}

//------------------------------------------------------------------------------

void ImagePickerInteractor::doStop() throw(fwTools::Failed)
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
    m_interactionCommand = NULL;
    this->unregisterServices();
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsType ImagePickerInteractor::getObjSrvConnections() const
{
    KeyConnectionsType connections;
    connections.push_back( std::make_pair( ::fwData::Image::s_MODIFIED_SIG, s_UPDATE_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::Image::s_SLICE_INDEX_MODIFIED_SIG, s_UPDATE_SLICE_INDEX_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT ) );

    return connections;
}

//------------------------------------------------------------------------------

} //namespace visuVTKAdaptor
