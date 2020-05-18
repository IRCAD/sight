/************************************************************************
 *
 * Copyright (C) 2017-2019 IRCAD France
 * Copyright (C) 2017-2019 IHU Strasbourg
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
    m_wheelWidget(vtkSmartPointer< ::fwRenderVTK::vtk::fwVtkWheelWidget >::New()),
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

    ::fwRenderVTK::vtk::fwVtkWheelRepresentation* wheelRepresentation =
        ::fwRenderVTK::vtk::fwVtkWheelRepresentation::New();

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
            const int viewportWidth  = this->getRenderer()->GetRenderWindow()->GetSize()[0];
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
