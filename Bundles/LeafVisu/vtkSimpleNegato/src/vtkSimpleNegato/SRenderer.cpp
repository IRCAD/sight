/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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

//-----------------------------------------------------------------------------

SRenderer::SRenderer() throw()
    : m_render( 0 ), m_bPipelineIsInit(false)
{
    SLM_TRACE_FUNC();
    newSlot(s_REFRESH_SLOT, &SRenderer::refresh, this);
}

//-----------------------------------------------------------------------------

SRenderer::~SRenderer() throw()
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void SRenderer::configuring() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->initialize();
}

//-----------------------------------------------------------------------------

void SRenderer::starting() throw(fwTools::Failed)
{
    this->create();

    m_interactorManager = ::fwRenderVTK::IVtkRenderWindowInteractorManager::createManager();
    m_interactorManager->installInteractor( this->getContainer() );

    m_bPipelineIsInit = false;

    // Renderer
    m_render = vtkRenderer::New();
    m_interactorManager->getInteractor()->GetRenderWindow()->AddRenderer(m_render);

}

//-----------------------------------------------------------------------------

void SRenderer::stopping() throw(fwTools::Failed)
{
    if( m_render == 0 )
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
    m_render = 0;

    m_interactorManager->uninstallInteractor();
    m_interactorManager.reset();

//    this->getContainer()->clean();
    this->destroy();
}

//-----------------------------------------------------------------------------

void SRenderer::updating() throw(fwTools::Failed)
{

//    m_interactorManager->getInteractor()->Render();
    refresh();
}

//-----------------------------------------------------------------------------

void SRenderer::refresh()
{
    ::fwData::Image::sptr img = this->getObject< ::fwData::Image >();
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
    vtkSmartPointer< vtkImageData > vtk_img = vtkSmartPointer< vtkImageData >::New();
    ::fwVtkIO::toVTKImage( this->getObject< ::fwData::Image >(), vtk_img);

    m_outline = vtkOutlineFilter::New();
    m_outline->SetInputData(vtk_img);

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
    m_negatoSagittal->GetPlaneProperty()->SetColor(1,0,0);
    m_negatoSagittal->TextureInterpolateOn();
    m_negatoSagittal->SetInputData(vtk_img);
    m_negatoSagittal->SetPlaneOrientationToXAxes();
    m_negatoSagittal->DisplayTextOn();
    m_negatoSagittal->On();
    m_negatoSagittal->InteractionOn();

    m_negatoFrontal = vtkImagePlaneWidget::New();
    m_negatoFrontal->SetInteractor( m_interactorManager->getInteractor() );
    m_negatoFrontal->SetKeyPressActivationValue('y');
    m_negatoFrontal->SetPicker(picker);
    m_negatoFrontal->GetPlaneProperty()->SetColor(0,1,0);
    m_negatoFrontal->TextureInterpolateOn();
    m_negatoFrontal->SetInputData(vtk_img);
    m_negatoFrontal->SetPlaneOrientationToYAxes();
    m_negatoFrontal->SetLookupTable( m_negatoSagittal->GetLookupTable());
    m_negatoFrontal->DisplayTextOn();
    m_negatoFrontal->UpdatePlacement();
    m_negatoFrontal->On();

    m_negatoAxial = vtkImagePlaneWidget::New();
    m_negatoAxial->SetInteractor( m_interactorManager->getInteractor() );
    m_negatoAxial->SetKeyPressActivationValue('z');
    m_negatoAxial->SetPicker(picker);
    m_negatoAxial->GetPlaneProperty()->SetColor(0,0,1);
    m_negatoAxial->TextureInterpolateOn();
    m_negatoAxial->SetInputData(vtk_img);
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
    assert(this->getObject< ::fwData::Image >());
    vtkSmartPointer< vtkImageData > vtk_img = vtkSmartPointer< vtkImageData >::New();
    ::fwVtkIO::toVTKImage( this->getObject< ::fwData::Image >(), vtk_img);

    m_outline->SetInputData(vtk_img);
    m_negatoSagittal->SetInputData(vtk_img);
    m_negatoFrontal->SetInputData(vtk_img);
    m_negatoAxial->SetInputData(vtk_img);
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsType SRenderer::getObjSrvConnections() const
{
    KeyConnectionsType connections;
    connections.push_back( std::make_pair( ::fwData::Image::s_MODIFIED_SIG, s_REFRESH_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::Image::s_BUFFER_MODIFIED_SIG, s_REFRESH_SLOT ) );

    return connections;
}

//-----------------------------------------------------------------------------

}
