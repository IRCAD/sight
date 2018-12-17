/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "vtkSimpleNegato/SRenderer.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/Image.hpp>

#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>

#include <fwServices/macros.hpp>

#include <fwVtkIO/vtk.hpp>

#include <vtkCellPicker.h>
#include <vtkCommand.h>
#include <vtkImageData.h>
#include <vtkLookupTable.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkSmartPointer.h>

//-----------------------------------------------------------------------------

fwServicesRegisterMacro( ::fwRender::IRender, ::vtkSimpleNegato::SRenderer, ::fwData::Image);

//-----------------------------------------------------------------------------

namespace vtkSimpleNegato
{

static const ::fwCom::Slots::SlotKeyType s_REFRESH_SLOT = "refresh";

static const std::string s_IMAGE_KEY = "image";

//-----------------------------------------------------------------------------

SRenderer::SRenderer() noexcept :
    m_render( nullptr ),
    m_bPipelineIsInit(false)
{
    SLM_TRACE_FUNC();
    newSlot(s_REFRESH_SLOT, &SRenderer::refresh, this);
}

//-----------------------------------------------------------------------------

SRenderer::~SRenderer() noexcept
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void SRenderer::configuring()
{
    SLM_TRACE_FUNC();
    this->initialize();
}

//-----------------------------------------------------------------------------

void SRenderer::starting()
{
    this->create();

    m_interactorManager = ::fwRenderVTK::IVtkRenderWindowInteractorManager::createManager();
    m_interactorManager->installInteractor( this->getContainer() );

    m_bPipelineIsInit = false;

    // Renderer
    m_render = vtkRenderer::New();
    m_interactorManager->getInteractor()->GetRenderWindow()->AddRenderer(m_render);

    this->refresh();
}

//-----------------------------------------------------------------------------

void SRenderer::stopping()
{
    if( m_render == nullptr )
    {
        return;
    }

    if(m_bPipelineIsInit)
    {
        m_negatoSagittal->Delete();
        m_negatoFrontal->Delete();
        m_negatoAxial->Delete();
        m_outline->Delete();
    }

    SLM_ASSERT("m_render not instanced", m_render);
    m_render->Delete();
    m_render = nullptr;

    m_interactorManager->uninstallInteractor();
    m_interactorManager.reset();

    this->destroy();
}

//-----------------------------------------------------------------------------

void SRenderer::updating()
{
    this->refresh();
}

//-----------------------------------------------------------------------------

void SRenderer::refresh()
{
    auto img = this->getInput< ::fwData::Image >(s_IMAGE_KEY);
    SLM_ASSERT("'" + s_IMAGE_KEY + "' key not found", img);
    bool imageIsValid = ::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity( img );
    if(imageIsValid )
    {
        if(!m_bPipelineIsInit)
        {
            initVTKPipeline();
            m_bPipelineIsInit = true;
        }
        else
        {
            updateVTKPipeline();
        }

        //
        int axialIndex    = static_cast<int>(img->getSize()[2]/2);
        int frontalIndex  = static_cast<int>(img->getSize()[1]/2);
        int sagittalIndex = static_cast<int>(img->getSize()[0]/2);

        m_negatoAxial->SetSliceIndex( axialIndex );
        m_negatoFrontal->SetSliceIndex( frontalIndex );
        m_negatoSagittal->SetSliceIndex( sagittalIndex );
        m_interactorManager->getInteractor()->Render();
    }
}

//-----------------------------------------------------------------------------

void SRenderer::initVTKPipeline()
{
    vtkSmartPointer< vtkImageData > vtkImg = vtkSmartPointer< vtkImageData >::New();

    auto image = this->getInput< ::fwData::Image >(s_IMAGE_KEY);
    SLM_ASSERT("'" + s_IMAGE_KEY + "' key not found", image);
    ::fwVtkIO::toVTKImage( image, vtkImg);

    m_outline = vtkOutlineFilter::New();
    m_outline->SetInputData(vtkImg);

    vtkPolyDataMapper* outlineMapper = vtkPolyDataMapper::New();
    outlineMapper->SetInputConnection(m_outline->GetOutputPort());

    vtkActor* outlineActor = vtkActor::New();
    outlineActor->SetMapper( outlineMapper);

    vtkCellPicker* picker = vtkCellPicker::New();
    picker->SetTolerance(0.005);

    //assign default props to the ipw's texture plane actor

    m_negatoSagittal = vtkImagePlaneWidget::New();
    m_negatoSagittal->SetInteractor( m_interactorManager->getInteractor() );
    m_negatoSagittal->SetKeyPressActivationValue('x');
    m_negatoSagittal->SetPicker(picker);
    m_negatoSagittal->GetPlaneProperty()->SetColor(1, 0, 0);
    m_negatoSagittal->TextureInterpolateOn();
    m_negatoSagittal->SetInputData(vtkImg);
    m_negatoSagittal->SetPlaneOrientationToXAxes();
    m_negatoSagittal->DisplayTextOn();
    m_negatoSagittal->On();
    m_negatoSagittal->InteractionOn();

    m_negatoFrontal = vtkImagePlaneWidget::New();
    m_negatoFrontal->SetInteractor( m_interactorManager->getInteractor() );
    m_negatoFrontal->SetKeyPressActivationValue('y');
    m_negatoFrontal->SetPicker(picker);
    m_negatoFrontal->GetPlaneProperty()->SetColor(0, 1, 0);
    m_negatoFrontal->TextureInterpolateOn();
    m_negatoFrontal->SetInputData(vtkImg);
    m_negatoFrontal->SetPlaneOrientationToYAxes();
    m_negatoFrontal->SetLookupTable( m_negatoSagittal->GetLookupTable());
    m_negatoFrontal->DisplayTextOn();
    m_negatoFrontal->UpdatePlacement();
    m_negatoFrontal->On();

    m_negatoAxial = vtkImagePlaneWidget::New();
    m_negatoAxial->SetInteractor( m_interactorManager->getInteractor() );
    m_negatoAxial->SetKeyPressActivationValue('z');
    m_negatoAxial->SetPicker(picker);
    m_negatoAxial->GetPlaneProperty()->SetColor(0, 0, 1);
    m_negatoAxial->TextureInterpolateOn();
    m_negatoAxial->SetInputData(vtkImg);
    m_negatoAxial->SetPlaneOrientationToZAxes();
    m_negatoAxial->SetLookupTable( m_negatoSagittal->GetLookupTable());
    m_negatoAxial->DisplayTextOn();
    m_negatoAxial->On();

    // Add the actors
    m_render->AddActor( outlineActor);

    // Repaint and resize window
    m_render->ResetCamera();

    picker->Delete();
    outlineActor->Delete();
    outlineMapper->Delete();
}

//-----------------------------------------------------------------------------

void SRenderer::updateVTKPipeline()
{
    auto image = this->getInput< ::fwData::Image >(s_IMAGE_KEY);
    SLM_ASSERT("'" + s_IMAGE_KEY + "' key not found", image);

    vtkSmartPointer< vtkImageData > vtkImg = vtkSmartPointer< vtkImageData >::New();
    ::fwVtkIO::toVTKImage( image, vtkImg);

    m_outline->SetInputData(vtkImg);
    m_negatoSagittal->SetInputData(vtkImg);
    m_negatoFrontal->SetInputData(vtkImg);
    m_negatoAxial->SetInputData(vtkImg);
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SRenderer::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push( s_IMAGE_KEY, ::fwData::Image::s_MODIFIED_SIG, s_REFRESH_SLOT );
    connections.push( s_IMAGE_KEY, ::fwData::Image::s_BUFFER_MODIFIED_SIG, s_REFRESH_SLOT );

    return connections;
}

//-----------------------------------------------------------------------------

}
