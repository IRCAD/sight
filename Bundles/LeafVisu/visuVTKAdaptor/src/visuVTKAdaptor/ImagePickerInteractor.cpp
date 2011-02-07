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

#include <fwComEd/InteractionMsg.hpp>
#include <fwComEd/ImageMsg.hpp>

#include <fwData/Material.hpp>
#include <fwData/Composite.hpp>
#include <fwData/Reconstruction.hpp>

#include <fwRenderVTK/vtk/Helpers.hpp>

#include <fwServices/Factory.hpp>
#include <fwServices/ObjectServiceRegistry.hpp>
#include <fwServices/macros.hpp>


#include "visuVTKAdaptor/ImagePickerInteractor.hpp"


#define START_INTERACTION_EVENT vtkCommand::LeftButtonPressEvent
#define STOP_INTERACTION_EVENT  vtkCommand::LeftButtonReleaseEvent

REGISTER_SERVICE( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::ImagePickerInteractor, ::fwData::Image ) ;

namespace visuVTKAdaptor
{

class ImagePickerInteractorCallback : public vtkCommand
{
public:
    static ImagePickerInteractorCallback *New()
    { return new ImagePickerInteractorCallback(); }

    ImagePickerInteractorCallback() : m_priority(-1)
    {
        m_picker = NULL;
        this->PassiveObserverOn();
    }

    ~ImagePickerInteractorCallback()
    {
    }

    virtual void Execute( vtkObject *caller, unsigned long eventId, void *)
    {
        assert(m_priority>=0);
        assert(m_adaptor);
        assert(m_picker);

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

        return  m_picker->Pick( display , m_adaptor->getRenderer() );
    }


    void process(vtkRenderWindowInteractor *caller, unsigned long eventId) // from
    {
        SLM_ASSERT("bad vtk caller", caller);
        m_caller = caller;

        switch (eventId)
        {
            case vtkCommand::LeftButtonPressEvent :
                this->notifyMsg(::fwComEd::InteractionMsg::MOUSE_LEFT_DOWN);
                break;
            case vtkCommand::LeftButtonReleaseEvent :
                this->notifyMsg(::fwComEd::InteractionMsg::MOUSE_LEFT_UP);
                break;
            case vtkCommand::MiddleButtonPressEvent :
                this->notifyMsg(::fwComEd::InteractionMsg::MOUSE_MIDDLE_DOWN);
                break;
            case vtkCommand::MiddleButtonReleaseEvent :
                this->notifyMsg(::fwComEd::InteractionMsg::MOUSE_MIDDLE_UP);
                break;
            case vtkCommand::RightButtonPressEvent :
                this->notifyMsg(::fwComEd::InteractionMsg::MOUSE_RIGHT_DOWN);
                break;
            case vtkCommand::RightButtonReleaseEvent :
                this->notifyMsg(::fwComEd::InteractionMsg::MOUSE_RIGHT_UP);
                break;
            //case vtkCommand::EnterEvent :
                //this->notifyMsg(::fwComEd::InteractionMsg::);
                //break;
            //case vtkCommand::LeaveEvent :
                //this->notifyMsg(::fwComEd::InteractionMsg::);
                //break;
            //case vtkCommand::KeyPressEvent :
                //this->notifyMsg(::fwComEd::InteractionMsg::KEY_DOWN);
                //break;
            //case vtkCommand::KeyReleaseEvent :
                //this->notifyMsg(::fwComEd::InteractionMsg::KEY_UP);
                //break;
            case vtkCommand::MouseMoveEvent :
                this->notifyMsg(::fwComEd::InteractionMsg::MOUSE_MOVE);
                break;
            case vtkCommand::MouseWheelForwardEvent :
                this->notifyMsg(::fwComEd::InteractionMsg::MOUSE_WHEELFORWARD_DOWN);
                this->notifyMsg(::fwComEd::InteractionMsg::MOUSE_WHEELFORWARD_UP);
                break;
            case vtkCommand::MouseWheelBackwardEvent :
                this->notifyMsg(::fwComEd::InteractionMsg::MOUSE_WHEELBACKWARD_DOWN);
                this->notifyMsg(::fwComEd::InteractionMsg::MOUSE_WHEELBACKWARD_UP);
                break;
            default:
                OSLM_ASSERT("Unknown vtk event: " << vtkCommand::GetStringFromEventId(eventId) ,0);
        };

    }


    void notifyMsg(std::string event)
    {
        double world[3] = {-1,0,0};
        if ( this->pickSomething() )
        {
            ::fwRenderVTK::vtk::getNearestPickedPosition(m_picker, m_adaptor->getRenderer(), world);
            OSLM_TRACE("PICK" << world[0] << " ," << world[1] << " ," << world[2] );

            ::fwComEd::InteractionMsg::NewSptr msg;

            int index[3];
            m_adaptor->worldToImageSliceIndex(world, index);
            msg->setEventPoint(index[0], index[1], index[2]);
            msg->setEvent(event);

            OSLM_TRACE( "Key Ctrl : " << m_caller->GetControlKey() );
            OSLM_TRACE( "Key Shift : " << m_caller->GetShiftKey() );
            msg->setModifiersStatus( ::fwComEd::InteractionMsg::CTRL,  m_caller->GetControlKey());
            msg->setModifiersStatus( ::fwComEd::InteractionMsg::SHIFT, m_caller->GetShiftKey());

            m_adaptor->notifyEvent( msg );
        }
    }

