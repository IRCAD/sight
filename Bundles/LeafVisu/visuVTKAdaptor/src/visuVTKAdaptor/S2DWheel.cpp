/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKAdaptor/S2DWheel.hpp"

#include <fwCom/Slots.hxx>

#include <fwServices/macros.hpp>

#include <vtkRenderer.h>
#include <vtkRenderWindow.h>

fwServicesRegisterMacro( ::fwRenderVTK::IAdaptor, ::visuVTKAdaptor::S2DWheel);

namespace visuVTKAdaptor
{

//------------------------------------------------------------------------------

S2DWheel::S2DWheel() noexcept :
    m_wheelWidget(vtkSmartPointer<fwVtkWheelWidget>::New())
{
}

//------------------------------------------------------------------------------

S2DWheel::~S2DWheel() noexcept
{
}

//------------------------------------------------------------------------------

void S2DWheel::configuring()
{
    this->configureParams();
}

//------------------------------------------------------------------------------

void S2DWheel::starting()
{
    this->initialize();

    fwVtkWheelRepresentation* wheelRepresentation = fwVtkWheelRepresentation::New();

    m_wheelWidget->SetInteractor(this->getRenderer()->GetRenderWindow()->GetInteractor());
    m_wheelWidget->SetRepresentation(wheelRepresentation);
    m_wheelWidget->On();
    m_wheelWidget->SetWheelUpdateCallback([](double cx, double cy, double o)
        {
            OSLM_DEBUG("Center : " << "(" << cx << ", " << cy << "), Orientation : " << o);
        });

    m_resizeCallback = vtkSmartPointer< ::fwVtkIO::helper::vtkLambdaCommand >::New();
    m_resizeCallback->SetCallback(
        [this](vtkObject*, long unsigned int, void* )
        {
            int viewportWidth = this->getRenderer()->GetRenderWindow()->GetSize()[0];
            int viewportHeight = this->getRenderer()->GetRenderWindow()->GetSize()[1];

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
    this->requestRender();
}

//------------------------------------------------------------------------------

// void S2DWheel::updateVisibility(bool _isVisible)
// {
//     m_axisActor->SetVisibility(_isVisible);
//     if(m_sphereOn)
//     {
//         m_sphereActor->SetVisibility(_isVisible);
//     }

//     this->setVtkPipelineModified();
//     this->requestRender();
// }

//------------------------------------------------------------------------------

} // namespace visuVTKAdaptor
