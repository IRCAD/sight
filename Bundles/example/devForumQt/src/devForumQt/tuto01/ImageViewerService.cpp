/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

// vtk
#include <vtkCellPicker.h>
#include <vtkCommand.h>
#include <vtkImageData.h>
#include <vtkLookupTable.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkSmartPointer.h>

// Image object
#include <fwData/Image.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

// Services tools
#include <fwServices/Base.hpp>

// vtkIO lib : intern lib used to convert image between vtk and fw4spl
#include <vtkIO/vtk.hpp>

#include "devForumQt/tuto01/ImageViewerService.hpp"

namespace devForum
{

namespace tuto01
{

//-----------------------------------------------------------------------------

// Declare a new service of type IRender for render Image
REGISTER_SERVICE( ::fwRender::IRender , ::devForum::tuto01::ImageViewerService , ::fwData::Image ) ;


//-----------------------------------------------------------------------------

ImageViewerService::ImageViewerService() throw()
    : m_render( 0 )
{}

//-----------------------------------------------------------------------------

ImageViewerService::~ImageViewerService() throw()
{}

//-----------------------------------------------------------------------------

void ImageViewerService::configuring() throw ( ::fwTools::Failed )
{
this->initialize();
}

//-----------------------------------------------------------------------------

void ImageViewerService::starting() throw ( ::fwTools::Failed )
{
    // Method of abstract class : use in our case to find wxWindow instance for this service
    // created by an IView or a IFrame service.
    // This panel is used to know where the vtk render must be created
    this->create();

    m_interactorManager = ::fwRenderVTK::IVtkRenderWindowInteractorManager::createManager();
    m_interactorManager->installInteractor( this->getContainer() );

    // Renderer
    m_render = vtkRenderer::New();
    m_interactorManager->getInteractor()->GetRenderWindow()->AddRenderer(m_render);

    // Initialize image associated to this service if it is not allocated.
    ::fwData::Image::sptr associatedImage = this->getObject< ::fwData::Image >();
    if ( associatedImage->getSize().empty() || associatedImage->getSize()[0] == 0 )
    {
        createSyntheticImage( associatedImage );
    }

    // Initialize vtk pipeline image render
    initVTKPipeline();
    alignNegatoOnImageCenter();
    (m_interactorManager->getInteractor())->Render();
}

//-----------------------------------------------------------------------------

void ImageViewerService::stopping() throw ( ::fwTools::Failed )
{
    if( m_render == 0 ) return;

    m_negatoSagittal->Delete();
    m_negatoFrontal->Delete();
    m_negatoAxial->Delete();

    assert( m_render );
    m_render->Delete();
    m_render = 0;

    m_interactorManager->uninstallInteractor();
    m_interactorManager.reset();

    this->destroy();
}

//-----------------------------------------------------------------------------

void ImageViewerService::updating() throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void ImageViewerService::updating( ::fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void ImageViewerService::initVTKPipeline()
{
    // Gets associated image
    ::fwData::Image::sptr associatedImage = this->getObject< ::fwData::Image >();

    // Converts an ::fwDada::Image to a vtk Image
    vtkSmartPointer< vtkImageData > vtk_img = vtkSmartPointer< vtkImageData >::New();
    ::vtkIO::toVTKImage( associatedImage, vtk_img );

    // Creates an image picker
    vtkCellPicker* picker = vtkCellPicker::New();
    picker->SetTolerance( 0.005 );

    // Configures Image Plane widget
    m_negatoSagittal = vtkImagePlaneWidget::New();
    m_negatoSagittal->SetInteractor(m_interactorManager->getInteractor() );
    m_negatoSagittal->SetKeyPressActivationValue('x');
    m_negatoSagittal->SetPicker(picker);
    m_negatoSagittal->GetPlaneProperty()->SetColor(1,0,0);
    m_negatoSagittal->TextureInterpolateOn();
    m_negatoSagittal->SetInput(vtk_img);
    m_negatoSagittal->SetPlaneOrientationToXAxes();
    m_negatoSagittal->DisplayTextOn();
    m_negatoSagittal->On();
    m_negatoSagittal->InteractionOn();

    m_negatoFrontal = vtkImagePlaneWidget::New();
    m_negatoFrontal->SetInteractor( m_interactorManager->getInteractor());
    m_negatoFrontal->SetKeyPressActivationValue('y');
    m_negatoFrontal->SetPicker(picker);
    m_negatoFrontal->GetPlaneProperty()->SetColor(0,1,0);
    m_negatoFrontal->TextureInterpolateOn();
    m_negatoFrontal->SetInput(vtk_img);
    m_negatoFrontal->SetPlaneOrientationToYAxes();
    m_negatoFrontal->SetLookupTable( m_negatoSagittal->GetLookupTable());
    m_negatoFrontal->DisplayTextOn();
    m_negatoFrontal->UpdatePlacement();
    m_negatoFrontal->On();

    m_negatoAxial = vtkImagePlaneWidget::New();
    m_negatoAxial->SetInteractor(m_interactorManager->getInteractor());
    m_negatoAxial->SetKeyPressActivationValue('z');
    m_negatoAxial->SetPicker(picker);
    m_negatoAxial->GetPlaneProperty()->SetColor(0,0,1);
    m_negatoAxial->TextureInterpolateOn();
    m_negatoAxial->SetInput(vtk_img);
    m_negatoAxial->SetPlaneOrientationToZAxes();
    m_negatoAxial->SetLookupTable( m_negatoSagittal->GetLookupTable());
    m_negatoAxial->DisplayTextOn();
    m_negatoAxial->On();

    // Reset camera
    m_render->ResetCamera();

    // Picker is now set, it can be deleted ( here, it is just dereference because it is used in vtk pipeline )
    picker->Delete();
}

//-----------------------------------------------------------------------------

void ImageViewerService::createSyntheticImage( ::fwData::Image::sptr _pImage )
{
    ::fwTools::Type pixelType("uint8");
    _pImage->setType( pixelType );

    _pImage->setSpacing( ::fwData::Image::SpacingType(3,1.0) );
    _pImage->setOrigin( ::fwData::Image::OriginType(3,0) );
    _pImage->setSize( ::fwData::Image::SizeType(3,100) );
    _pImage->setWindowCenter(0);
    _pImage->setWindowWidth(100);
    _pImage->allocate();

    // Build the buffer of a synthetic image grey level which represents a sphere
    unsigned char * buffer = static_cast< unsigned char* >(_pImage->getBuffer());
    for ( int z = 0; z < 100; z++ )
    {
        for ( int y = 0; y < 100; y++ )
        {
            for ( int x = 0; x < 100; x++ )
            {
                int val = sqrt ( (double)(( 50 - z ) * ( 50 - z ) + ( 50 - y ) * ( 50 - y ) + ( 50 - x ) * ( 50 - x ) ) );
                buffer[ x + y * 100 + z * 10000 ] = ( val / 5 ) * 10;
            }
        }
    }
}

//-----------------------------------------------------------------------------

void ImageViewerService::alignNegatoOnImageCenter()
{
    // Get associated image of this service
    ::fwData::Image::sptr associatedImage = this->getObject< ::fwData::Image >();

    // Compute image center
    unsigned int axialIndex = associatedImage->getSize()[2]/2;
    unsigned int frontalIndex = associatedImage->getSize()[1]/2;
    unsigned int sagittalIndex = associatedImage->getSize()[0]/2;

    // Modify negato to align slice on the center
    m_negatoAxial->SetSliceIndex( axialIndex );
    m_negatoFrontal->SetSliceIndex( frontalIndex );
    m_negatoSagittal->SetSliceIndex( sagittalIndex );
}

//-----------------------------------------------------------------------------

} // end of namespace tuto01

} // end of namespace devForum
