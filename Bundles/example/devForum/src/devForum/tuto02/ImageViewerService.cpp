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

// Services tools
#include <fwServices/Base.hpp>

// vtkIO lib : intern lib used to convert image between vtk and fw4spl
#include <vtkIO/vtk.hpp>

#include "devForum/tuto02/ImageViewerService.hpp"

namespace devForum
{

namespace tuto02
{

//-----------------------------------------------------------------------------

// Declare a new service of type IRender for render Image
REGISTER_SERVICE( ::fwRender::IRender , ::devForum::tuto02::ImageViewerService , ::fwData::Image ) ;

//-----------------------------------------------------------------------------

ImageViewerService::ImageViewerService() throw()
    :   ::devForum::tuto01::ImageViewerService()
{}

//-----------------------------------------------------------------------------

ImageViewerService::~ImageViewerService() throw()
{}

//-----------------------------------------------------------------------------
void ImageViewerService::configuring() throw ( ::fwTools::Failed )
{
    ::devForum::tuto01::ImageViewerService::configuring();
}

//-----------------------------------------------------------------------------
void ImageViewerService::updating() throw ( ::fwTools::Failed )
{
    updateVTKPipeline();
}

//-----------------------------------------------------------------------------

void ImageViewerService::updateVTKPipeline()
{
    // Get associated image
    ::fwData::Image::sptr associatedImage = this->getObject< ::fwData::Image >();

    // Convert an ::fwDada::Image to a vtk Image
    vtkSmartPointer< vtkImageData > vtk_img = vtkSmartPointer< vtkImageData >::New();
    ::vtkIO::toVTKImage( associatedImage, vtk_img );

    // Change negato input
    m_negatoSagittal->SetInput(vtk_img);
    m_negatoFrontal->SetInput(vtk_img);
    m_negatoAxial->SetInput(vtk_img);

    // Align negato on the new image center
    alignNegatoOnImageCenter();

    // Reset camera
    m_render->ResetCamera();

    // refresh vtk render
    (m_interactorManager->getInteractor())->Render();
}

//-----------------------------------------------------------------------------
void ImageViewerService::stopping() throw ( ::fwTools::Failed )
{
    ::devForum::tuto01::ImageViewerService::stopping();
}
//-----------------------------------------------------------------------------

} // end of namespace tuto02

} // end of namespace devForum