    void setAdaptor( ImagePickerInteractor::sptr adaptor)
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
    vtkRenderWindowInteractor *m_caller;
    ImagePickerInteractor::sptr m_adaptor;
    vtkAbstractPropPicker *m_picker;
    float    m_priority;

};

//------------------------------------------------------------------------------

ImagePickerInteractor::ImagePickerInteractor() throw()
    : m_priority(0.999)
{
    //handlingEventOff();
    addNewHandledEvent( ::fwComEd::ImageMsg::BUFFER );
    addNewHandledEvent( ::fwComEd::ImageMsg::NEW_IMAGE );
    addNewHandledEvent( ::fwComEd::ImageMsg::SLICE_INDEX );
}

//------------------------------------------------------------------------------

ImagePickerInteractor::~ImagePickerInteractor() throw()
{
}

//------------------------------------------------------------------------------

void ImagePickerInteractor::configuring() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();

    assert(m_configuration->getName() == "config");
    this->setPickerId( m_configuration->getAttributeValue("picker") );
    this->setRenderId( m_configuration->getAttributeValue("renderer") );
}

//------------------------------------------------------------------------------

void ImagePickerInteractor::doStart() throw(fwTools::Failed)
{
    ImagePickerInteractorCallback *observer = ImagePickerInteractorCallback::New();
    observer->setAdaptor( ImagePickerInteractor::dynamicCast(this->getSptr()) );
    observer->setPicker(this->getPicker());
    observer->setPriority(  m_priority );

    m_interactionCommand = observer;

    vtkRenderWindowInteractor *interactor = this->getInteractor();
    interactor->AddObserver(vtkCommand::LeftButtonPressEvent    , m_interactionCommand, m_priority);
    interactor->AddObserver(vtkCommand::LeftButtonReleaseEvent  , m_interactionCommand, m_priority);
    interactor->AddObserver(vtkCommand::MiddleButtonPressEvent  , m_interactionCommand, m_priority);
    interactor->AddObserver(vtkCommand::MiddleButtonReleaseEvent, m_interactionCommand, m_priority);
    interactor->AddObserver(vtkCommand::RightButtonPressEvent   , m_interactionCommand, m_priority);
    interactor->AddObserver(vtkCommand::RightButtonReleaseEvent , m_interactionCommand, m_priority);
    //interactor->AddObserver(vtkCommand::EnterEvent              , m_interactionCommand, m_priority);
    //interactor->AddObserver(vtkCommand::LeaveEvent              , m_interactionCommand, m_priority);
    //interactor->AddObserver(vtkCommand::KeyPressEvent           , m_interactionCommand, m_priority);
    //interactor->AddObserver(vtkCommand::KeyReleaseEvent         , m_interactionCommand, m_priority);
    interactor->AddObserver(vtkCommand::MouseMoveEvent          , m_interactionCommand, m_priority);
    interactor->AddObserver(vtkCommand::MouseWheelForwardEvent  , m_interactionCommand, m_priority);
    interactor->AddObserver(vtkCommand::MouseWheelBackwardEvent , m_interactionCommand, m_priority);

    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    this->updateImageInfos(image);
}

//------------------------------------------------------------------------------

void ImagePickerInteractor::doUpdate() throw(fwTools::Failed)
{}

//-----------------------------------------------------------------------------

void ImagePickerInteractor::doUpdate( ::fwServices::ObjectMsg::csptr msg) throw(fwTools::Failed)
{
    if ( msg->hasEvent( ::fwComEd::ImageMsg::BUFFER ) || ( msg->hasEvent( ::fwComEd::ImageMsg::NEW_IMAGE )) )
    {
        ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
        this->updateImageInfos(image);
    }

    if ( msg->hasEvent( ::fwComEd::ImageMsg::SLICE_INDEX ) )
    {
        ::fwComEd::ImageMsg::dynamicConstCast(msg)->getSliceIndex( m_axialIndex, m_frontalIndex, m_sagittalIndex);
    }
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
    vtkRenderWindowInteractor *interactor = this->getInteractor();
    interactor->RemoveObservers(vtkCommand::LeftButtonPressEvent    , m_interactionCommand);
    interactor->RemoveObservers(vtkCommand::LeftButtonReleaseEvent  , m_interactionCommand);
    interactor->RemoveObservers(vtkCommand::MiddleButtonPressEvent  , m_interactionCommand);
    interactor->RemoveObservers(vtkCommand::MiddleButtonReleaseEvent, m_interactionCommand);
    interactor->RemoveObservers(vtkCommand::RightButtonPressEvent   , m_interactionCommand);
    interactor->RemoveObservers(vtkCommand::RightButtonReleaseEvent , m_interactionCommand);
    //interactor->RemoveObservers(vtkCommand::EnterEvent              , m_interactionCommand);
    //interactor->RemoveObservers(vtkCommand::LeaveEvent              , m_interactionCommand);
    //interactor->RemoveObservers(vtkCommand::KeyPressEvent           , m_interactionCommand);
    //interactor->RemoveObservers(vtkCommand::KeyReleaseEvent         , m_interactionCommand);
    interactor->RemoveObservers(vtkCommand::MouseMoveEvent          , m_interactionCommand);
    interactor->RemoveObservers(vtkCommand::MouseWheelForwardEvent  , m_interactionCommand);
    interactor->RemoveObservers(vtkCommand::MouseWheelBackwardEvent , m_interactionCommand);

    m_interactionCommand->Delete();
    m_interactionCommand = NULL;
    this->unregisterServices();
}

//------------------------------------------------------------------------------

void ImagePickerInteractor::notifyEvent(::fwComEd::InteractionMsg::sptr msg)
{
    ::fwServices::IEditionService::notify(this->getSptr(), this->getObject(), msg);
}

//------------------------------------------------------------------------------

} //namespace visuVTKAdaptor
