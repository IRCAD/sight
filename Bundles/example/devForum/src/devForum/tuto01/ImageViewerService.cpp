/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

// wxWidget
#include <wx/panel.h>
#include <wx/window.h>

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

#include <fwGuiWx/container/WxContainer.hpp>

// Services tools
#include <fwServices/Base.hpp>

// vtkIO lib : intern lib used to convert image between vtk and fw4spl
#include <vtkIO/vtk.hpp>

#include "devForum/tuto01/ImageViewerService.hpp"

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

//    this->initRender();

    ::fwGuiWx::container::WxContainer::sptr wxContainer =  ::fwGuiWx::container::WxContainer::dynamicCast( this->getContainer() );
    wxWindow* const container = wxContainer->getWxContainer();
    assert( container ) ;

    // Create a aui manager to manage layout of this container
    m_wxmanager = new wxAuiManager( container );

    // Create a VTK-compliant window and insert it in m_container
    m_interactor = new ::wxVTKRenderWindowInteractor( container, -1 );
    m_wxmanager->AddPane( m_interactor, wxAuiPaneInfo().CentrePane() );

    // Renderer
    m_render = vtkRenderer::New();
    m_interactor->GetRenderWindow()->AddRenderer( m_render );

    // Repaint and resize window
    m_wxmanager->Update();

    // Initialize image associated to this service if it is not allocated.
    ::fwData::Image::sptr associatedImage = this->getObject< ::fwData::Image >();
    if ( associatedImage->getCRefSize().empty() || associatedImage->getCRefSize()[0] == 0 )
    {
        createSyntheticImage( associatedImage );
    }

    // Initialize vtk pipeline image render
    initVTKPipeline();
    alignNegatoOnImageCenter();
    m_interactor->Render();
}

//-----------------------------------------------------------------------------

void ImageViewerService::stopping() throw ( ::fwTools::Failed )
{
    assert( m_render );
    assert( m_wxmanager );
    assert( m_interactor );
    ::fwGuiWx::container::WxContainer::sptr wxContainer =  ::fwGuiWx::container::WxContainer::dynamicCast( this->getContainer() );
    wxWindow* const container = wxContainer->getWxContainer();
    assert( container ) ;

    // Delete plane widget
    m_negatoSagittal->Delete();
    m_negatoSagittal = 0;
    m_negatoFrontal->Delete();
    m_negatoFrontal = 0;
    m_negatoAxial->Delete();
    m_negatoAxial = 0;

    // Delete interactor
    m_interactor->Delete();
    m_interactor = 0;

    // unitialize layout of the panel associated to this services
    m_wxmanager->UnInit();
    delete m_wxmanager;
    m_wxmanager = 0;

    // Destroy panel content
    container->DestroyChildren() ;

    // Detroy render
    m_render->Delete();
    m_render = 0;

    // This method of IRender does nothing in our example.
//    this->stopRender();
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
    m_negatoSagittal->SetInteractor( m_interactor );
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
    m_negatoFrontal->SetInteractor( m_interactor);
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
    m_negatoAxial->SetInteractor( m_interactor);
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
    // Build the buffer of a synthetic image grey level which represents a sphere
    unsigned char * buffer = new unsigned char[100*100*100];
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

    // Replace image by current buffer ( re initialization of image without modify image ptr )
    ::fwTools::DynamicType pixelType;
    pixelType.setType<unsigned char>();
    _pImage->setDimension( 3 );
    _pImage->setBuffer( buffer );
    _pImage->setPixelType( pixelType );
    _pImage->setManagesBuff( true );
    _pImage->setCRefSpacing( std::vector<double>(3,1.0) );
    _pImage->setCRefOrigin( std::vector<double>(3,0) );
    _pImage->setCRefSize( std::vector< ::boost::int32_t >(3,100) );
    _pImage->setWindowCenter(0);
    _pImage->setWindowWidth(100);
    _pImage->setRescaleIntercept(1);
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
