/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKAdaptor/S2DWheel.hpp"

#include "fwVtkIO/helper/vtkLambdaCommand.hpp"

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

//    int viewportWidth  = this->getRenderer()->GetRenderWindow()->GetSize()[0];
//    int viewportHeight = this->getRenderer()->GetRenderWindow()->GetSize()[1];

#if 0
    vtkSmartPointer< ::fwVtkIO::helper::vtkLambdaCommand > progressCallback;
    progressCallback = vtkSmartPointer< ::fwVtkIO::helper::vtkLambdaCommand >::New();
    progressCallback->SetCallback(
        [this](vtkObject* caller, long unsigned int, void* )
        {
            m_wheelActor->SetDisplayPosition(0, 0);
            // m_wheelActor->SetWidth(0, 0);
            // m_wheelActor->SetPosition(42, 42);
            // m_wheelActor->SetPosition2(128, 128);
            int viewportWidth = this->getRenderer()->GetRenderWindow()->GetSize()[0];
            int viewportHeight = this->getRenderer()->GetRenderWindow()->GetSize()[1];

            // std::cout << "SIZE UPDATE: " << viewportWidth << " " << viewportHeight << std::endl;
            // std::cout << m_wheelActor->GetWidth() << " " << m_wheelActor->GetHeight() << std::endl;
            this->updating();
        }
        );
    // this->getRenderer()->GetRenderWindow()->AddObserver(vtkCommand::ModifiedEvent, progressCallback);

    vtkSmartPointer< ::fwVtkIO::helper::vtkLambdaCommand > progressCallback2;
    progressCallback2 = vtkSmartPointer< ::fwVtkIO::helper::vtkLambdaCommand >::New();
    progressCallback2->SetCallback(
        [this](vtkObject* caller, long unsigned int, void* )
        {
            int* clickPos = this->getRenderer()->GetRenderWindow()->GetInteractor()->GetEventPosition();

            std::cout << "PICKED " << std::endl;
            vtkSmartPointer<vtkPropPicker>  picker = vtkSmartPointer<vtkPropPicker>::New();
            picker->Pick(clickPos[0], clickPos[1], 0, this->getRenderer());

            double* pos = picker->GetPickPosition();
            std::cout << "Pick position (world coordinates) is: "
                      << pos[0] << " " << pos[1]
                      << " " << pos[2] << std::endl;

            std::cout << "Picked actor: " << picker->GetActor() << std::endl;
            vtkActor2D* pickedActor = picker->GetActor2D();
            if(pickedActor == m_wheelActor.Get())
            {
                std::cout << "Wheel Picked EVENTS" << std::endl;
            }
        }
        );
    std::cout << m_wheelActor->HasObserver(vtkCommand::MouseMoveEvent) << std::endl;
    // this->getRenderer()->GetRenderWindow()->GetInteractor()->AddObserver(vtkCommand::LeftButtonPressEvent,
    // progressCallback2);
    // this->getRenderer()->GetRenderWindow()->GetInteractor()->AddObserver(vtkCommand::MouseMoveEvent,
    // progressCallback2);
    std::cout << m_wheelActor->HasObserver(vtkCommand::MouseMoveEvent) << std::endl;
#endif

    vtkSmartPointer<fwVtkWheelRepresentation> wheelRepresentation =
        vtkSmartPointer<fwVtkWheelRepresentation>::New();

    m_wheelWidget->SetInteractor(this->getRenderer()->GetRenderWindow()->GetInteractor());
    m_wheelWidget->SetRepresentation(wheelRepresentation);
    m_wheelWidget->On();

    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------

void S2DWheel::stopping()
{
    m_wheelWidget->Off();

    this->removeAllPropFromRenderer();

    this->requestRender();
}

//------------------------------------------------------------------------------

void S2DWheel::updating()
{
//    int viewportWidth  = this->getRenderer()->GetRenderWindow()->GetSize()[0];
//    int viewportHeight = this->getRenderer()->GetRenderWindow()->GetSize()[1];

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
