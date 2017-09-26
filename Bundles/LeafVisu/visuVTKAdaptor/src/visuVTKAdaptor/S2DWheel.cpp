/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKAdaptor/S2DWheel.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hxx>

#include <fwServices/macros.hpp>

#include <fwVtkIO/helper/vtkLambdaCommand.hpp>

#include <vtkRenderer.h>
#include <vtkRenderWindow.h>

fwServicesRegisterMacro( ::fwRenderVTK::IAdaptor, ::visuVTKAdaptor::S2DWheel);

namespace visuVTKAdaptor
{

static const ::fwCom::Signals::SignalKeyType s_WHEEL_UPDATED_SIG = "wheelUpdated";

static const ::fwCom::Slots::SlotKeyType s_UPDATE_VISIBILITY_SLOT = "updateVisibility";

//------------------------------------------------------------------------------

S2DWheel::S2DWheel() noexcept :
    m_wheelWidget(vtkSmartPointer<fwVtkWheelWidget>::New()),
    m_visible(true)
{
    m_wheelUpdatedSignal = newSignal<WheelUpdatedSignalType>(s_WHEEL_UPDATED_SIG);

    newSlot(s_UPDATE_VISIBILITY_SLOT, &S2DWheel::updateVisibility, this);
}

//------------------------------------------------------------------------------

S2DWheel::~S2DWheel() noexcept
{
}

//------------------------------------------------------------------------------

void S2DWheel::configuring()
{
    this->configureParams();

    const ConfigType& config = this->getConfigTree();

    m_visible = config.get<bool>("config.<xmlattr>.visible", true);
}

//------------------------------------------------------------------------------

void S2DWheel::starting()
{
    this->initialize();

    fwVtkWheelRepresentation* wheelRepresentation = fwVtkWheelRepresentation::New();

    wheelRepresentation->SetVisibility(m_visible);
    m_wheelWidget->SetInteractor(this->getRenderer()->GetRenderWindow()->GetInteractor());
    m_wheelWidget->SetPriority(1.f); // Set a priority higher than that of the ::visuVTKAdaptor::SProbeCursor
    m_wheelWidget->SetRepresentation(wheelRepresentation);
    m_wheelWidget->On();
    m_wheelWidget->SetWheelUpdateCallback([this](double cx, double cy, double o)
        {
            OSLM_DEBUG("Center : " << "(" << cx << ", " << cy << "), Orientation : " << o);
            this->m_wheelUpdatedSignal->asyncEmit(cx, cy, o);
        });

    m_resizeCallback = vtkSmartPointer< ::fwVtkIO::helper::vtkLambdaCommand >::New();
    m_resizeCallback->SetCallback(
        [this](vtkObject*, long unsigned int, void* )
        {
            const int viewportWidth = this->getRenderer()->GetRenderWindow()->GetSize()[0];
            const int viewportHeight = this->getRenderer()->GetRenderWindow()->GetSize()[1];

            if(viewportHeight != 0 && viewportWidth != 0)
            {
                this->m_wheelWidget->GetRepresentation()->UpdateRepresentation();
            }

            this->requestRender();
        });
    this->getRenderer()->GetRenderWindow()->AddObserver(vtkCommand::ModifiedEvent, m_resizeCallback);

    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------

void S2DWheel::stopping()
{
    m_wheelWidget->Off();

    this->getRenderer()->GetRenderWindow()->RemoveObserver(m_resizeCallback);

    this->removeAllPropFromRenderer();

    this->requestRender();
}

//------------------------------------------------------------------------------

void S2DWheel::updating()
{
}

//------------------------------------------------------------------------------

void S2DWheel::updateVisibility(bool _isVisible)
{
    m_visible = _isVisible;
    this->m_wheelWidget->GetRepresentation()->SetVisibility(static_cast<int>(_isVisible));

    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------

} // namespace visuVTKAdaptor
