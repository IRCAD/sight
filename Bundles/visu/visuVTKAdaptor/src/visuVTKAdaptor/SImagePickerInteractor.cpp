/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "visuVTKAdaptor/SImagePickerInteractor.hpp"

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

#include <boost/tokenizer.hpp>

#include <vtkAbstractPropPicker.h>
#include <vtkActor.h>
#include <vtkCommand.h>
#include <vtkCubeSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindowInteractor.h>

#define START_INTERACTION_EVENT vtkCommand::LeftButtonPressEvent
#define STOP_INTERACTION_EVENT  vtkCommand::LeftButtonReleaseEvent

fwServicesRegisterMacro(::fwRenderVTK::IAdaptor, ::visuVTKAdaptor::SImagePickerInteractor);

namespace visuVTKAdaptor
{

static const ::fwServices::IService::KeyType s_IMAGE_INOUT = "image";

static const ::fwCom::Slots::SlotKeyType s_UPDATE_SLICE_INDEX_SLOT = "updateSliceIndex";

//------------------------------------------------------------------------------

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
        if( m_eventId->find( static_cast< SImagePickerInteractor::EventID>(eventId) ) != m_eventId->end() )
        {

            SLM_ASSERT("bad vtk caller", caller);

            if ( this->pickSomething() )
            {
                double world[3] = {-1, 0, 0};
                ::fwRenderVTK::vtk::getNearestPickedPosition(m_picker, m_adaptor->getRenderer(), world);
                OSLM_TRACE("PICK" << world[0] << " ," << world[1] << " ," << world[2] );

                ::fwDataTools::PickingInfo info;

                m_adaptor->m_helper.worldToImageSliceIndex(world, info.m_worldPos);

                const auto iter = SPickerInteractor::s_vtkEventIDConversion.find(eventId);
                SLM_ASSERT("Unknown eventId", iter != SPickerInteractor::s_vtkEventIDConversion.end());
                info.m_eventId = iter->second;

                info.m_keyPressed = caller->GetKeyCode();

                info.m_modifierMask =
                    caller->GetControlKey() ? ::fwDataTools::PickingInfo::CTRL : ::fwDataTools::PickingInfo::NONE;
                info.m_modifierMask |=
                    caller->GetShiftKey() ? ::fwDataTools::PickingInfo::SHIFT : ::fwDataTools::PickingInfo::NONE;

                auto sig = m_adaptor->signal<SPickerInteractor::PickedSignalType>(
                    SPickerInteractor::s_PICKED_SIGNAL);
                sig->asyncEmit(info);
            }
        }

    }

    //------------------------------------------------------------------------------

    void setAdaptor( SImagePickerInteractor::sptr adaptor)
    {
        m_adaptor = adaptor;
    }

    //------------------------------------------------------------------------------

    void setPicker( vtkAbstractPropPicker* picker)
    {
        m_picker = picker;
    }

    //------------------------------------------------------------------------------

    void setEventId(SImagePickerInteractor::SetEventIdType* eventId)
    {
        m_eventId = eventId;
    }

protected:
    SImagePickerInteractor::sptr m_adaptor;
    vtkAbstractPropPicker* m_picker;
    SImagePickerInteractor::SetEventIdType* m_eventId;
    SImagePickerInteractor::PickedSignalType::sptr m_sigPickedCell; ///< signal to emit
#ifdef __linux
    unsigned int m_skipMove = 0u;
#endif // __linux
};

//------------------------------------------------------------------------------

SImagePickerInteractor::SImagePickerInteractor() noexcept
{
    newSlot(s_UPDATE_SLICE_INDEX_SLOT, &SImagePickerInteractor::updateSliceIndex, this);
}

//------------------------------------------------------------------------------

SImagePickerInteractor::~SImagePickerInteractor() noexcept
{
}

//------------------------------------------------------------------------------

void SImagePickerInteractor::configuring()
{
    this->SPickerInteractor::configuring();
}

//------------------------------------------------------------------------------

void SImagePickerInteractor::starting()
{
    this->initialize();

    ImagePickerInteractorCallback* observer = ImagePickerInteractorCallback::New();
    observer->setAdaptor( SImagePickerInteractor::dynamicCast(this->getSptr()) );
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

    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("Missing image", image);

    m_helper.updateImageInfos(image);
}

//------------------------------------------------------------------------------

void SImagePickerInteractor::updating()
{
    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("Missing image", image);

    m_helper.updateImageInfos(image);
}

//-----------------------------------------------------------------------------

void SImagePickerInteractor::updateSliceIndex(int axial, int frontal, int sagittal)
{
    const int indexes[] = {sagittal, frontal, axial};
    m_helper.setSliceIndex(indexes);
}

//------------------------------------------------------------------------------

void SImagePickerInteractor::stopping()
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
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SImagePickerInteractor::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push( s_IMAGE_INOUT, ::fwData::Image::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push( s_IMAGE_INOUT, ::fwData::Image::s_SLICE_INDEX_MODIFIED_SIG, s_UPDATE_SLICE_INDEX_SLOT);
    connections.push( s_IMAGE_INOUT, ::fwData::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

} //namespace visuVTKAdaptor
